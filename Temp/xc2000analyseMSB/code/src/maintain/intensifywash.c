#include "intensifywash.h"
#include "vos_log.h"
#include "apputils.h"
#include "app_msg.h"
#include "socketmonitor.h"
#include "app_comstructdefine.h"
#include "app_xc8002shareinfo.h"

#define WAIT_RES_NUM     3   //等待响应的组件数

enum ExeStep
{
    RESET = 1,
    INTENSIFY_WASH = 2,
};

enum ExeTimeLen
{
    RESET_LEN = 300,
    INTENSIFY_WASH_LEN = 1500,
};

CIntensifyWash::CIntensifyWash(vos_u32 ulPid) : CCallBack(ulPid),m_ulResNum(0), m_ulStep(0), m_ulSrcPid(0)
{
    m_ulTimer = createTimer(SPid_IntensifyWash, "IntensifyWash");
    signDsk = false;
}

CIntensifyWash::~CIntensifyWash()
{

}

vos_u32 CIntensifyWash::CallBack(vos_msg_header_stru* pMsg)
{
    if (MSG_ReqIntensifyWash == pMsg->usmsgtype)
    {
        return _procReq(pMsg);
    }
    if (MSG_ResReset == pMsg->usmsgtype)
    {
        return _proResetRes(pMsg);
    }
    if (app_com_device_res_exe_act_msg_type == pMsg->usmsgtype)
    {
        return _procActRes(pMsg);
    }
    if (vos_pidmsgtype_timer_timeout == pMsg->usmsgtype)
    {
        WRITE_ERR_LOG("timeout\n");
        return _reportRes(EN_AF_Failed);
    }
    if (app_req_stop_midmach_msg_type == pMsg->usmsgtype)
    {
        m_ulStep = 0;
        return vos_stoptimer(m_ulTimer);
    }
    WRITE_ERR_LOG("Unknow msgtype=%lu\n", pMsg->usmsgtype);
    return VOS_OK;
}

vos_u32 CIntensifyWash::_procReq(vos_msg_header_stru* pMsg)
{
	CMsg stMsg;
	if (!stMsg.ParseMsg(pMsg))
		return VOS_FALSE;

	STReqIntensifyWash * inWash = dynamic_cast<STReqIntensifyWash*>(stMsg.pPara);

    if (!g_pMonitor->CanHandleCmd())
    {
        WRITE_ERR_LOG("IntensifyWash failed, status=%lu \n", g_pMonitor->m_ulState);
        return VOS_OK;
    }
    if (0 != m_ulStep)
    {
        WRITE_ERR_LOG("IntensifyWash is busy step=%d\n", m_ulStep);
        return VOS_OK;
    }
	m_ulSrcPid = pMsg->ulsrcpid;
    app_reportmidmachstatus(vos_midmachine_state_intensify_wash);
    if (VOS_OK != app_reqMachReset(SPid_IntensifyWash))
    {
        WRITE_ERR_LOG("MachReset Failed.\n");
        return _reportRes(EN_AF_Failed);
    }   
    m_ulStep = RESET;
    return startTimer(m_ulTimer, RESET_LEN);
}

vos_u32 CIntensifyWash::_reportRes(vos_u32 ulResult)
{
    CLOSE_P04(SPid_IntensifyWash);
    m_ulResNum = 0;
    m_ulStep = 0;

    if (EN_AF_Success != ulResult)
    {
        app_reportmidmachstatus(vos_midmachine_state_intensify_wash_err);
    }
    else
    {
        app_reportmidmachstatus(vos_midmachine_state_standby);
    }

    return vos_stoptimer(m_ulTimer);
}

