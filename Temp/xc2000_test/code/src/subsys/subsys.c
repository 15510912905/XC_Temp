#include "subsys.h"
#include "vos_timer.h"
#include <string.h>
#include "app_msg.h"
#include "app_errorcode.h"
#include "vos_log.h"
#include "apputils.h"
#include <sstream>
#include "app_comstructdefine.h"

#define SUBSYS_TIMEOUT_LEN 320

bool CSubSys::m_bReactDisk = false;
bool CSubSys::m_bAutoWash = false;
bool CSubSys::m_bRmix = false;
bool CSubSys::m_bSMix = false;

CSubSys::CSubSys(vos_u32 ulPid, vos_u32 ulDstSubsys)
    : CCallBack(ulPid)
    , m_ulDstSubsys(ulDstSubsys)
    , m_ulBusy(APP_NO)
{
    stringstream ss;
    ss << "SubSys" << ulPid;
    m_ulTimer = createTimer(ulPid, ss.str().c_str(), SUBSYS_TIMEOUT_LEN);
    memset(&m_stHead, 0, sizeof(StMsgHead));

    m_parseFucs[Act_UpdateDwn] = &CSubSys::ParseUpdateDwn;
    m_parseFucs[Act_WriteMotor] = &CSubSys::ParseWriteMotor;
	m_parseFucs[Act_Debug] = &CSubSys::ParseMachineDBG;
    m_parseFucs[Act_SaveOffset] = &CSubSys::ParseSaveOffset;
}

CSubSys::~CSubSys()
{
    _clearMsgBuf();
}
vos_u32 CSubSys::CallBack(vos_msg_header_stru* pMsg)
{
    if (VOS_NULL == pMsg)
    {
        WRITE_ERR_LOG("Input parameter is null \n");
        return app_para_null_err;
    }

    vos_u32 ulResult = VOS_OK;
    switch (pMsg->usmsgtype)
    {
        case MSG_ReqReset:
        case app_com_device_req_exe_act_msg_type:
        {
            ulResult = ProcReqMsg(pMsg);
            break;
        }
        case app_req_stop_midmach_msg_type:
        {
            _clearCurAct();
            _clearMsgBuf();
            break;
        }
        case app_res_dwnmach_act_cmd_msg_type:
        {
            _procResMsg(pMsg);
            ulResult = _procReqMsg();
            break;
        }
        case vos_pidmsgtype_timer_timeout:
        {
            WRITE_ERR_LOG("ACT Timeout subsys=%lu\n", m_ulPid);
            _initialize();
            ulResult = _procReqMsg();
            break;
        }
        default:
        {
            WRITE_ERR_LOG("Unknown msg type(%d) \n", pMsg->usmsgtype);
            ulResult = GENERRNO(app_reactsys_mng_errorcode_base, app_unknown_msg_type_err);
            break;
        }
    }
    return ulResult;
}
vos_u32 CSubSys::ProcReqMsg(vos_msg_header_stru* pMsg)
{
    vos_u32 ulResult = VOS_OK;
    ulResult = _pushMsgToBuf(pMsg);
    if (VOS_OK != ulResult)
    {
        WRITE_ERR_LOG("_pushMsgToBuf failed(%lu)\n", ulResult);
        return ulResult;
    }
    return _procReqMsg();
}

void CSubSys::_parseMsgHead(vos_msg_header_stru* pMsg)
{
    m_stHead.usResMsgType = app_com_device_res_exe_act_msg_type;
    if (MSG_ReqReset == pMsg->usmsgtype)
    {
        m_stHead.usResMsgType = MSG_ResReset;
    }
    m_stHead.ulSrcPid = pMsg->ulsrcpid;
}
STActInfo * CSubSys::InitActInfo(msg_stActHead& stActHead, vos_u32 cmdLen)
{
    STActInfo* pActInfo = VOS_NULL;
    vos_u32 bufLen = sizeof(STActInfo) + cmdLen;
    vos_u32 ulResult = app_mallocBuffer(bufLen, (void**)(&pActInfo));
    if (VOS_OK != ulResult)
    {
        WRITE_ERR_LOG("Call app_mallocBuffer() failed(%d) \n", ulResult);
        return VOS_NULL;
    }

    memcpy(&pActInfo->stActHead, &stActHead, sizeof(msg_stActHead));
    pActInfo->ulCmdInfoLen = cmdLen;

    m_stCurAct.push((char*)pActInfo);
    return pActInfo;
}

