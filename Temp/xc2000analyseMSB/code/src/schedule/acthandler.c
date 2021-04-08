#include "acthandler.h"
#include "apputils.h"
#include "typedefine.pb.h"
#include "reactcupmng.h"
#include "taskmng.h"
#include "reagent.h"
#include "vos_log.h"
#include "alarm.h"
#include "actreshandler.h"
#include "autoload.h"

#define TestOver (0xff)
#define CIndex (m_ulActIndex)
#define PIndex (m_ulActIndex-1)

extern vos_u32 g_ultestmode;
CActHandler * g_pActHandler = NULL;
CActHandler::CActHandler() :m_iAlarmLevel(0), m_iStatusCycle(SS_Idle), m_bStop(false)
{
    m_ulTimerId = createTimer(SPid_Task, "ActHandler");

    m_ulTIDCycle = createTimer(SPid_Task, "TIDCycle");
    m_ulTIDReact = createTimer(SPid_Task, "TIDReact");
    m_ulTIDOuter = createTimer(SPid_Task, "TIDOuter");
    m_ulTIDInner = createTimer(SPid_Task, "TIDInner");
    m_Pri = 36;// 53;
    m_Pri1 = 9;// 12;
    m_Pri2 = 25;// 32;
    m_Pri3 = 23;// 32;
    m_protolen = 10;
    m_mSubAct[SPid_SmpDsk] = new CSubAct(SPid_SmpDsk);
    m_mSubAct[SPid_SmpNdl] = new CSubAct(SPid_SmpNdl);
    m_mSubAct[SPid_R2Dsk] = new CSubAct(SPid_R2Dsk);
    m_mSubAct[SPid_R1] = new CSubAct(SPid_R1);
    m_mSubAct[SPid_R1Dsk] = new CSubAct(SPid_R1Dsk);
    m_mSubAct[SPid_R2] = new CSubAct(SPid_R2);
    m_mSubAct[SPid_ReactDsk] = new CSubAct(SPid_ReactDsk);
    m_mSubAct[SPid_AutoWash] = new CSubAct(SPid_AutoWash);
    m_mSubAct[SPid_SMix] = new CSubAct(SPid_SMix);
    m_mSubAct[SPid_R2Mix] = new CSubAct(SPid_R2Mix);
    m_mSubAct[SPid_RTmpCtrl] = new CSubAct(SPid_RTmpCtrl);
    m_mSubAct[SPid_CoolReag] = new CSubAct(SPid_CoolReag);
    m_mSubAct[SPid_R1b] = new CSubAct(SPid_R1b);
    m_mSubAct[SPid_R2b] = new CSubAct(SPid_R2b);
    m_mSubAct[SPid_SmpNdlb] = new CSubAct(SPid_SmpNdlb);
    m_mSubAct[SPid_SmpEmer] = new CSubAct(SPid_SmpEmer);

    //startAct
    m_mActFunc[SPid_AutoWash][Act_Wash] = (ActFun)&CActHandler::_autoWash;
    m_mActFunc[SPid_R2Mix][Act_Mix] = (ActFun)&CActHandler::_singleAct;
    m_mActFunc[SPid_SMix][Act_Mix] = (ActFun)&CActHandler::_singleAct;
    m_mActFunc[SPid_R2Mix][Act_Move] = (ActFun)&CActHandler::_mixMove;
    m_mActFunc[SPid_SMix][Act_Move] = (ActFun)&CActHandler::_mixMove;

    //DiskAct
    m_mActFunc[SPid_ReactDsk][Act_Timer] = (ActFun)&CActHandler::_reactTimer;
    m_mActFunc[SPid_ReactDsk][Act_Move] = (ActFun)&CActHandler::_reactDskMove;
    m_mActFunc[SPid_R1Dsk][Act_MoveOuter] = (ActFun)&CActHandler::_reagDskMove;
    m_mActFunc[SPid_R1Dsk][Act_MoveInner] = (ActFun)&CActHandler::_reagDskMove;
    m_mActFunc[SPid_R2Dsk][Act_MoveOuter] = (ActFun)&CActHandler::_reagDskMove;
    m_mActFunc[SPid_R2Dsk][Act_MoveInner] = (ActFun)&CActHandler::_reagDskMove;
    m_mActFunc[SPid_SmpDsk][Act_MoveOuter] = (ActFun)&CActHandler::_smpDskMove;
    m_mActFunc[SPid_SmpDsk][Act_MoveInner] = (ActFun)&CActHandler::_smpDskMove;
    //InnerAct
    m_mActFunc[SPid_R1b][Act_Wash] = (ActFun)&CActHandler::_singleAct;
    m_mActFunc[SPid_R1b][Act_MoveAbs] = (ActFun)&CActHandler::_ndlMoveAbsb;
    m_mActFunc[SPid_R1b][Act_Absorb] = (ActFun)&CActHandler::_ndlAbsorb;
    m_mActFunc[SPid_R1b][Act_MoveDisg] = (ActFun)&CActHandler::_ndlMoveDisg;
    m_mActFunc[SPid_R1b][Act_Discharge] = (ActFun)&CActHandler::_ndlDischarge;
    m_mActFunc[SPid_R1b][Act_MoveW] = (ActFun)&CActHandler::_ndlMoveWash;
    m_mActFunc[SPid_R2b][Act_Wash] = (ActFun)&CActHandler::_singleAct;
    m_mActFunc[SPid_R2b][Act_MoveAbs] = (ActFun)&CActHandler::_ndlMoveAbsb;
    m_mActFunc[SPid_R2b][Act_Absorb] = (ActFun)&CActHandler::_ndlAbsorb;
    m_mActFunc[SPid_R2b][Act_MoveDisg] = (ActFun)&CActHandler::_ndlMoveDisg;
    m_mActFunc[SPid_R2b][Act_Discharge] = (ActFun)&CActHandler::_ndlDischarge;
    m_mActFunc[SPid_R2b][Act_MoveW] = (ActFun)&CActHandler::_ndlMoveWash;
    m_mActFunc[SPid_SmpNdlb][Act_Wash] = (ActFun)&CActHandler::_singleAct;
    m_mActFunc[SPid_SmpNdlb][Act_MoveAbs] = (ActFun)&CActHandler::_ndlMoveAbsb;
    m_mActFunc[SPid_SmpNdlb][Act_Absorb] = (ActFun)&CActHandler::_ndlAbsorb;
    m_mActFunc[SPid_SmpNdlb][Act_MoveDisg] = (ActFun)&CActHandler::_ndlMoveDisg;
    m_mActFunc[SPid_SmpNdlb][Act_Discharge] = (ActFun)&CActHandler::_ndlDischarge;
    m_mActFunc[SPid_SmpNdlb][Act_MoveW] = (ActFun)&CActHandler::_ndlMoveWash;
    //OuterAct
    m_mActFunc[SPid_R1][Act_Wash] = (ActFun)&CActHandler::_singleAct;
    m_mActFunc[SPid_R1][Act_MoveAbs] = (ActFun)&CActHandler::_ndlMoveAbsb;
    m_mActFunc[SPid_R1][Act_Absorb] = (ActFun)&CActHandler::_ndlAbsorb;
    m_mActFunc[SPid_R1][Act_MoveDisg] = (ActFun)&CActHandler::_ndlMoveDisg;
    m_mActFunc[SPid_R1][Act_Discharge] = (ActFun)&CActHandler::_ndlDischarge;
    m_mActFunc[SPid_R1][Act_MoveW] = (ActFun)&CActHandler::_ndlMoveWash;
    m_mActFunc[SPid_R2][Act_Wash] = (ActFun)&CActHandler::_singleAct;
    m_mActFunc[SPid_R2][Act_MoveAbs] = (ActFun)&CActHandler::_ndlMoveAbsb;
    m_mActFunc[SPid_R2][Act_Absorb] = (ActFun)&CActHandler::_ndlAbsorb;
    m_mActFunc[SPid_R2][Act_MoveDisg] = (ActFun)&CActHandler::_ndlMoveDisg;
    m_mActFunc[SPid_R2][Act_Discharge] = (ActFun)&CActHandler::_ndlDischarge;
    m_mActFunc[SPid_R2][Act_MoveW] = (ActFun)&CActHandler::_ndlMoveWash;
    m_mActFunc[SPid_SmpNdl][Act_Wash] = (ActFun)&CActHandler::_singleAct;
    m_mActFunc[SPid_SmpNdl][Act_MoveAbs] = (ActFun)&CActHandler::_ndlMoveAbsb;
    m_mActFunc[SPid_SmpNdl][Act_Absorb] = (ActFun)&CActHandler::_ndlAbsorb;
    m_mActFunc[SPid_SmpNdl][Act_MoveDisg] = (ActFun)&CActHandler::_ndlMoveDisg;
    m_mActFunc[SPid_SmpNdl][Act_Discharge] = (ActFun)&CActHandler::_ndlDischarge;
    m_mActFunc[SPid_SmpNdl][Act_MoveW] = (ActFun)&CActHandler::_ndlMoveWash;

    m_mResFuncs[SPid_AutoWash][Act_Wash] = (ProcResFuc)&CActHandler::_procAutoWash;
    m_mResFuncs[SPid_SmpNdl][Act_Absorb] = (ProcResFuc)&CActHandler::_procAbsorbS;
    m_mResFuncs[SPid_SmpNdlb][Act_Absorb] = (ProcResFuc)&CActHandler::_procAbsorbS;
    m_mResFuncs[SPid_SmpNdl][Act_Discharge] = (ProcResFuc)&CActHandler::_procDischarge;
    m_mResFuncs[SPid_SmpNdlb][Act_Discharge] = (ProcResFuc)&CActHandler::_procDischarge;
    m_mResFuncs[SPid_R1][Act_Discharge] = (ProcResFuc)&CActHandler::_procDischarge;
    m_mResFuncs[SPid_R1b][Act_Discharge] = (ProcResFuc)&CActHandler::_procDischarge;
    m_mResFuncs[SPid_R2][Act_Discharge] = (ProcResFuc)&CActHandler::_procDischarge;
    m_mResFuncs[SPid_R2b][Act_Discharge] = (ProcResFuc)&CActHandler::_procDischarge;
    m_mResFuncs[SPid_SmpNdl][Act_Wash] = (ProcResFuc)&CActHandler::_procNdlWash;
    m_mResFuncs[SPid_SmpNdlb][Act_Wash] = (ProcResFuc)&CActHandler::_procNdlWash;
    m_mResFuncs[SPid_R1][Act_Wash] = (ProcResFuc)&CActHandler::_procNdlWash;
    m_mResFuncs[SPid_R1b][Act_Wash] = (ProcResFuc)&CActHandler::_procNdlWash;
    m_mResFuncs[SPid_R2][Act_Wash] = (ProcResFuc)&CActHandler::_procNdlWash;
    m_mResFuncs[SPid_R2b][Act_Wash] = (ProcResFuc)&CActHandler::_procNdlWash;
#ifndef Version_A
    m_mResFuncs[SPid_SmpEmer][Act_GetIn] = (ProcResFuc)&CActHandler::_procGetIn;
    m_mResFuncs[SPid_SmpEmer][Act_Move] = (ProcResFuc)&CActHandler::_procMove;
    m_mResFuncs[SPid_SmpEmer][Act_FreeShelfIn] = (ProcResFuc)&CActHandler::_procFreeIn;
#endif
    m_mAlarmFunc = (ProcResFuc)&CActHandler::_procAlarm;
}

