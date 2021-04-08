#include "msg.h"
#include "vos_log.h"
#include <sstream>

using namespace std;

CMsg::CMsg() : ulSrcPid(0), ulMsgType(0), pPara(NULL)
{
}

CMsg::~CMsg()
{
    if (NULL != pPara)
    {
        delete(pPara);
        pPara = NULL;
    }
}

bool CMsg::ParseMsg(vos_msg_header_stru* pMsg)
{
    ulSrcPid = pMsg->ulsrcpid;
    ulMsgType = (vos_u32)pMsg->usmsgtype;

    if (0 == pMsg->usrsv)
    {
        return false;
    }

    string strName(pMsg->aucmsgload, pMsg->usrsv);
    pPara = _createMsg(strName);
    if (NULL == pPara)
    {
        return false;
    }

    if (!pPara->ParseFromArray(pMsg->aucmsgload + pMsg->usrsv, pMsg->usmsgloadlen - pMsg->usrsv))
    {
        return false;
    }

    _traceMsg(pMsg->uldstpid);
    return true;
}

Message * CMsg::_createMsg(const string & typeName)
{
    Message * pMsg = NULL;
    const Descriptor * pDescriptor = DescriptorPool::generated_pool()->FindMessageTypeByName(typeName);
    if (pDescriptor)
    {
        const Message * pPrototype = MessageFactory::generated_factory()->GetPrototype(pDescriptor);
        if (pPrototype)
        {
            pMsg = pPrototype->New();
        }
    }
    return pMsg;
}

void CMsg::_traceMsg(vos_u32 ulPid)
{
    if (SPid_Log != ulPid)
    {
        ostringstream ss;
        ss << "Received Msg:SrcId=" << ulSrcPid << ", DstId=" << ulPid << ", MsgId=" << ulMsgType << "\n";
        if (NULL != pPara)
        {
            ss << pPara->GetTypeName() << "\n";
            ss << pPara->DebugString();
        }
        writeLog(3, ss.str().c_str());
    }
}


