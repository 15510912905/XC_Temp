#include "taskmng.h"
#include "app_errorcode.h"
#include "socketmonitor.h"
#include "alarm.h"
#include "vos_log.h"
#include "xc8002_middwnmachshare.h"
#include "reactcupmng.h"
#include "apputils.h"
#include "acthandler.h"
#include "task.h"
#define APP_MAX_PREPARE_ACT_TIMER_LEN (14)
#define APP_MAX_EXECUTE_ACT_TIMER_LEN (31)

#define TASK_RESULT_HANDLE_TIME 400

CTaskMng *g_pTaskMng = NULL;
CRscMng *g_pRscMng = NULL;
CTaskMng::CTaskMng() : m_iWashMsgId(0), m_iSimADIndex(0)
{
}

CTaskMng::~CTaskMng()
{

}

void CTaskMng::DeleteR2Task(bool bAdd, STNotifyFailedSmpTask& stPara)
{
    while (true)
    {
        CTask *pTask = GetWaitingTask(Circle_All);
        if (VOS_NULL == pTask)
        {
            return;
        }
        if (bAdd && !(pTask->NeedAddReag(2) || pTask->NeedAddReag(4)))
        {
            //单试剂任务继续测试
            AddTask(pTask);
            continue;
        }
        pTask->CheckSmpTask(stPara);
        DeleteShelfTask(pTask);
        delete(pTask);
    }
}

void CTaskMng::AddTask(CTask * pTask)
{
    pTask->SetSimulateId(m_iSimADIndex++);
    if (pTask->GetCircle() == Circle_Outer || (pTask->GetCircle() == Circle_All && m_qTasks.size()<m_qTasksInner.size()))
    {
        m_qTasks.push_back(pTask);
        app_print(ps_taskmng, "\n add task Outer-%d\n", pTask->GetTaskId());
    }
    else //if (pTask->GetCircle() == Circle_Inner || (pTask->GetCircle() == Circle_All && m_qTasks.size()>m_qTasksInner.size()))
    {
        m_qTasksInner.push_back(pTask);
        app_print(ps_taskmng, "\n add task Inner-%d\n", pTask->GetTaskId());
    } 
    pTask->PrintTaskInfo();
    AddShelf(pTask);
}

void CTaskMng::AddTaskFront(CTask * pTask)
{
    if (pTask->GetCircle() == Circle_Outer || (pTask->GetCircle() == Circle_All && m_qTasks.size() < m_qTasksInner.size()))
    {
        m_qTasks.push_front(pTask);
        app_print(ps_taskmng, "\n AddTaskFront Outer-%d\n", pTask->GetTaskId());
    }
    else //if (pTask->GetCircle() == Circle_Inner || (pTask->GetCircle() == Circle_All && m_qTasks.size()>m_qTasksInner.size()))
    {
        m_qTasksInner.push_front(pTask);
        app_print(ps_taskmng, "\n AddTaskFront Inner-%d\n", pTask->GetTaskId());
    }
}


void CTaskMng::DeleteShelfTask(CTask* pTask)
{
    if (pTask == NULL)
    {
        return;
    }
    app_u32 shelfNum = pTask->GetSmpShelf();
    app_u32 taskId = pTask->GetTaskId();
    map<app_u32, vector<app_u32> >::iterator iter = m_mShelfTasks.find(shelfNum);
    if (iter != m_mShelfTasks.end())
    {
        vector<app_u32>::iterator iterv;
        for (iterv = iter->second.begin(); iterv != iter->second.end(); iterv++)
        {
            if (taskId == *iterv)
            {
                iter->second.erase(iterv);
                app_print(ps_taskmng, "Delete Shelf(%d) task(%d)\n", shelfNum, taskId);
                break;
            }
        }
        if (iter->second.empty())
        {
            m_mShelfTasks.erase(iter);
            app_print(ps_taskmng, "\n__**Free Shelf(%d)**__\n", shelfNum);
            //TODO传架号，下位机根据架号释放常规还是急诊
            g_pActHandler->FreeShelf(pTask->GetMaintanType());
        }
    }
}

void CTaskMng::AddShelf(CTask* pTask)
{
    app_u32 shelfNum = pTask->GetSmpShelf();
    if (shelfNum > 0)
    {
        m_mShelfTasks[shelfNum].push_back(pTask->GetTaskId());
        app_print(ps_taskmng, "Add Shelf(%d) task(%d)\n", shelfNum, pTask->GetTaskId());
    }
}

CTask* CTaskMng::GetWaitingTask(ReactCircle circleType)
{
    if (!m_qTasks.empty() && (circleType == Circle_Outer || circleType == Circle_All))
    {
        CTask * pTask = m_qTasks.front();
        m_qTasks.pop_front();
        pTask->SetAnalyCircle(circleType);
		app_print(ps_taskmng, "\n GetWaitingTask outer Queue-%d\n", pTask->GetTaskId());
        return pTask;
    }
    else if (!m_qTasksInner.empty() && (circleType == Circle_Inner || circleType == Circle_All))
    {
        CTask * pTask = m_qTasksInner.front();
        m_qTasksInner.pop_front();
        pTask->SetAnalyCircle(circleType);
        app_print(ps_taskmng, "\n GetWaitingTask inner Queue-%d\n", pTask->GetTaskId());
        return pTask;
    }
    return VOS_NULL;
}