CActHandler::~CActHandler()
{

}

void CActHandler::OnNotifyState(STNotifState* pNotifState)
{
#ifdef Version_A
    if (pNotifState->ulloca() == EN_FWEmer && pNotifState->ulstate() == GState_Busy)
    {
        app_print(ps_taskmng, "OnNotifyState Get Emer Shelf-A (loca-%d) \n", pNotifState->ulloca());
        ::app_reqDwmCmd(SPid_Task, SPid_SmpEmer, Act_GetShelf, 0, NULL, APP_YES);
    }
    else if (pNotifState->ulloca() == EN_FWNorm && pNotifState->ulstate() == GState_Busy)
    {
        app_print(ps_taskmng, "OnNotifyState Get Norm Shelf-A (loca-%d) \n", pNotifState->ulloca());
        ::app_reqDwmCmd(SPid_Task, SPid_SmpDsk, Act_GetShelf, 0, NULL, APP_YES);
    }
#else
    act_stGetin stPara;
    if (pNotifState->ulloca() == EN_FWEmer && pNotifState->ulstate() == GState_Busy)
    {
        app_print(ps_taskmng, "OnNotifyState Get Emer Shelf-B (loca-%d) \n", pNotifState->ulloca());
        stPara.channel = 0;
        ::app_reqDwmCmd(SPid_Task, SPid_SmpDsk, Act_GetIn, sizeof(stPara), (char*)&stPara, APP_YES);
    }
    else if (pNotifState->ulloca() == EN_FWNorm && pNotifState->ulstate() == GState_Busy)
    {
        app_print(ps_taskmng, "OnNotifyState Get Norm Shelf-B (loca-%d) \n", pNotifState->ulloca());
        stPara.channel = 1;
        ::app_reqDwmCmd(SPid_Task, SPid_SmpEmer, Act_GetIn, sizeof(stPara), (char*)&stPara, APP_YES);
    }
#endif
}

void CActHandler::FreeShelf(vos_u32 taskType)
{
    if (m_bStop)
    {
        return;
    }
#ifdef Version_A
	STFreeShelf stPara;
    if (taskType == emergence_task && m_mState[AL_BackC]== GState_Idle)
    {
        //没有后缓存，需要等待后变轨准备好
        //app_reqDwmCmd(SPid_Task, SPid_SmpEmer, Act_FreeShelf, 0, NULL, APP_YES);
        m_mState[AL_Emer] = GState_Wait;
        stPara.set_loca(AL_Emer);
    }
    else if (m_mState[AL_BackW] == GState_Idle)
    {
        app_reqDwmCmd(SPid_Task, SPid_SmpDsk, Act_FreeShelf, 0, NULL, APP_YES);
        stPara.set_loca(AL_Norm);
    }
    app_print(ps_taskmng, "FreeShelf loca(%d) \n", stPara.loca());
    sendUpMsg(CPid_Relation, SPid_Task, MSG_FreeShelf, &stPara);
#else	
    STFreeShelf stParaShelf;
    if (taskType == emergence_task && m_mState[AL_BackC]== GState_Idle)
    {
        act_stGetin stPara;
        stPara.channel = 0;
        app_reqDwmCmd(SPid_Task, SPid_SmpDsk, Act_FreeShelf, sizeof(stPara), (char*)&stPara, APP_YES);
        m_mState[AL_Emer] = GState_Wait;
        stParaShelf.set_loca(AL_Emer);
    }
    else if (m_mState[AL_BackW] == GState_Idle)
    {
        act_stGetin stPara;
        stPara.channel = 1;
        app_reqDwmCmd(SPid_Task, SPid_SmpDsk, Act_FreeShelf, sizeof(stPara), (char*)&stPara, APP_YES);
        /*if (m_mState[AL_NormC] ==GState_Busy)
        {
            act_stGetin stPara;
            stPara.channel = 1;
            stPara.place = 1;
            ::app_reqDwmCmd(SPid_Task, SPid_SmpEmer, Act_Move, sizeof(stPara), (char*)&stPara, APP_YES);
        }*/
        stParaShelf.set_loca(AL_Norm);
    }
    app_print(ps_taskmng, "FreeShelf loca(%d) \n", stParaShelf.loca());
    sendUpMsg(CPid_Relation, SPid_Task, MSG_FreeShelf, &stParaShelf);

#ifdef COMPILER_IS_LINUX_GCC
	STNotifState stPara;
	stPara.set_enunit(Unit_A1);
	stPara.set_ulloca(AL_NormC);
	stPara.set_ulstate(GState_Idle);
	stPara.set_ulshelfnum(0);
	app_print(ps_reportData, "AL_NormC is Idle\n");
	sendUpMsg(CPid_Relation, Pid_ReportStatus, MSG_NotifyState, &stPara);
#endif // COMPILER_IS_LINUX_GCC

#endif
}

