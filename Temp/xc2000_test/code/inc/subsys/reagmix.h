#ifndef __REAGMIX_H__
#define __REAGMIX_H__

#include "subsys.h"
#include "app_downmsg.h"
#include <map>

class CReagMix : public CSubSys
{
public:
    CReagMix(vos_u32 ulPid);
    ~CReagMix();

private:
    //初始化解析函数
    void _initParseFuc();

    //解析动作
    vos_u32 _parseReset(msg_stComDevActInfo* pComAct);
    vos_u32 _parseMove(msg_stComDevActInfo* pComAct);
    vos_u32 _parseMix(msg_stComDevActInfo* pComAct);
    vos_u32 _parseWash(msg_stComDevActInfo* pComAct);
    vos_u32 _parseOnlyMix(msg_stComDevActInfo* pComAct);

	//微步调整
    vos_u32 _parseTrimOffset(msg_stComDevActInfo* pComAct);
	
    //垂直运动
    vos_u32 _parseNdlVrt(msg_stComDevActInfo* pComAct);

    //外壁清洗阀
    vos_u32 _parsePVCtrl(msg_stComDevActInfo* pComAct);

    //升级逻辑
    vos_u32 _parseUpFpga(msg_stComDevActInfo* pComAct);
};

#endif