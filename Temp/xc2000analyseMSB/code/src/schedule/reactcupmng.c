#include "reactcupmng.h"
#include "taskmng.h"
#include "apputils.h"
#include "vos_log.h"
#include "inituart.h"
#include "vos_errorcode.h"
#include "alarm.h"
#include "socketmonitor.h"
#include "acthandler.h"
#include "autoload.h"
#define  Differ_Dilu_POS  23 // 95- 72 样本稀释和普通加样位的间隔

CReactCupMng * g_pOuterCupMng = NULL;
CReactCupMng * g_pInnerCupMng = NULL;
CReactCupMng::CReactCupMng(ReactCircle circle) :m_iLeftTime(0), m_bUpdateCup(false),
    m_iBatachWashCount(0), m_bloodAbsrobMode(1), m_enCircle(circle), m_bGetAd(false)
{
    for (int i = 0; i < MAX_CUP_NUM; ++i)
    {
        CReactCup* pCup = new CReactCup(this, i + 1);
        m_vReactCups.push_back(pCup);
        m_mReactCups[i + 1] = pCup;
    }
}

CReactCupMng::~CReactCupMng()
{
    m_mReactCups.clear();
    for (int i = 0; i < MAX_CUP_NUM; ++i)
    {
        delete(m_vReactCups[i]);
        m_vReactCups[i] = NULL;
    }
    m_vReactCups.clear();
}

void CReactCupMng::ResetAllCup()
{
    m_mReactCups.clear();
    for (int i = 0; i < MAX_CUP_NUM; ++i)
    {
        m_vReactCups[i]->InitializeWithId(i + 1);
        m_mReactCups[i + 1] = m_vReactCups[i];
    }
}

bool CReactCupMng::CheckLongWash(int iPhyId)
{
    return m_mReactCups[iPhyId]->CheckLongWash();
}

void CReactCupMng::SetRegPollution(STReqRegPollution* pCross)
{
	m_stReqRegPollution.CopyFrom(*pCross);
}

void CReactCupMng::SetSmpPollution(STReqSmpPollution* pSmpPollution)
{
	m_stReqSmpPollution.CopyFrom(*pSmpPollution);
}

void CReactCupMng::SetRctPollution(STRCtRegPollution* pReqCross)
{
    m_stRctRegPollution.CopyFrom(*pReqCross);
}

void CReactCupMng::SkipDirtyCup(int iCupId)
{
	m_vReactCups[iCupId - 1]->SkipDirtyCup();
}

void CReactCupMng::SkipDilDirtyCup(int iCupId)
{
    int iLgcId = iCupId + Differ_Dilu_POS;
    if (iLgcId > MAX_CUP_NUM)
    {
        iLgcId = iLgcId - MAX_CUP_NUM;
    }
    m_vReactCups[iLgcId - 1]->SkipDilDirtyCup();
}

void CReactCupMng::SkipReactDirtyCup(int iCupId)
{
    int iLgcId = iCupId - Differ_Dilu_POS;
    if (iCupId <= Differ_Dilu_POS)
    {
        iLgcId = iCupId + (MAX_CUP_NUM - Differ_Dilu_POS);
    }
    m_vReactCups[iLgcId - 1]->SkipReactDirtyCup();
}

void CReactCupMng::CleanSkip()
{
    m_mReactCups[EN_AC_AddTask]->CheckSkip();
}

bool CReactCupMng::_needBatchWash()
{
	if (m_iBatachWashCount <= 0)
	{
		return false;
	}

	for (int i = 1; i <= MAX_CUP_NUM; ++i)
	{
		if (m_mReactCups[i]->IsToDoublePeriod() || m_mReactCups[i]->IsUnstartDilTask())
		{
			return false;
		}
	}
    m_iBatachWashCount--;
	return true;
}

bool CReactCupMng::_checkPollution(vos_u32 dstSmpType, const string& dstPrj, bool bDilut)
{
	CTask * pTask = m_mReactCups[EN_AC_CrossPollution]->GetTask();
	if (NULL != pTask)
	{
        string srcPrj = pTask->GetProjectName();
        for (int i = 0; i < m_stReqRegPollution.regpollutionlist_size(); ++i)
        {
            const RegPollution& rCross = m_stReqRegPollution.regpollutionlist(i);
            if (srcPrj == rCross.srcprj() && dstPrj == rCross.dstprj())
            {
                _procPollution(rCross.washcount(), rCross.washtype());
                return true;
            }
        }

        vos_u32 srcSmpType = pTask->GetSmpType();
        for (int i = 0; i < m_stReqSmpPollution.smppollutionlist_size(); ++i)
        {
            const SmpPollution& rPollution = m_stReqSmpPollution.smppollutionlist(i);
            if (srcSmpType == rPollution.srctype() && dstSmpType == rPollution.dsttype() && dstPrj == rPollution.dstprj())
            {
                _procPollution(rPollution.washcount(), rPollution.washtype());
                return true;
            }
        }
	}
	
    string srcPrj = m_mReactCups[EN_AC_AddTask]->GetPrePrjName();
    for (int i = 0; i < m_stRctRegPollution.rctpollutionlist_size(); ++i)
    {
        const RctPollution& rCross = m_stRctRegPollution.rctpollutionlist(i);
        if (srcPrj == rCross.srcprj() && dstPrj == rCross.dstprj())
        {
            _procPollution(1, bDilut ? 0 : rCross.washtype());
            return true;
        }
    }

    if (bDilut)
    {
        srcPrj = m_mReactCups[EN_AC_Dilut]->GetPrePrjName();
        for (int i = 0; i < m_stRctRegPollution.rctpollutionlist_size(); ++i)
        {
            const RctPollution& rCross = m_stRctRegPollution.rctpollutionlist(i);
            if (srcPrj == rCross.srcprj() && dstPrj == rCross.dstprj())
            {
                _procPollution(1, 0);//如果是稀释任务，当前40杯子不做
                return true;
            }
        }
    }
    
	return false;
}