void CActHandler::Initialize(app_u32 washCount)
{
    vos_stoptimer(m_ulTIDCycle);
    vos_stoptimer(m_ulTIDOuter);
    vos_stoptimer(m_ulTIDInner);
  
    m_iStatusCycle = SS_Idle;
    m_ulActIndex = 0;
    m_iAlarmLevel = 0;

    //g_pRscMng->Initialize();
    g_pOuterCupMng->ResetAllCup();
    g_pInnerCupMng->ResetAllCup();
}

void CActHandler::ProcActStart(bool bSuspend)
{
    INITP04(SPid_Task, Press_our);
    INITP04(SPid_Task, Press_in);

    if (!_checkAlarm())
    {
        //申请下发任务
        g_pTaskMng->NotifySendTask(m_iAlarmLevel);//         g_pOuterCupMng->CheckSmp();
        g_pTaskMng->NotifySendTask(m_iAlarmLevel);//         g_pInnerCupMng->CheckSmp();
        if (!bSuspend)
        {
            g_pOuterCupMng->AssignNewTask();//分配新任务
            g_pInnerCupMng->AssignNewTask();//分配新任务
        }

        m_ulActIndex++;
        app_print(ps_rscsch, "\n     **ActStart Index(%d) ==**\n", m_ulActIndex);
        if (TestOver == _setStartAct())
        {
            return _procTestOver(bSuspend);
        }  

        if (VOS_OK != _setReactAct())
        {
            return;
        }

        if (VOS_OK != SendNextAct())
        {
            WRITE_ERR_LOG("Call SendNextAct fail(%d)");
            return ProcStopEvent(vos_midmachine_state_midmach_err);
        }

        StartCycleTimer(SS_Start, m_Pri);
        StartInnerTimer(SS_Start, m_Pri1);
        StartOuterTimer(SS_Start, m_Pri2);
    }
}

void CActHandler::ProcStopEvent(vos_u32 ulStatus)
{
    //清除子系统动作
    m_bStop = true;
    INITP04(SPid_Task, Press_our);
    INITP04(SPid_Task, Press_in);
    ResetAct();

    //清任务
    g_pOuterCupMng->CancelTask();
    g_pInnerCupMng->CancelTask();
    g_pTaskMng->Clear();

    //恢复样本
    g_pReagent->ClearUnSmp();

    Initialize(0);

    //上报反应杯状态
    g_pOuterCupMng->SendReactCupStatus();
    g_pInnerCupMng->SendReactCupStatus();
    app_reportmidmachstatus(ulStatus, SPid_Task);
}


void CActHandler::StartCycleTimer(Schedu_state_enum iStatus, vos_u32 ulTimeLen)
{
    m_iStatusCycle = iStatus;
    startTimer(m_ulTIDCycle, ulTimeLen);
}

void CActHandler::StartOuterTimer(Schedu_state_enum iStatus, vos_u32 ulTimeLen)
{
    m_iStatusOuter = iStatus;
    startTimer(m_ulTIDOuter, ulTimeLen);
}

void CActHandler::StartInnerTimer(Schedu_state_enum iStatus, vos_u32 ulTimeLen)
{
    m_iStatusInner = iStatus;
    startTimer(m_ulTIDInner, ulTimeLen);
}

void CActHandler::StopCycleTimer()
{
    vos_stoptimer(m_ulTIDCycle);
}

void CActHandler::OnTimeOut(vos_u32 ultimerid, bool suspend)
{
    if (m_ulTIDCycle == ultimerid)
    {
        _onCycleTimeOut(suspend);
    }
    else if (m_ulTIDInner == ultimerid)
    {
        _onInnerTimeOut();
    }
    else if (m_ulTIDReact == ultimerid)
    {
        _onReactTimeOut();
    }
    else if (m_ulTIDOuter == ultimerid)
    {
        _onOuterTimeOut();
    }
    else
    {
        WRITE_ERR_LOG("UnKnown TimerID(%d)", ultimerid);
    }
}

bool CActHandler::_checkAlarm()
{
    vos_u32 faulttype = g_pAlarm->FindMaxLevelAlarm();
    //-------------防重入----------------
    if (m_iAlarmLevel >= (int)faulttype)
    {
        return false;
    }
    m_iAlarmLevel = faulttype;
    WRITE_ERR_LOG("app_procFaultFlow faulttype=%lu\n", faulttype);

    //整机停止
    if (faulttype == app_alarm_fault_thetimingstop)
    {
        ProcStopEvent(vos_midmachine_state_midmach_err);
        return true;
    }
    g_pOuterCupMng->CancelTask(false, m_iAlarmLevel);
    g_pInnerCupMng->CancelTask(false, m_iAlarmLevel);
    return false;
}

void CActHandler::_procTestOver(bool bSuspend)
{
    printf("All tasks have been tested over and all cups used have been washed, now stop machine.\n");
    WRITE_INFO_LOG("All tasks have been tested over and all cups used have been washed, now stop machine.\n");

    if (g_pTaskMng->IsWashCup())
    {
        g_pTaskMng->SendWashResult();
    }
    g_pReagent->ClearUnSmp();//恢复样本

    if (!bSuspend)
    {
        g_pTaskMng->Clear();
        app_reportmidmachstatus(vos_midmachine_state_standby);
    }
    Initialize(0);
    g_pOuterCupMng->SendReactCupStatus();
    //王晖俊 新增的反应盘监控接口
    g_pInnerCupMng->SendReactCupStatus();
    //end
	sendUpMsg(CPid_Sound, SPid_Task, MSG_TestOver);
}

void CActHandler::_onCycleTimeOut(bool bSuspend)
{
    vos_stoptimer(m_ulTIDCycle);

    switch (m_iStatusCycle)
    {
    case SS_Reset:
        g_pActHandler->ProcStopEvent(vos_midmachine_state_reseted_err);
        break;

    case SS_Start:
        if (g_pOuterCupMng->m_bGetAd == false)//监控是否每个周期均上报AD中断
        {
            StartCycleTimer(SS_Protect, m_protolen);
            return;
        }
        if (g_pInnerCupMng->m_bGetAd == false)//监控是否每个周期均上报AD中断
        {
            StartCycleTimer(SS_Protect, m_protolen);
            return;
        }
        //TODO,超时保护机制
        g_pOuterCupMng->UpdateCups(bSuspend);
        g_pInnerCupMng->UpdateCups(bSuspend);
        g_pOuterCupMng->ProcFinishTask();
        g_pInnerCupMng->ProcFinishTask();
        g_pOuterCupMng->CheckResultCalc();
        g_pInnerCupMng->CheckResultCalc();
		ProcActStart(bSuspend);
        g_pOuterCupMng->SendReactCupStatus();
        g_pInnerCupMng->SendReactCupStatus();
        break;
    case SS_Protect:
        if (g_pOuterCupMng->m_bGetAd == false)//监控是否每个周期均上报AD中断
        {
            WRITE_ERR_LOG("OuterCup AD not be received!\n");
        }
        if (g_pInnerCupMng->m_bGetAd == false)//监控是否每个周期均上报AD中断
        {
            WRITE_ERR_LOG("InnerCup AD not be received!\n");
        }
        g_pOuterCupMng->UpdateCups(bSuspend);
        g_pInnerCupMng->UpdateCups(bSuspend);
        g_pOuterCupMng->ProcFinishTask();
        g_pInnerCupMng->ProcFinishTask();
        g_pOuterCupMng->CheckResultCalc();
        g_pInnerCupMng->CheckResultCalc();
        ProcActStart(bSuspend);
        g_pOuterCupMng->SendReactCupStatus();
        g_pInnerCupMng->SendReactCupStatus();
        break;
    default:
        break;
    }
}

