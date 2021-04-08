#ifndef __AUTO_WASH_H__
#define __AUTO_WASH_H__

#include "subsys.h"
#include "app_downmsg.h"
#include <map>

class CAutoWash : public CSubSys
{
public:
    CAutoWash(vos_u32 ulPid, vos_u32 ulDstSubsys);
    ~CAutoWash();

private:
    //初始化解析函数
    void _initParseFuc();

    //解析自动清洗动作
    vos_u32 _parseAutoWash(msg_stComDevActInfo* pComAct);
    
    void _sendConsum(msg_stAutoWash * pDwnCmd);
    //解析微步调整
    vos_u32 _parseTrimOffset(msg_stComDevActInfo* pComAct);

    vos_u32 _parseNdlVrt(msg_stComDevActInfo* pComAct);
 
	//齿轮增压泵控制
	vos_u32 _ParseCtrlP04(msg_stComDevActInfo* pComAct);
    //泵阀开关
    vos_u32 _parsePVCtrl(msg_stComDevActInfo* pComAct);
    //液路传感器开关
    vos_u32 _parseLiquidsCheck(msg_stComDevActInfo* pComAct);
    //液路调试
    vos_u32 _parseLiquidDBG(msg_stComDevActInfo* pComAct);
    //设置浮子状态
    vos_u32 _parseSetFlState(msg_stComDevActInfo* pComAct);

	vos_u32 _parseSleep(msg_stComDevActInfo* pComAct);
    //状态切换
    vos_u32 _parseSwitch(msg_stComDevActInfo* pComAct);
};

#endif