void CReactCupMng::_procPollution(int nWashCount, int nWashType)
{
    if (nWashType > 0)
    {
        m_mReactCups[EN_AC_AddTask]->AssignWashTask(nWashType);
    }
    else
    {
        m_mReactCups[EN_AC_AddTask]->AssignNullTaske();
    }

    int nCup = EN_AC_Skip;
	for (int i = 1; i < nWashCount; ++i)
	{
        if (m_mReactCups[nCup]->IsToDoublePeriod() || m_mReactCups[nCup]->IsUnstartDilTask())
        {
            break;
        }

        if (nWashType > 0)
        {
            m_mReactCups[nCup]->AssignWaitWashTask(nWashType);
        }
        else
        {
            m_mReactCups[nCup]->AssignSkip();
        }

		nCup = (nCup + MAX_CUP_NUM - 41) % MAX_CUP_NUM;
		if (0 == nCup)
		{
			nCup = MAX_CUP_NUM; //转到165号位置了
		}
	}
}

void CReactCupMng::ProcSuspendTime()
{
    //只有测试态才需要更新暂停状态
    if (g_pMonitor->m_ulState >= vos_midmachine_state_testing && g_pMonitor->m_ulState <= vos_midmachine_state_wait_pause)
    {
        int iMaxTime = 0;
        for (int i = 0; i < MAX_CUP_NUM; ++i)
        {
            m_vReactCups[i]->ProcSuspendTime(iMaxTime);
        }

        int iStatus = (0 == iMaxTime ? vos_midmachine_state_pause : vos_midmachine_state_wait_pause);
        app_reportmidmachstatus(iStatus, SPid_Task, iMaxTime);
    }
}

bool CReactCupMng::_canWash(map<int, CSimuCup*>& mSimuCups)
{
	for (int i = EN_AC_Wash1; i <= EN_AC_AddTask; ++i)
	{
		int iLgcId = mSimuCups[i]->GetLgcId();
		bool bDoublePeriod = m_vReactCups[iLgcId - 1]->IsDoublePeriod();
		if (!mSimuCups[i]->CanWash(bDoublePeriod))
		{
			return false;
		}
	}
	return true;
}

bool CReactCupMng::_procWash(map<int, CSimuCup*>& mSimuCups)
{
    bool bWash = false;
	for (int i = EN_AC_Wash1; i <= EN_AC_AddTask; ++i)
    {
        if (mSimuCups[i]->ProcWash())
        {
            bWash = true;
        }
    }
    return bWash;
}

bool CReactCupMng::_isNeedOver(map<int, CSimuCup*>& mSimuCups)
{
    for (int i = 1; i <= MAX_CUP_NUM; ++i)
    {
        if (!mSimuCups[i]->IsOver())
        {
            return false;
        }
    }
    return true;
}

void CReactCupMng::_procFinish(map<int, CSimuCup*>& mSimuCups)
{
	int iLgcId = mSimuCups[EN_AC_AddTask]->GetLgcId();
	bool bDoublePeriod = m_vReactCups[iLgcId - 1]->IsDoublePeriod();
	mSimuCups[EN_AC_AddTask]->ProcFinish(bDoublePeriod);
}

void CReactCupMng::_calcLeftTime()
{
    vector<CSimuCup*> vSimuCups;
    map<int, CSimuCup*> mSimuCups;

    for (int i = 0; i < MAX_CUP_NUM; ++i)
    {
        CSimuCup* pCup = new CSimuCup();
        int iPhyId = m_vReactCups[i]->InitSimuCup(pCup);
        vSimuCups.push_back(pCup);
        mSimuCups[iPhyId] = pCup;
    }

    m_iLeftTime = 0;
    while (true)
    {  
        ++m_iLeftTime;
        _procFinish(mSimuCups);        
        if (_canWash(mSimuCups))
        {
            mSimuCups[EN_AC_AddTask]->StartDilTask();
            bool bWash = _procWash(mSimuCups);
            if (!bWash&&_isNeedOver(mSimuCups))
            {
                break;
            }
        }
        mSimuCups.clear();
        for (int i = 0; i < MAX_CUP_NUM; ++i)
        {
            int iPhyId = vSimuCups[i]->Update();
            mSimuCups[iPhyId] = vSimuCups[i];
        }
    }

    mSimuCups.clear();
    for (int i = 0; i < MAX_CUP_NUM; ++i)
    {
        delete(vSimuCups[i]);
        vSimuCups[i] = NULL;
    }
    vSimuCups.clear();
}

