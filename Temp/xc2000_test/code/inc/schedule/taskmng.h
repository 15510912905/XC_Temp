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
    //��������
    void AddTask(CTask * pTask);

	void AddTaskFront(CTask * pTask);

    void DeleteShelfTask(CTask* pTask);
    void AddShelf(CTask* pTask);
    CTask* GetWaitingTask(ReactCircle circleType);

    //��������
    void SuspendTask(map<app_u16, CTask*>& mSuspendTasks);

    void SetWashCup(int  iMsgId);
    bool IsWashCup(){ return 0 != m_iWashMsgId; }
	
	int  GetMsgID() {return  m_iWashMsgId;};
	
    void SetAdValue(vos_u32 adval[WAVE_NUM], vos_u32 ulLgcCupId);

    vos_u32 SendWashResult();

    void NotifySendTask(int iAlarm);

    //��������
    void SendTaskResult(CTask ** pTask, vos_u32 ulResult, int iLgcId = 0);

    //�������
    void Clear();

    bool HasTask(){ return !m_qTasks.empty() && !m_qTasksInner.empty(); }
	
	void DeleteR2Task(bool bAdd, STNotifyFailedSmpTask& stPara);

private:
    void _clearTaskQueqe(deque<CTask*>& pQTasks);

    app_u32 _getTaskCount(){ return (m_qTasks.size() + m_qTasksInner.size()); }

private:
    int  m_iWashMsgId;

    int m_iSimADIndex;   //ģ��ADֵ���

    deque<CTask*> m_qTasks; //�������
    deque<CTask*> m_qTasksInner; //�������

    STNotifyADValue m_stNotifyADVulue;

    map<app_u32, vector<app_u32> > m_mShelfTasks;//�������������
    int m_iPreShelf;//���Ӻ�
    int m_iPreShelfPos;//��ǰλ�ú�
};

class CRscMng
{
public:
    CRscMng();


public:
    void Initialize();
    void SetTimerLen(vos_u32 ulExeTimeLen, vos_u32 ulPreTimeLen);

    //�Ƿ�����ת����ʱ
    bool HasDskMoveTimeOut(bool bAlarm);

private:
    //�����ת��
    bool _checkDskTimeOut(bool bAlarm, EN_ActFlag dskFlag, vos_u32 ulFaultId, vos_u32 ulDskId);

public:
    EN_ActFlag enSimpleDskFlag;   //������ִ�б�ʶ
    EN_ActFlag enOuterDskFlag;     //�Լ�����ִ�б�ʶ
    EN_ActFlag enInnerDskFlag;     //�Լ�����ִ�б�ʶ
    EN_ActFlag enReactDskFlag;     //��Ӧ��ִ�б�ʶ

    vos_u32 m_ulExeTimeLen;        //ִ�����ڳ�ʱʱ��
    vos_u32 m_ulPreTimeLen;        //Ԥ�����ڳ�ʱʱ��
};

extern CTaskMng *g_pTaskMng;
extern CRscMng *g_pRscMng;
#endif
