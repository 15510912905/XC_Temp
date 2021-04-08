#include "dwmcmd.h"
#include "vos_log.h"
#include "apputils.h"
#include "app_msg.h"
#include "socketmonitor.h"
#include "app_errorcode.h"
#include "app_comstructdefine.h"
#include "oammng.h"
#include "app_maintain.h"
#include "vos_init.h"
#include "app_innermsg.h"
#include "comstruct.pb.h"

bool sleepFlag = true;
CDwnCmd::CDwnCmd(vos_u32 ulPid) : CCallBack(ulPid)
{
    m_reqFucs[Act_Reset] = &CDwnCmd::_reqReset;
    m_reqFucs[Act_TrimOffset] = &CDwnCmd::_reqTrimOffset;
    m_reqFucs[Act_SaveOffset] = &CDwnCmd::_reqSaveOffset;
    m_reqFucs[Act_DwnQuery] = &CDwnCmd::_reqDwnQuery;
    m_reqFucs[Act_Move] = &CDwnCmd::_reqMove;
    m_reqFucs[Act_DskHrotate] = &CDwnCmd::_reqDskHrotate;
    m_reqFucs[Act_NdlVrt] = &CDwnCmd::_reqNdlVrt;
    m_reqFucs[Act_Mix] = &CDwnCmd::_reqMix;
    m_reqFucs[Act_Wash] = &CDwnCmd::_reqWsah;
    m_reqFucs[Act_Light] = &CDwnCmd::_reqLightCtl; 
    m_reqFucs[Act_Debug] = &CDwnCmd::_reqMachineDebug; 
	m_reqFucs[Act_TempSwitch] = &CDwnCmd::_reqTempSwitch;
    m_reqFucs[Act_BcScan] = &CDwnCmd::_reqBcScan;
    m_reqFucs[Act_PVCtrl] = &CDwnCmd::_reqPVCtrl;
    m_reqFucs[Act_LiquidDBG] = &CDwnCmd::_reqLiquidDBG;
	m_reqFucs[Act_SetFlState] = &CDwnCmd::_reqSetFlState;
    m_reqFucs[Act_PumpCtrl] = &CDwnCmd::_reqPumpCtrl;
    m_reqFucs[Act_OlnyMix] = &CDwnCmd::_reqOlnyMix;
    m_reqFucs[Act_WriteMotor] = &CDwnCmd::_reqSaveMotor;
    m_reqFucs[Act_SetWTemp] = &CDwnCmd::_reqSetWTemp;
    m_reqFucs[Act_SetRTemp] = &CDwnCmd::_reqSetRTemp;

    m_mResFuncs[SPid_RTmpCtrl][Act_TempQuery] = &CDwnCmd::_procReactTemp;
    m_mResFuncs[SPid_WTmpCtrl][Act_TempQuery] = &CDwnCmd::_procWaterTemp;

    m_ulQueryTimer = createTimer(SPid_DwnCmd, "QueryVers");
    m_ulCmdTimer = createTimer(SPid_DwnCmd, "DwnCmd");
}
CDwnCmd::~CDwnCmd()
{

}