void CReactCupMng::_procLeftTime()
{
    if (g_pTaskMng->IsWashCup())
    {
        return;
    }

    if (g_pTaskMng->HasTask())
    {
        m_iLeftTime = 0;
    }
    else
    {
        _calcLeftTime();
    }
	app_reportmidmachstatus(vos_midmachine_state_testing, SPid_Task, m_iLeftTime);
}

bool CReactCupMng::_lightIsOut(vos_u32 count, vos_u32(*adValue)[14])
{
    vos_u32(*pAdValue)[14] = adValue;
    for (vos_u32 loop = 0; loop < count; loop++)
    {
        if (pAdValue[loop][0] >= 1000)
        {
            return false;
        }
    }
    return true;
}

void CReactCupMng::CheckSmp()
{
    for (vos_u32 cup = 1; cup <= MAX_CUP_NUM;++cup)
    {
        CReactCup *pCup = m_mReactCups[cup];
        if (pCup->GetPerioid() < PERIOD_SmpDsk_46)
        {
            pCup->CheckSmp();
        }   
   }
}

void CReactCupMng::StartWashCup(int iMsgId)
{
    if (MSG_ReqLightCheck == iMsgId)
    {
        m_vReactCups[39]->Initialize(MAX_WASH_NUM);
        m_vReactCups[81]->Initialize(MAX_WASH_NUM);
        m_vReactCups[122]->Initialize(MAX_WASH_NUM);
        m_vReactCups[163]->Initialize(MAX_WASH_NUM);
    }
    else
    {
        for (int i = 0; i < MAX_CUP_NUM; ++i)
        {
            m_vReactCups[i]->Initialize(MAX_WASH_NUM);
        }
    }
    g_pTaskMng->SetWashCup(iMsgId);
}

bool CReactCupMng::SetActResult(int iPhyId, int iResult)
{
    return m_mReactCups[iPhyId]->SetActResult(iResult);
}

void CReactCupMng::SendReactCupStatus()
{
    STNotifyRctCupStatus stPara;
    for (int i = 0; i < MAX_CUP_NUM; ++i)
    {
        STRctCupStatus * pStatus = stPara.add_stcupstatus();
        m_vReactCups[i]->GetStatus(pStatus);
    }
    stPara.set_encircle(m_enCircle);
    sendUpMsg(CPid_ReactDiskMonitor, SPid_Task, MSG_NotifyRctCupStatus, &stPara);
}

void CReactCupMng::CancelTask(bool bAll/*= true*/, int iAlarmLevel /*= 0*/, bool bSend /*= true*/)
{

    STNotifyFailedSmpTask  stPara;

    //处理正在测试的任务
    for (int i = 0; i < MAX_CUP_NUM; ++i)
    {
        m_vReactCups[i]->CancelTask(iAlarmLevel, stPara);
    }

    //处理未分配的任务
	_procUnHandleTask(stPara, bAll);

    if (bSend)
    {
        sendUpMsg(CPid_SmpTask, SPid_Task, MSG_NotifyFailedSmpTask, &stPara);
    }
}
void CReactCupMng::_procUnHandleTask(STNotifyFailedSmpTask& stPara, bool bAll)
{
	bool bAlarm = !(g_pAlarm->HasAlarm(1) || g_pAlarm->HasAlarm(2) || g_pAlarm->HasAlarm(4));
	bool bAdd = !bAll && bAlarm;
	g_pTaskMng->DeleteR2Task(bAdd, stPara);
}

bool CReactCupMng::IsActTimeOut(int iPhyId)
{
    return m_mReactCups[iPhyId]->IsDoing();
}

vos_u32 CReactCupMng::SendSmpNdlPreAct()
{
    return m_mReactCups[EN_AC_SimpleNdlToAbsorbPos]->SendSmpNdlRtt();
}

void CReactCupMng::CheckActTimeOut(int iPhyId, vos_u32 ulFaultId, vos_u32 ulComp, vos_u32 ulAct)
{
    if (IsActTimeOut(iPhyId))
    {
        g_pAlarm->GenerateAlarm(ulFaultId, ulComp, ulAct);
    }
}

//是否有超时未完成的任务
bool CReactCupMng::HasActCupTimeOut(const EN_ActCup *pCups, int ulSize)
{
    for (int i = 0; i < ulSize; ++i)
    {
        if (IsActTimeOut(pCups[i]))
        {
            return true;
        }
    }
    return false;
}

