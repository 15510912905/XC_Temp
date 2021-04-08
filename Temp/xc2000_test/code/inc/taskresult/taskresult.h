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
    //�ص�����
    virtual vos_u32 CallBack(vos_msg_header_stru* pMsg);    

private:
    //������ִ�н������
    vos_u32 _onNewResult(vos_msg_header_stru* pMsg);

    //��λ���յ�������ACK
    vos_u32 _onResultAck(vos_msg_header_stru* pMsg);

    //֪ͨ�ϴ����
    vos_u32 _onReportResult(vos_msg_header_stru* pMsg);

private:
    //��ʼ������ģ��
    void _initTaskResult();

    vos_u32 _sendTaskResult(vos_msg_header_stru* pMsg);

private:
    map<vos_u32, OnMsg>  m_msgFucs;     //����������������

    queue<char*> m_qTaskResults;
};


#endif
