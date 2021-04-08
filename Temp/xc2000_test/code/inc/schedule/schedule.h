#ifndef _Schedule_h_
#define _Schedule_h_

#include "vos_pid.h"
#include "task.h"
#include "app_msg.h"
#include <map>

using namespace std;

class CSchedule : public CCallBack
{
    typedef vos_u32(CSchedule::*OnMsg)(vos_msg_header_stru* pMsg);

public:
    CSchedule();
    ~CSchedule();

public:
    //回调函数
    virtual vos_u32 CallBack(vos_msg_header_stru* pMsg);

private:
    //初始化调度模块
    void _initSchModule();

    vos_u32 _onStart(vos_msg_header_stru* pMsg);
    //添加任务
    vos_u32 _onAddTask(vos_msg_header_stru* pMsg);

    bool _checkLiquid(CTask * pTask, vos_u32 maxAlarmLevel);

    //复位响应
    vos_u32 _onResetRes(vos_msg_header_stru* pMsg);

    //停止仪器
    vos_u32 _onStop(vos_msg_header_stru* pMsg);

    //试剂暂停
    vos_u32 _onReagentSuspend(vos_msg_header_stru* pMsg);

    //试剂信息
    vos_u32 _onReagentInfo(vos_msg_header_stru* pMsg);

    //光量检查
    vos_u32 _onLightCheck(vos_msg_header_stru* pMsg);

    //杯空白
    vos_u32 _onCupBlank(vos_msg_header_stru* pMsg);

    //清洗反应杯
    vos_u32 _onWashCup(vos_msg_header_stru* pMsg);

    //AD值准备好
    vos_u32 _onAdReady(vos_msg_header_stru* pMsg);

    //超时处理
    vos_u32 _onTimeOut(vos_msg_header_stru* pMsg);

    //动作响应
    vos_u32 _onActRes(vos_msg_header_stru* pMsg);

	vos_u32 _onSkipDirtyCup(vos_msg_header_stru* pMsg);
	
	//试剂污染
	vos_u32 _onRegPollution(vos_msg_header_stru* pMsg);

	//样本污染
	vos_u32 _onSmpPollution(vos_msg_header_stru* pMsg);
	
    vos_u32 _onRctPollution(vos_msg_header_stru* pMsg);
    vos_u32 _onBloodAbsrob(vos_msg_header_stru* pMsg);
    void _reqTask(app_u32 shelfNum);

    //开始测试
    void _startTest();
    vos_u32 _startWashCup(int iMsgId, vos_u32 ulStatus);
    vos_u32 _onNotifyState(vos_msg_header_stru* pMsg);
    vos_u32 _autoLoadTest(vos_msg_header_stru* pMsg);
private:
    map<vos_u32, OnMsg>  m_msgFucs;     //解析动作函数集合
    const MachUnit m_ulUnit;
    bool m_bSuspend;   //是否暂停
    bool m_bautomove;  //是否常规抓手执行定位
};

#endif