vos_u32 CDwnCmd::CallBack(vos_msg_header_stru* pMsg)
{

    switch (pMsg->usmsgtype)
    {
    case MSG_ReqActCmd:
    {
        CMsg stMsg;
        stMsg.ParseMsg(pMsg);
        STReqActCmd*  pReq = dynamic_cast<STReqActCmd*>(stMsg.pPara);
        if (pReq->uiacttype() == Act_DwnQuery || pReq->uiacttype() == Act_SaveOffset)
        {
            return _procActCmdReq(pMsg);
        }
        if (g_pMonitor->CanHandleCmd())
        {
            app_reportmidmachstatus(vos_midmachine_state_dwncmd, SPid_DwnCmd);
            startTimer(m_ulCmdTimer, 500);
            return _procActCmdReq(pMsg);
        }
        WRITE_ERR_LOG("Status=%lu\n", g_pMonitor->m_ulState);
        break;
    }
    case MSG_GetVres:
        if (g_pMonitor->CanHandleCmd())
        {
            app_reportmidmachstatus(vos_midmachine_state_dwncmd, SPid_DwnCmd);
            return _procQueryVres();
        }
        WRITE_ERR_LOG("Status=%lu\n", g_pMonitor->m_ulState);
        break;
    case MSG_ResVres:
        return _procResVers(pMsg);
    case vos_pidmsgtype_timer_timeout:
        return _procTimeOut(pMsg);
    case MSG_TempeQuery:
        return _procTempeQuery();
    case MSG_ReqShowAD:
        return _procTempeADQuery();
    case app_com_device_res_exe_act_msg_type:
        vos_stoptimer(m_ulCmdTimer);
        app_reportmidmachstatus(vos_midmachine_state_standby, SPid_DwnCmd);
        _procResMsg(pMsg);
        break;
    case MSG_ADtest:
    {
        app_reportmidmachstatus(vos_midmachine_state_dwncmd, SPid_DwnCmd);
        _procADtest(pMsg);
        app_reportmidmachstatus(vos_midmachine_state_standby, SPid_DwnCmd);    
        break;
    }
    case MSG_ReqShowSmooth:
    {
        _procShowWave(pMsg);
        break;
    }
    default:
        break;
    }

    return VOS_OK;
}

vos_u32 CDwnCmd::_procActCmdReq(vos_msg_header_stru* pMsg)
{     
    if (!g_pMonitor->CanHandleCmd())
    {
        WRITE_ERR_LOG("DwnCmd failed, status=%lu \n", g_pMonitor->m_ulState);
        return VOS_OK;
    }
    CMsg stMsg;
    stMsg.ParseMsg(pMsg);
	STReqActCmd*  pReq = dynamic_cast<STReqActCmd*>(stMsg.pPara);
    
    map<app_u16, ReqAct>::iterator iter = m_reqFucs.find(pReq->uiacttype());
    if (iter != m_reqFucs.end())
    {
        (this->*(iter->second))(pReq);
    }
    else
    {
        WRITE_ERR_LOG("Split the original act(act= 0x%x) failed. \r\n", pReq->uiacttype());
        return app_req_func_act_invalid_err;
    }
    
    return VOS_OK;    
}

void CDwnCmd::_reqReset(STReqActCmd* pReq)
{    
    WRITE_INFO_LOG("Reset:subsys(%u)\n",pReq->uisubsysid());
    vos_settimertimelen(m_ulCmdTimer, 300);//复位超时时间设为30s
    if(SPid_Reset == pReq->uisubsysid())
    {
        app_reqMachReset(SPid_DwnCmd);//整机复位
    }
    else if (VOS_OK != app_reqDwmCmd(SPid_DwnCmd, pReq->uisubsysid(), Act_Reset, 0, VOS_NULL, APP_NO))
    {
        WRITE_ERR_LOG("Call app_reqDwmCmd() failed.\n");
    }
}

void CDwnCmd::_reqTrimOffset(STReqActCmd* pReq)
{
    msg_stStepMove stepMove;

    stepMove.motorType = pReq->uiparams(0);
    stepMove.dir = pReq->uiparams(1);
    stepMove.step = pReq->uiparams(2);
    app_print(ps_dwnCmd,"TrimOffset:subsys(%u)\n",pReq->uisubsysid());
    app_print(ps_dwnCmd,"Motot type=%d, dir=%d ,step=%u \n", stepMove.motorType, stepMove.dir, stepMove.step); 
    if (VOS_OK != app_reqDwmCmd(SPid_DwnCmd, pReq->uisubsysid(), Act_TrimOffset, sizeof(stepMove), (char*)&stepMove, APP_YES))
    {
        WRITE_ERR_LOG("Call app_reqDwmCmd() failed.\n");
    } 
    return;
}

