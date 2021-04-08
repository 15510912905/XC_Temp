#include "reactcup.h"
#include "taskmng.h"
#include "vos_log.h"
#include "apputils.h"
#include "vos_errorcode.h"
#include "alarm.h"
#include "reactcupmng.h"
#include "reagent.h"
#define R1 1
#define R2 2
#define R3 3
#define R4 4

//?μí3?aê??á1aê±?ì?¨ò?
#define APP_READ_WATER_GAP_PERIOD     (33)
#define APP_START_NML_READ_AD_PERIOD  (48)

#define Two_Act_Num 2
#define One_Act_Num 1
extern app_u16 g_liquidFlag[6];
#define LiquidErr  0xf

vos_u32 g_washInfo[][6] =
{
    { 300, 162, 3000, 50, 1500, 50 },//水
    { 300, 161, 3000, 50, 1500, 50 },//碱洗
    { 300, 161, 3000, 50, 1500, 50 },//酸洗
};

CReactCup::CReactCup(CReactCupMng* pCupMng, int iLgcId)
    :m_pCupMng(pCupMng), m_iLgcId(iLgcId), m_iPhyId(iLgcId), m_iPeriod(0), m_iWashCount(0), m_iTotalVol(0)
    , m_iActCount(0), m_bDirtyCup(false), m_bSkipCup(false), m_pTask(NULL), m_pDilTwoTask(NULL), m_prePrjName("")
{
}

CReactCup::~CReactCup()
{
}

void CReactCup::Initialize(int iWashCount, bool bFinish /*= false*/)
{
	if (iWashCount < MAX_WASH_NUM)
	{
		m_iPeriod = 0;
	}
	m_iWashCount = iWashCount;
	m_iTotalVol = 0;
	m_iActCount = 0;
	m_bDirtyCup = false;
	if (NULL != m_pTask)
	{
        g_pTaskMng->DeleteShelfTask(m_pTask);
		delete(m_pTask);
		m_pTask = NULL;
	}
	if (NULL != m_pDilTwoTask && !bFinish)
	{
		delete(m_pDilTwoTask);
		m_pDilTwoTask = NULL;
	}
}

void CReactCup::InitializeWithId(int iPhyId)
{
    Initialize(MAX_WASH_NUM);
//     if (m_iPhyId==1)
//     {
//         Initialize(MAX_WASH_NUM);
//     }
    m_iPhyId = iPhyId;
}

void CReactCup::AssignNewTask(CTask * pTask)
{
// 	m_iPeriod = 1;
// 	m_iWashCount = MAX_WASH_NUM;
	m_pTask = pTask;
    m_prePrjName = m_pTask->GetProjectName();
    app_print(ps_taskmng, "start noml task-%d(cup-%d)\n", m_pTask->GetTaskId(), m_iLgcId);
}

bool CReactCup::CheckSkip()
{
    if (m_bSkipCup)
    {
        m_bSkipCup = false;
        m_prePrjName = "";
        if (m_pDilTwoTask != NULL)
        {
            WRITE_ERR_LOG("==Skip error==\n");
        }
        return true;
    }
    return false;
}

bool CReactCup::AssignSkip()
{
    if (m_pDilTwoTask == NULL)
	{
        m_bSkipCup = true;
        m_prePrjName = "";
        return true;
	}
    return false;
}

void CReactCup::AssignNullTaske()
{
    m_iWashCount = MAX_WASH_NUM;
    m_prePrjName = "";
}

void CReactCup::AssignWashTask(int washType /*= 0*/)
{
    m_prePrjName = "";
    if (0 != g_pAlarm->FindMaxLevelAlarm())
    {
        return;
    }

	STReqAddTask  rReqTask;
    if (!_initWashTask(rReqTask, washType))
	{
        return;
	}
	m_pTask = new CTask(SPid_Task, &rReqTask);
	m_pTask->SetDilutType(EN_DilutType);
	app_print(ps_taskmng, "start wask task-%d(cup-%d)\n", m_pTask->GetTaskId(), m_iLgcId);
}

void CReactCup::AssignWaitWashTask(int washType)
{
    if (0 != g_pAlarm->FindMaxLevelAlarm())
    {
        return;
    }

    STReqAddTask  rReqTask;
    if (!_initWashTask(rReqTask, washType))
    {
        return;
    }   
    m_pDilTwoTask = new CTask(SPid_Task, &rReqTask);
    m_pDilTwoTask->m_iDilutPeriod = 1;
    m_pDilTwoTask->SetDilutType(EN_DilutType);
}

bool CReactCup::AssignDilOneTask(CTask* pTask)
{
	STReqAddTask  rReqTask;
    if (!pTask->GetDilFirstTask(rReqTask))
	{
        return false;
	}
    m_iPeriod = 1;
    m_iWashCount = MAX_WASH_NUM;
	m_pTask = new CTask(pTask->GetSrcId(), &rReqTask);
	m_pTask->SetDilutType(EN_DilutType);
    m_prePrjName = "";//稀释任务不存在反应杯交叉污染
    app_print(ps_taskmng, "start Dil1 task-%d(cup-%d)\n", m_pTask->GetTaskId(), m_iLgcId);
    return true;
}

bool CReactCup::AssignWBTask(CTask* pTask)
{
    STReqAddTask  rReqTask;
    if (!pTask->GetWBFirstTask(rReqTask))
    {
        return false;
    }
    m_iPeriod = 1;
    m_iWashCount = MAX_WASH_NUM;
    m_pTask = new CTask(pTask->GetSrcId(), &rReqTask);
    m_pTask->SetDilutType(EN_DilutType);
    m_prePrjName = "";//稀释任务不存在反应杯交叉污染
    app_print(ps_taskmng, "start Dil1 task-%d(cup-%d)\n", m_pTask->GetTaskId(), m_iLgcId);
    return true;
}

