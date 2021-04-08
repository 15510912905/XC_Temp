#ifndef __LOOPTEST_H__
#define __LOOPTEST_H__


#include"xc8002_middwnmachshare.h"
#include "vos_pid.h"

#define  LOOPBACKNUM 5
#define  LOOPBACK_TIMEROUT_LEN 20//200ms 

extern map<app_u16, app_u16> g_uartHdl;

//单个下位机命令缓存
typedef struct
{
    app_u16 endwnmachsubsys;
    app_u16 usdevtype;
	app_u16 cmd;
    app_u16 usframeid;
    app_u16 usttl;
    app_u16 enloopbacklayer;  //环回层
}LoopTestCmdBufStru;

//定期串口传输工作模式
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

	//处理下位机动作请求
    vos_u32 _procLoopTestReq(vos_msg_header_stru* pReqMsg);
	app_u16 _setTtl(app_u16 LoopBackLayer);
	vos_u32 _setDeviceLoopTestState(app_u16 DwnName,app_u16 State,app_u16 Device);
	vos_u32 _sendToUart(LoopTestCmdBufStru * pMsg);

	//处理下位机动作响应
    vos_u32 _procLoopTestRes(vos_msg_header_stru* pResMsg);

	//处理超时
	vos_u32 _procTimeOut(vos_msg_header_stru* pResMsg);
	
    //处理通信压力测试
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
