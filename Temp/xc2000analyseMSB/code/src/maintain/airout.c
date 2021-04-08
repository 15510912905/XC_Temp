#include "airout.h"
#include "vos_timer.h"
#include "typedefine.pb.h"
#include "app_msgtypedefine.h"
#include "app_comstructdefine.h"
#include "app_maintain.h"
#include "upmsg.pb.h"
#include "socketmonitor.h"
#include "apputils.h"
#include "vos_log.h"
#include "app_msg.h"

#define  NDLNUM 3
#define  AIROUT_TIME_LEN 800

CAirOut::CAirOut(vos_u32 ulPid) : CCallBack(ulPid), m_ulResNum(0)
{
    m_ulTimer = createTimer(SPid_AirOut, "AirOut", AIROUT_TIME_LEN);
}

CAirOut::~CAirOut()
{
}

vos_u32 CAirOut::CallBack(vos_msg_header_stru* pMsg)
{
    CMsg stMsg;
    if (stMsg.ParseMsg(pMsg))
    {
        if (!g_pMonitor->CanHandleCmd())
        {
            WRITE_ERR_LOG("AirOut failed, status=%lu \n", g_pMonitor->m_ulState);
            return VOS_OK;
        }
        app_reportmidmachstatus(vos_midmachine_state_air_out);
        if (MSG_ReqAirOut == stMsg.ulMsgType)
        {
            return _procAirOut(stMsg);
        }
    }
    if (app_com_device_res_exe_act_msg_type == pMsg->usmsgtype)
    {
        _procActRes(pMsg);
    }
	
    if (vos_pidmsgtype_timer_timeout == pMsg->usmsgtype)
    {
        WRITE_ERR_LOG("AirOut time out.\n");
        _procResult(EN_AF_Failed);
    }
	
	if (app_req_stop_midmach_msg_type == pMsg->usmsgtype)
    {
        m_ulResNum = 0;
        return vos_stoptimer(m_ulTimer);
    }
    return VOS_OK;
}

void CAirOut::_procActRes(vos_msg_header_stru* pMsg)
{
    app_com_dev_act_result_stru * pstmvmsg = (app_com_dev_act_result_stru *)pMsg->aucmsgload;
    WRITE_INFO_LOG("procRes srcpid=%d, acttype= 0x%x, rslt= %u\n", pMsg->ulsrcpid, pstmvmsg->enacttype, pstmvmsg->enactrslt);
    if (EN_AF_Success != pstmvmsg->enactrslt)
    {
        vos_stoptimer(m_ulTimer);
        _procResult(EN_AF_Failed);
        return;
    }
    m_ulResNum += 1;
    if (NDLNUM == m_ulResNum)
    {
        vos_stoptimer(m_ulTimer);
        _procResult(EN_AF_Success);
    }
}

vos_u32 CAirOut::_procAirOut(CMsg& stMsg)
{
    OPEN_P04(SPid_AirOut);
    STReqAirOut * pAirOut = dynamic_cast<STReqAirOut*>(stMsg.pPara);

    //样本针排空气
    vos_u32 ulResult = _reqAirOut(SPid_SmpNdl, pAirOut->uiouttimes());
    if (VOS_OK != ulResult)
    {
        WRITE_ERR_LOG("_reqAirOut dstpid=%d failed.\n", SPid_SmpNdl);
        return ulResult;
    }

    //R1排空气
    ulResult = _reqAirOut(SPid_R1, pAirOut->uiouttimes());
    if (VOS_OK != ulResult)
    {
        WRITE_ERR_LOG("_reqAirOut dstpid=%d failed.\n", SPid_R1);
        return ulResult;
    }

    //R2排空气
    ulResult = _reqAirOut(SPid_R2, pAirOut->uiouttimes());
    if (VOS_OK != ulResult)
    {
        WRITE_ERR_LOG("_reqAirOut dstpid=%d failed.\n", SPid_R2);
        return ulResult;
    }

    return vos_starttimer(m_ulTimer);
}

vos_u32 CAirOut::_reqAirOut(vos_u32 ulDstPid, vos_u32 uiOutTimes)
{
    msg_stComDevActReq* pPara = VOS_NULL;
    vos_u32 ulParaLen = sizeof(msg_stComDevActReq) + sizeof(msg_stComDevActInfo) * 2 + sizeof(msg_stAirout);
    vos_u32 ulResult = app_mallocBuffer(ulParaLen, (void**)(&pPara));
    if (VOS_OK != ulResult)
    {
        WRITE_ERR_LOG("Call app_mallocBuffer() failed(0x%x) \n", ulResult);
        return ulResult;
    }
    pPara->usActNum = 2;

    msg_stComDevActInfo * pActInfo = (msg_stComDevActInfo*)(&(pPara->astActInfo[0]));
    pActInfo->ulDataLen = sizeof(msg_stActHead);
    pActInfo->stActHead.usActType = Act_Reset;
    pActInfo->stActHead.usSingleRes = APP_NO;

    pActInfo = (msg_stComDevActInfo *)(((char*)pActInfo) + sizeof(msg_stComDevActInfo));
    pActInfo->ulDataLen = sizeof(msg_stActHead) + sizeof(msg_stAirout);
    pActInfo->stActHead.usActType = Act_AirOut;
    pActInfo->stActHead.usSingleRes = APP_YES;
    msg_stAirout * pActPara = (msg_stAirout *)(pActInfo->aucPara);
    pActPara->usAirOutTimes = uiOutTimes;

    msg_stHead stHead;
    app_constructMsgHead(&stHead, ulDstPid, SPid_AirOut, app_com_device_req_exe_act_msg_type);
    return app_sendMsg(&stHead, pPara, ulParaLen);
}

void CAirOut::_procResult(int result)
{
    if (EN_AF_Success==result)
    {
        app_reportmidmachstatus(vos_midmachine_state_standby);
    }
    else
    {
        app_reportmidmachstatus(vos_midmachine_state_air_out_err);
    }
    CLOSE_P04(SPid_AirOut);
    m_ulResNum = 0;
}