void CReactCup::AssignDilTwoTask(CTask* pTask)
{
	m_pDilTwoTask = pTask;
	m_pDilTwoTask->m_iDilutPeriod = 1;
	m_pDilTwoTask->SetDilutType(EN_ReactionType);
	app_print(ps_taskmng, "Assign Dil2 task-%d(cup-%d)\n", m_pDilTwoTask->GetTaskId(), m_iLgcId);
}

bool CReactCup::StartDilTwoTask()
{
	if (NULL != m_pDilTwoTask)
	{
		app_print(ps_taskmng, "start Dil2 task-%d(cup-%d)\n", m_pDilTwoTask->GetTaskId(), m_iLgcId);
		m_iPeriod = 1;
		m_iWashCount = MAX_WASH_NUM;
		m_pDilTwoTask->m_iDilutPeriod = 0;
		m_pTask = m_pDilTwoTask;
		m_pDilTwoTask = NULL;
        m_prePrjName = m_pTask->GetProjectName();
		return true;
	}
	return false;
}

bool CReactCup::CheckLongWash()
{
    if (m_pTask != NULL)
    {
        return m_pTask->CheckLongWash();
    }
    return false;
}

bool CReactCup::CanAssignTwoTask()
{
    return (m_pDilTwoTask == NULL && !m_bSkipCup);
}

CupState CReactCup::GetActStatus()
{
	if (m_bDirtyCup)
	{
		return CUP_STATE_DIRTY;
	}
	if (NULL == m_pTask)
	{
		if (NULL != m_pDilTwoTask)
		{
			return CUP_STATE_BUTT;
		}
		if (MAX_WASH_NUM == m_iWashCount)
		{
			return CUP_STATE_DIRTY;
		}
		return m_iWashCount > 0 ? CUP_STATE_WASH : CUP_STATE_FREE;
	}

	return m_pTask->GetActStatus(m_iPeriod);
}

int CReactCup::Update()
{
    m_iPhyId = (m_iPhyId - 42) % MAX_CUP_NUM;
    if (m_iPhyId <= 0)
    {
        m_iPhyId += MAX_CUP_NUM; //翻转
    }

	if (m_iPeriod > 0)
	{
		m_iPeriod += 1;
	}
	if (NULL != m_pDilTwoTask)
	{
		m_pDilTwoTask->m_iDilutPeriod += 1;
	}
    return m_iPhyId;
}

bool CReactCup::SetActResult(int iResult)
{
    if (iResult != EN_AF_Success)
    {
        WRITE_ERR_LOG("SetActResult Failed phyid=%d,result=%d\n", m_iPhyId, iResult);
        return false;
    }

    if (0 == m_iActCount)
    {
        return false;
    }
    m_iActCount -= 1;
    if (0 == m_iActCount && m_iWashCount > 0)
    {
        m_iWashCount -= 1;
    }
    return true;
}

void CReactCup::SuspendTask(map<app_u16, CTask*>& mSuspendTasks)
{
	if (NULL != m_pDilTwoTask)
	{
		int nPeriod = m_pDilTwoTask->m_iDilutPeriod;
		m_pDilTwoTask->m_iDilutPeriod = 0;
		mSuspendTasks[nPeriod] = m_pDilTwoTask;
		m_pDilTwoTask = NULL;
	}

	if (m_iPeriod < PERIOD_R1_36)
	{
		if (NULL != m_pTask&&EN_DilutType != m_pTask->GetDilutType())
		{
			if (EN_ReactionType == m_pTask->GetDilutType())
			{
				mSuspendTasks[m_iPeriod + DilStartPeriod] = m_pTask;
			}
			else
			{
				mSuspendTasks[m_iPeriod] = m_pTask;
			}
            app_print(ps_taskmng, "SuspendTask-%lu\n", m_pTask->GetTaskId());
			m_pTask = NULL;
		}
		Initialize(m_iWashCount);
	}
}

void CReactCup::SkipDirtyCup()
{
    m_bDirtyCup = true;
    m_iWashCount += MAX_WASH_NUM;
    if (NULL != m_pTask)
    {
        int nDilType = m_pTask->GetDilutType();
        m_pTask->SkipDirtyCup();
        m_pTask->SendTaskRes(TR_Skip, m_iLgcId);
        if (EN_DilutType == nDilType)
        {
            delete(m_pTask);
            m_pCupMng->SkipReactDirtyCup(m_iLgcId);
        }
        else
        {
            g_pTaskMng->AddTaskFront(m_pTask);
            if (EN_ReactionType == nDilType)
            {
                m_pCupMng->SkipDilDirtyCup(m_iLgcId);
            }
        }
        m_pTask = NULL;
    }
}

void CReactCup::SkipDilDirtyCup()
{
    m_bDirtyCup = true;
    m_iWashCount += MAX_WASH_NUM;
    if (NULL != m_pTask&&(EN_DilutType == m_pTask->GetDilutType()))
    {
        m_pTask->SkipDirtyCup();
        m_pTask->SendTaskRes(TR_Skip, m_iLgcId);
        delete(m_pTask);
        m_pTask = NULL;
    }
}

void CReactCup::SkipReactDirtyCup()
{
    if (NULL != m_pDilTwoTask)
    {
        m_pDilTwoTask->SendTaskRes(TR_Skip, m_iLgcId);
        g_pTaskMng->AddTaskFront(m_pDilTwoTask);
        m_pDilTwoTask = NULL;
    }
}

