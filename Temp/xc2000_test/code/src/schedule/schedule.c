#include "schedule.h"
#include "typedefine.pb.h"
#include "adsimulate.h"
#include "reactcupmng.h"
#include "taskmng.h"
#include "reagent.h"
#include "actreshandler.h"
#include "app_msgtypedefine.h"
#include "vos_timer.h"
#include "vos_log.h"
#include "socketmonitor.h"
#include "alarm.h"
#include "apputils.h"
#include "vos_errorcode.h"
#include "acthandler.h"
#include "autoload.h"

//周期保护定时器
#define APP_MAX_PROTECT_TIMER_LEN     (5)
#define APP_MAX_EXEPROTECT_TIMER_LEN  (5)

CSchedule::CSchedule() : CCallBack(SPid_Task), m_bSuspend(false), m_ulUnit(Unit_A1)
{
    _initSchModule();
    m_bautomove = false;
}
void CSchedule::_initSchModule()
{
    m_msgFucs[MSG_ReqShelf] = &CSchedule::_onStart;
    m_msgFucs[MSG_ReqAddTask] = &CSchedule::_onAddTask;
    m_msgFucs[MSG_ResReset] = &CSchedule::_onResetRes;
    m_msgFucs[app_req_stop_midmach_msg_type] = &CSchedule::_onStop;
    m_msgFucs[MSG_ReqReagentSuspend] = &CSchedule::_onReagentSuspend;
    m_msgFucs[MSG_NotifyReagInfo] = &CSchedule::_onReagentInfo;
    m_msgFucs[MSG_ReqLightCheck] = &CSchedule::_onLightCheck;
    m_msgFucs[MSG_ReqCupBlank] = &CSchedule::_onCupBlank;
    m_msgFucs[MSG_ReqWashReactCup] = &CSchedule::_onWashCup;
    m_msgFucs[app_com_ad_data_ready_notice_msg_type] = &CSchedule::_onAdReady;
    m_msgFucs[vos_pidmsgtype_timer_timeout] = &CSchedule::_onTimeOut;
    m_msgFucs[app_com_device_res_exe_act_msg_type] = &CSchedule::_onActRes;
	m_msgFucs[MSG_ReqSkipCup] = &CSchedule::_onSkipDirtyCup;
	m_msgFucs[MSG_ReqRegPollution] = &CSchedule::_onRegPollution;
	m_msgFucs[MSG_ReqSmpPollution] = &CSchedule::_onSmpPollution;
    m_msgFucs[MSG_ReqRctPollution] = &CSchedule::_onRctPollution;
	m_msgFucs[MSG_BloodAbsorb] = &CSchedule::_onBloodAbsrob;
    m_msgFucs[MSG_NotifyState] = &CSchedule::_onNotifyState;
    m_msgFucs[MSG_AutoloadDebug] = &CSchedule::_autoLoadTest;

    g_pOuterCupMng = new CReactCupMng(Circle_Outer);
    g_pInnerCupMng = new CReactCupMng(Circle_Inner);
    g_pTaskMng = new CTaskMng();
    g_pRscMng = new CRscMng();
    g_pReagent = new CReagent();
    g_pActHandler = new CActHandler();
    g_pResHandler = new CResHandler();
    g_pAutoLoad = new CAutoLoad();

#ifdef COMPILER_IS_LINUX_GCC
    g_pADSimulate = new CAdSimulate;
    vos_u32 ulrunrslt = g_pADSimulate->LoadCfg();
    if (VOS_OK != ulrunrslt)
    {
        WRITE_ERR_LOG("CAdSimulate fail(%d).", ulrunrslt);
    }
#endif
}

CSchedule::~CSchedule()
{
}

vos_u32 CSchedule::CallBack(vos_msg_header_stru* pMsg)
{
    map<vos_u32, OnMsg>::iterator iter = m_msgFucs.find(pMsg->usmsgtype);
    if (iter != m_msgFucs.end())
    {
        return (this->*(iter->second))(pMsg);
    }
    WRITE_ERR_LOG("Unknown msg type(%lu)(pid:%d->%d)\n ", pMsg->usmsgtype, pMsg->ulsrcpid, pMsg->uldstpid);
    return VOS_OK;
}