vos_u32 CSubSys::_pushMsgToBuf(vos_msg_header_stru* pMsg)
{
    vos_u32 uBufLen = pMsg->usmsgloadlen + sizeof(vos_msg_header_stru);
    char* pMsgBuf = VOS_NULL;
    vos_u32 ulResult = app_mallocBuffer(uBufLen, (void **)(&pMsgBuf));
    if (VOS_OK != ulResult)
    {
        WRITE_ERR_LOG("Call app_mallocBuffer() failed(%d) \n", ulResult);
        return ulResult;
    }
    memcpy(pMsgBuf, pMsg, uBufLen);
    m_stMsgBuf.push(pMsgBuf);
    return VOS_OK;
}

vos_u32 CSubSys::_procReqMsg()
{
    if (APP_YES == m_ulBusy)
    {
        return VOS_OK;
    }
    vos_msg_header_stru* pMsg = (vos_msg_header_stru*)_popMsgFromBuf();
    if (VOS_NULL == pMsg)
    {
        return VOS_OK;
    }
    m_ulBusy = APP_YES;
    _parseMsgHead(pMsg);
    vos_u32 ulResult = ParseActInfo(pMsg);
    if (VOS_OK == ulResult)
    {
        ulResult = _sendCmdToDwnmach();
	}
    if (VOS_OK != ulResult)
    {
        WRITE_ERR_LOG("_procReqMsg failed(%lu).\n", ulResult);
        _initialize();
    }
    app_freeBuffer(pMsg);
    return ulResult;
}

char* CSubSys::_popMsgFromBuf()
{
    char* pMsgBuf = VOS_NULL;
    if (!m_stMsgBuf.empty())
    {
        pMsgBuf = m_stMsgBuf.front();
        m_stMsgBuf.pop();
    }
    return pMsgBuf;
}

vos_u32 CSubSys::_sendCmdToDwnmach()
{
    if (m_stCurAct.empty())
    {
        return app_inexistence_action_err;
    }
    STActInfo * pActInfo = (STActInfo *)m_stCurAct.front();
    if (!_checkAct(m_ulPid,pActInfo->stActHead.usActType))
    {
        WRITE_ERR_LOG("\n----SYS[%d],ACT[%d] CAN NOT BE SEND----\n", m_ulPid, pActInfo->stActHead.usActType);
        return app_ctrl_statemach_busy_err;
    }
    _setAct(m_ulPid, pActInfo->stActHead.usActType, true);
    return  _sendCmdAct(pActInfo);
}

vos_u32 CSubSys::_reportRes(STActInfo* pActInfo, vos_u32 ulResult)
{
    vos_stoptimer(m_ulTimer);
    vos_settimertimelen(m_ulTimer, SUBSYS_TIMEOUT_LEN);
    _setAct(m_ulPid, pActInfo->stActHead.usActType, false);
    if (APP_NO == pActInfo->stActHead.usSingleRes)
    {
        return VOS_OK;
    }

    app_com_dev_act_result_stru* pPara = VOS_NULL;
    vos_u32 ulParaLen = sizeof(app_com_dev_act_result_stru) + pActInfo->usRsltInfoLen;
    vos_u32 ulRunRslt = app_mallocBuffer(ulParaLen, (void**)(&pPara));
    if (VOS_OK != ulRunRslt)
    {
        WRITE_ERR_LOG("Call app_mallocBuffer() failed(%d) \n", ulRunRslt);
        return ulRunRslt;
    }
    pPara->enacttype = pActInfo->stActHead.usActType;
    pPara->usreactcupid = pActInfo->stActHead.usRctCupId;
    pPara->usdevtype = m_ulDstSubsys;
    pPara->ulActIndex = pActInfo->stActHead.ulActIndex;
    pPara->enactrslt = ulResult;
    if (0 != pActInfo->usRsltInfoLen)
    {
        memcpy((char*)(&(pPara->aucrsltinfo[0])), (char*)(&(pActInfo->aucActRsltInfo[0])), pActInfo->usRsltInfoLen);
        pPara->usrsltinfolen = pActInfo->usRsltInfoLen;
    }

    msg_stHead stHead;
    app_constructMsgHead(&stHead, m_stHead.ulSrcPid, m_ulPid, m_stHead.usResMsgType);
    return app_sendMsg(&stHead, (char *)pPara, ulParaLen);
}

