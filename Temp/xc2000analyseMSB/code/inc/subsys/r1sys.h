#ifndef __R1SYS_H__
#define __R1SYS_H__

#include "subsys.h"
#include "app_downmsg.h"
#include <map>

using namespace std;

typedef enum
{
    PLACE_R11RTT_DISCHARGE = 0,	/*水平排试剂位置*/
    PLACE_R11RTT_RESET = 1,	/*水平复位位置*/
    PLACE_R11RTT_WASH1 = 2,	/*水平洗液1位置*/
    PLACE_R11RTT_WASH2 = 3,	/*水平洗液2位置*/
    PLACE_R11RTT_WASH3 = 4,	/*水平洗液3位置*/
    PLACE_R11RTT_INHALATION1 = 5,	/*水平吸试剂1位置*/
    PLACE_R11RTT_INHALATION2 = 6,	/*水平吸试剂2位置*/
}EN_PLACE_R11RTT;

typedef enum
{
    PLACE_R12RTT_INHALATION2 = 0,	/*水平吸试剂2位置*/
    PLACE_R12RTT_INHALATION1 = 1,	/*水平吸试剂1位置*/
    PLACE_R12RTT_WASH3 = 2,	/*水平洗液3位置*/
    PLACE_R12RTT_WASH2 = 3,	/*水平洗液2位置*/
    PLACE_R12RTT_WASH1 = 4,	/*水平洗液1位置*/
    PLACE_R12RTT_RESET = 5,	/*水平复位位置*/
    PLACE_R12RTT_DISCHARGE = 6,	/*水平排样位置*/
}EN_PLACE_R12RTT;

class CR1Sys : public CSubSys
{
public:
    CR1Sys(vos_u32 ulPid, vos_u32 ulDstSubsys);
    ~CR1Sys();

private:
    //初始化解析函数
    void _initParseFuc();

    //R1旋转
    vos_u32 _parseR1Rtt(msg_stComDevActInfo* pComAct);

    //R1吸试剂
    vos_u32 _parseR1Absrb(msg_stComDevActInfo* pComAct);

    //R1排试剂
    vos_u32 _parseR1Discharge(msg_stComDevActInfo* pComAct);

    //R1清洗
    vos_u32 _parseR1Wash(msg_stComDevActInfo* pComAct);

    //R1余量扫描
    vos_u32 _parseR1MarginScan(msg_stComDevActInfo* pComAct);

    //强化清洗
    vos_u32 _parseIntensifyWash(msg_stComDevActInfo* pComAct);

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
