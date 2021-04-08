#ifndef __SMOOTH_H__
#define __SMOOTH_H__

#include "vos_pid.h"
#include "upmsg.pb.h"
#include "xc8002_middwnmachshare.h"

#define SMOOTH_TIME_LEN 50
#define DELAY_TIME_LEN_1000MS 1000

class CSmooth : public CCallBack
{
public:
    CSmooth(vos_u32 ulPid);

public:
	virtual vos_u32 CallBack(vos_msg_header_stru* pMsg);

private:

	//处理平坦区测试请求
	vos_u32 _procSmoothReq(vos_msg_header_stru * pReqMsg);

	//处理复位响应
	vos_u32 _procResetRes();

	//开启AD采集
	vos_u32 _openAdStart();

	//关闭AD采集
	vos_u32 _closeAdStart();

	//转动反应盘
	vos_u32 _rttReacDsk();

	//处理下位机动作响应
	vos_u32 _procDwnActRes();

	//处理AD值上报
	vos_u32 _procAdRes();

	//延时
	vos_u32 _timeDelay(vos_u32 timeLen);
	
	//上报采集到上位机
	vos_u32 _sendToUpmach();


private:
	vos_u32 m_srcPid;
	vos_u32 m_circleNum;
	vos_u32 m_cupId;
	vos_u32 m_busy;
	STResSmoothAreaTest m_stTestResResult;
};

#endif

