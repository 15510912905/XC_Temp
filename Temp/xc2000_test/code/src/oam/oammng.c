#include "oammng.h"
#include "vos_log.h"
#include "app_msg.h"
#include "apputils.h"
#include "app_comstructdefine.h"
#include "socketmonitor.h"
#include "alarm.h"

const vos_u32 g_LampFullBright = 100; //光源灯全亮
const vos_u32 g_RewarmTimeLen = 18000;  //复温超时时间

vos_u32 COamMng::m_ulTimer = 0;
vos_u32 COamMng::CompBackFlg = VOS_NO;

COamMng::COamMng(vos_u32 ulPid) : CCallBack(ulPid), m_waitCount(0)
{
    _initProcFuc();
    _initStopPid();
    m_ulInitTimer = createTimer(SPid_Oammng, "OamInit");
    m_ulTimer = createTimer(SPid_Oammng, "TempQuery", TEMPQUERY_TIME_LEN, vos_timer_type_loop);
#ifdef COMPILER_IS_ARM_LINUX_GCC
    //vos_starttimer(m_ulTimer);
#endif
}

COamMng::~COamMng()
{

}

void COamMng::_initProcFuc()
{
    m_procFucs[MSG_ReqStopMach] = &COamMng::_procStopMach;
    m_procFucs[app_outdisc_rotate_req] = &COamMng::_procKeyMoveOuterDsk;
    m_procFucs[app_indisc_rotate_req] = &COamMng::_procKeyMoveInnerDsk;
    m_procFucs[vos_pidmsgtype_timer_timeout] = &COamMng::_procTimeOut;
    m_procFucs[MSG_GetMidLog] = &COamMng::_procGetMidLog;  
    m_procFucs[MSG_ResReset] = &COamMng::_procResetRes;
    m_procFucs[MSG_ReqCompRecover] = &COamMng::_procCompRecover;
    m_procFucs[MSG_ReqRecoverMotor] = &COamMng::_procMotorRecover;

    m_procFucs[MSG_ReqCompBack] = &COamMng::_procReqCompBack;
    m_procFucs[MSG_ReqBackupMotor] = &COamMng::_procReqMotorBack;

    m_procFucs[MSG_ResCompBack] = &COamMng::_procResCompBack;
    m_procFucs[MSG_NotifyMotor] = &COamMng::_procResMotorBack;

    m_procFucs[app_com_device_res_exe_act_msg_type] = &COamMng::_procDwnRes;
	m_procFucs[MSG_NotifyTemp] = &COamMng::_procReactDiskTemp;
}

void COamMng::_initStopPid()
{
    m_vStopPids.push_back(SPid_Task);
    m_vStopPids.push_back(SPid_BcScan);
	m_vStopPids.push_back(SPid_MarginScan);
    m_vStopPids.push_back(SPid_CheckLightCup);
    m_vStopPids.push_back(SPid_DarkCurrent);
    m_vStopPids.push_back(SPid_PreciseAdd);
    m_vStopPids.push_back(SPid_IntensifyWash);
    m_vStopPids.push_back(SPid_Reset);
    m_vStopPids.push_back(SPid_AirOut);
    m_vStopPids.push_back(Spid_UpdateDwn);
	m_vStopPids.push_back(SPid_Showave);

    //其他模块
    m_vStopPids.push_back(Pid_Maintain);
    m_vStopPids.push_back(Pid_LoopTest);
    //王晖俊  增加停止动作处理自动装载模块
    m_vStopPids.push_back(SPid_AutoLoad       );
    m_vStopPids.push_back(SPid_Transport      );
    m_vStopPids.push_back(SPid_VerticalGripper); 
    m_vStopPids.push_back(SPid_L1             );
    m_vStopPids.push_back(SPid_L2             );
    //子系统模块
    for (vos_u32 ulSysPid = SPid_SmpNdl; ulSysPid <= SPid_SmpEmer; ++ulSysPid) 
    {
        m_vStopPids.push_back(ulSysPid);
    }
}