void CDwnCmd::_reqSaveOffset(STReqActCmd* pReq)
{
    vos_u32 ulResult = VOS_OK;
    vos_u32 ulParaLen = 0;
    msg_stSaveOffset * stPara = VOS_NULL;
    if (pReq->uiparams_size() < 2)
    {
        return;
    }

    ulParaLen = sizeof(msg_stSaveOffset) + sizeof(vos_u32)*(pReq->uiparams_size() - 2);
    ulResult = app_mallocBuffer(ulParaLen, (void**)(&stPara));
    if (VOS_OK != ulResult)
    {
        WRITE_ERR_LOG("Call app_mallocBuffer() failed(%d)\n", ulResult);
        return;
    }

    stPara->type = pReq->uiparams(0);
    stPara->paranum = pReq->uiparams(1);
    vos_u32 *pData = stPara->pPara;
    for (int loop = 2; loop < pReq->uiparams_size(); loop++)
    {
        *pData = pReq->uiparams(loop);
        pData++;
    }
    if (VOS_OK != app_reqDwmCmd(SPid_DwnCmd, pReq->uisubsysid(), Act_SaveOffset, ulParaLen, (char *)stPara, APP_YES))
    {
        WRITE_ERR_LOG("Call app_reqDwmCmd() failed.\n");
    } 
    app_freeBuffer(stPara);
}


void CDwnCmd::_reqDwnQuery(STReqActCmd* pReq)
{
    //msg_stDwnQuery dwnQuery;
    msg_st2000DwnQuery dwnQuery;
    dwnQuery.num = pReq->uiparams(0);
    dwnQuery.queryType = pReq->uiparams(1);
    
    if (dwnQuery.queryType == xc8002_dwnmach_report_compensation_type || dwnQuery.queryType == xc8002_dwnmach_report_motorline_type)
    {
        COamMng::CompBackFlg = VOS_NO;
    }
    
    app_print(ps_dwnCmd,"DwnQuery:subsys(%u)\n",pReq->uisubsysid());
    app_print(ps_dwnCmd,"queryType=%d \n", dwnQuery.queryType);
    if (VOS_OK != app_reqDwmCmd(SPid_DwnCmd, pReq->uisubsysid(), Act_DwnQuery, sizeof(dwnQuery), (char *)&dwnQuery, APP_YES))
    {
        WRITE_ERR_LOG("Call app_reqDwmCmd() failed.\n");
    } 
    return;
}

void CDwnCmd::_reqMove(STReqActCmd* pReq)
{
    switch (pReq->uisubsysid())
    {
        case SPid_SmpDsk:
        case SPid_ReactDsk:
        case SPid_R2Dsk:
        case SPid_R1Dsk:
            return _reqDskMove(pReq);
            break;
        default:
            return _reqNdlMove(pReq);
    }  
}

void CDwnCmd::_reqDskMove(STReqActCmd* pReq)
{
    act_stReagSysDskRtt dskHrotate;
    dskHrotate.usCupId = pReq->uiparams(0);
    dskHrotate.usDstPos = pReq->uiparams(1);

    if (VOS_OK != app_reqDwmCmd(SPid_DwnCmd, pReq->uisubsysid(), Act_Move, sizeof(dskHrotate), (char *)&dskHrotate, APP_YES))
    {
        WRITE_ERR_LOG("Call app_reqDwmCmd() failed.\n");
    }
    return;
}

void CDwnCmd::_reqNdlMove(STReqActCmd* pReq)
{
    msg_stNdlRtt ndlRtt;
    ndlRtt.usDstPos = pReq->uiparams(0);

    app_print(ps_dwnCmd,"NdlMove:subsys(%u)\n",pReq->uisubsysid());
    app_print(ps_dwnCmd,"Position=%d \n", ndlRtt.usDstPos);
   
    if (VOS_OK != app_reqDwmCmd(SPid_DwnCmd, pReq->uisubsysid(), Act_Move, sizeof(ndlRtt), (char *)&ndlRtt, APP_YES))
    {
        WRITE_ERR_LOG("Call app_reqDwmCmd() failed.\n");
    } 
    return;
    
}