void CActHandler::_onOuterTimeOut()
{
    app_print(ps_rscsch, "== OuterAct Index(%d) ==\n", m_ulActIndex);
    _setOuterAct();
    if (VOS_OK != SendNextAct())
    {
        WRITE_ERR_LOG("Call SendNextAct fail(%d)");
        ProcStopEvent(vos_midmachine_state_midmach_err);
    }
}

void CActHandler::_onReactTimeOut()
{
    m_mSubAct[SPid_ReactDsk]->SetDone(Act_Timer, true);
    SendNextAct();
}

void CActHandler::_onInnerTimeOut()
{
    app_print(ps_rscsch, "== InnerAct Index(%d) ==\n", m_ulActIndex);
    _setInnerAct();


    if (VOS_OK != SendNextAct())
    {
        WRITE_ERR_LOG("Call SendNextAct fail(%d)");
        ProcStopEvent(vos_midmachine_state_midmach_err);
    }
}

app_u32 CActHandler::_setStartAct()
{
    //TODO区分不能洗和不需要洗
    static bool bPreRMix = false;
    static bool bPreSMix = false;
    static bool bWashMix = true;
    bool bWash = g_pOuterCupMng->CheckAutoWash(m_usOuterBitMap);
    bWash |= g_pInnerCupMng->CheckAutoWash(m_usInnerBitMap);

    if (!bWash && g_pOuterCupMng->IsNeedOver() && g_pInnerCupMng->IsNeedOver())
    {
        printf("TestOver\n");
        return TestOver;
    }
    if (_checkAlarm() || !(m_mSubAct[SPid_AutoWash]->IsFinish() && m_mSubAct[SPid_R2Mix]->IsFinish() 
        && m_mSubAct[SPid_SMix]->IsFinish() && m_mSubAct[SPid_ReactDsk]->IsFinish()))
    {
        WRITE_ERR_LOG("***Set StartAct error***\n");
        ProcStopEvent(vos_midmachine_state_midmach_err);
        return 1;
    }

    bool bRMix = g_pInnerCupMng->NeedRMixAct() || g_pOuterCupMng->NeedRMixAct();
    bool bSMix = g_pInnerCupMng->NeedSMixAct() || g_pOuterCupMng->NeedSMixAct();

    if (bWash) InsertAct(SPid_AutoWash, Act_Wash, SPid_ReactDsk, Act_Move, PIndex);

    if (bRMix || bPreRMix) InsertAct(SPid_R2Mix, Act_Mix, SPid_ReactDsk, Act_Move, PIndex);
    if (bRMix || bPreRMix) InsertAct(SPid_R2Mix, Act_Move);
    if (bSMix || bPreSMix) InsertAct(SPid_SMix, Act_Mix, SPid_ReactDsk, Act_Move, PIndex);
    if (bSMix || bPreSMix) InsertAct(SPid_SMix, Act_Move);
    if (bWashMix && !(bRMix || bPreRMix) && !(bSMix || bPreSMix))//第一次预备清洗
    {
        InsertAct(SPid_R2Mix, Act_Mix, SPid_ReactDsk, Act_Move, PIndex);
        InsertAct(SPid_R2Mix, Act_Move);
        InsertAct(SPid_SMix, Act_Mix, SPid_ReactDsk, Act_Move, PIndex);
        InsertAct(SPid_SMix, Act_Move);
        bWashMix = false;
    }

    if (bWash) app_print(ps_taskmng, "bWash = %d,OuterCup(%#x),InnerCup(%#x)\n", bWash, m_usOuterBitMap, m_usInnerBitMap);
    if (bRMix) app_print(ps_taskmng, "bRMix = %d \n", bRMix);
    if (bSMix) app_print(ps_taskmng, "bSMix = %d \n", bSMix);

    bPreRMix = bRMix;
    bPreSMix = bSMix;
    return VOS_OK;
}