vos_u32 COamMng::CallBack(vos_msg_header_stru* pMsg)
{
    map<app_u16, ProcMsg>::iterator iter = m_procFucs.find(pMsg->usmsgtype);
    if (iter != m_procFucs.end())
    {
        return (this->*(iter->second))(pMsg);
    }
    WRITE_ERR_LOG("Unknown msgtype=%lu\n", pMsg->usmsgtype);
    return VOS_OK;
}

vos_u32 COamMng::_procStopMach(vos_msg_header_stru* pMsg)
{
    vos_u32 ulResult = VOS_OK;
    vector<vos_u32>::iterator iter = m_vStopPids.begin();
    for (; iter != m_vStopPids.end(); ++iter)
    {
        ulResult = sendInnerMsg(*iter, SPid_Oammng, app_req_stop_midmach_msg_type);
        if (VOS_OK != ulResult)
        {
            WRITE_ERR_LOG("sendInnerMsg error=%lu\n", ulResult);
            break;
        }
    }

    app_smpLightCtl(SPid_Oammng, samp_key_led_off);
    INITP04(SPid_Oammng, Press_our);
    INITP04(SPid_Oammng, Press_in);
    app_reportmidmachstatus(vos_midmachine_state_standby, SPid_Oammng);
    return ulResult;
}

vos_u32 COamMng::_procKeyMoveOuterDsk(vos_msg_header_stru* pMsg)
{
    if (g_pMonitor->CanMoveReagDsk())
    {
        return _moveReagDsk(SPid_R1Dsk, 12);
    }
    return VOS_OK;
}

vos_u32 COamMng::_procKeyMoveInnerDsk(vos_msg_header_stru* pMsg)
{
    if (g_pMonitor->CanMoveReagDsk())
    {
        return _moveReagDsk(SPid_R2Dsk, 12);
    }
    return VOS_OK;
}

vos_u32 COamMng::_moveReagDsk(vos_u32 dstPid, char ucCupNum)
{
    vos_u32 ulrunrslt = VOS_OK;
	vos_u32 ulmsglength = 0;
    msg_stHead rHead;

	msg_stSingleActUni* pstdwnmachreqmsg = VOS_NULL;
    msg_stDskHrotate *pdstrst = VOS_NULL;

	ulmsglength = sizeof(msg_stSingleActUni) + sizeof(msg_stDskHrotate);   

	ulrunrslt = app_mallocBuffer(ulmsglength,(void * *)&pstdwnmachreqmsg);
	if (VOS_OK != ulrunrslt)
    {
        WRITE_ERR_LOG("Call app_mallocBuffer() failed(%d) \n", ulrunrslt);
        return ulrunrslt;
    }

	app_constructMsgHead(&rHead, dstPid, SPid_Oammng, app_com_device_req_exe_act_msg_type);
	//填数据内容
    pstdwnmachreqmsg->usActNum = 1;
    pstdwnmachreqmsg->stActHead.usActType = Act_DskHrotate;
	pstdwnmachreqmsg->stActHead.usSingleRes = APP_YES;
    pstdwnmachreqmsg->ulDataLen = sizeof(msg_stDskHrotate) + sizeof(msg_stActHead) ;

	pdstrst = (msg_stDskHrotate*)(&(pstdwnmachreqmsg->astActInfo[0]));
	pdstrst->dir = cmd_motor_dir_ANTICLOCKWISE;
	pdstrst->num = ucCupNum;
    
    return app_sendMsg(&rHead, pstdwnmachreqmsg, ulmsglength);
}