void CDwnCmd::_reqDskHrotate(STReqActCmd* pReq)
{
    msg_stDskHrotate dskHrotate;
    dskHrotate.dskId = pReq->uiparams(0);
    dskHrotate.dir   = pReq->uiparams(1);
    dskHrotate.num   = pReq->uiparams(2);

    app_print(ps_dwnCmd,"DskHrotate:subsys(%u)\n",pReq->uisubsysid());
    app_print(ps_dwnCmd,"dskId=%d, dir=%d, num=%d \n", dskHrotate.dskId, dskHrotate.dir, dskHrotate.num);
    if (VOS_OK != app_reqDwmCmd(SPid_DwnCmd, pReq->uisubsysid(), Act_DskHrotate, sizeof(dskHrotate), (char *)&dskHrotate, APP_YES))
    {
        WRITE_ERR_LOG("Call app_reqDwmCmd() failed.\n");
    } 
    return;
}

void CDwnCmd::_reqNdlVrt(STReqActCmd* pReq)
{
    msg_stVrt ndlVrt;

    ndlVrt.dir = pReq->uiparams(0);
    ndlVrt.marginFlag = 0;
    ndlVrt.totalVol = 0;
    ndlVrt.cupId = 1;
    ndlVrt.diskId = 0;
    ndlVrt.reactCupId = 0;

    app_print(ps_dwnCmd,"NdlVrt:subsys(%u)\n",pReq->uisubsysid());
    app_print(ps_dwnCmd,"dir=%d (0-up,1-down)\n",ndlVrt.dir);
    if (VOS_OK != app_reqDwmCmd(SPid_DwnCmd, pReq->uisubsysid(), Act_NdlVrt, sizeof(ndlVrt), (char *)&ndlVrt, APP_YES))
    {
        WRITE_ERR_LOG("Call app_reqDwmCmd() failed.\n");
    } 
    return;
}

void CDwnCmd::_reqMix(STReqActCmd* pReq)
{
    app_print(ps_dwnCmd,"Mix:subsys(%u)\n",pReq->uisubsysid());
    if (VOS_OK != app_reqDwmCmd(SPid_DwnCmd, pReq->uisubsysid(), Act_Mix, 0, VOS_NULL, APP_YES))
    {
        WRITE_ERR_LOG("Call app_reqDwmCmd() failed.\n");
    }
    return;
}

void CDwnCmd::_reqWsah(STReqActCmd* pReq)
{
    vos_u32 paraLen = 0;
    char* pPara = VOS_NULL;
    if (SPid_AutoWash == pReq->uisubsysid())
    {
        act_stReactSysAutoWash autoWash;
        autoWash.usWashOutBitMap = pReq->uiparams(0);
        pPara = (char*)&autoWash;
        paraLen = sizeof(autoWash);
    }
    
    app_print(ps_dwnCmd,"Wsah:subsys(%u)\n",pReq->uisubsysid());
    if (VOS_OK != app_reqDwmCmd(SPid_DwnCmd, pReq->uisubsysid(), Act_Wash, paraLen, pPara, APP_YES))
    {
        WRITE_ERR_LOG("Call app_reqDwmCmd() failed.\n");
    }
    return; 
}

void CDwnCmd::_reqLightCtl(STReqActCmd* pReq)
{
    msg_stLightControl lightCtl;

    lightCtl.lightIntensity = pReq->uiparams(0);
    
    app_print(ps_dwnCmd,"LightCtl:\n");
    app_print(ps_dwnCmd,"lightIntensity= %d\n", lightCtl.lightIntensity);
    if (VOS_OK != app_reqDwmCmd(SPid_DwnCmd, SPid_ReactDsk, Act_Light, sizeof(lightCtl), (char *)&lightCtl, APP_YES))
    {
        WRITE_ERR_LOG("Call app_reqDwmCmd() failed.\n");
    }
    return;
}

void CDwnCmd::_reqMachineDebug(STReqActCmd* pReq)
{
    msg_stMachineDebug debug;
    
    debug.operation = pReq->uiparams(0);
    app_print(ps_dwnCmd,"MachineDebug:\n");
    app_print(ps_dwnCmd,"operation= %u\n", debug.operation);
    
    if (VOS_OK != app_reqDwmCmd(SPid_DwnCmd, pReq->uisubsysid(), Act_Debug, sizeof(debug), (char *)&debug, APP_YES))
    {
        WRITE_ERR_LOG("Call app_reqDwmCmd() failed.\n");
    }
    return;
}