vos_u32 CSubSys::_sendCmdAct(STActInfo * pActInfo)
{
    CMD_Single_TYPE* pCmp = (CMD_Single_TYPE*)pActInfo->stDwnCmd;
    if ((m_ulPid == SPid_ReactDsk) && (pCmp->cmd == CMD_DSK_RESET))
    {
        vos_settimertimelen(m_ulTimer, 120000);
    }
    vos_u32 ulResult = vos_starttimer(m_ulTimer);
    if (VOS_OK != ulResult)
    {
        WRITE_ERR_LOG("Call vos_starttimer() failed(0x%x) \n", ulResult);
        return ulResult;
    }
    msg_stDwnmachActionReq*  pPara = VOS_NULL;
    vos_u32 ulParaLen = pActInfo->ulCmdInfoLen + sizeof(msg_stDwnmachActionReq);
    ulResult = app_mallocBuffer(ulParaLen, (void**)(&pPara));
    if (VOS_OK != ulResult)
    {
        WRITE_ERR_LOG("Call app_mallocBuffer() failed(%d) \n", ulResult);
        return ulResult;
    }
    pPara->enDwnmachSubSys = m_ulDstSubsys;
    pPara->usDevType = pActInfo->stActHead.usDevType;
    memcpy((char*)&(pPara->aucCmdLoad), (char*)&(pActInfo->stDwnCmd), pActInfo->ulCmdInfoLen);
    app_print(ps_cmdSch,"TIME %ld sendCmdAct subsys =%lu cmd = %#x\n",getTime_ms(),m_ulPid-900,*(vos_u16*)&(pActInfo->stDwnCmd));
    msg_stHead stHead;
    app_constructMsgHead(&stHead, Pid_DwnMachUart, m_ulPid, app_req_dwnmach_act_cmd_msg_type);
    return app_sendMsg(&stHead, pPara, ulParaLen);   
}

vos_u32 CSubSys::_procResMsg(vos_msg_header_stru* pMsg)
{
    app_dwnmach_action_res_msg_stru* pRes = (app_dwnmach_action_res_msg_stru*)pMsg;

    if (m_stCurAct.empty())
    {
        if (VOS_YES == m_ulBusy)
        {
            m_ulBusy = VOS_NO;
            vos_stoptimer(m_ulTimer);
            return VOS_OK;
        }
        WRITE_ERR_LOG("NO Act,res subsys=%lu,cmd=%#x\n", pRes->endwnmachsubsys, pRes->encmdtype);
        return vos_list_no_sps_node_err;
    }

    if (EN_AF_Success != pRes->encmdexerslt)
    {
        WRITE_ERR_LOG("Act Failed,res subsys=%lu,cmd=%lu\n", pRes->endwnmachsubsys, pRes->encmdtype);
        _initialize();
        _clearMsgBuf();
        return VOS_OK;
    }
    STActInfo * pCurAct = (STActInfo *)m_stCurAct.front();
    if (0 != pRes->ulpayloadlen)
    {
        memcpy((char*)(&(pCurAct->aucActRsltInfo[0])), (char*)(&(pRes->aucpayload[0])), pRes->ulpayloadlen);
        pCurAct->usRsltInfoLen = pRes->ulpayloadlen;
    }
	
    _reportRes(pCurAct, pRes->encmdexerslt);
	m_stCurAct.pop();
    app_freeBuffer(pCurAct);
    vos_u32 ulResult = _sendCmdToDwnmach();
    if (VOS_OK != ulResult)
    {
        _initialize();
    }    
    return ulResult;
}

