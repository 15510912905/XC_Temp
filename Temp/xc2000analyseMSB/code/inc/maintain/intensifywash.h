#ifndef __INTENSIFY_WASH_H__
#define __INTENSIFY_WASH_H__

#include "vos_pid.h"

#include "msg.h"

class CIntensifyWash : public CCallBack
{
public:
    CIntensifyWash(vos_u32 ulPid);
    ~CIntensifyWash();

public:
    virtual vos_u32 CallBack(vos_msg_header_stru* pMsg);

private:
    //处理请求
	vos_u32 _procReq(vos_msg_header_stru* pMsg);

    //上报结果
    vos_u32 _reportRes(vos_u32 ulResult);

    //处理复位响应
    vos_u32 _proResetRes(vos_msg_header_stru* pMsg);

    void _procMoveSmpDsk();
    void _procMoveReagDsk(app_u16 usCupId, app_u16 usDstPos, vos_u32 ulDstPid);

    //处理动作执行响应
    vos_u32 _procActRes(vos_msg_header_stru* pMsg);

    //发送强化清洗请求
    vos_u32 _sendIntensifyWash(vos_u32 ulDstPid);

    //处理执行结果
    vos_u32 _procActResult();

private:
    vos_u32 m_ulTimer;     //定时器
    vos_u32 m_ulResNum; //响应返回的个数
    vos_u32 m_ulSrcPid; //源Pid
    vos_u32 m_ulStep;
    bool   signDsk;
};

#endif