void CTaskMng::SuspendTask(map<app_u16, CTask*>& mSuspendTasks)
{
    map<app_u16, CTask*>::iterator iter = mSuspendTasks.begin();
    for (; iter != mSuspendTasks.end(); ++iter)
    {
        CTask * pTask = iter->second;
        AddTaskFront(pTask);
    }
}

void CTaskMng::SetAdValue(vos_u32 adval[WAVE_NUM], vos_u32 ulLgcCupId)
{
    STCupADValue *pCupInfo = m_stNotifyADVulue.add_stcupinfo();
    pCupInfo->set_uicupid(ulLgcCupId);
    for (vos_u32 i = 0; i < WAVE_NUM; ++i)
    {
        pCupInfo->add_uiadpoints(adval[i]);
    }
}

vos_u32 CTaskMng::SendWashResult()
{
    if (MSG_ReqWashReactCup != m_iWashMsgId)
    {
        return sendUpMsg(CPid_Maintain, SPid_Task, MSG_NotifyADValue, &m_stNotifyADVulue);
    }
    return VOS_OK;
}

void CTaskMng::SetWashCup(int  iMsgId)
{
    m_iWashMsgId = iMsgId;
    m_stNotifyADVulue.Clear();
}

void CTaskMng::SendTaskResult(CTask ** pTask, vos_u32 ulResult, int iLgcId)
{
    if (ulResult != TR_Success)
    {
        app_print(ps_taskmng, "CancelTask-%lu \n",(*pTask)->GetTaskId());
    }
    DeleteShelfTask(*pTask);
    (*pTask)->SendTaskRes(ulResult, iLgcId);
    delete(*pTask);
    (*pTask) = NULL;
}

void CTaskMng::NotifySendTask(int iAlarm)
{
    bool bAlarm = g_pAlarm->HasAlarm(1) || g_pAlarm->HasAlarm(2) || g_pAlarm->HasAlarm(4);
    if (!bAlarm && !IsWashCup() && _getTaskCount() <= 2)
    {
        sendUpMsg(CPid_SendTask, SPid_Task, MSG_NotifySendTask);
    }
}

void CTaskMng::Clear()
{
    _clearTaskQueqe(m_qTasks);
    m_iWashMsgId = 0;
    m_stNotifyADVulue.Clear();
    m_iSimADIndex = 0;
}

void CTaskMng::_clearTaskQueqe(deque<CTask*>& pQTasks)
{
    CTask * pTask = NULL;
    while (!pQTasks.empty())
    {
        pTask = pQTasks.front();
        pQTasks.pop_front();
        DeleteShelfTask(pTask);
        delete(pTask);
    }
}

CRscMng::CRscMng()
{
    Initialize();
    SetTimerLen(APP_MAX_EXECUTE_ACT_TIMER_LEN, APP_MAX_PREPARE_ACT_TIMER_LEN);
}
void CRscMng::Initialize()
{
    enSimpleDskFlag = EN_AF_Unstart;   //样本盘执行标识
    enOuterDskFlag = EN_AF_Unstart;     //试剂外盘执行标识
    enInnerDskFlag = EN_AF_Unstart;     //试剂内盘执行标识
    enReactDskFlag = EN_AF_Unstart;     //反应盘执行标识
}
void CRscMng::SetTimerLen(vos_u32 ulExeTimeLen, vos_u32 ulPreTimeLen)
{
    m_ulExeTimeLen = ulExeTimeLen;
    m_ulPreTimeLen = ulPreTimeLen;
}

bool CRscMng::HasDskMoveTimeOut(bool bAlarm)
{
    //反应盘
    if (_checkDskTimeOut(bAlarm, enReactDskFlag, ALarm_React_System, xc8002_dwnmach_name_reactdisk))
    {
        return true;
    }

    //试剂内盘
    if (_checkDskTimeOut(bAlarm, enInnerDskFlag, ALarm_R2_System, xc8002_dwnmach_name_R2disk))
    {
        return true;
    }

    //样本盘
    if (_checkDskTimeOut(bAlarm, enSimpleDskFlag, ALarm_Smp_System, xc8002_dwnmach_name_smpldisk))
    {
        return true;
    }

    //试剂外盘
    return _checkDskTimeOut(bAlarm, enOuterDskFlag, ALarm_R1_System, xc8002_dwnmach_name_R1disk);
}

bool  CRscMng::_checkDskTimeOut(bool bAlarm, EN_ActFlag dskFlag, vos_u32 ulFaultId, vos_u32 ulDskId)
{
    if (EN_AF_Doing == dskFlag)
    {
        if (bAlarm)
        {
            g_pAlarm->GenerateAlarm(ulFaultId, ulDskId, Act_Move);
        }
        WRITE_ERR_LOG("TimeOut Move Dsk=%lu\n", ulDskId);
        return true;
    }
    return false;
}

