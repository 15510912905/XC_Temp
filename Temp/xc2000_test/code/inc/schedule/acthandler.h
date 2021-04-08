#ifndef _ActHandler_h_
#define _ActHandler_h_

#include "vos_basictypedefine.h"
#include "subActMng.h"

//调度位置定义
typedef enum
{
    EN_Emer1 = 1,   //第一急诊位
    EN_Emer2,       //第二急诊位
    EN_Scan,        //扫描位
    EN_FWNorm,      //常规前等待位
    EN_FWEmer,      //急诊前等待位
    EN_RcycW,       //回收等待位
    EN_Rcyc,        //回收位
    EN_Schedu,      //调度位
    EN_Cache1,      //缓存
}EN_GWayLocal;

//分析部定义
typedef enum
{
    AL_NormC = 1,    //常规缓存
    AL_Norm,        //常规位
    AL_Emer,        //急诊位
    AL_BackW,       //后等待位
    AL_BackC,       //后变轨
    AL_RcycC,       //回收缓存
}EN_AnalyLocal;

//轨道单元状态
typedef enum
{
    GState_Idle = 0,    //空闲
    GState_Busy = 1,    //忙碌
    GState_Wait = 2     //等待
}Gway_State;
//资源调度状态机状态定义
typedef enum
{
    SS_Idle = 0,       //空闲状态
    SS_Reset = 1,      //复位状态,每次仪器开始测试前执行的状态

    SS_Start = 2,      //周期开始
    SS_Protect = 3,   //周期保护
    SS_DMove = 6,
}Schedu_state_enum;

class CActHandler : public CSubActMng
{
public:
    CActHandler();
    ~CActHandler();


    void OnNotifyState(STNotifState* pNotifState);
    void FreeShelf(vos_u32 taskType);
public:
    //初始化
    void Initialize(app_u32 washCount);

    //处理动作
    void ProcActStart(bool bSuspend);

    //预备动作
    void ProcPreEvent(bool bSuspend);

    //执行动作
    void ProcExeEvent(bool bSuspend);

    //停止
    void ProcStopEvent(vos_u32 ulStatus);

    int GetStatus(){ return m_iStatusCycle; }

    int GetAlarmLevel(){ return m_iAlarmLevel; }

    //启动定时器
    //  void StartCycleTimer(app_u32 uiStatus);
    void StartCycleTimer(Schedu_state_enum iStatus, vos_u32 ulTimeLen);
    void StartOuterTimer(Schedu_state_enum iStatus, vos_u32 ulTimeLen);
    void StartInnerTimer(Schedu_state_enum iStatus, vos_u32 ulTimeLen);
    void StopCycleTimer();
    void OnTimeOut(vos_u32 ultimerid, bool suspend);
    void SetPri(app_u32 time, app_u32 time1, app_u32 time2, app_u32 time3){ m_Pri = time; m_Pri1 = time1; m_Pri2 = time2; m_Pri3 = time3; }
private:
    bool _checkAlarm();

    vos_u32 _procPreAct();

    vos_u32 _procExeAct(bool bSuspend);

    vos_u32 _procAutoWashAct(bool bSuspend);

    void _procTestOver(bool bSuspend);

    void _sendAct();
    void _onCycleTimeOut(bool bSuspend);
    void _onOuterTimeOut();
    void _onReactTimeOut();
    void _onInnerTimeOut();

    app_u32 _setStartAct();
    app_u32 _setInnerAct();
    app_u32 _setReactAct();
    app_u32 _setOuterAct();

    app_u32 _reactTimer(app_u32 subSys, app_u16 actType);
    app_u32 _singleAct(app_u32 subSys, app_u16 actType);//无参数的单一命令
    app_u32 _autoWash(app_u32 subSys, app_u16 actType);
    app_u32 _reactDskMove(app_u32 subSys, app_u16 actType);
    app_u32 _reagDskMove(app_u32 subSys, app_u16 actType);
    app_u32 _smpDskMove(app_u32 subSys, app_u16 actType);
    app_u32 _ndlAbsorb(app_u32 subSys, app_u16 actType);
    app_u32 _ndlDischarge(app_u32 subSys, app_u16 actType);
    app_u32 _ndlMoveAbsb(app_u32 subSys, app_u16 actType);
    app_u32 _ndlMoveDisg(app_u32 subSys, app_u16 actType);
    app_u32 _ndlMoveWash(app_u32 subSys, app_u16 actType);
    app_u32 _mixMove(app_u32 subSys, app_u16 actType);