vos_u32 COamMng::_procTimeOut(vos_msg_header_stru* pMsg)
{
    vos_timeroutmsg_stru* pstmsg = (vos_timeroutmsg_stru*)pMsg;
    if (pstmsg->ultimerid == m_ulTimer)
    {
        return _procQueryTemp(pMsg);
    }
    if (vos_midmachine_state_resetting == g_pMonitor->m_ulState)
    {
        g_pAlarm->SetFilterInReset(false);
        app_reportmidmachstatus(vos_midmachine_state_reseted_err, SPid_Oammng);
    }
    else if (vos_midmachine_state_rewarming == g_pMonitor->m_ulState)
    {
        app_reportmidmachstatus(vos_midmachine_state_rewarming_err, SPid_Oammng);
    }
    return VOS_OK;
}
vos_u32 COamMng::_procQueryTemp(vos_msg_header_stru* pMsg)
{
    app_reqDwmCmd(SPid_Oammng, SPid_RTmpCtrl, Act_TempQuery, 0, NULL, APP_NO);
    return app_reqDwmCmd(SPid_Oammng, SPid_CoolReag, Act_TempQuery, 0, NULL, APP_NO);
}

extern string g_szUpMachIp;
vos_u32 COamMng::_procGetMidLog(vos_msg_header_stru* pMsg)
{
    CMsg stMsg;
    stMsg.ParseMsg(pMsg);
	STGetMidLog*  pReq = dynamic_cast<STGetMidLog*>(stMsg.pPara);
    string pstIp  = pReq->strip();
    if (pstIp == "")
    {
        pstIp = g_szUpMachIp;
    }
    printf("get log ip - %s \n", pstIp.c_str());

    #ifdef COMPILER_IS_ARM_LINUX_GCC 
    char  aucexetftpcmd[128] = {0};
    system("tar zcvf midlog.tar.gz /root/app/log/");
    sprintf(aucexetftpcmd, "tftp -p -r midlog.tar.gz %s", pstIp.c_str());
    system(aucexetftpcmd);
    system("rm midlog.tar.gz");
    #endif
    return VOS_OK;
}

void COamMng::InitMachine()
{
    printf(" Now begin to reset......\n");
    startTimer(m_ulInitTimer, g_ResetTimeLen);
    app_reportmidmachstatus(vos_midmachine_state_resetting, SPid_Oammng);
//     SENDBOOTFINISH2DOWNM();//发送中位机初始化完成到下位机
//  开机模块自动复位
    app_reqMachReset(SPid_Oammng);
    g_pAlarm->SetFilterInReset(true);   
}

vos_u32 COamMng::_procResetRes(vos_msg_header_stru* pMsg)
{
    vos_stoptimer(m_ulInitTimer);
    g_pAlarm->SetFilterInReset(false);
    CMsg stMsg;
    stMsg.ParseMsg(pMsg);
    STResResult * pPara = (STResResult *)stMsg.pPara;

	printf("COamMng::_procResetRes  %d\n", pPara->uiresult());
    if (EN_AF_Success != pPara->uiresult())
    {
        app_reportmidmachstatus(vos_midmachine_state_reseted_err, SPid_Oammng);
        return VOS_OK;
    }

#ifdef COMPILER_IS_ARM_LINUX_GCC
    startTimer(m_ulInitTimer, g_RewarmTimeLen);
    app_reportmidmachstatus(vos_midmachine_state_rewarming, SPid_Oammng);
#else
    app_reportmidmachstatus(vos_midmachine_state_standby, SPid_Oammng);
#endif // COMPILER_IS_ARM_LINUX_GCC
    _openControl();
    return _setLampBrightness();
}

//开光源灯复温
vos_u32 COamMng::_setLampBrightness()
{
    return 0;
    printf(" Now begin to rewarming......\n");
    msg_stLightControl stPara;
    stPara.lightIntensity = g_LampFullBright;
    return app_reqDwmCmd(APP_COM_PID, SPid_ReactDsk, Act_Light, sizeof(stPara), (char*)&stPara, APP_NO);
//     vos_u32 ulResult = VOS_OK;
//     msg_stComDevActReq * pPara = VOS_NULL;
//     vos_u32 ulParaLen = sizeof(msg_stComDevActReq) + sizeof(msg_stComDevActInfo) + sizeof(msg_stLightControl);
// 
//     ulResult = app_mallocBuffer(ulParaLen, (void**)(&pPara));
//     if (VOS_OK != ulResult)
//     {
//         WRITE_ERR_LOG("Call app_mallocBuffer() failed(%d)\n", ulResult);
//         return ulResult;
//     }
//     pPara->usActNum = 1;
//     msg_stComDevActInfo * pActInfo = (msg_stComDevActInfo *)pPara->astActInfo;
//     pActInfo->ulDataLen = sizeof(msg_stActHead) + sizeof(msg_stLightControl);
//     pActInfo->stActHead.usActType = Act_Light;
//     pActInfo->stActHead.usSingleRes = APP_NO;
//     msg_stLightControl * pLight = (msg_stLightControl *)pActInfo->aucPara;
//     pLight->lightIntensity = g_LampFullBright;
// 
//     msg_stHead stHead;
//     app_constructMsgHead(&stHead, SPid_ReactDsk, APP_COM_PID, app_com_device_req_exe_act_msg_type);
// 
//     return app_sendMsg(&stHead, pPara, ulParaLen);
}