void CReactCup::GetStatus(STRctCupStatus * pStatus)
{
    pStatus->set_uirctcuppos(m_iLgcId);
    pStatus->set_encupstate(GetActStatus());

    if (NULL != m_pTask)
    {
        int iLeftPeriod = 166 - m_iPeriod;
        if (m_pTask->IsDoublePeriod())
        {
            iLeftPeriod = 331 - m_iPeriod;
        }
        pStatus->set_uircttimes(iLeftPeriod);
        m_pTask->GetCupStatus(pStatus);
        pStatus->set_uidiluttype(m_pTask->GetDilutType());
    }
	else if (NULL != m_pDilTwoTask)
	{
		int iLeftPeriod = 166;
		if (m_pDilTwoTask->IsDoublePeriod())
		{
			iLeftPeriod = 331;
		}
		pStatus->set_uircttimes(iLeftPeriod);
		m_pDilTwoTask->GetCupStatus(pStatus);
		pStatus->set_uidiluttype(m_pDilTwoTask->GetDilutType());
	}
}

void CReactCup::CancelTask(int iAlarmLevel, STNotifyFailedSmpTask& stPara)
{
	if (NULL != m_pDilTwoTask)
	{
		m_pDilTwoTask->CheckSmpTask(stPara);
		delete(m_pDilTwoTask);
		m_pDilTwoTask = NULL;
	}

    if (NULL == m_pTask)
    {
        return;
    }

    switch (iAlarmLevel)
    {
        //加R2停止
        case app_alarm_fault_addreagentstop_r2:
        {
            if ((m_pTask->NeedAddReag(R2) && m_iPeriod <= PERIOD_R2_97) || (m_pTask->NeedAddReag(R4) && m_iPeriod <= PERIOD_R4_262))
            {
                _cancelTask(stPara);
            }
            break;
        }
        //加样停止
        case app_alarm_fault_addsamplestop:
        {
            if (m_pTask->NeedAddSmp() && m_iPeriod <= PERIOD_Smp_47)
            {
                _cancelTask(stPara);
            }
            break;
        }
        //加R1停止
        case app_alarm_fault_addreagentstop_r1:
        {
            if ((m_pTask->NeedAddReag(R1) && m_iPeriod <= PERIOD_R1_36) || (m_pTask->NeedAddReag(R3) && m_iPeriod <= PERIOD_R3_201))
            {
                _cancelTask(stPara);
            }
            break;
        }
        //整机停止
        default:
            _cancelTask(stPara);
            break;
    }
}

bool CReactCup::CheckWash()
{
    if (m_iPhyId == EN_AC_Wash1 && m_iPeriod < MAX_CUP_NUM)
    {
        m_iPeriod = 1;//第一阶清洗时周期执成1
    }
    if (CanWash())
    {
        if (m_iWashCount > 0 && (MAX_WASH_NUM != m_iWashCount || EN_AC_Wash1 == m_iPhyId))//排除201-209的第一次清洗
        {
            return true;
        }
    }
    return false;
}

bool CReactCup::_initWashTask(STReqAddTask &rReq, int washType)
{
    rReq.set_uimaintantype(wash_task);
    rReq.set_strprojectname("D");

	vos_u32 faulttype = g_pAlarm->FindMaxLevelAlarm();

	STLiquidInfo *pReagInfo = rReq.add_streagents();
	STCupPos *pPos = NULL;
	if (faulttype < 1)
	{
        if (!g_pReagent->GetReagCup(ReagDisk_R1, g_washInfo[washType][3], "D2", ReType_R1))
        {
            return false;
        }
		pReagInfo->set_uiaddvol(g_washInfo[washType][2]);
		pPos = pReagInfo->mutable_stcuppos();
		pPos->set_uidskid(R1);
		pPos->set_uicupid(g_washInfo[washType][3]);
	}

	if (faulttype < 2)
	{
        if (!g_pReagent->CheckSmp(0, g_washInfo[washType][1]))
        {
            return false;
        }
		STLiquidInfo *pSmpInfo = rReq.mutable_stsmpinfo();
		pSmpInfo->set_uiaddvol(g_washInfo[washType][0]); 
		pPos = pSmpInfo->mutable_stcuppos();
		pPos->set_uidskid(1);
		pPos->set_uicupid(g_washInfo[washType][1]);
	}

	if (faulttype < 3)
	{
        if (!g_pReagent->GetReagCup(ReagDisk_R2, g_washInfo[washType][5], "D3", ReType_R2))
        {
            return false;
        }
		pReagInfo = rReq.add_streagents();
		pReagInfo->set_uiaddvol(g_washInfo[washType][4]);
		pPos = pReagInfo->mutable_stcuppos();
		pPos->set_uidskid(R2);
		pPos->set_uicupid(g_washInfo[washType][5]);
	}
    return true;
}

vos_u32 CReactCup::_sendSmpNdlAbsorb(int  actType)
{
    msg_stSampleSysAbsorb stPara = { 0 };
	
	stPara.enAbsrbType = ((m_pTask->GetSmpType() == WHOLE_BLOOD) && g_pOuterCupMng->GetBloodAbsorbMode()) ? 1 : 0;
    stPara.reportMarginFlag = 1;
    stPara.ulSampleVolume = m_pTask->GetSmpAddVol();
    m_pTask->CalcDiskCup(stPara.diskId, stPara.cupId);
    stPara.reactCup = m_iLgcId;
	return app_reqDwmCmd(SPid_Task, SPid_SmpNdl, actType, sizeof(msg_stSampleSysAbsorb), (char*)&stPara, APP_YES);
}

vos_u32 CReactCup::_sendSmpNdlDischarge()
{
    msg_stSampleSysDischarge stPara = { 0 };
    stPara.usDischPos = SmpPosDischNml;
    stPara.ulSampleVolume = m_pTask->GetSmpAddVol();
    m_iTotalVol += (app_u16)(stPara.ulSampleVolume);
    stPara.usTotalVol = m_iTotalVol; //杯总体积
    stPara.ulDlntVolume = 0;
    return app_reqDwmCmd(SPid_Task, SPid_SmpNdl, Act_Discharge, sizeof(msg_stSampleSysDischarge), (char*)&stPara, APP_YES);
}