vos_u32 CIntensifyWash::_proResetRes(vos_msg_header_stru* pMsg)
{
    if (RESET != m_ulStep)
    {
        WRITE_ERR_LOG("IntensifyWash Step=%d.\n", m_ulStep);
        return VOS_OK;
    }
    vos_stoptimer(m_ulTimer);

    CMsg stMsg;
    stMsg.ParseMsg(pMsg);
    STResResult * pPara = (STResResult *)stMsg.pPara;
    if (EN_AF_Success != pPara->uiresult())
    {
        WRITE_ERR_LOG("Reset Failed.\n");
        return _reportRes(EN_AF_Failed);
    }
    OPEN_P04(SPid_IntensifyWash);
    _procMoveSmpDsk();
    _procMoveReagDsk(R1_WASH_CUP, cmd_inner_reagentdisk_r1_absbpos, SPid_R1Dsk);
    _procMoveReagDsk(R2_WASH_CUP, cmd_inner_reagentdisk_r2_absbpos, SPid_R2Dsk);

    m_ulResNum = WAIT_RES_NUM;
    m_ulStep = INTENSIFY_WASH;
    return startTimer(m_ulTimer, INTENSIFY_WASH_LEN);
}

void CIntensifyWash::_procMoveSmpDsk()
{
    vos_u32 ulResult = VOS_OK;
    msg_stComDevActReq * pPara = VOS_NULL;
    vos_u32 ulParaLen = sizeof(msg_stComDevActReq) + sizeof(msg_stComDevActInfo) + sizeof(msg_paraSampleDskRtt);

    ulResult = app_mallocBuffer(ulParaLen, (void**)(&pPara));
    if (VOS_OK != ulResult)
    {
        WRITE_ERR_LOG("Call app_mallocBuffer() failed(%d)\n", ulResult);
        return;
    }
    pPara->usActNum = 1;
    msg_stComDevActInfo * pActInfo = (msg_stComDevActInfo *)pPara->astActInfo;
    pActInfo->ulDataLen = sizeof(msg_stActHead) + sizeof(msg_paraSampleDskRtt);
    app_setActHead(&pActInfo->stActHead, Act_Move, 0, 0, APP_YES);
    msg_paraSampleDskRtt * pMove = (msg_paraSampleDskRtt *)pActInfo->aucPara;
    pMove->usCircleId = cmd_sample_dsk_circle_4_id;
    pMove->usSmpCupId = SMP_W_POS;

    msg_stHead stHead;
    app_constructMsgHead(&stHead, SPid_SmpDsk, SPid_IntensifyWash, app_com_device_req_exe_act_msg_type);

    if (VOS_OK != app_sendMsg(&stHead, pPara, ulParaLen))
    {
        WRITE_ERR_LOG("Call app_sendMsg() failed.\n");
    }
}

void CIntensifyWash::_procMoveReagDsk(app_u16 usCupId, app_u16 usDstPos, vos_u32 ulDstPid)
{
    vos_u32 ulResult = VOS_OK;
    msg_stComDevActReq * pPara = VOS_NULL;
    vos_u32 ulParaLen = sizeof(msg_stComDevActReq) + sizeof(msg_stComDevActInfo) + sizeof(act_stReagSysDskRtt);

    ulResult = app_mallocBuffer(ulParaLen, (void**)(&pPara));
    if (VOS_OK != ulResult)
    {
        WRITE_ERR_LOG("Call app_mallocBuffer() failed(%d)\n", ulResult);
        return;
    }
    pPara->usActNum = 1;
    msg_stComDevActInfo * pActInfo = (msg_stComDevActInfo *)pPara->astActInfo;
    pActInfo->ulDataLen = sizeof(msg_stActHead) + sizeof(act_stReagSysDskRtt);
    app_setActHead(&pActInfo->stActHead, Act_Move, 0, 0, APP_YES);

    act_stReagSysDskRtt * pMove = (act_stReagSysDskRtt *)pActInfo->aucPara;
    pMove->usCupId = usCupId;
    pMove->usDstPos = usDstPos;

    msg_stHead stHead;
    app_constructMsgHead(&stHead, ulDstPid, SPid_IntensifyWash, app_com_device_req_exe_act_msg_type);

    if (VOS_OK != app_sendMsg(&stHead, pPara, ulParaLen))
    {
        WRITE_ERR_LOG("Call app_sendMsg() failed.\n");
    }
}

