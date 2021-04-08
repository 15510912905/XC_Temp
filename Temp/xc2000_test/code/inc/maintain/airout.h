#ifndef __AIROUT_H__
#define __AIROUT_H__

#include "vos_pid.h"

#include "msg.h"

class CAirOut : public CCallBack
{
public:
    CAirOut(vos_u32 ulPid);
    ~CAirOut();

public:
    virtual vos_u32 CallBack(vos_msg_header_stru* pMsg);

private:
    vos_u32 _procAirOut(CMsg& stMsg);
    vos_u32 _reqAirOut(vos_u32 ulDstPid, vos_u32 uiOutTimes);
    void _procActRes(vos_msg_header_stru* pMsg);
    void _procResult(int result);

private:
    vos_u32 m_ulTimer;     //定时器
    vos_u32 m_ulResNum; //响应返回的个数
};

#endif