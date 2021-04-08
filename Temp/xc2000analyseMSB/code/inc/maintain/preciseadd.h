#ifndef __PreciseAdd_H__
#define __PreciseAdd_H__

#include "vos_pid.h"

#include "msg.h"
#include <map>
#include "subActMng.h"

using namespace std;

struct STDskPos
{
    vos_u32  ulDiskID;     //吸样盘编号;
    vos_u32  ulCupId;       //吸样杯编号;
};

class CPreciseAdd : public CCallBack, public CSubActMng
{
    typedef vos_u32(CPreciseAdd::*ProcStep)();

public:
    CPreciseAdd(vos_u32 ulPid);
    ~CPreciseAdd();


public:
    virtual vos_u32 CallBack(vos_msg_header_stru* pMsg);

private:
    void _reset(app_u32 subSys);
    void _addPrecisea(app_u32 ndlSys);
    app_u32 _stop();
    app_u32 _nextAdd();

    app_u32 _diskMove(app_u32 subSys, app_u16 actType);
    app_u32 _ndlMove(app_u32 subSys, app_u16 actType);
    app_u32 _ndlAbsorb(app_u32 subSys, app_u16 actType);
    app_u32 _ndlDischarge(app_u32 subSys, app_u16 actType);
    app_u32 _singleAct(app_u32 subSys, app_u16 actType);
    vos_u32 _procRecieveReq(vos_msg_header_stru* pMsg);
    vos_u32 _procActResult(vos_u32 ulResult);
    void _procNdlWash(app_com_dev_act_result_stru* pRes);
private:
    vos_u32 m_ulTimer;      //定时器
    vos_u32 m_ulLeftTimes;    //剩余测试次数
    STReqPreciseAdd m_stReqPreciseAdd;   //加液请求
};

#endif