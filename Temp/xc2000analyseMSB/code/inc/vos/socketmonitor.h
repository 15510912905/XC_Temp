#ifndef __Socket_Monitor_H__
#define __Socket_Monitor_H__

#include "vos_pid.h"
#include "compensation.pb.h"
#include "msg.h"
#include "vos_timer.h"
#include "vos_configure.h"

vos_u32 getCurRunState();

#define  ConnTimeLen 20

//�����Դ��״̬
typedef enum{
    STATE_OFF      			=0,   //��Դ��OFF
    STATE_ON       			=1,   //��Դ��ON
}lightStateEunm;

class CSocketMonitor : public CCallBack
{
public:
    CSocketMonitor(vos_u32 ulPid);
    ~CSocketMonitor();

public:
    //��Ϣ�ص�
    virtual vos_u32 CallBack(vos_msg_header_stru* pMsg);

    //�Ƿ����ӳɹ�
    bool IsConnect(); 

    //��ȡ��λ��״̬
    bool CanMoveReagDsk();

    //�ܷ�������
    bool CanHandleTask();

    //�ܷ�������λ
    bool CanReset();

    //�ܷ�������
    bool CanHandleCmd();


    //�ܷ�����ɨ��
    bool CanBCScan();

    vos_u32 Disconnect(vos_u32 connTimeLen = ConnTimeLen, vos_u32 ulPort = VOS_SERVER_SOCKET_PORT_ID);

    //��ȡײ������
    vos_u32 GetCollideNdlConfig();
public:
	static vos_u32 m_ulState;      //��λ��״̬

private:
    vos_u32 _startTimer(vos_u32 ulTimer, vos_u32 ulTimeLen);
    vos_u32 _procTimeOut(vos_timeroutmsg_stru* pMsg);
    vos_u32 _sayHello();
    vos_u32 _procSetTime(vos_msg_header_stru* pstvosmsg);
    vos_u32 _procNotifyState(vos_msg_header_stru* pstvosmsg);

    //������λ��״̬
    vos_u32 _reportState(vos_u32 ulState, vos_u32 ulLeftTime = 0);
    //ײ������
    vos_u32 _procCollideNdlConfig(vos_msg_header_stru* pstvosmsg);
   
    bool _isFree();

    //��ȡʱ�����
    vos_u32 _getDiffDay(vos_u32  nTime1, vos_u32  nTime2);

    //������־
    void _backLog();
    void _backLog(char* szTraceDir, CBackLog& rBackLog);

	//��Դ�ƿ���
	vos_u32 _controlLight(vos_u32 intensity);

	//����״̬���ع�Դ��
	vos_u32 _controlLightByState(vos_u32 m_ulState);

	vos_u32 _openLight();
	vos_u32 _restLight();
    void _sleepPump();
	void _sleepTemp();
    void _recoverPumpTemp();
	vos_u32 _ctlByState(vos_u32 m_ulState);
private:
	vos_u32 m_ulPort;
    bool m_bConnect;
    void*  m_pMutexConn;

    vos_u32 m_ulHelloTimer;        //���ֶ�ʱ��
    vos_u32 m_ulLightTimer;        //��Դ�ƶ�ʱ��
    vos_u32 m_ulLiquidTimer;         //��Դ�ƶ�ʱ��
	vos_u32 m_ulTempTimer;			//�¿ض�ʱ��
    vos_u32 m_ulLiquidState;         //��Դ�ƶ�ʱ��
    vos_u32 m_ulCollideNdl;        //ײ�̴����ʾ
    vos_u32 m_lightState;          //��Դ��״̬
	vos_u32 m_TempState;			//�¿�״̬
};

extern CSocketMonitor * g_pMonitor;

#endif