vos_u32 CIntensifyWash::_procActRes(vos_msg_header_stru* pMsg)
{
    if (INTENSIFY_WASH != m_ulStep)
    {
        WRITE_ERR_LOG("procActRes step=%lu.\n", m_ulStep);
        return VOS_OK;
    }
    app_com_dev_act_result_stru * pstmvmsg = (app_com_dev_act_result_stru *)pMsg->aucmsgload;
    WRITE_INFO_LOG("procRes srcpid=%d, acttype= 0x%x, rslt= %u\n", pMsg->ulsrcpid, pstmvmsg->enacttype, pstmvmsg->enactrslt);
    if (EN_AF_Success != pstmvmsg->enactrslt)
    {
        return _reportRes(EN_AF_Failed);
    }

    switch (pMsg->ulsrcpid)
    {
        case SPid_SmpDsk:
        {
            _sendIntensifyWash(SPid_SmpNdlb);
            return _sendIntensifyWash(SPid_SmpNdl);
        }
        case SPid_R1Dsk:
        {
            if (signDsk)
            {
                return _sendIntensifyWash(SPid_R1);
            }
            else{
                return _sendIntensifyWash(SPid_R1b);
            }
            
        }
        case SPid_R2Dsk:
        {
            if (signDsk)
            {
                return _sendIntensifyWash(SPid_R2);
            }
            else
            {
                return _sendIntensifyWash(SPid_R2b);
            }
            
        }
        case SPid_R1b:
        {
            signDsk = true;
            _procMoveReagDsk(R1_WASH_CUP, cmd_outer_reagentdisk_r1_absbpos, SPid_R1Dsk);
            return 0;
        }
        case SPid_R2:
        {
            _procMoveReagDsk(R2_WASH_CUP, cmd_outer_reagentdisk_r2_absbpos, SPid_R2Dsk);
            return 0;
        }
        case SPid_R1:
        {
            signDsk = false;
            break;
        }
        default:
            return _procActResult();
    }
}

vos_u32 CIntensifyWash::_sendIntensifyWash(vos_u32 ulDstPid)
{
    vos_u32 ulResult = VOS_OK;
    msg_stComDevActReq * pPara = VOS_NULL;
    vos_u32 ulParaLen = sizeof(msg_stComDevActReq) + sizeof(msg_stComDevActInfo) * 3;

    ulResult = app_mallocBuffer(ulParaLen, (void**)(&pPara));
    if (VOS_OK != ulResult)
    {
        WRITE_ERR_LOG("Call app_mallocBuffer() failed(%d)\n", ulResult);
        return ulResult;
    }
    pPara->usActNum = 3;
    msg_stComDevActInfo * pActInfo = (msg_stComDevActInfo *)pPara->astActInfo;
    pActInfo->ulDataLen = sizeof(msg_stActHead);
    app_setActHead(&pActInfo->stActHead, Act_IntensifyWash, 0, 0, APP_NO);

    pActInfo = (msg_stComDevActInfo *)(((char*)pActInfo) + sizeof(msg_stComDevActInfo));
    pActInfo->ulDataLen = sizeof(msg_stActHead);
    app_setActHead(&pActInfo->stActHead, Act_IntensifyWash, 0, 0, APP_NO);

    pActInfo = (msg_stComDevActInfo *)(((char*)pActInfo) + sizeof(msg_stComDevActInfo));
    pActInfo->ulDataLen = sizeof(msg_stActHead);
    app_setActHead(&pActInfo->stActHead, Act_IntensifyWash, 0, 0, APP_YES);

    msg_stHead stHead;
    app_constructMsgHead(&stHead, ulDstPid, SPid_IntensifyWash, app_com_device_req_exe_act_msg_type);

    return app_sendMsg(&stHead, pPara, ulParaLen);
}

vos_u32 CIntensifyWash::_procActResult()
{
    m_ulResNum -= 1;
    if (0 == m_ulResNum)
    {
        return _reportRes(EN_AF_Success);
    }
    return VOS_OK;
}