    void _procAutoWash(app_u32 subSys, app_com_dev_act_result_stru* pRes);
    void _procAbsorbS(app_u32 subSys, app_com_dev_act_result_stru* pRes);
    void _procDischarge(app_u32 subSys, app_com_dev_act_result_stru* pRes);
    void _procNdlWash(app_u32 subSys, app_com_dev_act_result_stru* pRes);
#ifndef Version_A
    void _procGetIn(app_u32 subSys, app_com_dev_act_result_stru* pRes);
    void _procMove(app_u32 subsys, app_com_dev_act_result_stru* pRes);
    void _procFreeIn(app_u32 subSys, app_com_dev_act_result_stru* pRes);
#endif
    void _procAlarm(app_u32 subSys, app_com_dev_act_result_stru* pRes);
private:
    int m_iAlarmLevel;   //告警级别 
    app_u32 m_ulTimerId;

    Schedu_state_enum m_iStatusCycle;     //调度状态
    Schedu_state_enum m_iStatusOuter;     //调度状态
    Schedu_state_enum m_iStatusInner;     //调度状态
    app_u32 m_ulTIDCycle;
    app_u32 m_ulTIDReact;
    app_u32 m_ulTIDOuter;
    app_u32 m_ulTIDInner;

    app_u32 m_ulTLenCycle;
    app_u32 m_ulTLenReact;
    app_u32 m_ulTLenOuter;
    app_u32 m_ulTLenInner;

    app_u32 m_ulTLenOuter1;//2.6
    app_u32 m_ulTLenOuter2;//
    app_u32 m_ulTLenOuter3;
    app_u32 m_ulTLenInner1;
    app_u32 m_ulTLenInner2;
    app_u32 m_ulTLenInner3;

    app_u16 m_usOuterBitMap;
    app_u16 m_usInnerBitMap;

    msg_stNdlRtt m_stSmpRtt;
    msg_stNdlRtt m_stSmpbRtt;
    msg_stNdlRtt m_stR1Rtt;
    msg_stNdlRtt m_stR1bRtt;
    msg_stNdlRtt m_stR2Rtt;
    msg_stNdlRtt m_stR2bRtt;

    msg_stSampleSysAbsorb m_stSmpAbsb;
    msg_stSampleSysAbsorb m_stSmpbAbsb;
    msg_stReagSysAbsorb m_stR1Absb;
    msg_stReagSysAbsorb m_stR1bAbsb;
    msg_stReagSysAbsorb m_stR2Absb;
    msg_stReagSysAbsorb m_stR2bAbsb;

    msg_stSampleSysDischarge m_stSmpDisg;
    msg_stSampleSysDischarge m_stSmpbDisg;
    msg_stReagSysDischarge m_stR1Disg;
    msg_stReagSysDischarge m_stR1bDisg;
    msg_stReagSysDischarge m_stR2Disg;
    msg_stReagSysDischarge m_stR2bDisg;

    msg_paraSampleDskRtt m_stSmpDskRtt;
    msg_paraSampleDskRtt m_stSmpbDskRtt;
    act_stReagSysDskRtt m_stR1DskRtt;
    act_stReagSysDskRtt m_stR1bDskRtt;
    act_stReagSysDskRtt m_stR2DskRtt;
    act_stReagSysDskRtt m_stR2bDskRtt;
    app_u32 m_Pri;
    app_u32 m_Pri1;
    app_u32 m_Pri2;
    app_u32 m_Pri3;
    app_u32 m_protolen;
    std::map <app_u32, Gway_State> m_mState;
public:
    bool m_bStop;
};
extern CActHandler * g_pActHandler;

#endif
