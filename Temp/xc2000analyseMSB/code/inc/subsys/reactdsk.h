#ifndef __REACT_DSK_H__
#define __REACT_DSK_H__

#include "subsys.h"
#include "app_downmsg.h"
#include <map>

class CReactDsk : public CSubSys
{
public:
    CReactDsk(vos_u32 ulPid);
    ~CReactDsk();

private:
    //初始化解析函数
    void _initParseFuc();

    //解析自动清洗动作
    vos_u32 _parseReset(msg_stComDevActInfo* pComAct);
    vos_u32 _parseMove(msg_stComDevActInfo* pComAct);
    vos_u32 _parseLight(msg_stComDevActInfo* pComAct);

    vos_u32 _parseAd(msg_stComDevActInfo* pComAct);
        
    //解析微步调整
    vos_u32 _parseTrimOffset(msg_stComDevActInfo* pComAct);

    //正、逆时针转动
    vos_u32 _parseDskHrotate(msg_stComDevActInfo* pComAct);  
};

#endif