void CSubSys::_clearCurAct()
{
    STActInfo * pActInfo = NULL;
    while (!m_stCurAct.empty())
    {
        pActInfo = (STActInfo *)m_stCurAct.front();
        m_stCurAct.pop();
        app_freeBuffer(pActInfo);
    }
    m_bSMix = false;
    m_bRmix = false;
    m_bReactDisk = false;
    m_bAutoWash = false;
}

void CSubSys::_initialize()
{
    STActInfo * pActInfo = NULL;
    while (!m_stCurAct.empty())
    {
        pActInfo = (STActInfo *)m_stCurAct.front();
        if (VOS_NULL != pActInfo)
        {
            _reportRes(pActInfo, app_unknown_timer_error);
        }
        m_stCurAct.pop();
        app_freeBuffer(pActInfo);
    }
    memset(&m_stHead, 0, sizeof(StMsgHead));
    m_ulBusy = APP_NO;
}

void CSubSys::_clearMsgBuf()
{
    char* pMsgBuf = VOS_NULL;
    while (!m_stMsgBuf.empty())
    {
        pMsgBuf = m_stMsgBuf.front();
        app_freeBuffer(pMsgBuf);
        m_stMsgBuf.pop();
    }
}

bool CSubSys::_checkAct(app_u32 subSys, app_u32 actType)
{
    if (subSys == SPid_ReactDsk && (actType == Act_Move || actType == Act_Reset)
        && (m_bSMix || m_bRmix || m_bAutoWash))
    {
        return false;
    }
    if (subSys == SPid_SMix && (actType == Act_Mix || actType == Act_Reset || actType == Act_Wash)
        && m_bReactDisk)
    {
        return false;
    }
    if (subSys == SPid_R2Mix && (actType == Act_Mix || actType == Act_Reset || actType == Act_Wash)
        && m_bReactDisk)
    {
        return false;
    }
    if (subSys == SPid_AutoWash && actType == Act_Wash
        && m_bReactDisk)
    {
        return false;
    }
    return true;
}

void CSubSys::_setAct(app_u32 subSys, app_u32 actType, bool bAct)
{
    if (subSys == SPid_ReactDsk && (actType == Act_Move || actType == Act_Reset))
    {
        m_bReactDisk = bAct;
    }
    if (subSys == SPid_SMix && (actType == Act_Mix || actType == Act_Reset || actType == Act_Wash))
    {
        m_bSMix = bAct;
    }
    if (subSys == SPid_R2Mix && (actType == Act_Mix || actType == Act_Reset || actType == Act_Wash))
    {
        m_bRmix = bAct;
    }
    if (subSys == SPid_AutoWash && actType == Act_Wash)
    {
        m_bAutoWash = bAct;
    }
}

vos_u32 CSubSys::ParseSingleCMD(msg_stComDevActInfo* pComAct)
{
    vos_u32 cmdSize = sizeof(CMD_Single_TYPE);

    STActInfo* pActInfo = InitActInfo(pComAct->stActHead, cmdSize);
    if (VOS_NULL == pActInfo)
    {
        WRITE_ERR_LOG("Call InitActInfo() failed(VOS_NULL) \n");
        return vos_malloc_mem_err;
    }

    CMD_Single_TYPE* pCmp = (CMD_Single_TYPE*)pActInfo->stDwnCmd;
    pCmp->cmd = m_mActCMD[pComAct->stActHead.usActType];
    return VOS_OK;
}

