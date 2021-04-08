#ifndef __L2SYS_H__
#define __L2SYS_H__

#include "subsys.h"
#include "app_downmsg.h"
#include <map>

class CL2Sys : public CSubSys
{
public:
    CL2Sys(vos_u32 ulPid, vos_u32 ulDstSubsys);
    ~CL2Sys();

private:
    //初始化解析函数
    void _initParseFuc();
    vos_u32 _parseReset(msg_stComDevActInfo* pComAct);
    vos_u32 _parseMove(msg_stComDevActInfo* pComAct);
};

#endif