vos_u32 COamMng::_openControl()
{
    msg_stateSwitch stpara;
    stpara.state = 2;
    app_reqDwmCmd(APP_COM_PID, SPid_AutoWash, Act_StateSwitch, sizeof(stpara), (char*)&stpara, APP_NO);
    msg_stTempSwitch stTemp;
    stTemp.switchState = 1;
    app_reqDwmCmd(APP_COM_PID, SPid_RTmpCtrl, Act_TempControl, sizeof(stTemp), (char*)&stTemp, APP_NO);
    msg_stTempSwitch WTmpTemp;
    WTmpTemp.switchState = 1;
    app_reqDwmCmd(APP_COM_PID, SPid_WTmpCtrl, Act_TempControl, sizeof(WTmpTemp), (char*)&WTmpTemp, APP_NO);
    return VOS_OK;
}

void COamMng::_setOffset(CMcu *pCMcu)
{   
    vos_u32 ulResult = VOS_OK;
    vos_u32 comps = pCMcu->comps_size();
    vos_u32 bufLen = sizeof(msg_stCompensation) + comps*sizeof(msg_stCmpPara);
    msg_stCompensation * compInfo = NULL;
    ulResult = app_mallocBuffer(bufLen, (void **)&compInfo);
    if (VOS_OK != ulResult)
    {
        WRITE_ERR_LOG("Call app_mallocBuffer() failed(0x%x) \n", ulResult);
        return;
    }

    compInfo->type = pCMcu->comptype();
    compInfo->paranum = comps;
    
    msg_stCmpPara *pCompdata = (msg_stCmpPara*)(compInfo->pPara);
    for (vos_u32 index = 0; index < comps; ++index)
    {
        const CCompInfo& pCompInfo = pCMcu->comps(index);
        pCompdata->location = pCompInfo.location();
        pCompdata->dir= pCompInfo.dir();
        pCompdata->step= pCompInfo.step();
        pCompdata++;
    }
    
    if (VOS_OK != app_reqDwmCmd(SPid_Oammng, pCMcu->subsysid(), Act_SaveOffset, bufLen, (char *)compInfo, VOS_YES))
    {
        WRITE_ERR_LOG("Call app_reqDwmCmd() failed.\n");
    }
    app_freeBuffer(compInfo);
}

vos_u32 COamMng::_procCompRecover(vos_msg_header_stru* pMsg)
{
    CMsg stMsg;
    stMsg.ParseMsg(pMsg);
	CMcuList*  pReq = dynamic_cast<CMcuList*>(stMsg.pPara);
    m_waitCount = pReq->data_size();
    app_reportmidmachstatus(vos_midmachine_state_dwncmd, SPid_DwnCmd);

    m_compSetRes.Clear();
    for (int index = 0; index<pReq->data_size(); ++index)    
    {
        CMcu *pCMcu = pReq->mutable_data(index);
        if(VOS_NULL != pCMcu && pCMcu->comps_size() > 0)
        {
            _setOffset(pCMcu);
        } 
    }
    return VOS_OK;
}

