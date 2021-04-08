#ifndef __RESET_H__
#define __RESET_H__

#include "vos_pid.h"

#include "msg.h"
#include "subActMng.h"

class CReset : public CCallBack, public CSubActMng
{
public:
    CReset(vos_u32 ulPid);
    ~CReset();

public:
    virtual vos_u32 CallBack(vos_msg_header_stru* pMsg);

private:
    vos_u32 _procResetReq(vos_msg_header_stru* pMsg);

    vos_u32 _reportRes(vos_u32 ulResult);

    vos_u32 _procResetRes(vos_msg_header_stru* pMsg);

    app_u32 _reqReset(app_u32 subSys, app_u16 actType);
    app_u32 _reqSampleMove(app_u32 subSys, app_u16 actType);
    app_u32 _reqMixUp(app_u32 subSys, app_u16 actType);
    app_u32 _reqMixMove(app_u32 subSys, app_u16 actType);
    vos_u32 _startTimer(vos_u32 ulTimeLen);

    app_u32 _insertAct();
private:
    vos_u32 m_ulTimer;     //定时器
    bool m_bBusy;
    vos_u32 m_ulSrcPid;
    bool m_bUpReset;        //是否是上位机请求复位
};

#endif