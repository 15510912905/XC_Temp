#ifndef __Socket_Monitor_H__
#define __Socket_Monitor_H__

#include "vos_pid.h"
#include "compensation.pb.h"
#include "msg.h"
#include "vos_timer.h"
#include "vos_configure.h"

vos_u32 getCurRunState();

#define  ConnTimeLen 20

//定义光源灯状态
typedef enum{
    STATE_OFF      			=0,   //光源灯OFF
    STATE_ON       			=1,   //光源灯ON
}lightStateEunm;

class CSocketMonitor : public CCallBack
{
public:
    CSocketMonitor(vos_u32 ulPid);
    ~CSocketMonitor();

public:
    //消息回调
    virtual vos_u32 CallBack(vos_msg_header_stru* pMsg);

    //是否连接成功
    bool IsConnect(); 

    //获取中位机状态
    bool CanMoveReagDsk();

    //能否处理任务
    bool CanHandleTask();

    //能否整机复位
    bool CanReset();

    //能否处理命令
    bool CanHandleCmd();


    //能否条码扫描
    bool CanBCScan();

    vos_u32 Disconnect(vos_u32 connTimeLen = ConnTimeLen, vos_u32 ulPort = VOS_SERVER_SOCKET_PORT_ID);

    //获取撞针配置
    vos_u32 GetCollideNdlConfig();
public:
	static vos_u32 m_ulState;      //中位机状态

private:
    vos_u32 _startTimer(vos_u32 ulTimer, vos_u32 ulTimeLen);
    vos_u32 _procTimeOut(vos_timeroutmsg_stru* pMsg);
    vos_u32 _sayHello();
    vos_u32 _procSetTime(vos_msg_header_stru* pstvosmsg);
    vos_u32 _procNotifyState(vos_msg_header_stru* pstvosmsg);

    //报告中位机状态
    vos_u32 _reportState(vos_u32 ulState, vos_u32 ulLeftTime = 0);
    //撞针配置
    vos_u32 _procCollideNdlConfig(vos_msg_header_stru* pstvosmsg);
   
    bool _isFree();

    //获取时间差异
    vos_u32 _getDiffDay(vos_u32  nTime1, vos_u32  nTime2);

    //备份日志
    void _backLog();
    void _backLog(char* szTraceDir, CBackLog& rBackLog);

	//光源灯控制
	vos_u32 _controlLight(vos_u32 intensity);

	//根据状态开关光源灯
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

    vos_u32 m_ulHelloTimer;        //握手定时器
    vos_u32 m_ulLightTimer;        //光源灯定时器
    vos_u32 m_ulLiquidTimer;         //光源灯定时器
	vos_u32 m_ulTempTimer;			//温控定时器
    vos_u32 m_ulLiquidState;         //光源灯定时器
    vos_u32 m_ulCollideNdl;        //撞盘处理标示
    vos_u32 m_lightState;          //光源灯状态
	vos_u32 m_TempState;			//温控状态
};

extern CSocketMonitor * g_pMonitor;

#endif
