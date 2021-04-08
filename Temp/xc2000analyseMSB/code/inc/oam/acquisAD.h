#ifndef __ACQUISAD_H__
#define __ACQUISAD_H__

#include "vos_pid.h"
#include "upmsg.pb.h"
#include "xc8002_middwnmachshare.h"

#define  TEMPAD_TIME_LEN  10

class CAcquisAD : public CCallBack
{
public:
	CAcquisAD(vos_u32 ulPid);

public:
	virtual vos_u32 CallBack(vos_msg_header_stru* pMsg);

private:
	vos_u32 m_Timer;
	//处理平坦区测试请求
	vos_u32 _procAcquisAdReq(vos_msg_header_stru * pMsg);//处理AD请求
	//处理AD值上报
	vos_u32 _procAdRes();
};









#endif