vos_u32 CSubSys::ParseTrimOffset(msg_stComDevActInfo* pComAct, vos_u32 cmd)
{
    vos_u32 cmdLen = sizeof(cmd_stStepMove);
    STActInfo* pActInfo = InitActInfo(pComAct->stActHead, cmdLen);
    if (VOS_NULL == pActInfo)
    {
        WRITE_ERR_LOG("Call InitActInfo() failed(VOS_NULL) \n");
        return vos_malloc_mem_err;
    }
    
    msg_stStepMove* pPara = (msg_stStepMove*)pComAct->aucPara;
    cmd_stStepMove* stepMove = (cmd_stStepMove*)pActInfo->stDwnCmd;
    stepMove->cmd = cmd;
    stepMove->motorType = pPara->motorType;
    stepMove->dir = pPara->dir;
    stepMove->step = pPara->step;
    
    return VOS_OK;
}

vos_u32 CSubSys::ParseSaveOffset(msg_stComDevActInfo* pComAct)
{
    msg_stSaveOffset* pPara = (msg_stSaveOffset*)pComAct->aucPara;
    vos_u32 cmdLen = sizeof(CMD_COMMON_DATA_RW_PAGE_DATA_TYPE) + pPara->paranum*sizeof(vos_u32);       
    WRITE_INFO_LOG("SaveOffset count-%d\n", pPara->paranum);
    STActInfo* pActInfo = InitActInfo(pComAct->stActHead, cmdLen);
    if (VOS_NULL == pActInfo)
    {
        WRITE_ERR_LOG("Call InitActInfo() failed(VOS_NULL) \n");
        return vos_malloc_mem_err;
    }

    CMD_COMMON_DATA_RW_PAGE_DATA_TYPE* compensation = (CMD_COMMON_DATA_RW_PAGE_DATA_TYPE*)pActInfo->stDwnCmd;
    compensation->cmd = CMD_COM_RW_PARAMETER_PAGE;
    compensation->number = pPara->type;
    compensation->rw = (pPara->paranum == 0) ? 0 : 1;//0¶Á£¬1Ð´
    if (pPara->paranum>0)
    {
        memcpy(compensation->datas, pPara->pPara, pPara->paranum*sizeof(vos_u32));
        vos_u32*pData = (vos_u32*)compensation->datas;
        for (vos_u32 i = 0; i < pPara->paranum; i++)
        {
            WRITE_INFO_LOG("SaveOffset:(%d)\n", *pData);
            pData++;
        }
    }   
    return VOS_OK;
}

vos_u32 CSubSys::ParseDwnQuery(msg_stComDevActInfo* pComAct)
{
    vos_u32 cmdLen = sizeof(cmd_stDwnQuery);
    STActInfo* pActInfo = InitActInfo(pComAct->stActHead, cmdLen);
    if (VOS_NULL == pActInfo)
    {
        WRITE_ERR_LOG("Call InitActInfo() failed(VOS_NULL) \n");
        return vos_malloc_mem_err;
    }
    
    //msg_stDwnQuery* pPara = (msg_stDwnQuery*)pComAct->aucPara;
    msg_st2000DwnQuery* pPara = (msg_st2000DwnQuery*)pComAct->aucPara;
    cmd_st2000DwnQuery* dwnQuery = (cmd_st2000DwnQuery*)pActInfo->stDwnCmd;
    dwnQuery->cmd = XC8002_COM_COMQUERY_CMD;
    dwnQuery->num = pPara->num;
    dwnQuery->queryType = pPara->queryType;
    return VOS_OK;
}

vos_u32 CSubSys::ParseMachineDBG(msg_stComDevActInfo* pComAct)
{
    vos_u32 cmdLen = sizeof(cmd_stMachineDebug);
    STActInfo* pActInfo = InitActInfo(pComAct->stActHead, cmdLen);
    if (VOS_NULL == pActInfo)
    {
        WRITE_ERR_LOG("Call InitActInfo() failed(VOS_NULL) \n");
        return vos_malloc_mem_err;
    }
    
    msg_stMachineDebug* pPara = (msg_stMachineDebug*)pComAct->aucPara;
    cmd_stMachineDebug* pDebug = (cmd_stMachineDebug*)pActInfo->stDwnCmd;
    pDebug->cmd = 1;
    pDebug->operation= pPara->operation;

    return VOS_OK;
}

