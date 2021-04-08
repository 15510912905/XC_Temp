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
    //�ص�����
    virtual vos_u32 CallBack(vos_msg_header_stru* pMsg);

private:
    //��ʼ������ģ��
    void _initSchModule();

    vos_u32 _onStart(vos_msg_header_stru* pMsg);
    //�������
    vos_u32 _onAddTask(vos_msg_header_stru* pMsg);

    bool _checkLiquid(CTask * pTask, vos_u32 maxAlarmLevel);

    //��λ��Ӧ
    vos_u32 _onResetRes(vos_msg_header_stru* pMsg);

    //ֹͣ����
    vos_u32 _onStop(vos_msg_header_stru* pMsg);

    //�Լ���ͣ
    vos_u32 _onReagentSuspend(vos_msg_header_stru* pMsg);

    //�Լ���Ϣ
    vos_u32 _onReagentInfo(vos_msg_header_stru* pMsg);

    //�������
    vos_u32 _onLightCheck(vos_msg_header_stru* pMsg);

    //���հ�
    vos_u32 _onCupBlank(vos_msg_header_stru* pMsg);

    //��ϴ��Ӧ��
    vos_u32 _onWashCup(vos_msg_header_stru* pMsg);

    //ADֵ׼����
    vos_u32 _onAdReady(vos_msg_header_stru* pMsg);

    //��ʱ����
    vos_u32 _onTimeOut(vos_msg_header_stru* pMsg);

    //������Ӧ
    vos_u32 _onActRes(vos_msg_header_stru* pMsg);

	vos_u32 _onSkipDirtyCup(vos_msg_header_stru* pMsg);
	
	//�Լ���Ⱦ
	vos_u32 _onRegPollution(vos_msg_header_stru* pMsg);

	//������Ⱦ
	vos_u32 _onSmpPollution(vos_msg_header_stru* pMsg);
	
    vos_u32 _onRctPollution(vos_msg_header_stru* pMsg);
    vos_u32 _onBloodAbsrob(vos_msg_header_stru* pMsg);
    void _reqTask(app_u32 shelfNum);

    //��ʼ����
    void _startTest();
    vos_u32 _startWashCup(int iMsgId, vos_u32 ulStatus);
    vos_u32 _onNotifyState(vos_msg_header_stru* pMsg);
    vos_u32 _autoLoadTest(vos_msg_header_stru* pMsg);
private:
    map<vos_u32, OnMsg>  m_msgFucs;     //����������������
    const MachUnit m_ulUnit;
    bool m_bSuspend;   //�Ƿ���ͣ
    bool m_bautomove;  //�Ƿ񳣹�ץ��ִ�ж�λ
};

#endif