void CDwnCmd::_reqTempSwitch(STReqActCmd* pReq)
{
    msg_stTempSwitch tempSwitch;
    
    tempSwitch.switchState = pReq->uiparams(0);
    app_print(ps_dwnCmd,"TempSwitch:\n");
    app_print(ps_dwnCmd,"switchState = %u\n", tempSwitch.switchState);
    
    if (VOS_OK != app_reqDwmCmd(SPid_DwnCmd, pReq->uisubsysid(), Act_TempSwitch, sizeof(tempSwitch), (char *)&tempSwitch, APP_YES))
    {
        WRITE_ERR_LOG("Call app_reqDwmCmd() failed.\n");
    }
    return;
}

vos_u32 CDwnCmd::_procResVers(vos_msg_header_stru* pMsg)
{
    if (m_versions.vers_size() < 12)
    {
        CMsg stMsg;
        stMsg.ParseMsg(pMsg);
        STSubVers*  pRes = dynamic_cast<STSubVers*>(stMsg.pPara);
        STSubVers* subVers = m_versions.add_vers();
        subVers->CopyFrom(*pRes);
        if (m_versions.vers_size() >= 12)
        {
            return  _sendVersions();
        }
    }   
    return VOS_OK;
}

vos_u32 CDwnCmd::_procQueryVres()
{
    m_versions.Clear();
    startTimer(m_ulQueryTimer,30);//保护时间3秒
    
    msg_stDwnQuery dwnQuery;
    dwnQuery.queryType = xc8002_dwnmach_report_downmachin_versions;
    //12个单片机
    for (vos_u32 subSys = 0; subSys < 12; subSys++)
    {
        app_reqDwmCmd(SPid_DwnCmd, subSys+900, Act_DwnQuery, sizeof(dwnQuery), (char *)&dwnQuery, APP_NO);
    }
    return VOS_OK;
}


vos_u32 CDwnCmd::_procTimeOut(vos_msg_header_stru* pMsg)
{
    vos_timeroutmsg_stru* pstmsg = (vos_timeroutmsg_stru*)pMsg;
    if (pstmsg->ultimerid == m_ulQueryTimer)
    {
        return _sendVersions();
    }
    app_reportmidmachstatus(vos_midmachine_state_standby, SPid_DwnCmd);
    return VOS_OK;
}

vos_u32 CDwnCmd::_sendVersions()
{
    STSubVers* midVers = m_versions.add_vers();
    midVers->set_ulsubsysid(0);
    midVers->set_strtime(PBUILTTIME);
    midVers->set_strversions(PBUILDVERSION);
    midVers->set_fpgaver(0);

    vos_stoptimer(m_ulQueryTimer);
    app_reportmidmachstatus(vos_midmachine_state_standby, SPid_DwnCmd);
    return sendUpMsg(CPid_Maintain, SPid_DwnCmd, MSG_ResVres, &m_versions);
}

void CDwnCmd::_reqBcScan(STReqActCmd* pReq)
{
    msg_stReagBcScanPara bcScan;
    bcScan.usCupId = pReq->uiparams(0);

    app_print(ps_dwnCmd, "BcScan(%u):\n", pReq->uisubsysid());
    app_print(ps_dwnCmd, "CupId = %u\n", bcScan.usCupId);
    if (VOS_OK != app_reqDwmCmd(SPid_DwnCmd, pReq->uisubsysid(), Act_BcScan, sizeof(bcScan), (char *)&bcScan, APP_YES))
    {
        WRITE_ERR_LOG("Call app_reqDwmCmd() failed.\n");
    }
    return;
}

