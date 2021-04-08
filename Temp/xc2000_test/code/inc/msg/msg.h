#ifndef __MSG_H__
#define __MSG_H__

#include "google/protobuf/message.h"
#include "vos_basictypedefine.h"
#include <map>
#include "upmsg.pb.h"
#include "typedefine.pb.h"
#include "vos_messagecenter.h"

using namespace std;
using namespace google::protobuf;

//#define MSG_STAddLiquid 1001

class CMsg
{
public:
    CMsg();
    ~CMsg();

public:
    bool ParseMsg(vos_msg_header_stru* pMsg);
   
    vos_u32 ulSrcPid;          //源PID
    vos_u32 ulMsgType;      //消息类型
    Message *pPara;

private:  
    void _traceMsg(vos_u32 ulPid);

    Message * _createMsg(const string & typeName);
};

#endif