//检查执行动作是否超时
void CReactCupMng::CheckNdlTimeOut()
{
    //R1系统
    m_mReactCups[EN_AC_R1AbsorbAndDischarge]->CheckNdlTimeOut(ALarm_R1_System, xc8002_dwnmach_name_r1Ndl);

    //样本系统-(72排样位)
    m_mReactCups[EN_AC_SimpleNdlAbsorb]->CheckNdlTimeOut(ALarm_Smp_System, xc8002_dwnmach_name_smpndl);

    //样本系统-(95稀释位)
    m_mReactCups[EN_AC_SimpleNdlAbsorb]->CheckNdlTimeOut(ALarm_Smp_System, xc8002_dwnmach_name_smpndl);

    //R2系统
    m_mReactCups[EN_AC_R2AbsorbAndDischarge]->CheckNdlTimeOut(ALarm_R2_System, xc8002_dwnmach_name_r2Ndl);
}

void CReactCupMng::CheckResultCalc()
{
    for (int i = 0; i < MAX_CUP_NUM; ++i)
    {
        m_vReactCups[i]->CheckResultCalc();
    }
}

// void CReactCupMng::_procSmpLight()
// {
//     if (CheckCupAct(EN_AC_MoveSimpleDsk, EN_ACT_SimpleNdlToAbsorbPos, 0))
//     {
//         //常亮
//         app_smpLightCtl(SPid_Task, samp_key_led_on);
//     }
//     else if (CheckCupAct(EN_AC_FlickerSimpleDsk, EN_ACT_SimpleNdlToAbsorbPos, 0))
//     {
//         //闪灯
//         app_smpLightCtl(SPid_Task, samp_key_led_flicker);
//     }
//     else if (CheckCupAct(EN_AC_SimpleNdlAbsorb, EN_ACT_SimpleNdlAbsorb, 0))
//     {
//         //关灯
//         app_smpLightCtl(SPid_Task, samp_key_led_off);
//     }
// }

vos_u32 CReactCupMng::ProcSmpExeAct()
{
    //样本指标灯
   // _procSmpLight();
    CReactCup * pCup = m_mReactCups[EN_AC_SimpleNdlAbsorb];
    if (pCup->NeedSmpNdlExe())
    {
        return pCup->SendSmpNdlExe();
    }
    return CheckSmpDsk();
}

vos_u32 CReactCupMng::ProcSmpExeActDilu()
{
    //样本指标灯
   // _procSmpLight();
	CReactCup * pCup = m_mReactCups[EN_AC_SimpleNdlAbsorb_DILU];
	if (pCup->NeedSmpNdlExeDilu())
	{
		return pCup->SendSmpNdlExeDilu();
	}
	return CheckSmpDskDilu();
}


vos_u32 CReactCupMng::CheckSmpDsk()
{
    CReactCup *pCup = m_mReactCups[EN_AC_MoveSimpleDsk];
    if (pCup->NeedMoveSmpdsk())
    {
	    return pCup->SendSmpDskRtt(Act_Move);
    }
    return VOS_OK;
}

vos_u32 CReactCupMng::CheckSmpDskDilu()
{
	CReactCup *pCup = m_mReactCups[EN_AC_MoveSimpleDsk_DILU];
	if (pCup->NeedMoveSmpdskDilu())
	{
		return pCup->SendSmpDskRtt(Act_Move);
	}
	return VOS_OK;
}

vos_u32 CReactCupMng::ProcR1SysExeAct()
{
    CReactCup * pCup = m_mReactCups[EN_AC_R1AbsorbAndDischarge];
    if (pCup->NeedR1NdlExe())
    {
        return pCup->SendReagNdlExeAct(SPid_R1);
    }
    return CheckOuterDsk();
}

vos_u32 CReactCupMng::CheckOuterDsk()
{
    CReactCup *pCup = m_mReactCups[EN_AC_MoveR1Dsk];
    if (pCup->NeedMoveOuterDsk())
    {
        return pCup->SendOuterDskRtt();
    }
    return VOS_OK;
}

vos_u32 CReactCupMng::ProcR2SysExeAct()
{
    CReactCup * pCup = m_mReactCups[EN_AC_R2AbsorbAndDischarge];
    if (pCup->NeedR2NdlExe())
    {
        return pCup->SendReagNdlExeAct(SPid_R2);
    }

    return CheckInnerDsk();
}

vos_u32 CReactCupMng::CheckInnerDsk()
{
    CReactCup *pCup = m_mReactCups[EN_AC_MoveR2Dsk];
    if (pCup->NeedMoveInnerDsk())
    {
        return pCup->SendInnerDskRtt();
    }
    return VOS_OK;
}

