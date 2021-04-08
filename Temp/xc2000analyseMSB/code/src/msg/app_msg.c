#include "app_msg.h"
#include <stdlib.h>
#include <stdio.h>
#include "app_msgtypedefine.h"
#include "vos_errorcode.h"
#include "vos_messagecenter.h"
#include "vos_init.h"
#include "vos_log.h"
#include <sstream>

using namespace std;

void app_constructMsgHead(msg_stHead * pHead, vos_u32 dstPid, vos_u32 srcPid, vos_u32 msgType)
{
    memset(pHead, 0, sizeof(msg_stHead));
    pHead->ulDstPid = dstPid;
    pHead->ulSrcPid = srcPid;
    pHead->ulMsgType = msgType;
}

void app_traceMsg(msg_stHead * pHead, google::protobuf::Message* pPara)
{
    if (SPid_Log != pHead->ulDstPid)
    {
        ostringstream ss;
        ss << " Send Msg:SrcId=" << pHead->ulSrcPid << ", DstId=" << pHead->ulDstPid << ", MsgId=" << pHead->ulMsgType << "\n";
        if (NULL != pPara)
        {
            ss << pPara->GetTypeName() << "\n";
            ss << pPara->DebugString();
        }
        writeLog(3, ss.str().c_str());
    }
}

vos_u32 sendMsg(msg_stHead * pHead, google::protobuf::Message* pPara,bool bOuter)
{
    if (NULL == pPara)
    {
        return app_sendMsg(pHead, NULL, 0, 0, bOuter);
    }
    app_traceMsg(pHead, pPara);

    vos_u16 ulNameLen = pPara->GetTypeName().size();
    vos_u16 ulDataLen = pPara->ByteSize();
    vos_u32 ulParaLen = ulNameLen + ulDataLen;

    void * pBuff = VOS_NULL;
    vos_u32 ulResult = app_mallocBuffer(ulParaLen, &pBuff);
    if (VOS_OK != ulResult)
    {
        WRITE_ERR_LOG("Call app_mallocBuffer() failed(0x%x)\n", ulResult);
        return ulResult;
    }

    void * pData = NULL;
    ulResult = app_mallocBuffer(ulDataLen, &pData);
    if (VOS_OK != ulResult)
    {
        app_freeBuffer(pBuff);
        return ulResult;
    }

    if (!pPara->SerializeToArray(pData, ulDataLen))
    {
        app_freeBuffer(pBuff);
        app_freeBuffer(pData);
        return vos_malloc_mem_err;
    }
    memcpy(pBuff, pPara->GetTypeName().c_str(), ulNameLen);
    memcpy(((char*)pBuff) + ulNameLen, pData, ulDataLen);
    app_freeBuffer(pData);
    return app_sendMsg(pHead, pBuff, ulParaLen, ulNameLen, bOuter);
}

vos_u32 sendUpMsg(vos_u32 dstPid, vos_u32 srcPid, vos_u32 msgType, google::protobuf::Message* pPara)
{
    msg_stHead stHead;
    app_constructMsgHead(&stHead, dstPid, srcPid, msgType);
    return sendMsg(&stHead, pPara, true);
}

vos_u32 sendInnerMsg(vos_u32 dstPid, vos_u32 srcPid, vos_u32 msgType, google::protobuf::Message* pPara)
{
    msg_stHead stHead;
    app_constructMsgHead(&stHead, dstPid, srcPid, msgType);
    return sendMsg(&stHead, pPara, false);
}

vos_u32 app_sendMsg(msg_stHead * pHead, void* pPara, vos_u32 ulParaLen, vos_u16 ulNameLen, bool bOuter)
{
    vos_msg_header_stru * pstreqmsg = VOS_NULL;
    vos_u32 ulRunRslt = VOS_OK;
    vos_u32 ulMsgLen = 0;

    ulMsgLen = sizeof(vos_msg_header_stru) + ulParaLen;
    ulRunRslt = vos_mallocmsgbuffer(ulMsgLen, (void**)(&pstreqmsg));
    if (VOS_OK != ulRunRslt)
    {
        app_freeBuffer(pPara);
        return ulRunRslt;
    }

    pstreqmsg->uldstpid = pHead->ulDstPid;
    pstreqmsg->ulsrcpid = pHead->ulSrcPid;
    pstreqmsg->usmsgtype = pHead->ulMsgType;
    pstreqmsg->usmsgloadlen = ulParaLen;
    pstreqmsg->usrsv = ulNameLen;

    if (VOS_NULL != pPara)
    {
        memcpy(&(pstreqmsg->aucmsgload[0]), pPara, ulParaLen);
        app_freeBuffer(pPara);
    }
    return vos_sendmsg(pstreqmsg, bOuter);
}

vos_u32 app_mallocBuffer(vos_u32 ulBufLen, void** pBuffer)
{
    void*  pMem = VOS_NULL;
    pMem = (void*)malloc(ulBufLen);
    if (VOS_NULL == pMem)
    {
        return vos_malloc_mem_err;
    }
    memset(pMem, 0, ulBufLen);
    *pBuffer = pMem;
    return VOS_OK;
}
void app_freeBuffer(void * pBuffer)
{
    if (NULL != pBuffer)
    {
        free(pBuffer);
        pBuffer = VOS_NULL;
    }
}