vos_u32 CReactCup::_sendSmpNdlDischargeDilu()
{
    msg_stSampleSysDischarge stPara = { 0 };
	stPara.usDischPos = SmpPosDilute;
	stPara.ulSampleVolume = m_pTask->GetSmpAddVol();
	m_iTotalVol += (app_u16)(stPara.ulSampleVolume);
	stPara.usTotalVol = m_iTotalVol; //杯总体积
	stPara.ulDlntVolume = 0;
	return app_reqDwmCmd(SPid_Task, SPid_SmpNdl, Act_DischargeDilu, sizeof(msg_stSampleSysDischarge), (char*)&stPara, APP_YES);
}


vos_u32 CReactCup::SendSmpNdlRtt(int actType)
{
    m_iActCount = 1;
    msg_stNdlRtt stPara = { 0 };
    m_pTask->CalcDiskCup(stPara.usDstPos);
	return app_reqDwmCmd(SPid_Task, SPid_SmpNdl, actType, sizeof(msg_stNdlRtt), (char*)&stPara, APP_YES);
}

vos_u32 CReactCup::SendSmpNdlRttDilu()
{
	m_iActCount = 1;
	msg_stNdlRtt stPara;
	stPara.usDstPos = SmpPosDilute;
	return app_reqDwmCmd(SPid_Task, SPid_SmpNdl, Act_Move, sizeof(msg_stNdlRtt), (char*)&stPara, APP_YES);
}

vos_u32 CReactCup::_sendReagNdlAbsorb(int iDstPid)
{
    const STLiquidInfo& reagent = m_pTask->GetReagInfo(m_iPeriod);

    msg_stReagSysAbsorb stPara;
    stPara.usAbsrbPos = reagent.stcuppos().uidskid();
    stPara.enAbsrbType = 0;
    stPara.reportMarginFlag = 1;
    stPara.ulVolume = reagent.uiaddvol();
    stPara.diskId = reagent.stcuppos().uidskid();
    stPara.cupId = reagent.stcuppos().uicupid();
    stPara.reactCup = m_iLgcId;
    return app_reqDwmCmd(SPid_Task, iDstPid, Act_Absorb, sizeof(msg_stReagSysAbsorb), (char*)&stPara, APP_YES);
}

vos_u32 CReactCup::SendReagNdlDischarge(int iDstPid)
{
    const STLiquidInfo& reagent = m_pTask->GetReagInfo(m_iPeriod);

    msg_stReagSysDischarge stPara;
    stPara.usDischPos = app_reagent_ndl_stop_pos_discharg;
    stPara.ulReagVolume = reagent.uiaddvol();
    stPara.ulDlntVolume = 0;
    m_iTotalVol += stPara.ulReagVolume;
    stPara.usTotalVol = m_iTotalVol; //杯总体积

    return app_reqDwmCmd(SPid_Task, iDstPid, Act_Discharge, sizeof(msg_stReagSysDischarge), (char*)&stPara, APP_YES);
}
bool CReactCup::_checkLiquid(vos_u32& usR1CupId, vos_u32& usR2CupId, EN_ActFlag& dskFlag)
{
	STNotifyFailedSmpTask stPara;
	TaskResult TR_type = TR_Success;
	if (!m_pTask->CheckAllLiquid(m_iPeriod, usR1CupId, usR2CupId, TR_type))
	{
		dskFlag = EN_AF_Unstart;
		CancelTask(m_iLgcId, TR_type);
		return false;
	}

	return true;
}
void CReactCup::CancelTask(int iLgcId, vos_u32 ulResult)
{
    if (NULL != m_pTask)
    {
        if (EN_DilutType == m_pTask->GetDilutType())
        {
            ulResult = (ulResult == TR_AddR1Err) ? TR_ReagErr : ulResult;
            m_pCupMng->CancelDilTask(iLgcId, ulResult);
        }
        if (EN_ReactionType == m_pTask->GetDilutType())
        {
            m_pCupMng->CancelReactTask(iLgcId, ulResult);
        }
        g_pTaskMng->SendTaskResult(&m_pTask, ulResult, iLgcId);
        int iWashCount = m_iPeriod >= PERIOD_R1_36 ? MAX_WASH_NUM : m_iWashCount;
        Initialize(iWashCount);
    }
}

void CReactCup::CancelDilTwoTask(int iLgcId, vos_u32 ulResult)
{
    if (NULL != m_pDilTwoTask)
    {
        g_pTaskMng->SendTaskResult(&m_pDilTwoTask, ulResult, iLgcId);
    }
}

void CReactCup::CancelDilTask(int iLgcId, vos_u32 ulResult)
{
    if (NULL != m_pTask && m_iPeriod >= DilStartPeriod)
    {
        if (EN_DilutType == m_pTask->GetDilutType())
        {
            g_pTaskMng->SendTaskResult(&m_pTask, ulResult, iLgcId);
        }  
    }
}

void CReactCup::ProcSuspendTime(int& iMaxTime)
{
    if (m_iPeriod< PERIOD_R1_36 || m_iPeriod>PERIOD_R4_262 || m_pTask == NULL)
    {
        return;
    }
    if (m_pTask->NeedAddReag(R4))
    {
        int iPeriodNum = PERIOD_R4_262 - m_iPeriod + 1;
        iMaxTime = (iMaxTime > iPeriodNum) ? iMaxTime : iPeriodNum;
        return;
    }
    if (m_pTask->NeedAddReag(R3))
    {
        int iPeriodNum = PERIOD_R3_201 - m_iPeriod + 1;
        iMaxTime = (iMaxTime > iPeriodNum) ? iMaxTime : iPeriodNum;
        return;
    }

    if (m_pTask->NeedAddReag(R2))
    {
        int iPeriodNum = PERIOD_R2_97 - m_iPeriod + 1;
        iMaxTime = (iMaxTime > iPeriodNum) ? iMaxTime : iPeriodNum;
    }
}