vos_u32 COamMng::_procReqCompBack(vos_msg_header_stru* pMsg)
{
    app_reportmidmachstatus(vos_midmachine_state_dwncmd, SPid_Oammng);

    m_compBack.Clear();
    CompBackFlg = VOS_YES;

    msg_stDwnQuery dwnQuery;
    dwnQuery.queryType = xc8002_dwnmach_report_compensation_type;
    for (int iSysId = SPid_SmpDsk; iSysId <= SPid_R2Mix; ++iSysId)
    {
        app_reqDwmCmd(SPid_Oammng, iSysId, Act_DwnQuery, sizeof(dwnQuery), (char *)&dwnQuery, APP_NO);
    }
    return VOS_OK;
}

vos_u32 COamMng::_procReqMotorBack(vos_msg_header_stru* pMsg)
{
    app_reportmidmachstatus(vos_midmachine_state_dwncmd, SPid_Oammng);
    m_motorList.Clear();
    CompBackFlg = VOS_YES;

    msg_stDwnQuery dwnQuery;
    dwnQuery.queryType = xc8002_dwnmach_report_motorline_type;
    //10个单片机有电机调试
    for (int iSysId = SPid_SmpDsk; iSysId <= SPid_R2Mix; ++iSysId)
    {
        app_reqDwmCmd(SPid_Oammng, iSysId, Act_DwnQuery, sizeof(dwnQuery), (char *)&dwnQuery, APP_NO);
    }

    return VOS_OK;
}


CMcu* COamMng::_findCompData(vos_u32 comptype, vos_u32 subsysid)
{
    for (int index = 0; index<m_compBack.data_size(); index++)
    {
        CMcu* compData = m_compBack.mutable_data(index); 
        if ((compData->subsysid()==subsysid) && (comptype==compData->comptype()))
        {
            return compData;
        }
    }
    return NULL;
}

void COamMng::_saveOffsetPara(const STOffsetPara & pPara, vos_u32 subsysid)
{   
//     CMcu* compData = _findCompData(pPara.uitype(),subsysid);
//     if (NULL == compData)
//     {
//         compData = m_compBack.add_data();
//         compData->set_subsysid(subsysid);
//         compData->set_comptype(pPara.uitype());
//     }
//     CCompInfo *pComps = compData->add_comps();
//     pComps->set_location(pPara.uipos());
//     pComps->set_dir(pPara.uidir());
//     pComps->set_step(pPara.uistep());
}

vos_u32 COamMng::_procResCompBack(vos_msg_header_stru* pMsg)
{
//     CMsg stMsg;
//     stMsg.ParseMsg(pMsg);
// 	STNotifyOffset*  pRes = dynamic_cast<STNotifyOffset*>(stMsg.pPara);
//     app_print(ps_oam, "Get Compensation (%lu)\n",pRes->uisubsysid());
//     if (CompBackFlg == VOS_YES && m_compBack.data_size() < 20)//10个单片机有补偿，每个单片机2个data
//     {
//         for (int index = 0; index<pRes->stoffset_size(); index++)
//         {
//             _saveOffsetPara(pRes->stoffset(index),pRes->uisubsysid());
//         }
//         if (m_compBack.data_size() >= 20)
//         {
//             CompBackFlg = VOS_NO;
//             app_reportmidmachstatus(vos_midmachine_state_standby, SPid_Oammng);
//             return sendUpMsg(CPid_OffSet, SPid_Oammng, MSG_ResCompBack, &m_compBack);
//         }
//     }
//     else if (CompBackFlg == VOS_NO)
//     {
//         return sendUpMsg(CPid_OffSet, SPid_Oammng, MSG_NotifyOffset, pRes);
//     }
    return VOS_OK;
}

vos_u32 COamMng::_procDwnRes(vos_msg_header_stru* pMsg)
{
    app_com_dev_act_result_stru * pstmvmsg = (app_com_dev_act_result_stru *)pMsg->aucmsgload;
    if (pstmvmsg->enacttype == Act_SaveOffset || pstmvmsg->enacttype == Act_WriteMotor)
    {
        m_waitCount--;
        if (0== m_waitCount)
        {
            app_reportmidmachstatus(vos_midmachine_state_standby, SPid_Oammng);
        }
    }
    return VOS_NO;
}