vos_u32 CSchedule::_onStart(vos_msg_header_stru* pMsg)
{
    if (!g_pMonitor->CanHandleTask())
    {
        WRITE_ERR_LOG("AddTask failed, status=%lu \n", g_pMonitor->m_ulState);
        return VOS_OK;
    }

    vos_u32 mode_arm_max = g_pAlarm->FindMaxLevelAlarm();
    vos_u32 act_arm_max = g_pActHandler->GetAlarmLevel();
    vos_u32 maxAlarmLevel = mode_arm_max > act_arm_max ? mode_arm_max : act_arm_max;
    if (maxAlarmLevel == 0)
    {
        //只有在空闲状态机下才需执行启动资源调度状态机
        if ((SS_Idle == g_pActHandler->GetStatus()) && (!m_bSuspend))
        {
            app_reportmidmachstatus(vos_midmachine_state_testing);
            _startTest();
        }
    }
    return VOS_OK;
}

vos_u32 CSchedule::_onAddTask(vos_msg_header_stru* pMsg)
{
    if (!g_pMonitor->CanHandleTask())
    {
        WRITE_ERR_LOG("AddTask failed, status=%lu \n", g_pMonitor->m_ulState);
        return VOS_OK;
    }
    CMsg stMsg;
    if (!stMsg.ParseMsg(pMsg))
    {
        WRITE_ERR_LOG("ParseMsg failed\n");
        return VOS_OK;
    }

    STReqAddTask*  pReqTask = dynamic_cast<STReqAddTask*>(stMsg.pPara);
    //王晖俊增加关于自动测试任务的处理流程
    uint32_t tys = pReqTask->uimaintantype();
    if ((tys == AutoTeskTask) || (colormetry_task == tys)|| (pollution_task == tys))
    {
        m_bautomove = true;
    }
    CTask * pTask = new CTask(pMsg->ulsrcpid, pReqTask);
	bool needR2ndl = pTask->NeedAddReag(2) || pTask->NeedAddReag(4);

    vos_u32 mode_arm_max = g_pAlarm->FindMaxLevelAlarm();
	vos_u32 act_arm_max = g_pActHandler->GetAlarmLevel();

	vos_u32 maxAlarmLevel = mode_arm_max > act_arm_max ? mode_arm_max : act_arm_max;

	if (maxAlarmLevel == 0 || (maxAlarmLevel == 3 && !needR2ndl))
    {
        g_pTaskMng->AddTask(pTask);
        //王晖俊 增加自动装载获取接口
        g_pAutoLoad->AddTaskInfo(pTask);
        //end
        if ((SS_Idle == g_pActHandler->GetStatus()) && (!m_bSuspend))
        {
            app_reportmidmachstatus(vos_midmachine_state_testing);
            _startTest();
        }
    }
	else if (maxAlarmLevel == 3 && needR2ndl)
	{
		g_pTaskMng->SendTaskResult(&pTask, TR_AddR2Err);
	}
    else
    {
        g_pTaskMng->SendTaskResult(&pTask, TR_SysFault);
    }
    return VOS_OK;
}

bool CSchedule::_checkLiquid(CTask * pTask, vos_u32 maxAlarmLevel)
{
    vos_u32 r1Cup = 0, r2Cup = 0;
    TaskResult TR_type = TR_Success;
    if (!pTask->CheckAllLiquid(0, r1Cup, r2Cup, TR_type))
    {
        WRITE_ERR_LOG("CheckAllLiquid failed, task(%d) is canceled , TaskResult=%lu!\n", pTask->GetTaskId(), TR_type);
        g_pTaskMng->SendTaskResult(&pTask, TR_type);
        g_pTaskMng->NotifySendTask(maxAlarmLevel);
        return false;
    }
    return true;
}