vos_u32 CReactCupMng::ProcSmpMixExeAct()
{
    CReactCup *pCup = m_mReactCups[EN_AC_SimpleMix];
    if (pCup->NeedSmpMix())
    {
        pCup->SetActCount(1);
        return app_reqDwmCmd(SPid_Task, SPid_SMix, Act_Mix, 0, NULL, APP_YES);
    }

    if (m_mReactCups[EN_AC_SampleMixNdlWash]->NeedSmpMixWash())
    {
#ifdef Edition_A
		return app_reqDwmCmd(SPid_Task, SPid_SMix, Act_Wash, 0, NULL, APP_NO);       
#else
        MixWashCtrl(SPid_Task, SPid_SMix, APP_OPEN);
        return app_reqDwmCmd(SPid_Task, SPid_SMix, Act_Wash, 0, NULL, APP_YES);
#endif
    }
    return VOS_OK;
}

vos_u32 CReactCupMng::ProcReagMixExeAct()
{
    CReactCup *pCup = m_mReactCups[EN_AC_ReagMix];
    if (pCup->NeedReagMix())
    {
        pCup->SetActCount(1);
        return app_reqDwmCmd(SPid_Task, SPid_R2Mix, Act_Mix, 0, NULL, APP_YES);
    }

    if (m_mReactCups[EN_AC_ReagMixNdlWash]->NeedReagMixWash())
    {
#ifdef Edition_A
        return app_reqDwmCmd(SPid_Task, SPid_R2Mix, Act_Wash, 0, NULL, APP_NO);
#else
        MixWashCtrl(SPid_Task, SPid_R2Mix, APP_OPEN);
        return app_reqDwmCmd(SPid_Task, SPid_R2Mix, Act_Wash, 0, NULL, APP_YES);
#endif
    }
    return VOS_OK;
}

vos_u32 CReactCupMng::ProcPreAct()
{
    //转到样本搅拌位
    CReactCup *pCup = m_mReactCups[EN_AC_SimpleMix];
    if (pCup->NeedSmpMix())
    {
        pCup->SetActCount(1);
        _sendNdlRttPreAct(app_reactsys_mix_ndl_mixing_38pos, SPid_SMix);
    }

    //转到试剂搅拌位
    pCup = m_mReactCups[EN_AC_ReagMix];
    if (pCup->NeedReagMix())
    {
        pCup->SetActCount(1);
        _sendNdlRttPreAct(app_reactsys_mix_ndl_mixing_67pos, SPid_R2Mix);
    }

    //转到R1吸试剂位
    pCup = m_mReactCups[EN_AC_R1AbsorbAndDischarge];
    if (pCup->NeedR1NdlExe())
    {
        pCup->SetActCount(1);
        _sendNdlRttPreAct(pCup->GetR1Pos(), SPid_R1);
    }
	else if (g_pActHandler->GetAlarmLevel() < 1)
	{
		app_reqDwmCmd(SPid_Task, SPid_R1, Act_Wash, 0, NULL, APP_NO);
	}

    //转到R2吸试剂位
    pCup = m_mReactCups[EN_AC_R2AbsorbAndDischarge];
    if (pCup->NeedR2NdlExe())
    {
        pCup->SetActCount(1);
        _sendNdlRttPreAct(pCup->GetR2Pos(), SPid_R2);
    }
	else  if (g_pActHandler->GetAlarmLevel() < 3)
	{
		app_reqDwmCmd(SPid_Task, SPid_R2, Act_Wash, 0, NULL, APP_NO);
	}

    //转到吸样本位
    pCup = m_mReactCups[EN_AC_SimpleNdlAbsorb];
    if (pCup->NeedSmpNdlExe())
    {
        if (m_mReactCups[EN_AC_SimpleNdlAbsorb_DILU]->NeedSmpNdlExeDilu())
        {
            WRITE_ERR_LOG("Schedule Error, Stop Test!\n");
            return VOS_INVALID_U32;
        }

		if (pCup->GetTask()->GetDilutType() == EN_ReactionType)
		{
			return pCup->SendSmpNdlRttDilu();//样本针转到稀释位
		}
		else
		{
			return pCup->SendSmpNdlRtt(Act_Move);//样本针转到样本盘吸样位
		}
    }
    else if (m_mReactCups[EN_AC_SimpleNdlAbsorb_DILU]->NeedSmpNdlExeDilu())//转到吸样本位稀释
	{
        return m_mReactCups[EN_AC_SimpleNdlAbsorb_DILU]->SendSmpNdlRtt(Act_MoveDilu);
	}
	else  if (g_pActHandler->GetAlarmLevel() < 2)
	{
        app_print(ps_rscsch, "ProcPreAct Wash\n");
		return app_reqDwmCmd(SPid_Task, SPid_SmpNdl, Act_Wash, 0, NULL, APP_NO);
	}
    return VOS_OK;
}

vos_u32 CReactCupMng::_sendNdlRttPreAct(app_u16 usDstPos, vos_u32 ulDstPid)
{
    msg_stNdlRtt stPara;
    stPara.usDstPos = usDstPos;
    return app_reqDwmCmd(SPid_Task, ulDstPid, Act_Move, sizeof(msg_stNdlRtt), (char*)&stPara, APP_YES);
}

