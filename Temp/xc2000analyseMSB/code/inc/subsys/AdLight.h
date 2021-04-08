#pragma once
#include "subsys.h"
#include "app_downmsg.h"
#include <map>

class CAdLight :public CSubSys
{
public:
    CAdLight(vos_u32 ulPid, vos_u32 ulDstSubsys);
    ~CAdLight();
private:
    void _initParseFuc();

    vos_u32 _parseReadAD(msg_stComDevActInfo* pComAct);
    vos_u32 _parseReadOneAD(msg_stComDevActInfo* pComAct);
    vos_u32 _parseSetGain(msg_stComDevActInfo* pComAct);
    vos_u32 _parseShowWave(msg_stComDevActInfo* pComAct);
};