void CSchedule::_startTest()
{
    g_pActHandler->Initialize(MAX_WASH_NUM);
    //王晖俊 增加复位时间长度
    g_pActHandler->StartCycleTimer(SS_Reset, 12000);// 300);

    if (VOS_OK != app_reqMachReset(SPid_Task))
    {
        g_pActHandler->ProcStopEvent(vos_midmachine_state_midmach_err);
    }
}

vos_u32 CSchedule::_onStop(vos_msg_header_stru* pMsg)
{
    m_bSuspend = false;
    g_pActHandler->ProcStopEvent(vos_midmachine_state_standby);
    return VOS_OK;
}

vos_u32 CSchedule::_onResetRes(vos_msg_header_stru* pMsg)
{
    g_pActHandler->m_bStop = false;
    g_pActHandler->StopCycleTimer();
    if (m_bautomove)
    {
        msg_paraSampleDskRtt samLoc;
        samLoc.usSmpCupId = 1;
        m_bautomove = false;
        app_reqDwmCmd(0, SPid_SmpDsk, Act_MoveInner, sizeof(msg_paraSampleDskRtt), (char*)&samLoc, APP_NO);
    }

    CMsg stMsg;
    if (!stMsg.ParseMsg(pMsg))
    {
        WRITE_ERR_LOG("ParseMsg failed\n");
        return VOS_OK;
    }
    STResResult * pResult = (STResResult *)stMsg.pPara;
    if (app_action_res_success != pResult->uiresult())
    {
        WRITE_ERR_LOG("Reset failed ret=%d", pResult->uiresult());
        g_pActHandler->ProcStopEvent(vos_midmachine_state_reseted_err);
        return VOS_OK;
    }
    g_pActHandler->ProcActStart(m_bSuspend);
    return VOS_OK;
}

vos_u32 CSchedule::_onReagentSuspend(vos_msg_header_stru* pMsg)
{
    CMsg stMsg;
    if (!stMsg.ParseMsg(pMsg))
    {
        WRITE_ERR_LOG("ParseMsg failed\n");
        return VOS_OK;
    }
    STReqReagentSuspend * pReq = (STReqReagentSuspend *)stMsg.pPara;
    m_bSuspend = pReq->bsuspend();
    if (m_bSuspend)
    {
        g_pOuterCupMng->ProcSuspendTime();
    }
    else
    {
        app_reportmidmachstatus(vos_midmachine_state_testing);
        //停机后恢复需要重新启动
        if (SS_Idle == g_pActHandler->GetStatus())
        {
            _startTest();
        }
    }
    return VOS_OK;
}

vos_u32 CSchedule::_onReagentInfo(vos_msg_header_stru* pMsg)
{
    CMsg stMsg;
    if (!stMsg.ParseMsg(pMsg))
    {
        WRITE_ERR_LOG("ParseMsg failed\n");
        return VOS_OK;
    }
    //王晖俊 自动装载获取试剂信息
    g_pAutoLoad->InitReagent(stMsg);
    return g_pReagent->InitReagent(stMsg);
}

vos_u32 CSchedule::_onWashCup(vos_msg_header_stru* pMsg)
{
    return _startWashCup(pMsg->usmsgtype, vos_midmachine_state_wash_rctcup);
}

vos_u32 CSchedule::_onAdReady(vos_msg_header_stru* pMsg)
{
	if(g_pOuterCupMng->m_bUpdateCup == true )//监控是否每个周期均上报AD中断
    {
		WRITE_ERR_LOG("OuterCup AD too late !\n");
    }
    if (g_pInnerCupMng->m_bUpdateCup == true)//监控是否每个周期均上报AD中断
    {
        WRITE_ERR_LOG("InnerCup AD too late not be received!\n");
    }

    msg_ADValue* pAdValue = (msg_ADValue*)pMsg->aucmsgload;
    if (xc8002_dwnmach_name_ADOuter == pAdValue->dwnName)
    {
        g_pOuterCupMng->m_bGetAd = true;
        return g_pOuterCupMng->ProcAdReady(pAdValue);
    }
    else if (xc8002_dwnmach_name_ADInner == pAdValue->dwnName)
    {
        g_pInnerCupMng->m_bGetAd = true;
        return g_pInnerCupMng->ProcAdReady(pAdValue);
    }
    return VOS_OK;
}