bool CReactCupMng::CanWash()
{
    for (int i = EN_AC_Wash10; i <= EN_AC_Wash2; ++i)
    {
        if (!m_mReactCups[i]->CanWash())
        {
            return false;
        }
    }
    return m_mReactCups[EN_AC_Wash1]->CanWash();
}

void CReactCupMng::ProcLiquid(EN_ActCup cup, TaskResult Trslt, vos_u16 dwnMachine)
{
	m_mReactCups[cup]->ProcLiquid(Trslt, dwnMachine);
}

void CReactCupMng::CancelDilTask(int iLgcId, int nResult)
{
	int iDilCupId = iLgcId - Differ_Dilu_POS;
	if (iLgcId <= Differ_Dilu_POS)
	{
		iDilCupId = iLgcId + (MAX_CUP_NUM - Differ_Dilu_POS);
	}
    m_vReactCups[iDilCupId - 1]->CancelDilTwoTask(iDilCupId, nResult);
}

void CReactCupMng::CancelReactTask(int iLgcId, int nResult)
{
    int iDilCupId = iLgcId + Differ_Dilu_POS;
    if (iDilCupId > MAX_CUP_NUM)
    {
        iDilCupId = iDilCupId - MAX_CUP_NUM;
    }
    m_vReactCups[iDilCupId - 1]->CancelDilTask(iDilCupId, nResult);
}

void CReactCupMng::SetBloodAbsorbMode(bool mode)
{
	m_bloodAbsrobMode = mode;
}

bool CReactCupMng::GetBloodAbsorbMode()
{
	return m_bloodAbsrobMode;
}

void CReactCupMng::_updateCups(map<app_u16, CTask*>& mSuspendTasks, bool bSuspend)
{
    m_mReactCups.clear();
    for (int i = 0; i < MAX_CUP_NUM; ++i)
    {
        int iPhyId = m_vReactCups[i]->Update();
        m_mReactCups[iPhyId] = m_vReactCups[i];
        if (bSuspend)
        {
            m_vReactCups[i]->SuspendTask(mSuspendTasks);
        }
        if (i == 0)
        {
            app_print(ps_taskmng, "cup1 at %d\n", iPhyId);
        }
    }
    m_bUpdateCup = true;
}

bool CReactCupMng::NeedSmpNdlAct(msg_paraSampleDskRtt& stDskRtt, msg_stNdlRtt& stNdlRtt, msg_stSampleSysAbsorb& stNdllAbsb, msg_stSampleSysDischarge& stNdllDisg)
{
    CReactCup * pCup = m_mReactCups[EN_AC_SimpleNdlAbsorb];
    if (pCup->NeedSmpNdlExe(stDskRtt, stNdlRtt, stNdllAbsb, stNdllDisg))
    {
        app_print(ps_taskmng, "--SmpNdlAct--\n");
        return true;
    }
    int diluCup = (m_enCircle == Circle_Inner) ? EN_AC_SimpleNdlAbsorb_DILU : EN_AC_SimpleNdlAbsorb_DILU_Outer;
    pCup = m_mReactCups[diluCup];
    if (pCup->NeedSmpNdlExeDilu(stDskRtt, stNdlRtt, stNdllAbsb, stNdllDisg, m_enCircle))
    {
        return true;
    }

    return false;
}

bool CReactCupMng::NeedR1NdlAct(act_stReagSysDskRtt& stDskRtt, msg_stNdlRtt& stNdlRtt, msg_stReagSysAbsorb& stNdllAbsb, msg_stReagSysDischarge& stNdllDisg)
{
    CReactCup * pCup = m_mReactCups[EN_AC_R1AbsorbAndDischarge];
    if (pCup->NeedR1NdlExe(stDskRtt,stNdlRtt, stNdllAbsb, stNdllDisg))
    {
        app_print(ps_taskmng, "--R1NdlAct--\n");
        return true;
    }
    return false;
}

bool CReactCupMng::NeedR2NdlAct(act_stReagSysDskRtt& stRDskRtt, msg_stNdlRtt& stNdlRtt, msg_stReagSysAbsorb& stNdllAbsb, msg_stReagSysDischarge& stNdllDisg)
{
    CReactCup * pCup = m_mReactCups[EN_AC_R2AbsorbAndDischarge];
    if (pCup->NeedR2NdlExe(stRDskRtt, stNdlRtt, stNdllAbsb, stNdllDisg))
    {
        app_print(ps_taskmng, "--R2NdlAct--\n");
        return true;
    }
    return false;
}

bool CReactCupMng::NeedSMixAct()
{
    CReactCup *pCup = m_mReactCups[EN_AC_SimpleMix];
    return pCup->NeedSmpMix();
}

bool CReactCupMng::NeedRMixAct()
{
    CReactCup *pCup = m_mReactCups[EN_AC_ReagMix];
    return pCup->NeedReagMix();
}

