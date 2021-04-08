#ifndef __LOOPTEST_H__
#define __LOOPTEST_H__


#include"xc8002_middwnmachshare.h"
#include "vos_pid.h"

#define  LOOPBACKNUM 5
#define  LOOPBACK_TIMEROUT_LEN 20//200ms 

extern map<app_u16, app_u16> g_uartHdl;

//������λ�������
typedef struct
{
    app_u16 endwnmachsubsys;
    app_u16 usdevtype;
	app_u16 cmd;
    app_u16 usframeid;
    app_u16 usttl;
    app_u16 enloopbacklayer;  //���ز�
}LoopTestCmdBufStru;

//���ڴ��ڴ��乤��ģʽ
typedef enum
{
    app_uart_tran_mode_normal   = 0,
    app_uart_tran_mode_loopback = 1,

    app_uart_tran_mode_butt
}app_uart_mode_enum;

class CLoopTest : public CCallBack
{
public:
    CLoopTest(vos_u32 ulPid);
    ~CLoopTest();

public:
	virtual vos_u32 CallBack(vos_msg_header_stru* pMsg);

private:

	//������λ����������
    vos_u32 _procLoopTestReq(vos_msg_header_stru* pReqMsg);
	app_u16 _setTtl(app_u16 LoopBackLayer);
	vos_u32 _setDeviceLoopTestState(app_u16 DwnName,app_u16 State,app_u16 Device);
	vos_u32 _sendToUart(LoopTestCmdBufStru * pMsg);

	//������λ��������Ӧ
    vos_u32 _procLoopTestRes(vos_msg_header_stru* pResMsg);

	//����ʱ
	vos_u32 _procTimeOut(vos_msg_header_stru* pResMsg);
	
    //����ͨ��ѹ������
    vos_u32 _procPressTest(vos_msg_header_stru* pReqMsg);

    vos_u32 _procPressTestRes(vos_msg_header_stru* pMsg);
private:
	vos_u32 m_SrcPid;
	vos_u32 m_LoopBackNum;
	vos_u32 m_ulTimer;
    vos_u32 m_ulPressTimer;
	LoopTestCmdBufStru* pLoopBuf;
    map<uint32,bool> m_mLoopState;
    uint32 m_ulTimerLen;
};

#endif