vos_u32 CSchedule::_onTimeOut(vos_msg_header_stru* pMsg)
{
    vos_timeroutmsg_stru* pTimeOutMsg = (vos_timeroutmsg_stru*)pMsg;
    g_pActHandler->OnTimeOut(pTimeOutMsg->ultimerid, m_bSuspend);
    return VOS_OK;
}

vos_u32 CSchedule::_onActRes(vos_msg_header_stru* pMsg)
{  
    //王晖俊 自动装载处理返回任务
    if (pMsg->ulsrcpid>950)
    {
        return g_pAutoLoad->_proResponse(pMsg);
    }
    //end
    return g_pActHandler->ProcResAct(pMsg);
/*    return g_pResHandler->ProcActRes(pMsg);*/
}

vos_u32 CSchedule::_onSkipDirtyCup(vos_msg_header_stru* pMsg)
{
	CMsg stMsg;
	if (!stMsg.ParseMsg(pMsg))
	{
		WRITE_ERR_LOG("ParseMsg failed\n");
		return VOS_OK;
	}

	STReqSkipCup*  pSkipCup = dynamic_cast<STReqSkipCup*>(stMsg.pPara);
	g_pOuterCupMng->SkipDirtyCup(pSkipCup->cupid());
    return VOS_OK;
}

vos_u32 CSchedule::_onRegPollution(vos_msg_header_stru* pMsg)
{
	CMsg stMsg;
	if (!stMsg.ParseMsg(pMsg))
	{
		WRITE_ERR_LOG("ParseMsg failed\n");
		return VOS_OK;
	}

	STReqRegPollution*  pReqCross = dynamic_cast<STReqRegPollution*>(stMsg.pPara);
	g_pOuterCupMng->SetRegPollution(pReqCross);
	return VOS_OK;
}

vos_u32 CSchedule::_onSmpPollution(vos_msg_header_stru* pMsg)
{
	CMsg stMsg;
	if (!stMsg.ParseMsg(pMsg))
	{
		WRITE_ERR_LOG("ParseMsg failed\n");
		return VOS_OK;
	}

	STReqSmpPollution*  pReqSmp = dynamic_cast<STReqSmpPollution*>(stMsg.pPara);
	g_pOuterCupMng->SetSmpPollution(pReqSmp);
	return VOS_OK;
}

vos_u32 CSchedule::_onRctPollution(vos_msg_header_stru* pMsg)
{
    CMsg stMsg;
    if (!stMsg.ParseMsg(pMsg))
    {
        WRITE_ERR_LOG("ParseMsg failed\n");
        return VOS_OK;
    }

    STRCtRegPollution*  pReqCross = dynamic_cast<STRCtRegPollution*>(stMsg.pPara);
    g_pOuterCupMng->SetRctPollution(pReqCross);
    return VOS_OK;
}

vos_u32 CSchedule::_onBloodAbsrob(vos_msg_header_stru* pMsg)
{
	CMsg stMsg;
	if (!stMsg.ParseMsg(pMsg))
	{
		WRITE_ERR_LOG("ParseMsg failed\n");
		return VOS_OK;
	}
	BloodAbsorbMode* pMode = dynamic_cast<BloodAbsorbMode*>(stMsg.pPara);
	bool mode = pMode->mode();
	g_pOuterCupMng->SetBloodAbsorbMode(mode);
	return VOS_OK;
}

void CSchedule::_reqTask(app_u32 shelfNum)
{
    app_print(ps_taskmng, "ReqTask shelfNum(%d) \n", shelfNum);

    STShelfMng stPara;
    stPara.set_ctrltype(Shelf_Req);
    stPara.set_shelfnum(shelfNum);
    sendUpMsg(CPid_Relation, SPid_Task, MSG_ReqShelf, &stPara);
}

vos_u32 CSchedule::_onLightCheck(vos_msg_header_stru* pMsg)
{
    return _startWashCup(pMsg->usmsgtype, vos_midmachine_state_light_check);
}