vos_u32 COamMng::_procReactDiskTemp(vos_msg_header_stru* pMsg)
{
	CMsg stMsg;
	stMsg.ParseMsg(pMsg);
	STNotifyTemp  *pTemp  = dynamic_cast<STNotifyTemp*>(stMsg.pPara);

	static uint32 tempCount = 0;
	if (g_pMonitor->m_ulState == vos_midmachine_state_rewarming)
	{
		if (pTemp->itemp() >= 367 && pTemp->itemp() <= 373)
		{
			tempCount++;
		}
		if (tempCount == 100)
		{
			app_reportmidmachstatus(vos_midmachine_state_standby, Pid_ReportStatus);
		}
	}
	return 0;
}

vos_u32 COamMng::_procResMotorBack(vos_msg_header_stru* pMsg)
{
    CMsg stMsg;
    stMsg.ParseMsg(pMsg);
    STSubSysMotor*  pRes = dynamic_cast<STSubSysMotor*>(stMsg.pPara);
    app_print(ps_oam, "Get Motor sysid=(%lu)\n", pRes->uisysid());
    if (CompBackFlg == VOS_YES)//9个单片机有补偿
    {
        STSubSysMotor * pMotor = m_motorList.add_data();
        pMotor->CopyFrom(*pRes);
        if (m_motorList.data_size() >= 10)
        {
            CompBackFlg = VOS_NO;
            app_reportmidmachstatus(vos_midmachine_state_standby, SPid_Oammng);
            return sendUpMsg(CPid_OffSet, SPid_Oammng, MSG_ResBackupMotor, &m_motorList);
        }
    }
    else if (CompBackFlg == VOS_NO)
    {
        return sendUpMsg(CPid_OffSet, SPid_Oammng, MSG_NotifyMotor, pRes);
    }
    return VOS_OK;
}

vos_u32 COamMng::_procMotorRecover(vos_msg_header_stru* pMsg)
{
    CMsg stMsg;
    stMsg.ParseMsg(pMsg);
    STMotorList*  pReq = dynamic_cast<STMotorList*>(stMsg.pPara);
    m_waitCount = pReq->data_size();
    app_reportmidmachstatus(vos_midmachine_state_dwncmd, SPid_DwnCmd);
    for (int index = 0; index < pReq->data_size(); ++index)
    {
        const STSubSysMotor& pReqMotor = pReq->data(index);
        _saveMotor(pReqMotor);
    }
    return VOS_OK;
}

void COamMng::_saveMotor(const STSubSysMotor& pReqMotor)
{
    vos_u32 ulParaLen = sizeof(msg_stMotor) + pReqMotor.stpara_size() *sizeof(msg_stMotorPara);

    msg_stMotor * pMotor = VOS_NULL;
    vos_u32 ulResult = app_mallocBuffer(ulParaLen, (void**)(&pMotor));
    if (VOS_OK != ulResult)
    {
        WRITE_ERR_LOG("Call app_mallocBuffer() failed(%d)\n", ulResult);
        return;
    }

    pMotor->paranum = pReqMotor.stpara_size();
    msg_stMotorPara * pPara = (msg_stMotorPara *)pMotor->pPara;

    for (int i = 0; i < pReqMotor.stpara_size(); ++i)
    {
        const STMotorPara& pReq = pReqMotor.stpara(i);
        pPara->drvId = pReq.uilineid();
        pPara->lineId = pReq.uilineid();
        pPara->startFre = pReq.uistartfre();
        pPara->endFre = pReq.uiendfre();
        pPara->echLaderStep = pReq.uiechstep();
        pPara->upMaxLader = pReq.uimaxladder();
        pPara->sPar = pReq.uispar();
        pPara++;
    }
    if (VOS_OK != app_reqDwmCmd(SPid_Oammng, pReqMotor.uisysid(), Act_WriteMotor, ulParaLen, (char *)pMotor, APP_YES))
    {
        WRITE_ERR_LOG("Call app_reqDwmCmd() failed.\n");
    }
    app_freeBuffer(pMotor);
}