vos_u32 CSubSys::ParseDwnFault(msg_stComDevActInfo* pComAct)
{
    vos_u32 cmdLen = sizeof(cmd_stRsmFault);
    STActInfo* pActInfo = InitActInfo(pComAct->stActHead, cmdLen);
    if (VOS_NULL == pActInfo)
    {
        WRITE_ERR_LOG("Call InitActInfo() failed(VOS_NULL) \n");
        return vos_malloc_mem_err;
    }
    
    msg_stRsmFault* pPara = (msg_stRsmFault*)pComAct->aucPara;
    cmd_stRsmFault* pCmd = (cmd_stRsmFault*)pActInfo->stDwnCmd;
    pCmd->cmd = XC8002_COM_FAULT_RESTORE_CMD;
    pCmd->rsv = 0;
	pCmd->faultId = pPara->faultId;
	pCmd->boardId = pPara->boardId;
	pCmd->deviceId = pPara->deviceId;

    return VOS_OK;
}

vos_u32 CSubSys::ParseMidState(msg_stComDevActInfo* pComAct)
{
    vos_u32 cmdLen = sizeof(cmd_stMidState);
    STActInfo* pActInfo = InitActInfo(pComAct->stActHead, cmdLen);
    if (VOS_NULL == pActInfo)
    {
        WRITE_ERR_LOG("Call InitActInfo() failed(VOS_NULL) \n");
        return vos_malloc_mem_err;
    }
    
    msg_stMidState* pPara = (msg_stMidState*)pComAct->aucPara;
    cmd_stMidState* pCmd = (cmd_stMidState*)pActInfo->stDwnCmd;
    pCmd->cmd = XC8002_COM_MIDRUNSTATE_CMD;
    pCmd->midState = pPara->midState;

    return VOS_OK;
}

vos_u32 CSubSys::ParseBBCLEAR(msg_stComDevActInfo* pComAct)
{
    vos_u32 cmdLen = sizeof(msg_stOneCmd);
    STActInfo* pActInfo = InitActInfo(pComAct->stActHead, cmdLen);
    if (VOS_NULL == pActInfo)
    {
        WRITE_ERR_LOG("Call InitActInfo() failed(VOS_NULL) \n");
        return vos_malloc_mem_err;
    }
    cmd_stMidState* pCmd = (cmd_stMidState*)pActInfo->stDwnCmd;
    pCmd->cmd = XC8002_COM_BBCLEAR_CMD;
    return VOS_OK;
}

vos_u32 CSubSys::ParseActInfo(vos_msg_header_stru* pMsg)
{
    vos_u32 ulResult = VOS_OK;
    msg_stComDevActReq* pActReq = (msg_stComDevActReq*)pMsg->aucmsgload;
    msg_stComDevActInfo* pComAct = (msg_stComDevActInfo*)(pActReq->astActInfo);
    for (app_u16 i = 0; i < pActReq->usActNum; ++i)
    {
        ulResult = app_req_func_act_invalid_err;
        map<app_u16, ParseAct>::iterator iter = m_parseFucs.find(pComAct->stActHead.usActType);
        if (iter != m_parseFucs.end())
        {
            ulResult = (this->*(iter->second))(pComAct);
		}
        if (VOS_OK != ulResult)
        {
            WRITE_ERR_LOG("SPid=%lu,Split the original act=%lu failed(0x%x) \r\n", m_ulPid, pComAct->stActHead.usActType, ulResult);
            break;
        }
        pComAct = (msg_stComDevActInfo*)(((char*)pComAct) + pComAct->ulDataLen + sizeof(pComAct->ulDataLen));
    }
    return ulResult;
}