bool CReactCupMng::NeedR1AutoLoad(act_stReagSysDskRtt& stRtt)
{
    CReactCup *pCup = m_mReactCups[EN_AC_R1AbsorbAndDischarge];
    if (pCup->GetTask()==NULL)
    {
        return false;
    }
    if (pCup->GetTask()->GetMaintanType() == wheelset_task)
    {
        stRtt.usCupId = pCup->GetTask()->GetReagInfo(PERIOD_R1_36).stcuppos().uicupid();
        stRtt.usDstPos = 21;
        if (pCup->GetTask()->GetReagInfo(PERIOD_R1_36).uiaddvol() == 0)
        {
            return false;
        }
        app_print(ps_autoload, "EN_AC_R1AbsorbAndDischarge cup R1 task maintanType is wheelset_task\n");
        return true;
    }
    return false;
}

bool CReactCupMng::NeedR2AutoLoad(act_stReagSysDskRtt& stNdlRtt)
{
    CReactCup *pCup = m_mReactCups[EN_AC_R2AbsorbAndDischarge];
    if (pCup->GetTask() == NULL)
    {
        return false;
    }
    if (pCup->GetTask()->GetMaintanType() == wheelset_task)
    {
        stNdlRtt.usCupId = pCup->GetTask()->GetReagInfo(PERIOD_R2_97).stcuppos().uicupid();
        stNdlRtt.usDstPos =56;
        if (pCup->GetTask()->GetReagInfo(PERIOD_R2_97).uiaddvol()==0)
        {
            return false;
        }
        app_print(ps_autoload, "EN_AC_R2AbsorbAndDischarge cup R2 task maintanType is wheelset_task\n");
        return true;
    }
    return false;
}

bool CReactCupMng::CheckAutoWash(app_u16& usWashBit)
{
    usWashBit = 0;
    bool bWash = false;
    app_u32 bit = 0;
    if (m_mReactCups[EN_AC_Wash1]->CheckWash())
    {
        usWashBit |= 0x01 << bit;
        bWash = true;
    }
    bit++;
    for (int i = EN_AC_Wash2; i >= EN_AC_Wash10; --i)
    {
        if (m_mReactCups[i]->CheckWash())
        {
            usWashBit |= 0x01 << bit;
            bWash = true;
        }
        bit++;
    }
    return bWash;
}

void CReactCupMng::ProcAutoWash()
{
    for (int i = EN_AC_Wash10; i <= EN_AC_Wash2; ++i)
    {
        m_mReactCups[i]->ProcAutoWash(EN_AF_Success);
    }
    m_mReactCups[EN_AC_Wash1]->ProcAutoWash(EN_AF_Success);
}

void CReactCupMng::ProcFinishTask()
{
    m_mReactCups[EN_AC_Wash1]->ProcFinishTask();
}

vos_u32 CReactCupMng::ProcAdReady(msg_ADValue* pAdValue)
{
//     vos_u32 uladnum = 0;
//     vos_u32 auladval[APP_RTT_RCTCUP_PER_PERIOD][WAVE_NUM] = { 0 };
//     vos_u32 ulrunrslt = GetAbsValueFromShareMem(&uladnum, (vos_u32*)auladval, sizeof(auladval));
//     if (VOS_OK != ulrunrslt)
//     {
//         WRITE_ERR_LOG("Call GetAbsValueFromShareMem() failed(0x%x)(%d)", ulrunrslt, m_enCircle);
//         return ulrunrslt;
//     }
// 
//     if (uladnum != APP_RTT_RCTCUP_PER_PERIOD) //监控AD上报杯位数是否足够
//     {
//         WRITE_ERR_LOG("AD num(%d) is not enough!", uladnum);
//     }
// 
//     if (_lightIsOut(uladnum, auladval))
//     {
//         g_pAlarm->GenerateAlarm(ALarm_Light, xc8002_dwnmach_name_reactdisk);
//     }

    int iStartPhyId = 82;//更新反应杯之前处理AD值(82到123)
    if (m_bUpdateCup == true)
    {
        WRITE_ERR_LOG("ProcAdReady after Update\n");
        iStartPhyId += 42;//更新反应杯之后处理AD值(124到165)
    }
    
    app_u32 *p = pAdValue->value;
    for (int i = 0; i < APP_RTT_RCTCUP_PER_PERIOD; ++i)//
    {
        vos_u32 auladval[WAVE_NUM] = { 0 };
        for (int index = 0; index < WAVE_NUM; index++)
        {
            auladval[index] = *p;
            p++;
        }     
        m_mReactCups[iStartPhyId + i]->ProcAdReady(auladval, m_enCircle);
    }
    return VOS_OK;
}

void CReactCupMng::UpdateCups(bool bSuspend)
{
    map<app_u16, CTask*> mSuspendTasks;   //等待挂起任务
    _updateCups(mSuspendTasks, bSuspend);
//     if (bSuspend)
//     {
//         g_pTaskMng->SuspendTask(mSuspendTasks);
//         ProcSuspendTime();
//     }
//     else
//     {
//         _procLeftTime();
//     }
}

