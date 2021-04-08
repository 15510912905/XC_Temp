#ifndef __INNER_DSK_H__
#define __INNER_DSK_H__

#include "subsys.h"
#include "app_downmsg.h"
#include <map>

class CInnerDsk : public CSubSys
{
public:
    CInnerDsk(vos_u32 ulPid);
    ~CInnerDsk();

private:
    //初始化解析函数
    void _initParseFuc();

    vos_u32 _parseMove(msg_stComDevActInfo* pComAct);
    vos_u32 _parseBcScan(msg_stComDevActInfo* pComAct);

    //解析微步调整
    vos_u32 _parseTrimOffset(msg_stComDevActInfo* pComAct);

    //正、逆时针转动
    vos_u32 _parseDskHrotate(msg_stComDevActInfo* pComAct);

	//虚拟示波器开关控制
	vos_u32 ParseShowave(msg_stComDevActInfo* pComAct);
};

#endif