vos_u32 CSchedule::_onCupBlank(vos_msg_header_stru* pMsg)
{
    return _startWashCup(pMsg->usmsgtype, vos_midmachine_state_cup_blank);
}

vos_u32 CSchedule::_startWashCup(int iMsgId, vos_u32 ulStatus)
{
    if (!g_pMonitor->CanHandleCmd())
    {
        WRITE_ERR_LOG("WashRctCup failed, status=%lu \n", g_pMonitor->m_ulState);
        return vos_para_invalid_err;
    }
    if (SS_Idle != g_pActHandler->GetStatus())
    {
        WRITE_ERR_LOG("app_procWashRctCup error state is %lu\n", g_pActHandler->GetStatus());
        return vos_para_invalid_err;
    }
    vos_u32 faulttype = g_pAlarm->FindMaxLevelAlarm();
    if (faulttype == app_alarm_fault_thetimingstop)
    {
        app_reportmidmachstatus(vos_midmachine_state_midmach_err);
        return vos_para_invalid_err;
    }
    _startTest();
    g_pOuterCupMng->StartWashCup(iMsgId);
    app_reportmidmachstatus(ulStatus, SPid_Task);
    return VOS_OK;
}

vos_u32 CSchedule::_onNotifyState(vos_msg_header_stru* pMsg)
{
    CMsg stMsg;
    if (!stMsg.ParseMsg(pMsg))
    {
        WRITE_ERR_LOG("ParseMsg failed\n");
        return VOS_OK;
    }
    
    STNotifState* pNotifState = dynamic_cast<STNotifState*>(stMsg.pPara);
    if (pNotifState->enunit() == Unit_Gway)
    {
        if (pNotifState->ulloca() == EN_FWNorm || pNotifState->ulloca() == EN_FWEmer)
        {
            _reqTask(pNotifState->ulshelfnum());
        }
        g_pActHandler->OnNotifyState(pNotifState);
    }
    return VOS_OK;
}

vos_u32 CSchedule::_autoLoadTest(vos_msg_header_stru* pMsg)
{
    CMsg stMsg;
    if (!stMsg.ParseMsg(pMsg))
    {
        WRITE_ERR_LOG("ParseMsg failed\n");
        return VOS_OK;
    }
    STOffsetPara*  pst = dynamic_cast<STOffsetPara*>(stMsg.pPara);
    //构造L盘试剂信息
    STNotifyReagInfo lreagInfo;
    STReagInfo* linfo = lreagInfo.add_streaginfo();
    STCupPos* lpos = linfo->mutable_stcuppos();
    lpos->set_uidskid(pst->uitype());
    lpos->set_uicupid(pst->uipos());
    linfo->set_strreagname("123");
    linfo->set_enbtype(BT_standard);
    g_pAutoLoad->GetDebugLreagent(lreagInfo);
    //构造任务信息
    STReqAddTask  pReqTask;
    pReqTask.set_uitaskid(1);
    STLiquidInfo* stinfo=pReqTask.add_streagents();
    stinfo->set_uiaddvol(5);
    STCupPos* stpos = stinfo->mutable_stcuppos();
    stpos->set_uidskid(pst->uidir());
    stpos->set_uicupid(pst->uistep());
    CTask * pTask = new CTask(pMsg->ulsrcpid, &pReqTask);
    g_pAutoLoad->AddTaskInfo(pTask);
    //构造试剂盘试剂信息
    STNotifyReagInfo reagInfo;
    STThreshold* shold = reagInfo.add_stthreshold();
    shold->set_enbtype(BT_standard);
    shold->set_uithreshold(5);
    STReagInfo* rinfo = reagInfo.add_streaginfo();
    STCupPos* rpos = rinfo->mutable_stcuppos();
    rpos->set_uidskid(pst->uidir());
    rpos->set_uicupid(pst->uistep());
    rinfo->set_strreagname("123");
    rinfo->set_enbtype(BT_standard);
    rinfo->set_uimargindepth(10);
    g_pAutoLoad->GetDebugRreagent(reagInfo);
    return VOS_OK;
}