vos_u32 CSubSys::ParseUpdateDwn(msg_stComDevActInfo* pComAct)
{
    vos_u32 cmdLen = sizeof(cmd_updateDwn);
    STActInfo* pActInfo = InitActInfo(pComAct->stActHead, cmdLen);
    if (VOS_NULL == pActInfo)
    {
        WRITE_ERR_LOG("Call InitActInfo() failed(VOS_NULL) \n");
        return vos_malloc_mem_err;
    }

    msg_updateDwn* pPara= (msg_updateDwn*)pComAct->aucPara;
    cmd_updateDwn * pDwnCmd = (cmd_updateDwn *)&(pActInfo->stDwnCmd);

    pDwnCmd->cmd = XC8002_COM_UPDATEDWN_CMD;
    pDwnCmd->exeCmd = pPara->exeCmd;
    pDwnCmd->dataPage = pPara->dataPage;
    memcpy(pDwnCmd->data, pPara->data, sizeof(pPara->data));

    return VOS_OK;
}

vos_u32 CSubSys::ParseWriteMotor(msg_stComDevActInfo* pComAct)
{
    msg_stMotor* pPara = (msg_stMotor*)pComAct->aucPara;
    vos_u32 cmdLen = sizeof(xc8002_report_dwnmachine_motorspeed) + (pPara->paranum)*sizeof(xc8002_report_motor_para);
    STActInfo* pActInfo = InitActInfo(pComAct->stActHead, cmdLen);
    if (VOS_NULL == pActInfo)
    {
        WRITE_ERR_LOG("Call InitActInfo() failed(VOS_NULL) \n");
        return vos_malloc_mem_err;
    }

    xc8002_report_dwnmachine_motorspeed * pDwnCmd = (xc8002_report_dwnmachine_motorspeed *)&(pActInfo->stDwnCmd);

    pDwnCmd->cmd = XC8002_COM_SET_MOTOR_CMD;
    pDwnCmd->enstatustype = xc8002_dwnmach_report_motorline_type;
    pDwnCmd->dwnmachinetype = m_ulDstSubsys;
    pDwnCmd->paranum = pPara->paranum;

    memcpy(pDwnCmd->ppara, pPara->pPara, (pPara->paranum)*sizeof(xc8002_report_motor_para));

    return VOS_OK;
}
vos_u32 CSubSys::ParseUpFpga(msg_stComDevActInfo* pComAct, vos_u32 cmd)
{
    vos_u32 cmdLen = sizeof(cmd_updateFpga);
    STActInfo* pActInfo = InitActInfo(pComAct->stActHead, cmdLen);
    if (VOS_NULL == pActInfo)
    {
        WRITE_ERR_LOG("Call InitActInfo() failed(VOS_NULL) \n");
        return vos_malloc_mem_err;
    }

    msg_updateFpga* pPara = (msg_updateFpga*)pComAct->aucPara;
    cmd_updateFpga* pCmd = (cmd_updateFpga*)pActInfo->stDwnCmd;
    pCmd->cmd = cmd;
    pCmd->flag = pPara->flag;
    pCmd->dataPage = pPara->dataPage;
    pCmd->length = pPara->length;
    memcpy(pCmd->data, pPara->data, sizeof(pPara->data));

    return VOS_OK;
}

vos_u32 CSubSys::ParseNdlReset(msg_stComDevActInfo* pComAct)
{
    vos_u32 cmdSize = sizeof(CMD_NDL_RESET_TYPE);

    STActInfo* pActInfo = InitActInfo(pComAct->stActHead, cmdSize);
    if (VOS_NULL == pActInfo)
    {
        WRITE_ERR_LOG("Call InitActInfo() failed(VOS_NULL) \n");
        return vos_malloc_mem_err;
    }

    //msg_stAirout *pAiroutInfo = (msg_stAirout *)(pComAct->aucPara);
    CMD_NDL_RESET_TYPE* pCmd = (CMD_NDL_RESET_TYPE*)&(pActInfo->stDwnCmd);
    pCmd->cmd = CMD_NDL_RESET;
    pCmd->dis_wash = 0;
    pCmd->time = 0;
    return VOS_OK;
}