app_u32 CActHandler::_setInnerAct()
{
    if (_checkAlarm() || !(m_mSubAct[SPid_R2b]->IsFinish() && m_mSubAct[SPid_R1b]->IsFinish() && m_mSubAct[SPid_SmpNdlb]->IsFinish()))
    {
        if (m_iStatusInner==SS_Start)
        {
            StartInnerTimer(SS_Protect, m_protolen);
            vos_stoptimer(m_ulTIDReact);
            startTimer(m_ulTIDReact, m_protolen+m_Pri3-m_Pri1);
            vos_stoptimer(m_ulTIDOuter);
            StartOuterTimer(SS_Start, m_protolen + m_Pri2 - m_Pri1);
            StopCycleTimer();
            StartCycleTimer(SS_Start, m_protolen + m_Pri - m_Pri1);
            return 1;
        }
        if (m_iStatusInner==SS_Protect)
        {
            WRITE_ERR_LOG("***Set InnerAct error***\n");
            ProcStopEvent(vos_midmachine_state_midmach_err);
            return 1;
        }
       
    }
    //王晖俊 自动装载
    bool bR1AutoLoad = g_pInnerCupMng->NeedR1AutoLoad(m_stR1bDskRtt);
    bool bR2AutoLoad = g_pInnerCupMng->NeedR2AutoLoad(m_stR2bDskRtt);
    if (bR1AutoLoad)
    {
        InsertAct(SPid_R1Dsk, Act_MoveInner);
        g_pAutoLoad->startAutoLoad();
        InsertAct(SPid_R2b, Act_Wash);
        InsertAct(SPid_R1b, Act_Wash);
        InsertAct(SPid_SmpNdlb, Act_Wash);
        return VOS_OK;
    }
    if (bR2AutoLoad)
    {
        InsertAct(SPid_R2Dsk, Act_MoveInner);
        g_pAutoLoad->startAutoLoad();
        InsertAct(SPid_R2b, Act_Wash);
        InsertAct(SPid_R1b, Act_Wash);
        InsertAct(SPid_SmpNdlb, Act_Wash);
        return VOS_OK;
    }
    //end
    bool bSmpAct = g_pInnerCupMng->NeedSmpNdlAct(m_stSmpbDskRtt, m_stSmpbRtt, m_stSmpbAbsb, m_stSmpbDisg);
    bool bR1Act = g_pInnerCupMng->NeedR1NdlAct(m_stR1bDskRtt, m_stR1bRtt, m_stR1bAbsb, m_stR1bDisg);
    bool bR2Act = g_pInnerCupMng->NeedR2NdlAct(m_stR2bDskRtt, m_stR2bRtt, m_stR2bAbsb, m_stR2bDisg);

    InsertAct(SPid_R2b, Act_Wash);
    if (bR2Act&&!bR1AutoLoad&&!bR2AutoLoad)
    {
        
        InsertAct(SPid_R2b, Act_MoveAbs, SPid_R2, Act_Absorb, PIndex);      
        InsertAct(SPid_R2b, Act_Absorb, SPid_R2Dsk, Act_MoveInner, CIndex);
        InsertAct(SPid_R2b, Act_MoveDisg, SPid_R2, Act_Discharge, PIndex);
        InsertAct(SPid_R2b, Act_Discharge, SPid_ReactDsk, Act_Move, CIndex);//, SPid_R2b, Act_Timer, CIndex);
        InsertAct(SPid_R2b, Act_MoveW);
        InsertAct(SPid_R2Dsk, Act_MoveInner, SPid_R2, Act_Absorb, PIndex);
    }

    InsertAct(SPid_R1b, Act_Wash);
    if (bR1Act&&!bR1AutoLoad&&!bR2AutoLoad)
    {
        
        InsertAct(SPid_R1b, Act_MoveAbs, SPid_R1, Act_Absorb, PIndex);
        InsertAct(SPid_R1b, Act_Absorb, SPid_R1Dsk, Act_MoveInner, CIndex);
        InsertAct(SPid_R1b, Act_MoveDisg, SPid_R1, Act_Discharge, PIndex);
        InsertAct(SPid_R1b, Act_Discharge, SPid_ReactDsk, Act_Move, CIndex);// , SPid_R1b, Act_Timer, CIndex);
        InsertAct(SPid_R1b, Act_MoveW);
        InsertAct(SPid_R1Dsk, Act_MoveInner, SPid_R1, Act_Absorb, PIndex);
    }

    InsertAct(SPid_SmpNdlb, Act_Wash);
    if (bSmpAct)
    {
        
        InsertAct(SPid_SmpNdlb, Act_MoveAbs, SPid_SmpNdl, Act_Absorb, PIndex);
        InsertAct(SPid_SmpNdlb, Act_Absorb, SPid_SmpDsk, Act_MoveInner, CIndex);
        InsertAct(SPid_SmpNdlb, Act_MoveDisg, SPid_SmpNdl, Act_Discharge, PIndex);
        InsertAct(SPid_SmpNdlb, Act_Discharge, SPid_ReactDsk, Act_Move, CIndex);//, SPid_SmpNdlb, Act_Timer, CIndex);
        InsertAct(SPid_SmpNdlb, Act_MoveW);
        InsertAct(SPid_SmpDsk, Act_MoveInner, SPid_SmpNdl, Act_Absorb, PIndex);
    }

    if (bR1Act)  app_print(ps_taskmng, "Inner bR1Act =1 R1 Move to(%d), R1Dsk(%d)-(%d), aVol(%d),dVol(%d)\n",
        m_stR1bRtt.usDstPos, m_stR1bDskRtt.usCupId, m_stR1bDskRtt.usDstPos, m_stR1bAbsb.ulVolume, m_stR1bDisg.ulReagVolume);
    if (bSmpAct) app_print(ps_taskmng, "Inner bSmpAct=1 S  Move to(%d), Smp at(%d)(%d), aVol(%d),dVol(%d)\n",
        m_stSmpbRtt.usDstPos, m_stSmpbDskRtt.usCircleId, m_stSmpbDskRtt.usSmpCupId, m_stSmpbAbsb.ulSampleVolume, m_stSmpbDisg.ulSampleVolume);
    if (bR2Act)  app_print(ps_taskmng, "Inner bR2Act =1 R2 Move to(%d), R2Dsk(%d)-(%d), aVol(%d),dVol(%d)\n",
        m_stR2bRtt.usDstPos, m_stR2bDskRtt.usCupId, m_stR2bDskRtt.usDstPos, m_stR2bAbsb.ulVolume, m_stR2bDisg.ulReagVolume);
    return VOS_OK;
}

app_u32 CActHandler::_setReactAct()
{
    app_print(ps_rscsch, "== ReactAct Index(%d) ==\n", m_ulActIndex);
    if (_checkAlarm())
    {
        WRITE_ERR_LOG("***Set ReactAct error***\n");
        ProcStopEvent(vos_midmachine_state_midmach_err);
        return 1;
    }

    STSubActInfo stActInfo;
    stActInfo.set_bdone(false);
    stActInfo.set_uiacttype(Act_Move);

    STRAct* pRelyAct = stActInfo.add_strelyact();
    pRelyAct->set_uisubsys(SPid_SMix);
    pRelyAct->set_uiacttype(Act_Mix);
    pRelyAct->set_uiactindex(CIndex);
    pRelyAct = stActInfo.add_strelyact();
    pRelyAct->set_uisubsys(SPid_R2Mix);
    pRelyAct->set_uiacttype(Act_Mix);
    pRelyAct->set_uiactindex(CIndex);
    pRelyAct = stActInfo.add_strelyact();
    pRelyAct->set_uisubsys(SPid_AutoWash);
    pRelyAct->set_uiacttype(Act_Wash);
    pRelyAct->set_uiactindex(CIndex);

    pRelyAct = stActInfo.add_strelyact();
    pRelyAct->set_uisubsys(SPid_SmpNdl);
    pRelyAct->set_uiacttype(Act_Discharge);
    pRelyAct->set_uiactindex(PIndex);
    pRelyAct = stActInfo.add_strelyact();
    pRelyAct->set_uisubsys(SPid_SmpNdlb);
    pRelyAct->set_uiacttype(Act_Discharge);
    pRelyAct->set_uiactindex(PIndex);

    pRelyAct = stActInfo.add_strelyact();
    pRelyAct->set_uisubsys(SPid_R1);
    pRelyAct->set_uiacttype(Act_Discharge);
    pRelyAct->set_uiactindex(PIndex);
    pRelyAct = stActInfo.add_strelyact();
    pRelyAct->set_uisubsys(SPid_R1b);
    pRelyAct->set_uiacttype(Act_Discharge);
    pRelyAct->set_uiactindex(PIndex);

    pRelyAct = stActInfo.add_strelyact();
    pRelyAct->set_uisubsys(SPid_R2);
    pRelyAct->set_uiacttype(Act_Discharge);
    pRelyAct->set_uiactindex(PIndex);
    pRelyAct = stActInfo.add_strelyact();
    pRelyAct->set_uisubsys(SPid_R2b);
    pRelyAct->set_uiacttype(Act_Discharge);
    pRelyAct->set_uiactindex(PIndex);

    pRelyAct = stActInfo.add_strelyact();
    pRelyAct->set_uisubsys(SPid_ReactDsk);
    pRelyAct->set_uiacttype(Act_Timer);
    pRelyAct->set_uiactindex(CIndex);

    InsertAct(SPid_ReactDsk, Act_Timer);
    InsertActST(SPid_ReactDsk, stActInfo);
    app_print(ps_taskmng, "ReactDsk Move =1\n");
    return VOS_OK;
}