void CReactCup::CheckNdlTimeOut(vos_u32 ulFaultId, vos_u32 ulComp)
{
    if (Two_Act_Num == m_iActCount)
    {
        g_pAlarm->GenerateAlarm(ulFaultId, ulComp, Act_Absorb);
    }
    else if (One_Act_Num == m_iActCount)
    {
        g_pAlarm->GenerateAlarm(ALarm_React_System, ulComp, Act_Discharge);
    }
}

void CReactCup::CheckResultCalc()
{
    if (NULL != m_pTask)
    {
        m_pTask->CheckResultCalc(m_iLgcId);
    }
}

bool CReactCup::CanWash()
{
    if (NULL != m_pTask && m_iPeriod > MAX_CUP_NUM)
    {
        app_print(ps_taskmng, "CanWash=false taskid=%d,lgcId=%d,PhyId=%d\n", m_pTask->GetTaskId(), m_iLgcId, m_iPhyId);
        return false;
    }
    return true;
}

vos_u32 CReactCup::SendSmpNdlExe()
{
	if (VOS_OK != _sendSmpNdlAbsorb(Act_Absorb))
	{
		WRITE_ERR_LOG("SendSmpNdlAbsorb failed");
		return vos_malloc_mem_err;
	}
	return _sendSmpNdlDischarge();
}

vos_u32 CReactCup::SendSmpNdlExeDilu()
{
	m_iActCount = 2;
	if (VOS_OK != _sendSmpNdlAbsorb(Act_AbsorbDilu))
	{
		WRITE_ERR_LOG("SendSmpNdlAbsorb failed");
		return vos_malloc_mem_err;
	}
	return _sendSmpNdlDischargeDilu();
}

bool CReactCup::NeedMoveSmpdsk()
{
    if (NULL == m_pTask)
    {
        return false;
    }
    if (PERIOD_SmpDsk_46 != m_iPeriod)
    {
        return false;
    }
    return m_pTask->NeedAddSmp();
}


bool CReactCup::NeedMoveSmpdskDilu()
{

	if (NULL == m_pTask)
	{
		return false;
	}

	if (PERIOD_SmpDsk_110 != m_iPeriod)
	{
		return false;
	}
	else
	{
		return m_pTask->NeedAddSmpDilu();
	}	
}



vos_u32 CReactCup::SendReagNdlExeAct(int iDstPId)
{
    m_iActCount = 2;
    if (VOS_OK != _sendReagNdlAbsorb(iDstPId))
    {
        WRITE_ERR_LOG("_sendReagNdlAbsorb failed\n");
        return vos_para_invalid_err;
    }
    return SendReagNdlDischarge(iDstPId);
}
vos_u32 CReactCup::SendInnerDskRtt()
{
    m_r2DiskCup = 0;
    vos_u32 usR1CupId = 0, usR2CupId = 0;
    if (_checkLiquid(usR1CupId, usR2CupId, g_pRscMng->enInnerDskFlag))
    {
        m_r2DiskCup = usR2CupId;
        g_pRscMng->enInnerDskFlag = EN_AF_Doing;
        act_stReagSysDskRtt stPara;
        stPara.usCupId = usR2CupId;
        stPara.usDstPos = app_reagent_dsk_stop_pos_r2_absrb;

        return app_reqDwmCmd(SPid_Task, SPid_R2Dsk, Act_Move, sizeof(act_stReagSysDskRtt), (char*)&stPara, APP_YES);
    }
    return VOS_OK;
}

vos_u32 CReactCup::SendOuterDskRtt()
{
    m_r1DiskCup = 0;
    vos_u32 usR1CupId = 0, usR2CupId = 0;
    if (_checkLiquid(usR1CupId, usR2CupId, g_pRscMng->enOuterDskFlag))
    {
        m_r1DiskCup = usR1CupId;
        g_pRscMng->enOuterDskFlag = EN_AF_Doing;
        act_stReagSysDskRtt stPara;
        stPara.usCupId = usR1CupId;
        stPara.usDstPos = app_reagent_dsk_stop_pos_r1_absrb;

        return app_reqDwmCmd(SPid_Task, SPid_R1Dsk, Act_Move, sizeof(act_stReagSysDskRtt), (char*)&stPara, APP_YES);
    }
    return VOS_OK;
}

vos_u32 CReactCup::SendSmpDskRtt(int actType)
{
    vos_u32 usR1CupId = 0, usR2CupId = 0;
    if (_checkLiquid(usR1CupId, usR2CupId, g_pRscMng->enSimpleDskFlag))
    {
        g_pRscMng->enSimpleDskFlag = EN_AF_Doing;
        msg_paraSampleDskRtt stPara;
        m_pTask->CalcDiskCup(stPara.usCircleId, stPara.usSmpCupId);
        return app_reqDwmCmd(SPid_Task, SPid_SmpDsk, actType, sizeof(msg_paraSampleDskRtt), (char*)&stPara, APP_YES);
    }
    return VOS_OK;
}
bool CReactCup::NeedMoveOuterDsk()
{
    if (NULL == m_pTask)
    {
        return false;
    }
    if (PERIOD_R1Dsk_35 == m_iPeriod)
    {
        return m_pTask->NeedAddReag(R1);
    }

    if (PERIOD_R3Dsk_200 == m_iPeriod)
    {
        return m_pTask->NeedAddReag(R3);
    }
    return false;
}

bool CReactCup::NeedMoveInnerDsk()
{
    if (NULL == m_pTask)
    {
        return false;
    }
    if (PERIOD_R2Dsk_96 == m_iPeriod)
    {
        return m_pTask->NeedAddReag(R2);
    }

    if (PERIOD_R4Dsk_361 == m_iPeriod)
    {
        return m_pTask->NeedAddReag(R4);
    }
    return false;
}

