#ifndef __COOL_REAG_H__
#define __COOL_REAG_H__

#include "subsys.h"
#include "app_downmsg.h"
#include <map>

class CCoolReag : public CSubSys
{
public:
    CCoolReag(vos_u32 ulPid, vos_u32 ulDstSubsys);
    ~CCoolReag();

private:
    //初始化解析函数
    void _initParseFuc();

    //解析温度查询
    vos_u32 _parseTempQuery(msg_stComDevActInfo* pComAct);
    //制冷开关
    vos_u32 _parseCoolingCtrl(msg_stComDevActInfo* pComAct);
};

#endif