app_u32 CActHandler::_setOuterAct()
{
    if (_checkAlarm() || !m_mSubAct[SPid_R2]->IsFinish() || !m_mSubAct[SPid_R1]->IsFinish()
        || !m_mSubAct[SPid_SmpNdl]->IsFinish() || !m_mSubAct[SPid_R2Dsk]->IsFinish())
    {
        if (m_iStatusOuter == SS_Start)
        {
            StartOuterTimer(SS_Protect, m_protolen);
            StopCycleTimer();
            StartCycleTimer(SS_Start, m_protolen + m_Pri-m_Pri2);
            return 1;
        }
        if (m_iStatusOuter==SS_Protect)
        {
            WRITE_ERR_LOG("***Set OuterAct error***\n");
            ProcStopEvent(vos_midmachine_state_midmach_err);
            return 1;
        }
    }
    //王晖俊 自动装载
    bool bR1AutoLoad = g_pOuterCupMng->NeedR1AutoLoad(m_stR1DskRtt);
    bool bR2AutoLoad = g_pOuterCupMng->NeedR2AutoLoad(m_stR2DskRtt);
    if (bR1AutoLoad)
    {
        InsertAct(SPid_R1Dsk, Act_MoveInner);
        g_pAutoLoad->startAutoLoad();
    }
    if (bR2AutoLoad)
    {
        InsertAct(SPid_R2Dsk, Act_MoveInner);
        g_pAutoLoad->startAutoLoad();
    }
    //end
    bool bSmpAct = g_pOuterCupMng->NeedSmpNdlAct(m_stSmpDskRtt, m_stSmpRtt, m_stSmpAbsb, m_stSmpDisg);
    bool bR1Act = g_pOuterCupMng->NeedR1NdlAct(m_stR1DskRtt, m_stR1Rtt, m_stR1Absb, m_stR1Disg);
    bool bR2Act = g_pOuterCupMng->NeedR2NdlAct(m_stR2DskRtt, m_stR2Rtt, m_stR2Absb, m_stR2Disg);

    InsertAct(SPid_R2, Act_Wash);
    if (bR2Act&&!bR1AutoLoad&&!bR2AutoLoad)
    {
        
        InsertAct(SPid_R2, Act_MoveAbs, SPid_R2b, Act_Absorb, CIndex);
        InsertAct(SPid_R2, Act_Absorb, SPid_R2Dsk, Act_MoveOuter, CIndex);
        InsertAct(SPid_R2, Act_MoveDisg, SPid_R2b, Act_Discharge, CIndex);
        InsertAct(SPid_R2, Act_Discharge, SPid_ReactDsk, Act_Move, CIndex);
        InsertAct(SPid_R2, Act_MoveW);
        InsertAct(SPid_R2Dsk, Act_MoveOuter, SPid_R2b, Act_Absorb, CIndex);
    }

    InsertAct(SPid_R1, Act_Wash);
    if (bR1Act&&!bR1AutoLoad&&!bR2AutoLoad)
    {
        
        InsertAct(SPid_R1, Act_MoveAbs, SPid_R1b, Act_Absorb, CIndex);
        InsertAct(SPid_R1, Act_Absorb, SPid_R1Dsk, Act_MoveOuter, CIndex);
        InsertAct(SPid_R1, Act_MoveDisg, SPid_R1b, Act_Discharge, CIndex);
        InsertAct(SPid_R1, Act_Discharge, SPid_ReactDsk, Act_Move, CIndex);
        InsertAct(SPid_R1, Act_MoveW);
        InsertAct(SPid_R1Dsk, Act_MoveOuter, SPid_R1b, Act_Absorb, CIndex);
    }

    InsertAct(SPid_SmpNdl, Act_Wash);
    if (bSmpAct)
    {
        
        InsertAct(SPid_SmpNdl, Act_MoveAbs, SPid_SmpNdlb, Act_Absorb, CIndex);
        InsertAct(SPid_SmpNdl, Act_Absorb, SPid_SmpDsk, Act_MoveOuter, CIndex);
        InsertAct(SPid_SmpNdl, Act_MoveDisg, SPid_SmpNdlb, Act_Discharge, CIndex);
        InsertAct(SPid_SmpNdl, Act_Discharge, SPid_ReactDsk, Act_Move, CIndex);
        InsertAct(SPid_SmpNdl, Act_MoveW);
        InsertAct(SPid_SmpDsk, Act_MoveOuter, SPid_SmpNdlb, Act_Absorb, CIndex);
    }

    if (bR1Act)  app_print(ps_taskmng, "Outer bR1Act =1 R1 Move to(%d), R1Dsk(%d)-(%d), aVol(%d),dVol(%d)\n",
        m_stR1Rtt.usDstPos, m_stR1DskRtt.usCupId, m_stR1DskRtt.usDstPos, m_stR1Absb.ulVolume, m_stR1Disg.ulReagVolume);
    if (bSmpAct) app_print(ps_taskmng, "Outer bSmpAct=1 S  Move to(%d), Smp at(%d)(%d), aVol(%d),dVol(%d)\n",
        m_stSmpRtt.usDstPos, m_stSmpDskRtt.usCircleId, m_stSmpDskRtt.usSmpCupId, m_stSmpAbsb.ulSampleVolume, m_stSmpDisg.ulSampleVolume);
    if (bR2Act)  app_print(ps_taskmng, "Outer bR2Act =1 R2 Move to(%d), R2Dsk(%d)-(%d), aVol(%d),dVol(%d)\n",
        m_stR2Rtt.usDstPos, m_stR2DskRtt.usCupId, m_stR2DskRtt.usDstPos, m_stR2Absb.ulVolume, m_stR2Disg.ulReagVolume);
    return VOS_OK;
}

app_u32 CActHandler::_reactTimer(app_u32 subSys, app_u16 actType)
{
    return startTimer(m_ulTIDReact, m_Pri3);//m_ulTLenReact);//25,2500ms后开始转盘，转动时间预计1100ms
}

app_u32 CActHandler::_singleAct(app_u32 subSys, app_u16 actType)
{
    if (actType == Act_Wash)
    {
        OPEN_P04(subSys);
    }
    return app_reqDwmCmd(SPid_Task, subSys, actType, 0, NULL, APP_YES);
}

app_u32 CActHandler::_autoWash(app_u32 subSys, app_u16 actType)
{
    act_stReactSysAutoWash stPara;
    stPara.usWashOutBitMap = m_usOuterBitMap;
    stPara.usWashInBitMap2 = m_usInnerBitMap;
    return app_reqDwmCmd(SPid_Task, SPid_AutoWash, Act_Wash, sizeof(act_stReactSysAutoWash), (char*)&stPara, APP_YES);
}

app_u32 CActHandler::_reactDskMove(app_u32 subSys, app_u16 actType)
{
    g_pOuterCupMng->m_bUpdateCup = false;
    g_pInnerCupMng->m_bUpdateCup = false;
    g_pOuterCupMng->m_bGetAd = false;
    g_pInnerCupMng->m_bGetAd = false;
    //控制AD采集
    g_ultestmode = TESTMODE_NORMAL;
    act_stReadAD para;
    para.AdCount = APP_RTT_RCTCUP_PER_PERIOD;
    ::app_reqDwmCmd(SPid_Task, SPid_ADOuter, Act_Ad, sizeof(para), (char*)&para, APP_NO);
    ::app_reqDwmCmd(SPid_Task, SPid_ADInner, Act_Ad, sizeof(para), (char*)&para, APP_NO);

    //转盘
    act_stReactsysDskRtt stPara;
    stPara.usRttedCupNum = APP_RTT_RCTCUP_PER_PERIOD;
    stPara.dir = cmd_motor_dir_CLOCKWISE;
    return app_reqDwmCmd(SPid_Task, SPid_ReactDsk, Act_Move, sizeof(act_stReactsysDskRtt), (char*)&stPara, APP_YES);
}

app_u32 CActHandler::_reagDskMove(app_u32 subSys, app_u16 actType)
{
    act_stReagSysDskRtt* stPara = (subSys == SPid_R1Dsk && actType == Act_MoveOuter) ? &m_stR1DskRtt
        : (subSys == SPid_R1Dsk && actType == Act_MoveInner) ? &m_stR1bDskRtt
        : (subSys == SPid_R2Dsk && actType == Act_MoveOuter) ? &m_stR2DskRtt
        : (subSys == SPid_R2Dsk && actType == Act_MoveInner) ? &m_stR2bDskRtt : NULL;
    return app_reqDwmCmd(SPid_Task, subSys, actType, sizeof(act_stReagSysDskRtt), (char*)stPara, APP_YES);
}

app_u32 CActHandler::_smpDskMove(app_u32 subSys, app_u16 actType)
{
    msg_paraSampleDskRtt* stPara = (actType == Act_MoveOuter) ? &m_stSmpDskRtt
        : (actType == Act_MoveInner) ? &m_stSmpbDskRtt : NULL;
    return app_reqDwmCmd(SPid_Task, subSys, actType, sizeof(msg_paraSampleDskRtt), (char*)stPara, APP_YES);
}