bool CReactCup::NeedSmpMixWash()
{
    if (NULL == m_pTask)
    {
        return false;
    }
    if (PERIOD_SmpMixWash_49 == m_iPeriod)
    {
        return m_pTask->NeedAddSmp();
    }

    if (PERIOD_R3MixWash_214 == m_iPeriod)
    {
        return m_pTask->NeedAddReag(R3);
    }
    return false;
}

bool CReactCup::NeedReagMixWash()
{
    if (NULL == m_pTask)
    {
        return false;
    }
    if (PERIOD_R2MixWash_98 == m_iPeriod)
    {
        return m_pTask->NeedAddReag(R2);
    }

    if (PERIOD_R4MixWash_263 == m_iPeriod)
    {
        return m_pTask->NeedAddReag(R4);
    }
    return false;
}

bool CReactCup::IsToDoublePeriod()
{
	if (NULL == m_pTask)
	{
		return false;
	}

	if (m_iPeriod > 0 && m_iPeriod <= PERIOD_END_210)
	{
		return m_pTask->IsDoublePeriod();
	}
	return false;
}

bool CReactCup::IsUnstartDilTask()
{
	return NULL != m_pDilTwoTask;
}

void CReactCup::_cancelTask(STNotifyFailedSmpTask& stPara)
{
    m_pTask->CheckSmpTask(stPara);
    int iWashCount = m_iPeriod >= PERIOD_R1_36 ? MAX_WASH_NUM : m_iWashCount;
    Initialize(iWashCount);    
}

bool CReactCup::IsDoublePeriod()
{
    if (NULL != m_pDilTwoTask)
    {
        return m_pDilTwoTask->IsDoublePeriod();
    }
    if (NULL != m_pTask)
    {
        return m_pTask->IsDoublePeriod();
    }
    return false;
}

int CReactCup::InitSimuCup(CSimuCup * pSimuCup)
{
    bool bNeedStart = false;
	if (NULL != m_pDilTwoTask)
    {
		bNeedStart = true;
    }
    pSimuCup->init(m_iLgcId, m_iPhyId, m_iPeriod, m_iWashCount, bNeedStart);
    return m_iPhyId;
}

void CReactCup::SetTask(CTask* pTask)
{
	m_pTask = pTask;
}

CSimuCup::CSimuCup()
    :m_iLgcId(0), m_iPhyId(0), m_iPeriod(0), m_iWashCount(0), m_bNeedStart(false)
{

}

CSimuCup::~CSimuCup()
{

}

int CSimuCup::Update()
{
    m_iPhyId = (m_iPhyId + 41) % MAX_CUP_NUM;
    if (0 == m_iPhyId)
    {
        m_iPhyId = MAX_CUP_NUM; //转到165号位置了
    }

    if (m_iPeriod > 0)
    {
        m_iPeriod += 1;
    }
    return m_iPhyId;
}

void CSimuCup::StartDilTask()
{
    if (m_bNeedStart)
    {
        m_iPeriod = 1;
        m_bNeedStart = false;
    }
}

void CSimuCup::ProcFinish(bool bDoublePeriod)
{
    if (0 == m_iPeriod)
    {
        return;
    }

    if (1 != (m_iPeriod%MAX_CUP_NUM))
    {
        return;
    }

    if ((MAX_CUP_NUM) == (m_iPeriod - 1) && bDoublePeriod)
    {
        return;
    }
    m_iPeriod = 0;
    m_iWashCount = MAX_WASH_NUM;
}

bool CSimuCup::ProcWash()
{
    bool bWash = false;
	if (m_iWashCount > 0)
	{
		--m_iWashCount;
        bWash = true;
	}
    return bWash;
}

void CSimuCup::init(int iLgcId, int iPhyId, int iPeriod, int iWashCount, bool bNeedStart)
{
    m_iLgcId = iLgcId;
    m_iPhyId = iPhyId;
    m_iPeriod = iPeriod;
    m_iWashCount = iWashCount;
    m_bNeedStart = bNeedStart;
}

bool CSimuCup::CanWash(bool bDoublePeriod)
{
	if (bDoublePeriod && m_iPeriod > MAX_CUP_NUM && m_iPeriod < (2 * MAX_CUP_NUM))
	{
		return false;
	}
	return true;
}

bool CSimuCup::IsOver()
{
    return 0 == m_iPeriod && 0 == m_iWashCount;
}
void CReactCup::ProcLiquid(TaskResult Trslt, vos_u16 dwnMachine)
{
	if (m_pTask == NULL || colormetry_task == m_pTask->GetMaintanType() ||
		pollution_task == m_pTask->GetMaintanType() || oam_task == m_pTask->GetMaintanType())
	{
		g_liquidFlag[dwnMachine] = 0;
		return;
	}
	if (g_liquidFlag[dwnMachine] == LiquidErr)
	{
		g_liquidFlag[dwnMachine] = 0;
        if (m_iPeriod >= 165)
        {
            Trslt = (Trslt == TR_AddR1Err) ? TR_AddR3Err
                : (Trslt == TR_AddR2Err) ? TR_AddR4Err : Trslt;
        }
		CancelTask(m_iLgcId, Trslt);
	}
}

void CReactCup::CheckSmp()
{
    vos_u32 usR1CupId = 0, usR2CupId = 0;
    EN_ActFlag dskFlag;
	if (m_pTask!=NULL)
	{
        _checkLiquid(usR1CupId, usR2CupId, dskFlag);
	}
}


