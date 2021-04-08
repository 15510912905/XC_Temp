#ifndef __SMPDSK_H__
#define __SMPDSK_H__

#include "subsys.h"
#include "app_downmsg.h"
#include <map>

class CSmpDsk : public CSubSys
{
public:
    CSmpDsk(vos_u32 ulPid, vos_u32 ulDstSubsys);
    ~CSmpDsk();

private:
    //初始化解析函数
    void _initParseFuc();

    vos_u32 _parseMove(msg_stComDevActInfo* pComAct);
	vos_u32 _parseBcScan(msg_stComDevActInfo* pComAct);
	
	//微步调整
    vos_u32 _parseTrimOffset(msg_stComDevActInfo* pComAct);
	
    //正、逆时针转动
    vos_u32 _parseDskHrotate(msg_stComDevActInfo* pComAct);
    
    //样本指示灯控制
    vos_u32 _parseLightCtrl(msg_stComDevActInfo* pComAct);

	//虚拟示波器开关控制
	vos_u32 ParseShowave(msg_stComDevActInfo* pComAct);

    //升级逻辑
    vos_u32 _parseUpFpga(msg_stComDevActInfo* pComAct);
#ifdef Version_A
    vos_u32 _parseGetShelf(msg_stComDevActInfo* pComAct);
    vos_u32 _parseFreeShelf(msg_stComDevActInfo* pComAct);
#else
    vos_u32 _parseGetIn(msg_stComDevActInfo* pComAct);
    vos_u32 _parseFreeShelf(msg_stComDevActInfo* pComAct);
    vos_u32 _parseFreeShelfIn(msg_stComDevActInfo* pComAct);
    vos_u32 _parseGetShelf(msg_stComDevActInfo* pComAct);
#endif
};

#endif