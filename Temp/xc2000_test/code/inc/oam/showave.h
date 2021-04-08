#ifndef __SHOWAVE_H__
#define __SHOWAVE_H__

#include "vos_pid.h"
#include "upmsg.pb.h"
#include "xc8002_middwnmachshare.h"
#include <map>

using namespace std;

#define WAVE_SMP_SIZE (30)

//虚拟示波器采样上报
typedef struct
{
    app_u16 cmd;              //上报命令关键字:0xff04
	app_u16 usstatustype; //状态类型  
    vos_u32 wave_smp[WAVE_SMP_SIZE];
} xc8002_wave_smp_report_stru;

class CShowave : public CCallBack
{
public:
    CShowave(vos_u32 ulPid);

public:
	virtual vos_u32 CallBack(vos_msg_header_stru* pMsg);

private:
	//处理端口切换请求
	vos_u32 _procChangePort(vos_msg_header_stru * pReqMsg);
		
	//处理虚拟示波器请求
	vos_u32 _procShowaveReq(vos_msg_header_stru * pReqMsg);

	//开关虚拟示波
	vos_u32 _ctrlShowave(STReqShowWave * pReq);

	//停止请求
	vos_u32 _procStopReq();

	//下发控制命令
	vos_u32 _sendToDwn(vos_u32 dstPid, vos_u32 dwnChannel, vos_u32 switchState);

	//处理信号值上报
	vos_u32 _procSigValReport(vos_msg_header_stru * pResMsg);
	
	//上报采集到上位机
	vos_u32 _sendToUpmach();


private:
	vos_u32 m_srcPid;
	#ifdef COMPILER_IS_LINUX_GCC
	vos_u32 m_chan0Open;
	vos_u32 m_chan1Open;
	map<vos_u32,vos_u32> m_chanToSig;
	#endif
	map<vos_u32,vos_u32> m_SigTochan;
	STResShowWave m_stTestResResult;
};

#endif