bool CReactCup::NeedSmpNdlExe(msg_paraSampleDskRtt& stDskRtt, msg_stNdlRtt& stNdlRtt, msg_stSampleSysAbsorb& stNdllAbsb, msg_stSampleSysDischarge& stNdllDisg)
{
    //TODO,检查任务参数是否满足
    if (NULL == m_pTask || !m_pTask->NeedAddSmp())// || m_iPeriod != PERIOD_Smp_47)
    {
        return false;
    }
    vos_u32 usR1CupId = 0, usR2CupId = 0;
    if (!_checkLiquid(usR1CupId, usR2CupId, g_pRscMng->enSimpleDskFlag))
    {
        return false;
    }
    m_pTask->CalcDiskCup(stDskRtt.usCircleId, stDskRtt.usSmpCupId);
    m_pTask->GetSmpNdlPos(stNdlRtt.usDstPos);

    stNdllAbsb.enAbsrbType = ((m_pTask->GetSmpType() == WHOLE_BLOOD) && g_pOuterCupMng->GetBloodAbsorbMode()) ? 1 : 0;
    stNdllAbsb.reportMarginFlag = 1;
    stNdllAbsb.ulSampleVolume = m_pTask->GetSmpAddVol();
    m_pTask->CalcDiskCup(stNdllAbsb.diskId, stNdllAbsb.cupId);
    stNdllAbsb.reactCup = m_iLgcId;

    stNdllDisg.usDischPos = SNdlPosDischarg;
    stNdllDisg.ulSampleVolume = m_pTask->GetSmpAddVol();
    m_iTotalVol += (app_u16)(stNdllDisg.ulSampleVolume);
    stNdllDisg.usTotalVol = m_iTotalVol; //杯总体积
    stNdllDisg.ulDlntVolume = 0;

    return true;
}


bool CReactCup::NeedSmpNdlExeDilu(msg_paraSampleDskRtt& stDskRtt, msg_stNdlRtt& stNdlRtt, msg_stSampleSysAbsorb& stNdllAbsb, msg_stSampleSysDischarge& stNdllDisg, ReactCircle circle)
{
    //TODO,检查任务参数是否满足
    int period = (circle == Circle_Outer) ? PERIOD_DilutOuter
        : (circle == Circle_Inner) ? PERIOD_DilutInner : 0;
    if (NULL == m_pTask || !m_pTask->NeedAddSmpDilu())// || period != m_iPeriod)
    {
        return false;
    }
    // 	if (PERIOD_Smp_111 != m_iPeriod)
    // 	{
    // 		return false;
    // 	}
    vos_u32 usR1CupId = 0, usR2CupId = 0;
    if (!_checkLiquid(usR1CupId, usR2CupId, g_pRscMng->enSimpleDskFlag))
    {
        return false;
    }
    m_pTask->CalcDiskCup(stDskRtt.usCircleId, stDskRtt.usSmpCupId);
    m_pTask->GetSmpNdlPos(stNdlRtt.usDstPos);

    stNdllAbsb.enAbsrbType = ((m_pTask->GetSmpType() == WHOLE_BLOOD) && g_pOuterCupMng->GetBloodAbsorbMode()) ? 1 : 0;
    stNdllAbsb.reportMarginFlag = 1;
    stNdllAbsb.ulSampleVolume = m_pTask->GetSmpAddVol();
    m_pTask->CalcDiskCup(stNdllAbsb.diskId, stNdllAbsb.cupId);
    stNdllAbsb.reactCup = m_iLgcId;

    stNdllDisg.usDischPos = SNdlPosDilute;
    stNdllDisg.ulSampleVolume = m_pTask->GetSmpAddVol();
    m_iTotalVol += (app_u16)(stNdllDisg.ulSampleVolume);
    stNdllDisg.usTotalVol = m_iTotalVol; //杯总体积
    stNdllDisg.ulDlntVolume = 0;
    return true;
}


bool CReactCup::NeedR1NdlExe(act_stReagSysDskRtt& stDskRtt, msg_stNdlRtt& stNdlRtt, msg_stReagSysAbsorb& stNdllAbsb, msg_stReagSysDischarge& stNdllDisg)
{
    if (NULL == m_pTask || (!m_pTask->NeedAddReag(R1) && !m_pTask->NeedAddReag(R3)))
    {
        return false;
    }
    vos_u32 usR1CupId = 0, usR2CupId = 0;
    if (!_checkLiquid(usR1CupId, usR2CupId, g_pRscMng->enOuterDskFlag))
    {
        return false;
    }
    stDskRtt.usCupId = usR1CupId;
    stDskRtt.usDstPos = (m_pCupMng->GetCircle() == Circle_Outer) ? R1DskPosouter1 : R1DskPosInner1;
    stNdlRtt.usDstPos = RNdlPosAbsorb1;

    const STLiquidInfo& reagent = m_pTask->GetReagInfo(m_iPeriod);
    stNdllAbsb.enAbsrbType = 0;
    stNdllAbsb.reportMarginFlag = 1;
    stNdllAbsb.ulVolume = reagent.uiaddvol();
    stNdllAbsb.diskId = reagent.stcuppos().uidskid();
    stNdllAbsb.cupId = reagent.stcuppos().uicupid();
    stNdllAbsb.reactCup = m_iLgcId;

    stNdllDisg.usDischPos = RNdlPosDischarg;
    stNdllDisg.ulReagVolume = reagent.uiaddvol();
    stNdllDisg.ulDlntVolume = 0;
    m_iTotalVol += stNdllDisg.ulReagVolume;
    stNdllDisg.usTotalVol = m_iTotalVol; //杯总体积
    return true;
}

