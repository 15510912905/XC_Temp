#ifndef _VERTICALGRIPPER_H_
#define _VERTICALGRIPPER_H_
#include "subsys.h"
#include "app_downmsg.h"
#include <map>
class CVerticalGripper :public CSubSys
{
public:
    CVerticalGripper(vos_u32 ulPid, vos_u32 ulDstSubsys);
    ~CVerticalGripper();
    //初始化解析函数
    void _initParseFuc();
private:
    //解析定位动作
    vos_u32 _parseLoction(msg_stComDevActInfo* pComAct);
    vos_u32 ParseSingleCMD(msg_stComDevActInfo* pComAct);
};
#endif