vos_u32 CDwnCmd::_procTempeQuery()
{
    app_reqDwmCmd(SPid_DwnCmd, SPid_RTmpCtrl, Act_TempQuery, 0, 0, APP_YES);
    return app_reqDwmCmd(SPid_DwnCmd, SPid_WTmpCtrl, Act_TempQuery, 0, 0, APP_YES);
// 
//     msg_stDwnQuery dwnQuery;
//     dwnQuery.queryType = xc8002_dwnmach_report_reagdsk_temp_query_type;
//     if (VOS_OK != app_reqDwmCmd(SPid_DwnCmd, SPid_CoolReag, Act_DwnQuery, sizeof(dwnQuery), (char *)&dwnQuery, APP_NO))
//     {
//         WRITE_ERR_LOG("Call app_reqDwmCmd() failed.\n");
//     }
// 
// 	dwnQuery.queryType = xc8002_dwnmach_report_electricity_type;
// 	if (VOS_OK != app_reqDwmCmd(SPid_DwnCmd, SPid_CoolReag, Act_DwnQuery, sizeof(dwnQuery), (char *)&dwnQuery, APP_NO))
// 	{
// 		WRITE_ERR_LOG("Call app_reqDwmCmd() failed.\n");
// 	}
// 
//     dwnQuery.queryType = xc8002_dwnmach_report_thermal_control_temp_type;
//     if (VOS_OK != app_reqDwmCmd(SPid_DwnCmd, SPid_RTmpCtrl, Act_DwnQuery, sizeof(dwnQuery), (char *)&dwnQuery, APP_NO))
//     {
//         WRITE_ERR_LOG("Call app_reqDwmCmd() failed.\n");
//     }
//     return VOS_OK;
}
vos_u32 CDwnCmd::_procTempeADQuery()
{
	msg_stDwnQuery dwnQuery;
	dwnQuery.queryType = xc8002_dwnmach_report_ad_temp_type;
	if (VOS_OK != app_reqDwmCmd(SPid_DwnCmd, SPid_RTmpCtrl, Act_DwnQuery, sizeof(dwnQuery), (char *)&dwnQuery, APP_NO))
	{
		WRITE_ERR_LOG("Call app_reqDwmCmd() failed.\n");
	}
	return VOS_OK;
}

void CDwnCmd::_reqPVCtrl(STReqActCmd* pReq)
{
    msg_PVCtrl PVCtrl;
    PVCtrl.PVNum = pReq->uiparams(0);
    PVCtrl.operateType = pReq->uiparams(1);

    app_print(ps_dwnCmd, "PV(%u):\n", PVCtrl.PVNum);
    app_print(ps_dwnCmd, "operateType = %u\n", PVCtrl.operateType);
    if (VOS_OK != app_reqDwmCmd(SPid_DwnCmd, pReq->uisubsysid(), Act_PVCtrl, sizeof(PVCtrl), (char *)&PVCtrl, APP_YES))
    {
        WRITE_ERR_LOG("Call app_reqDwmCmd() failed.\n");
    }
    return;
}

void CDwnCmd::_reqLiquidDBG(STReqActCmd* pReq)
{
    msg_liquidDBG liquidDBG;
    liquidDBG.debugType = LiquidDBG_Type(pReq->uiparams(0));
    //发往子系统
    if (liquidDBG.debugType == washDBG_CMVA || liquidDBG.debugType == washDBG_OMVA || liquidDBG.debugType == washDBG_WR || liquidDBG.debugType == washDBG_WW)
    {
        if (VOS_OK != app_reqDwmCmd(SPid_DwnCmd, pReq->uisubsysid(), pReq->uiacttype(), sizeof(msg_liquidDBG), (char *)&liquidDBG, APP_YES))
        {
            WRITE_ERR_LOG("Call app_reqDwmCmd() failed.\n");
        }
    }
    else if (liquidDBG.debugType == app_accuracyAddS || liquidDBG.debugType == app_accuracyAddR1 || liquidDBG.debugType == app_accuracyAddR2)
    {
		sendInnerMsg(SPid_Maintain, SPid_DwnCmd, MSG_STAddLiquid, pReq);
    }
    else
    {
		sendInnerMsg(SPid_Maintain, SPid_DwnCmd, app_maintain_msg_type, pReq);
    }
    return;
}

