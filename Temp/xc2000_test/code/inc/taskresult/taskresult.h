#ifndef _taskresult_h_
#define _taskresult_h_

#include "vos_pid.h"
#include "vos_basictypedefine.h"
#include <map>
#include <queue>

using namespace std;

class CTaskResult : public CCallBack
{
    typedef vos_u32(CTaskResult::*OnMsg)(vos_msg_header_stru* pMsg);

public:
    CTaskResult();
    ~CTaskResult();

public:
    //回调函数
    virtual vos_u32 CallBack(vos_msg_header_stru* pMsg);    

private:
    //新任务执行结果处理
    vos_u32 _onNewResult(vos_msg_header_stru* pMsg);

    //上位机收到任务结果ACK
    vos_u32 _onResultAck(vos_msg_header_stru* pMsg);

    //通知上传结果
    vos_u32 _onReportResult(vos_msg_header_stru* pMsg);

private:
    //初始化调度模块
    void _initTaskResult();

    vos_u32 _sendTaskResult(vos_msg_header_stru* pMsg);

private:
    map<vos_u32, OnMsg>  m_msgFucs;     //解析动作函数集合

    queue<char*> m_qTaskResults;
};


#endif