app_u32 CActHandler::_ndlAbsorb(app_u32 subSys, app_u16 actType)
{
    if ((subSys == SPid_SmpNdl) || (subSys == SPid_SmpNdlb))
    {
        msg_stSampleSysAbsorb* stPara = (subSys == SPid_SmpNdl) ? &m_stSmpAbsb
            : (subSys == SPid_SmpNdlb) ? &m_stSmpbAbsb : NULL;
        return app_reqDwmCmd(SPid_Task, subSys, actType, sizeof(msg_stSampleSysAbsorb), (char*)stPara, APP_YES);
    }

    msg_stReagSysAbsorb* stPara = (subSys == SPid_R1) ? &m_stR1Absb
        : (subSys == SPid_R1b) ? &m_stR1bAbsb
        : (subSys == SPid_R2) ? &m_stR2Absb
        : (subSys == SPid_R2b) ? &m_stR2bAbsb : NULL;
    return app_reqDwmCmd(SPid_Task, subSys, actType, sizeof(msg_stReagSysAbsorb), (char*)stPara, APP_YES);
}

app_u32 CActHandler::_ndlDischarge(app_u32 subSys, app_u16 actType)
{
    if ((subSys == SPid_SmpNdl) || (subSys == SPid_SmpNdlb))
    {
        msg_stSampleSysDischarge* stPara = (subSys == SPid_SmpNdl) ? &m_stSmpDisg
            : (subSys == SPid_SmpNdlb) ? &m_stSmpbDisg : NULL;
        return app_reqDwmCmd(SPid_Task, subSys, actType, sizeof(msg_stSampleSysDischarge), (char*)stPara, APP_YES);
    }

    msg_stReagSysDischarge* stPara = (subSys == SPid_R1) ? &m_stR1Disg
        : (subSys == SPid_R1b) ? &m_stR1bDisg
        : (subSys == SPid_R2) ? &m_stR2Disg
        : (subSys == SPid_R2b) ? &m_stR2bDisg : NULL;
    return app_reqDwmCmd(SPid_Task, subSys, actType, sizeof(msg_stReagSysDischarge), (char*)stPara, APP_YES);
}

app_u32 CActHandler::_ndlMoveAbsb(app_u32 subSys, app_u16 actType)
{
    msg_stNdlRtt* stPara = (subSys == SPid_R1) ? &m_stR1Rtt
        : (subSys == SPid_R1b) ? &m_stR1bRtt
        : (subSys == SPid_SmpNdl) ? &m_stSmpRtt
        : (subSys == SPid_SmpNdlb) ? &m_stSmpbRtt
        : (subSys == SPid_R2) ? &m_stR2Rtt
        : (subSys == SPid_R2b) ? &m_stR2bRtt : NULL;
    return app_reqDwmCmd(SPid_Task, subSys, actType, sizeof(msg_stNdlRtt), (char*)stPara, APP_YES);
}

app_u32 CActHandler::_ndlMoveDisg(app_u32 subSys, app_u16 actType)
{
    msg_stNdlRtt stPara;
    stPara.usDstPos = (subSys == SPid_R1) ? m_stR1Disg.usDischPos
        : (subSys == SPid_R1b) ? m_stR1bDisg.usDischPos
        : (subSys == SPid_SmpNdl) ? m_stSmpDisg.usDischPos
        : (subSys == SPid_SmpNdlb) ? m_stSmpbDisg.usDischPos
        : (subSys == SPid_R2) ? m_stR2Disg.usDischPos
        : (subSys == SPid_R2b) ? m_stR2bDisg.usDischPos : 0;
    return app_reqDwmCmd(SPid_Task, subSys, actType, sizeof(msg_stNdlRtt), (char*)&stPara, APP_YES);
}

app_u32 CActHandler::_ndlMoveWash(app_u32 subSys, app_u16 actType)
{
    msg_stNdlRtt stPara;
    stPara.usDstPos = (subSys == SPid_R1) ? RNdlPosReset
        : (subSys == SPid_R1b) ? RNdlPosReset
        : (subSys == SPid_SmpNdl) ? SNdlPosReset
        : (subSys == SPid_SmpNdlb) ? SNdlPosReset
        : (subSys == SPid_R2) ? RNdlPosReset
        : (subSys == SPid_R2b) ? RNdlPosReset : 0;
    return app_reqDwmCmd(SPid_Task, subSys, actType, sizeof(msg_stNdlRtt), (char*)&stPara, APP_YES);
}

app_u32 CActHandler::_mixMove(app_u32 subSys, app_u16 actType)
{
    msg_stNdlRtt stPara = { 0 };
    return app_reqDwmCmd(SPid_Task, subSys, actType, sizeof(stPara), (char*)&stPara, APP_YES);
}


void CActHandler::_procAutoWash(app_u32 subSys, app_com_dev_act_result_stru* pRes)
{
    if (EN_AF_Success == pRes->enactrslt)
    {
        g_pOuterCupMng->ProcAutoWash();
        g_pInnerCupMng->ProcAutoWash();
    }
}

void CActHandler::_procAbsorbS(app_u32 subSys, app_com_dev_act_result_stru* pRes)
{
    if (subSys == SPid_SmpNdl)
    {
        g_pTaskMng->DeleteShelfTask(g_pOuterCupMng->GetTask(m_stSmpAbsb.reactCup));
    }
    else if (subSys == SPid_SmpNdlb)
    {
        g_pTaskMng->DeleteShelfTask(g_pInnerCupMng->GetTask(m_stSmpbAbsb.reactCup));
    }
}
void CActHandler::_procDischarge(app_u32 subSys, app_com_dev_act_result_stru* pRes)
{
    //OPEN_P04(subSys);
}

void CActHandler::_procNdlWash(app_u32 subSys, app_com_dev_act_result_stru* pRes)
{
    CLOSE_P04(subSys);
}

#ifndef Version_A
void CActHandler::_procGetIn(app_u32 subSys, app_com_dev_act_result_stru* pRes)
{
    act_stGetin stPara;
    stPara.channel = 1; 
    stPara.place = 1;
    //王晖俊 修改适应C版定位需求
    //::app_reqDwmCmd(SPid_Task, SPid_SmpEmer, Act_FreeShelfIn, sizeof(stPara), (char*)&stPara, APP_YES);
    //if (m_mState[AL_Norm] != GState_Busy)
    //{
        ::app_reqDwmCmd(SPid_Task, SPid_SmpEmer, Act_Move, sizeof(stPara), (char*)&stPara, APP_YES);
    //}
    //m_mState[AL_NormC] = GState_Busy;
    //end
    //app_reqDwmCmd(SPid_Task, SPid_SmpDsk, Act_GetShelf, sizeof(stPara), (char*)&stPara, APP_YES);
}
void CActHandler::_procMove(app_u32 subsys, app_com_dev_act_result_stru* pRes)
{
    act_stGetin stPara;
    stPara.channel = 1;
    ::app_reqDwmCmd(SPid_Task, SPid_SmpEmer, Act_FreeShelfIn, sizeof(stPara), (char*)&stPara, APP_YES);
    m_mState[AL_NormC] = GState_Idle;
}
void CActHandler::_procFreeIn(app_u32 subSys, app_com_dev_act_result_stru* pRes)
{
    act_stGetin stPara;
    stPara.channel = 1;
    stPara.place = 1;
    //app_reqDwmCmd(SPid_Task, SPid_SmpEmer, Act_FreeShelfIn, sizeof(stPara), (char*)&stPara, APP_YES);
    ::app_reqDwmCmd(SPid_Task, SPid_SmpDsk, Act_GetShelf, sizeof(stPara), (char*)&stPara, APP_YES);
}