CTask* CReactCupMng::GetTask(app_u16 cup)
{
    CTask* pTask = m_vReactCups[cup - 1]->GetTask();
    return pTask;
}

bool CReactCupMng::IsNeedOver()
{
    for (int i = 0; i < MAX_CUP_NUM; ++i)
    {
        if (!m_vReactCups[i]->IsNeedOver())
        {
            printf("**CUP %d NEED WASH%d** \n", i + 1, m_vReactCups[i]->GetWashCount());
            return false;
        }
    }
    return true;
}

void CReactCupMng::AssignNewTask()
{
    if (!m_mReactCups[EN_AC_AddTask]->CheckClean())
    {
        return;
    }

    if (m_mReactCups[EN_AC_AddTask]->CheckSkip())
    {
        return;
    }

    if (m_mReactCups[EN_AC_AddTask]->IsDoublePeriod())
    {
        return;
    }
    //与40号位相邻的40到48号杯位是要进入第二周期了
    //     for (int i = EN_AC_AddTask; i <= EN_AC_AddTask + 9; ++i)
    //     {
    //         if (m_mReactCups[i]->IsToDoublePeriod())
    //         {           
    //             app_print(ps_taskmng, "\nAssignNewTask=false phyId=%d,taskid=%d\n", i, m_mReactCups[i]->GetTask()->GetTaskId());
    //             return;
    //         }
    //     }

    //     if (m_mReactCups[EN_AC_AddTask]->StartDilTwoTask())
    //     {
    //         m_iBatachWashCount = 2;
    //         return;
    //     }

    bool bBatchWash = true;
    while (true)
    {
        CTask * pTask = g_pTaskMng->GetWaitingTask(m_enCircle);

        if (VOS_NULL == pTask)
        {
            if (bBatchWash && _needBatchWash())
            {
                app_print(ps_taskmng, "==ADD BatchWash ONCE==\n");
                //m_mReactCups[EN_AC_AddTask]->AssignWashTask(1);
            }
            bBatchWash = true;
            return;
        }

        //王晖俊 增加轮空查询
        CTask * wTask = NULL;
        if (g_pAutoLoad->NeedWheelset(pTask, wTask))
        {
            g_pTaskMng->AddTaskFront(pTask);
            m_mReactCups[EN_AC_AddTask]->AssignNewTask(wTask);
            m_iBatachWashCount = 2;
            return;
        }
        //end

        vos_u32 r1Cup = 0, r2Cup = 0;
        TaskResult TR_type = TR_Success;
        if (!pTask->CheckAllLiquid(0, r1Cup, r2Cup, TR_type))
        {
            WRITE_ERR_LOG("CheckAllLiquid failed, task(%d) is canceled , TaskResult=%lu!\n", pTask->GetTaskId(), TR_type);
            g_pTaskMng->SendTaskResult(&pTask, TR_type);
            bBatchWash = false;
            continue;
        }

        if (_checkPollution(pTask->GetSmpType(), pTask->GetProjectName(), pTask->HasDilutVol()))
        {
            g_pTaskMng->AddTaskFront(pTask);
            return;
        }

        //常规任务
        if (!pTask->HasDilutVol())
        {
            m_mReactCups[EN_AC_AddTask]->AssignNewTask(pTask);
            m_iBatachWashCount = 2;
            return;
        }

        WRITE_ERR_LOG("**Can not get here！！!\n");

        //以下是稀释、双周期
#if 0
        //稀释杯可能有双周期任务
        for (int i = EN_AC_Dilut - 8; i <= EN_AC_Dilut + 8; ++i)
        {
            if (m_mReactCups[i]->IsToDoublePeriod())
            {
                g_pTaskMng->AddTaskFront(pTask);
                return;
            }
        }

        //分配带稀释流程的任务，首先检查17号杯第二任务是否可分配任务
        if (!m_mReactCups[EN_AC_Dilut]->CanAssignTwoTask())
        {
            g_pTaskMng->AddTaskFront(pTask);
            return;
        }

        if (pTask->CheckWblood())
        {
            if (!m_mReactCups[EN_AC_Skip]->AssignSkip())
            {
                g_pTaskMng->AddTaskFront(pTask);
                return;
            }
            if (!m_mReactCups[EN_AC_AddTask]->AssignWBTask(pTask))
            {
                g_pTaskMng->SendTaskResult(&pTask, TR_ReagErr);
                bBatchWash = false;
                continue;
            }
        }
        else
        {
            if (!m_mReactCups[EN_AC_AddTask]->AssignDilOneTask(pTask))
            {
                g_pTaskMng->SendTaskResult(&pTask, TR_ReagErr);
                bBatchWash = false;
                continue;
            }
        }
        m_mReactCups[EN_AC_Dilut]->AssignDilTwoTask(pTask);
#endif
        return;
    }
}