vos_u32 CDwnCmd::_procADtest(vos_msg_header_stru* pMsg)
{
    CMsg stMsg;
    stMsg.ParseMsg(pMsg);
    STSwitch*  pReq = dynamic_cast<STSwitch*>(stMsg.pPara);
    if (pReq->uiswitch())
    {
        if (pReq->uipara() == 1)
        {
            return staticTestAD();
        }
        return dynamicTsetAD();
    }
    return VOS_OK;
}

void CDwnCmd::_reqSetFlState(STReqActCmd* pReq)
{
    msg_PVCtrl FlState;
    FlState.PVNum = pReq->uiparams(0);
    FlState.operateType = pReq->uiparams(1);
    app_reqDwmCmd(SPid_DwnCmd, SPid_AutoWash, Act_SetFlState, sizeof(FlState), (char *)&FlState, APP_YES);
}

void CDwnCmd::_reqPumpCtrl(STReqActCmd* pReq)
{
    msg_PumpCtrl pumpCtrl;
    pumpCtrl.dir = pReq->uiparams(0);
    pumpCtrl.volume = pReq->uiparams(1);
    app_reqDwmCmd(SPid_DwnCmd, pReq->uisubsysid(), Act_PumpCtrl, sizeof(pumpCtrl), (char *)&pumpCtrl, APP_YES);
}

void CDwnCmd::_reqOlnyMix(STReqActCmd* pReq)
{    
    WRITE_INFO_LOG("_reqOlnyMix:subsys(%u)\n",pReq->uisubsysid());
    if (VOS_OK != app_reqDwmCmd(SPid_DwnCmd, pReq->uisubsysid(), Act_OlnyMix, 0, VOS_NULL, APP_YES))
    {
        WRITE_ERR_LOG("Call app_reqDwmCmd() failed.\n");
    }
}

void CDwnCmd::_reqSaveMotor(STReqActCmd* pReq)
{
    vos_u32 ulResult = VOS_OK;
    vos_u32 ulParaLen = 0;
    msg_stMotor * pMotor = VOS_NULL;

    ulParaLen = sizeof(msg_stMotor) + sizeof(msg_stMotorPara);
    ulResult = app_mallocBuffer(ulParaLen, (void**)(&pMotor));
    if (VOS_OK != ulResult)
    {
        WRITE_ERR_LOG("Call app_mallocBuffer() failed(%d)\n", ulResult);
        return;
    }

    pMotor->paranum = 1;
    msg_stMotorPara * pPara = (msg_stMotorPara *)pMotor->pPara;

    app_print(ps_dwnCmd, "SaveMotor:subsys(%u)(%d)\n", pReq->uisubsysid(), pMotor->paranum);
    pPara->drvId = pReq->uiparams(0);
    pPara->lineId = pReq->uiparams(1);
    pPara->startFre = pReq->uiparams(2);
    pPara->endFre = pReq->uiparams(3);
    pPara->echLaderStep = pReq->uiparams(4);
    pPara->upMaxLader = pReq->uiparams(5);
    pPara->sPar = pReq->uiparams(6);

    if (VOS_OK != app_reqDwmCmd(SPid_DwnCmd, pReq->uisubsysid(), Act_WriteMotor, ulParaLen, (char *)pMotor, APP_YES))
    {
        WRITE_ERR_LOG("Call app_reqDwmCmd() failed.\n");
    }
    app_freeBuffer(pMotor);
}

void CDwnCmd::_reqSetWTemp(STReqActCmd* pReq)
{
    msg_stWTemp stWTemp;
    stWTemp.temp1 = pReq->uiparams(0);
    stWTemp.temp2 = pReq->uiparams(1);
    stWTemp.temp3 = pReq->uiparams(2); 
    app_reqDwmCmd(SPid_DwnCmd, pReq->uisubsysid(), Act_SetWTemp, sizeof(stWTemp), (char *)&stWTemp, APP_YES);
}

void CDwnCmd::_reqSetRTemp(STReqActCmd* pReq)
{
    msg_stRTemp stRTemp;
    stRTemp.tempreact = pReq->uiparams(0);
    stRTemp.delta = pReq->uiparams(1);
    app_reqDwmCmd(SPid_DwnCmd, pReq->uisubsysid(), Act_SetRTemp, sizeof(stRTemp), (char *)&stRTemp, APP_YES);
}

