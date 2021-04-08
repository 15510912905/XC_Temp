#ifndef __SMPNDL_H__
#define __SMPNDL_H__

#include "subsys.h"
#include "app_downmsg.h"
#include <map>

typedef enum
{
    PLACE_S1RTT_DISCHARGE = 0,	/*水平排样位置*/
    PLACE_S1RTT_THINNING = 1,	/*水平稀释位置*/
    PLACE_S1RTT_RESET = 2,	/*水平复位位置*/
    PLACE_S1RTT_WASH1 = 3,	/*水平洗液1位置*/
    PLACE_S1RTT_WASH2 = 4,	/*水平洗液2位置*/
    PLACE_S1RTT_WASH3 = 5,	/*水平洗液3位置*/
    PLACE_S1RTT_INHALATION1 = 6,	/*急诊吸样*/
    PLACE_S1RTT_INHALATION2 = 7,	/*常规吸样*/
}EN_PLACE_S1RTT;

typedef enum
{
    PLACE_S2RTT_THINNING = 0,	/*水平稀释位置*/
    PLACE_S2RTT_DISCHARGE = 1,	/*水平排样位置*/
    PLACE_S2RTT_RESET = 2,	/*水平复位位置*/
    PLACE_S2RTT_WASH1 = 3,	/*水平洗液1位置*/
    PLACE_S2RTT_WASH2 = 4,	/*水平洗液2位置*/
    PLACE_S2RTT_WASH3 = 5,	/*水平洗液3位置*/
    PLACE_S2RTT_INHALATION1 = 6,	/*常规吸样*/
    PLACE_S2RTT_INHALATION2 = 7,	/*急诊吸样*/
}EN_PLACE_S2RTT;

class CSmpNdl : public CSubSys
{
public:
    CSmpNdl(vos_u32 ulPid, vos_u32 ulDstSubsys);
    ~CSmpNdl();

private:
    //初始化解析函数
    void _initParseFuc();

    //解析复位动作
    vos_u32 _parseReset(msg_stComDevActInfo* pComAct);

	//解析针水平转动动作
    vos_u32 _parseNdlRtt(msg_stComDevActInfo* pComAct);

    //解析吸动作
    vos_u32 _parseAbsrb(msg_stComDevActInfo* pComAct);

    //解析排动作
    vos_u32 _parseDischarge(msg_stComDevActInfo* pComAct);

    //解析清洗动作
    vos_u32 _parseWashing(msg_stComDevActInfo* pComAct);

    //长清洗工作
    vos_u32 _parseWashLong(msg_stComDevActInfo* pComAct);
    //解析强化清洗动作
    vos_u32 _parseIntensifyWash(msg_stComDevActInfo* pComAct);

    //设置样本针水平转动信息
    void _setSampleNdlRttPos(msg_stNdlHrotate* pHrotate, app_u16 dstPos);

    //注册泵排空气
    vos_u32 _parseAirOut(msg_stComDevActInfo* pComAct);
	
	//微步调整    
    vos_u32 _parseTrimOffset(msg_stComDevActInfo* pComAct);

    //垂直运动
    vos_u32 _parseNdlVrt(msg_stComDevActInfo* pComAct);

    //内外壁清洗阀
    vos_u32 _parsePVCtrl(msg_stComDevActInfo* pComAct);

    //注射泵操作
    vos_u32 _parsePumpCtrl(msg_stComDevActInfo* pComAct);
    uint8_t _setPos(app_u16 usDstPos);
};

#endif
