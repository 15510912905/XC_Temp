#pragma once
#include "subsys.h"
#include "app_downmsg.h"
#include <map>

class CLight :public CSubSys
{
public:
    CLight(vos_u32 ulPid, vos_u32 ulDstSubsys);
    ~CLight();
private:
    void _initParseFuc();  
    vos_u32 _parseSetLight(msg_stComDevActInfo* pComAct);
};