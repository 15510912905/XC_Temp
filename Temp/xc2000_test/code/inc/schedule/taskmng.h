#ifndef _taskmng_h_
#define _taskmng_h_

#include "upmsg.pb.h"
#include "task.h"
#include <deque>
#include "msg.h"
#include "app_msg.h"

using namespace std;

class CTaskMng
{
public:
    CTaskMng();
    ~CTaskMng();

public:
    //处理请求
    void AddTask(CTask * pTask);

	void AddTaskFront(CTask * pTask);

    void DeleteShelfTask(CTask* pTask);
    void AddShelf(CTask* pTask);
    CTask* GetWaitingTask(ReactCircle circleType);

    //挂起任务
    void SuspendTask(map<app_u16, CTask*>& mSuspendTasks);

    void SetWashCup(int  iMsgId);
    bool IsWashCup(){ return 0 != m_iWashMsgId; }
	
	int  GetMsgID() {return  m_iWashMsgId;};
	
    void SetAdValue(vos_u32 adval[WAVE_NUM], vos_u32 ulLgcCupId);

    vos_u32 SendWashResult();

    void NotifySendTask(int iAlarm);

    //发送任务
    void SendTaskResult(CTask ** pTask, vos_u32 ulResult, int iLgcId = 0);

    //清除数据
    void Clear();

    bool HasTask(){ return !m_qTasks.empty() && !m_qTasksInner.empty(); }
	
	void DeleteR2Task(bool bAdd, STNotifyFailedSmpTask& stPara);

private:
    void _clearTaskQueqe(deque<CTask*>& pQTasks);

    app_u32 _getTaskCount(){ return (m_qTasks.size() + m_qTasksInner.size()); }

private:
    int  m_iWashMsgId;

    int m_iSimADIndex;   //模拟AD值序号

    deque<CTask*> m_qTasks; //任务队列
    deque<CTask*> m_qTasksInner; //任务队列

    STNotifyADValue m_stNotifyADVulue;

    map<app_u32, vector<app_u32> > m_mShelfTasks;//样本架任务个数
    int m_iPreShelf;//架子号
    int m_iPreShelfPos;//当前位置号
};

class CRscMng
{
public:
    CRscMng();


public:
    void Initialize();
    void SetTimerLen(vos_u32 ulExeTimeLen, vos_u32 ulPreTimeLen);

    //是否有盘转动超时
    bool HasDskMoveTimeOut(bool bAlarm);

private:
    //检查盘转动
    bool _checkDskTimeOut(bool bAlarm, EN_ActFlag dskFlag, vos_u32 ulFaultId, vos_u32 ulDskId);

public:
    EN_ActFlag enSimpleDskFlag;   //样本盘执行标识
    EN_ActFlag enOuterDskFlag;     //试剂外盘执行标识
    EN_ActFlag enInnerDskFlag;     //试剂内盘执行标识
    EN_ActFlag enReactDskFlag;     //反应盘执行标识

    vos_u32 m_ulExeTimeLen;        //执行周期超时时间
    vos_u32 m_ulPreTimeLen;        //预备周期超时时间
};

extern CTaskMng *g_pTaskMng;
extern CRscMng *g_pRscMng;
#endif
