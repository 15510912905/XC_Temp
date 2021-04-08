#ifndef __DARKCURRENT_H__
#define __DARKCURRENT_H__

#include "vos_pid.h"
#include "upmsg.pb.h"
#include "xc8002_middwnmachshare.h"
#include <vector>

using namespace std;

#define DARKCURRENT_TIME_LEN 50
#define DELAY_TIME_LEN_100MS 10
#define WAIT_LIGHT_CLOSE_TIME_LEN 30


//定义测试状态
typedef enum{
	STATE_TEST_IDLE        			=0,   //空闲
    STATE_CLOSE_LIGHT      			=1,   //关光源灯
    STATE_OPEN_LIGHT       			=2,   //开光源灯
    STATE_DARKCURRENT_TEST_OVER     =3,   //测试结束
     
    STATE_TEST_BUFF,
}darkCurrentStateEunm;

class CDarkCurrent : public CCallBack
{
public:
    CDarkCurrent(vos_u32 ulPid);

public:
	virtual vos_u32 CallBack(vos_msg_header_stru* pMsg);

private:

	//处理暗电流测试请求
	vos_u32 _procDarkCurrentReq(vos_msg_header_stru * pReqMsg);
	
	//调节光源灯光强
	vos_u32 _controlLight(vos_u32 intensity);

	//处理下位机动作响应
	vos_u32 _procDwnActRes(vos_msg_header_stru * pResMsg);

	//处理关灯响应
	vos_u32 _procCloseLightRes();

	//处理开灯响应
	vos_u32 _procOpenLightRes();
	
	//启动暗电流测试
	vos_u32 _enDarkCurrentTest();

	//处理AD值上报
	vos_u32 _procAdRes(vos_msg_header_stru * pResMsg);

	//延时100ms
	vos_u32 _timeDelay(vos_u32 timeLen);
	
	//上报采集到上位机
	vos_u32 _sendToUpmach();

	//处理停止请求
	vos_u32 _procStopReq();

	//处理超时
	vos_u32 _procTimeOut(vos_msg_header_stru* pResMsg);

	//读取暗电流消息
	vos_u32 _prodpValue(vos_msg_header_stru * pResMsg);
private:
	vos_u32 m_timerId;
	vos_u32 m_srcPid;
	int m_testCount;
	vos_u32 m_testState;
	STResDarkCurrentTest m_stTestResResult;
};

#endif

