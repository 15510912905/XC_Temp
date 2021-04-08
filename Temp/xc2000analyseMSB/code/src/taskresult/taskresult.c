#include "taskresult.h"
#include "typedefine.pb.h"
#include "apputils.h"
#include "vos_log.h"
#include "msg.h"
#include "app_msg.h"
#include "socketmonitor.h"
#include "vos_errorcode.h"

CTaskResult::CTaskResult() : CCallBack(SPid_TaskResult)
{
    _initTaskResult();
}
CTaskResult::~CTaskResult()
{    
}
void CTaskResult::_initTaskResult()
{
    m_msgFucs[MSG_ResAddTask] = &CTaskResult::_onNewResult;
    m_msgFucs[MSG_AckTaskResult] = &CTaskResult::_onResultAck;
    m_msgFucs[MSG_NotifyReportResult] = &CTaskResult::_onReportResult;
}

vos_u32 CTaskResult::CallBack(vos_msg_header_stru* pMsg)
{
    map<vos_u32, OnMsg>::iterator iter = m_msgFucs.find(pMsg->usmsgtype);
    if (iter != m_msgFucs.end())
    {
        return (this->*(iter->second))(pMsg);
    }
    WRITE_ERR_LOG("Unknown msg type(0x%lu)(pid:%d->%d)\n ", pMsg->usmsgtype, pMsg->ulsrcpid, pMsg->uldstpid);
    return VOS_OK;
}

vos_u32 CTaskResult::_sendTaskResult(vos_msg_header_stru* pMsg)
{
    CMsg stMsg;
    if (!stMsg.ParseMsg(pMsg))
    {
        WRITE_ERR_LOG("ParseMsg failed\n");
        return vos_para_invalid_err;
    }
    return sendUpMsg(pMsg->ulsrcpid, SPid_Task, MSG_ResAddTask, stMsg.pPara);
}

vos_u32 CTaskResult::_onNewResult(vos_msg_header_stru* pMsg)
{
    vos_u32 ulMsgLen = pMsg->usmsgloadlen + sizeof(vos_msg_header_stru);
    char* pPara = VOS_NULL;
    vos_u32 ulResult = app_mallocBuffer(ulMsgLen, (void**)(&pPara));
    if (VOS_OK != ulResult)
    {
        WRITE_ERR_LOG("call vos_mallocmemory failed(0x%x)\r\n", ulResult);
        return ulResult;
    }

    memcpy(pPara, (char*)pMsg, ulMsgLen);
    if (m_qTaskResults.empty())
    {
        ulResult = _sendTaskResult((vos_msg_header_stru*)(pPara));
    }
    m_qTaskResults.push(pPara);
    return ulResult;
}

vos_u32 CTaskResult::_onResultAck(vos_msg_header_stru* pMsg)
{
    char * pPara = m_qTaskResults.front();
    m_qTaskResults.pop();
    app_freeBuffer(pPara);

    if (!m_qTaskResults.empty())
    {
        pPara = m_qTaskResults.front();
        return _sendTaskResult((vos_msg_header_stru*)(pPara));
    }
    return VOS_OK;
}

vos_u32 CTaskResult::_onReportResult(vos_msg_header_stru* pMsg)
{
    if (m_qTaskResults.empty())
    {
        return VOS_OK;
    }
    char *pPara = m_qTaskResults.front();
    return _sendTaskResult((vos_msg_header_stru*)(pPara));
}