vos_u32 CDwnCmd::_simulStaticTestAD()
{
	STTestAD msg;
	for (int i = 0; i < 1; i++)
	{
		STTestVal* stVal = msg.add_stval();
		stVal->set_uicup(i);
		for (int j = 0; j < WAVE_NUM; j++)
		{
			stVal->add_uival(42000+j);
		}
	}
	return sendUpMsg(CPid_Maintain, SPid_DwnCmd, MSG_ADtest, &msg);
}

void CDwnCmd::_procResMsg(vos_msg_header_stru* pMsg)
{
    app_com_dev_act_result_stru* pActRes = (app_com_dev_act_result_stru*)pMsg->aucmsgload;
    app_u32 subSys = pMsg->ulsrcpid;
    app_u32 actType = pActRes->enacttype;

    map<app_u32, map<app_u32, ProcResFuc> >::iterator iter = m_mResFuncs.find(subSys);
    if (iter != m_mResFuncs.end())
    {
        map<app_u32, ProcResFuc>::iterator item = iter->second.find(actType);
        if (item != iter->second.end())
        {
            (this->*(item->second))(subSys, pActRes);
        }
    }
}

void CDwnCmd::_procReactTemp(app_u32 subSys, app_com_dev_act_result_stru* pRes)
{
    ReactTemp_st *pPara = (ReactTemp_st*)pRes->aucrsltinfo;
    STTempQuery stPara;
    stPara.set_ulsubsysid(SPid_RTmpCtrl);
    STKeyValue *pData = stPara.add_temp();
    pData->set_uikey(0);
    pData->set_uival(pPara->slavertemp);
    pData = stPara.add_temp();
    pData->set_uikey(1);
    pData->set_uival(pPara->slcurtemp1);
    pData = stPara.add_temp();
    pData->set_uikey(2);
    pData->set_uival(pPara->slcurtemp2);
    pData = stPara.add_temp();
    pData->set_uikey(3);
    pData->set_uival(pPara->slcurtemp3);
    pData = stPara.add_temp();
    pData->set_uikey(4);
    pData->set_uival(pPara->slcurtemp4);
    sendUpMsg(CPid_Maintain, SPid_DwnCmd, MSG_TempeQuery, &stPara);
}

void CDwnCmd::_procWaterTemp(app_u32 subSys, app_com_dev_act_result_stru* pRes)
{
    WarterTemp_st *pPara = (WarterTemp_st*)pRes->aucrsltinfo;
    STTempQuery stPara;
    stPara.set_ulsubsysid(SPid_RTmpCtrl);
    STKeyValue *pData = stPara.add_temp();
    pData->set_uikey(5);
    pData->set_uival(pPara->slmachine2);
    pData = stPara.add_temp();
    pData->set_uikey(6);
    pData->set_uival(pPara->slmachine1);
    pData = stPara.add_temp();
    pData->set_uikey(7);
    pData->set_uival(pPara->slcleaner);
    pData = stPara.add_temp();
    pData->set_uikey(8);
    pData->set_uival(pPara->slwater);
    sendUpMsg(CPid_Maintain, SPid_DwnCmd, MSG_TempeQuery, &stPara);

}

void CDwnCmd::_procShowWave(vos_msg_header_stru* pMsg)
{
    CMsg stMsg;
    stMsg.ParseMsg(pMsg);
    STReqShowSmooth*  pReq = dynamic_cast<STReqShowSmooth*>(stMsg.pPara);

    app_reqDwmCmd(Pid_DebugTty, SPid_ADInner, Act_Showave, 0, NULL, APP_NO);
    app_reqDwmCmd(Pid_DebugTty, SPid_ADOuter, Act_Showave, 0, NULL, APP_NO);

    act_stReactsysDskRtt stPara2;
    stPara2.usRttedCupNum = pReq->count();
    stPara2.dir = cmd_motor_dir_CLOCKWISE;
    app_reqDwmCmd(Pid_DebugTty, SPid_ReactDsk, Act_Move, sizeof(stPara2), (char*)&stPara2, APP_NO);
}