#endif
void CActHandler::_procAlarm(app_u32 subSys, app_com_dev_act_result_stru* pRes)
{
    ProcStopEvent(vos_midmachine_state_midmach_err);//TODO异常处理待完善
    vos_u32 ulAlarmId = ALarm_React_System;
    vos_u32 ulActType = pRes->enacttype;
    if ((subSys == SPid_R1 || subSys == SPid_R1b || subSys == SPid_R1Dsk) && (ulActType != Act_Discharge))
    {
        ResetAct(SPid_R1);
        ResetAct(SPid_R1b);
        ResetAct(SPid_R1Dsk);
        ulAlarmId = ALarm_R1_System;
    }

    if ((subSys == SPid_SmpNdl || subSys == SPid_SmpNdlb || subSys == SPid_SmpDsk) && (ulActType != Act_Discharge))
    {
        ResetAct(SPid_SmpNdl);
        ResetAct(SPid_SmpNdlb);
        ResetAct(SPid_SmpDsk);
        ulAlarmId = ALarm_Smp_System;
    }

    if ((subSys == SPid_R2 || subSys == SPid_R2b || subSys == SPid_R2Dsk) && (ulActType != Act_Discharge))
    {
        ResetAct(SPid_R2);
        ResetAct(SPid_R2b);
        ResetAct(SPid_R2Dsk);
        ulAlarmId = ALarm_R2_System;
    }
    //临时取消上传中位机报警
    //g_pAlarm->GenerateAlarm(ulAlarmId, subSys - 900, ulActType);  
}

#define  old
void CActHandler::ProcPreEvent(bool bSuspend)
{
    WRITE_INFO_LOG("+++++++ProcPreEvent++++++++++\n");

    if (!_checkAlarm())
    {
        //更新系统状态
        g_pOuterCupMng->UpdateCups(bSuspend);

#ifdef COMPILER_IS_ARM_LINUX_GCC
        //每次反应盘转动前设置PDA采样
        app_adfpgapowerswitch(app_ad_fpga_power_mode_on); //打开采样开关

//         if (g_pOuterCupMng->m_bUpdateCup == true)//监控是否每个周期均上报AD中断
//         {
//             WRITE_ERR_LOG("Last AD not be get!\n");
//         }
//         else
//         {
//             g_pOuterCupMng->m_bUpdateCup = true;
//         }

        g_ultestmode = TESTMODE_NORMAL;
#endif

        if (VOS_OK == _procPreAct())
        {
            StartCycleTimer(SS_Start, g_pRscMng->m_ulPreTimeLen);
        }
        else
        {
            WRITE_ERR_LOG("_procPreAct fail(%d)\n");
            ProcStopEvent(vos_midmachine_state_midmach_err);
        }
    }
    g_pOuterCupMng->CheckResultCalc();
}

void CActHandler::ProcExeEvent(bool bSuspend)
{
    WRITE_INFO_LOG("+++++++ProcExeEvent++++++++++\n");
    if (!_checkAlarm())
    {
        INITP04(SPid_Task, Press_our);
        INITP04(SPid_Task, Press_in);
        g_pOuterCupMng->CheckSmp();
        //执行动作
        vos_u32 ulrunrslt = _procExeAct(bSuspend);
        if (VOS_OK == ulrunrslt)
        {
#ifdef COMPILER_IS_LINUX_GCC
            //g_pOuterCupMng->ProcAdReady(pAdValue);
#endif
            //每上报一次反应盘状态
            g_pOuterCupMng->SendReactCupStatus();
            //王晖俊 新增反应盘监控
            g_pInnerCupMng->SendReactCupStatus();
            //end
            StartCycleTimer(SS_DMove, g_pRscMng->m_ulExeTimeLen);
        }
        else if (app_statemach_run_over == ulrunrslt)
        {
            _procTestOver(bSuspend);
        }
        else
        {
            WRITE_ERR_LOG("Call exe act fail(%d).", ulrunrslt);
            ProcStopEvent(vos_midmachine_state_midmach_err);
        }
    }
}


vos_u32 CActHandler::_procPreAct()
{
    //转盘
    act_stReactsysDskRtt stPara;
    stPara.usRttedCupNum = APP_RTT_RCTCUP_PER_PERIOD;
    if (VOS_OK == app_reqDwmCmd(SPid_Task, SPid_ReactDsk, Act_Move, sizeof(act_stReactsysDskRtt), (char*)&stPara, APP_YES))
    {
        g_pRscMng->enReactDskFlag = EN_AF_Doing;
    }

    return g_pOuterCupMng->ProcPreAct();
}

vos_u32 CActHandler::_procExeAct(bool bSuspend)
{
    //请求自动清洗的执行动作
    vos_u32 ulrunrslt = _procAutoWashAct(bSuspend);
    if (VOS_OK != ulrunrslt)
    {
        if (app_statemach_run_over != ulrunrslt)
        {
            WRITE_ERR_LOG("Call app_reqReactSysExeAction() failed(%d)\n", ulrunrslt);
        }
        return ulrunrslt;
    }

    //申请下发任务
    g_pTaskMng->NotifySendTask(m_iAlarmLevel);

    //请求样本系统的执行动作
    vos_u32 ulResult = g_pOuterCupMng->ProcSmpExeAct();
    if (VOS_OK != ulResult)
    {
        WRITE_ERR_LOG("Call _procSmpSysExeAct() failed(%lu)\n", ulResult);
        return ulResult;
    }

    //请求R1系统的执行动作
    ulResult = g_pOuterCupMng->ProcR1SysExeAct();
    if (VOS_OK != ulResult)
    {
        WRITE_ERR_LOG("Call ProcR1SysExeAct() failed(%lu)\n", ulResult);
        return ulResult;
    }

    //请求R2系统的执行动作
    ulResult = g_pOuterCupMng->ProcR2SysExeAct();
    if (VOS_OK != ulResult)
    {
        WRITE_ERR_LOG("Call _procR2SysExeAct() failed(%lu)\n", ulResult);
        return ulResult;
    }

    //请求样本搅拌的执行动作
    ulResult = g_pOuterCupMng->ProcSmpMixExeAct();
    if (VOS_OK != ulResult)
    {
        WRITE_ERR_LOG("Call _procSmpMixExeAct() failed(%lu)\n", ulResult);
        return ulResult;
    }

    //请求样本系统稀释位的执行动作
    ulResult = g_pOuterCupMng->ProcSmpExeActDilu();
    if (VOS_OK != ulResult)
    {
        WRITE_ERR_LOG("Call _procSmpSysExeAct() failed(%lu)\n", ulResult);
        return ulResult;
    }

    //请求试剂搅拌的执行动作
    return g_pOuterCupMng->ProcReagMixExeAct();
}

vos_u32 CActHandler::_procAutoWashAct(bool bSuspend)
{
    if (!g_pOuterCupMng->CanWash())
    {
        g_pOuterCupMng->CleanSkip();
        return VOS_OK;
    }

    if (!bSuspend)
    {
        g_pOuterCupMng->AssignNewTask();
    }

    app_u16 usCupBitMap = 0;
    bool bWash = g_pOuterCupMng->CheckAutoWash(usCupBitMap);
    if (!bWash && g_pOuterCupMng->IsNeedOver())
    {
        return app_statemach_run_over;
    }

    if (bWash)
    {
        g_pResHandler->SetAutoWash();
        act_stReactSysAutoWash stPara;
        stPara.usWashOutBitMap = usCupBitMap;
        return app_reqDwmCmd(SPid_Task, SPid_AutoWash, Act_Wash, sizeof(act_stReactSysAutoWash), (char*)&stPara, APP_YES);
    }
    return VOS_OK;
}
