#ifndef __R2SYS_H__
#define __R2SYS_H__

#include "subsys.h"
#include "app_downmsg.h"
#include <map>

using namespace std;
typedef enum
{
    PLACE_R21RTT_DISCHARGE = 0,	/*水平排试剂位置*/
    PLACE_R21RTT_RESET = 1,	/*水平复位位置*/
    PLACE_R21RTT_WASH1 = 2,	/*水平洗液1位置*/
    PLACE_R21RTT_WASH2 = 3,	/*水平洗液2位置*/
    PLACE_R21RTT_INHALATION1 = 4,	/*水平吸试剂1位置*/
    PLACE_R21RTT_INHALATION2 = 5,	/*水平吸试剂2位置*/
}EN_PLACE_R21RTT;

typedef enum
{
    PLACE_R22RTT_DISCHARGE = 0,	/*水平排试剂位置*/
    PLACE_R22RTT_RESET = 1,	/*水平复位位置*/
    PLACE_R22RTT_WASH1 = 2,	/*水平洗液1位置*/
    PLACE_R22RTT_WASH2 = 3,	/*水平洗液2位置*/
    PLACE_R22RTT_INHALATION1 = 4,	/*水平吸试剂1位置*/
    PLACE_R22RTT_INHALATION2 = 5,	/*水平吸试剂2位置*/
}EN_PLACE_R22RTT;


class CR2Sys : public CSubSys
{
public:
    CR2Sys(vos_u32 ulPid, vos_u32 ulDstSubsys);
    ~CR2Sys();

private:

    //初始化解析函数
    void _initParseFuc();

    //R2旋转
    vos_u32 _parseR2Rtt(msg_stComDevActInfo* pComAct);

    //R2吸试剂
    vos_u32 _parseR2Absrb(msg_stComDevActInfo* pComAct);

    //R2排试剂
    vos_u32 _parseR2Discharge(msg_stComDevActInfo* pComAct);

    //R2清洗
    vos_u32 _parseR2Wash(msg_stComDevActInfo* pComAct);

    //R2余量扫描
    vos_u32 _parseR2MarginScan(msg_stComDevActInfo* pComAct);

    //强化清洗
    vos_u32 _parseIntensifyWash(msg_stComDevActInfo* pComAct);

    //注册泵排空气
    vos_u32 _parseAirOut(msg_stComDevActInfo* pComAct);
    
    //解析微步调整
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