bool CReactCup::NeedR2NdlExe(act_stReagSysDskRtt& stRDskRtt, msg_stNdlRtt& stNdlRtt, msg_stReagSysAbsorb& stNdllAbsb, msg_stReagSysDischarge& stNdllDisg)
{
    //TODO,检查任务参数是否满足
    if (NULL == m_pTask || (!m_pTask->NeedAddReag(R2) && !m_pTask->NeedAddReag(R4)))
    {
        return false;
    }
    vos_u32 usR1CupId = 0, usR2CupId = 0;
    if (!_checkLiquid(usR1CupId, usR2CupId, g_pRscMng->enOuterDskFlag))
    {
        return false;
    }
    stRDskRtt.usCupId = usR2CupId;
    stRDskRtt.usDstPos = (m_pCupMng->GetCircle() == Circle_Outer) ? R2DskPosouter1 : R2DskPosInner1;
    stNdlRtt.usDstPos = RNdlPosAbsorb1;

    const STLiquidInfo& reagent = m_pTask->GetReagInfo(m_iPeriod);
    stNdllAbsb.enAbsrbType = 0;
    stNdllAbsb.reportMarginFlag = 1;
    stNdllAbsb.ulVolume = reagent.uiaddvol();
    stNdllAbsb.diskId = reagent.stcuppos().uidskid();
    stNdllAbsb.cupId = reagent.stcuppos().uicupid();
    stNdllAbsb.reactCup = m_iLgcId;

    stNdllDisg.usDischPos = RNdlPosDischarg;
    stNdllDisg.ulReagVolume = reagent.uiaddvol();
    stNdllDisg.ulDlntVolume = 0;
    m_iTotalVol += stNdllDisg.ulReagVolume;
    stNdllDisg.usTotalVol = m_iTotalVol; //杯总体积
    return true;
}

bool CReactCup::NeedSmpMix()
{
    if (NULL == m_pTask)
    {
        return false;
    }
    
    if (PERIOD_SmpMix_50 == m_iPeriod)
    {
        return m_pTask->NeedAddSmp();
    }

    if (PERIOD_R3Mix_215 == m_iPeriod)
    {
        return m_pTask->NeedAddReag(R3);
    }
    return false;
}

bool CReactCup::NeedReagMix()
{
    if (NULL == m_pTask)
    {
        return false;
    }
    if (PERIOD_R2Mix_99 == m_iPeriod)
    {
        return m_pTask->NeedAddReag(R2);
    }

    if (PERIOD_R4Mix_264 == m_iPeriod)
    {
        return m_pTask->NeedAddReag(R4);
    }
    return false;
}

void CReactCup::ProcAutoWash(int iResult)
{
    if (EN_AF_Success == iResult)
    {
        if ((m_iWashCount > 0) && (MAX_WASH_NUM != m_iWashCount || EN_AC_Wash1 == m_iPhyId))
        {
            m_iWashCount--;
        }
    }
    if (m_iLgcId == 1)
    {
        printf("m_iWashCount = %d\n", m_iWashCount);
    }
}
void CReactCup::ProcFinishTask()
{
    if (m_iLgcId == 1)
    {
        app_print(ps_taskmng, "cup 1 m_iPeriod=%d\n", m_iPeriod);
    }
    if (NULL == m_pTask)
    {
        m_iPeriod = 0;
        return;
    }

    if (1 != (m_iPeriod % MAX_CUP_NUM))
    {
        return;
    }

    if ((MAX_CUP_NUM) == (m_iPeriod - 1) && m_pTask->IsDoublePeriod())
    {
        return;
    }
    app_print(ps_taskmng, "--cup(%d),task(%d) (%d)is Finish--\n", m_iLgcId, m_pTask->GetTaskId(), m_iPeriod);
    g_pTaskMng->SendTaskResult(&m_pTask, TR_Success, m_iLgcId);
    Initialize(MAX_WASH_NUM, true);
    m_iPeriod = 0;
}

void CReactCup::ProcAdReady(vos_u32 auladval[WAVE_NUM], ReactCircle circle)
{
    if (g_pTaskMng->IsWashCup() && 3 == m_iWashCount)
    {
        vos_u32 ulLgcCupId = m_iLgcId;
        if (circle == Circle_Inner)
        {
            ulLgcCupId += 1000;
        }
        g_pTaskMng->SetAdValue(auladval, ulLgcCupId);
        return;
    }

    if ((APP_START_NML_READ_AD_PERIOD > m_iPeriod) && (APP_READ_WATER_GAP_PERIOD != m_iPeriod))
    {
        return;
    }

    if (APP_READ_WATER_GAP_PERIOD == m_iPeriod)
    {
        STCupADValue stNotify;
        stNotify.set_uicupid(m_iLgcId);

        for (int i = 0; i < WAVE_NUM; ++i)
        {
#ifdef COMPILER_IS_LINUX_GCC
            stNotify.add_uiadpoints(i * 1000);
#else
            stNotify.add_uiadpoints(auladval[i]);
#endif
        }
        sendUpMsg(CPid_Maintain, SPid_Task, MSG_NotifyTestingADValue, &stNotify);
        memcpy(m_cupBlank, auladval, sizeof(vos_u32)*WAVE_NUM);
        return;
    }

    if (NULL == m_pTask)
    {
        return;
    }
    //王晖俊 设置反应盘圈号
    //m_pTask->SetAnalyCircle(circle);
    //end
    if (APP_START_NML_READ_AD_PERIOD == m_iPeriod)
    {
        app_print(ps_ADval, "%s", (circle == Circle_Inner) ? "Inner" : "Outer");
        app_print(ps_ADval, "CUP[%2d]--", m_iLgcId);
        if (VOS_OK != m_pTask->SetAdValue(m_cupBlank, APP_READ_WATER_GAP_PERIOD))
        {
            g_pTaskMng->SendTaskResult(&m_pTask, TR_GetADErr, m_iLgcId);
            Initialize(MAX_WASH_NUM);
        }
    }
    app_print(ps_ADval, "%s", (circle == Circle_Inner) ? "Inner" : "Outer");
    app_print(ps_ADval, "CUP[%2d]--", m_iLgcId);
    if (VOS_OK != m_pTask->SetAdValue(auladval, m_iPeriod))
    {
        g_pTaskMng->SendTaskResult(&m_pTask, TR_GetADErr, m_iLgcId);
        Initialize(MAX_WASH_NUM);
    }
}