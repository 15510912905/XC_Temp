#ifndef __MARGINSCAN_H__
#define __MARGINSCAN_H__

#include <queue>
#include "upmsg.pb.h"

#include "vos_pid.h"

using namespace std;

#define  MarginScan_TIME_LEN 100

//余量扫描响应负载
typedef struct
{
    app_u16 usemptydepth;  //针下降高度
    app_u16 usrsv;
}msg_stScanResLoad;

//定义测试状态
typedef enum{
	STATE_MARGINSCAN_IDLE        =0,   //空闲
    STATE_MARGINSCAN_RESET_NDL   =1,   //复位针
    STATE_MARGINSCAN_RESET_DSK   =2,   //复位盘
    STATE_MARGINSCAN_TEST_OVER   =3,   //测试完成
     
    STATE_MARGINSCAN_BUFF,
}marginScanStateEunm;

class CMarginScan : public CCallBack
{
	typedef vos_u32(CMarginScan::*MsgFuc)(vos_msg_header_stru* pMsg);

public:
    CMarginScan(vos_u32 ulPid);

public:
	virtual vos_u32 CallBack(vos_msg_header_stru* pMsg);

private:
	//处理条码扫描请求
	vos_u32 _procMarginScanReq(vos_msg_header_stru* pReqMsg);
	
    //将待扫描杯位信息缓存
    vos_u32 _putCupposIntoBuffer(vos_msg_header_stru* pReqMsg);
	
	//针复位
	vos_u32 _ndlReset();

	//处理复位响应
	vos_u32 _procResetRes(vos_msg_header_stru* pMsg);

	//处理针复位
	vos_u32 _procNdlReset();

	//处理盘复位
	vos_u32 _procDskReset(vos_msg_header_stru* pMsg);

	//外盘转动
	vos_u32 _outDiskRtt();

	//内盘转动
	vos_u32 _inDiskRtt();

	//盘转动基础函数
    vos_u32 _singleDiskRtt(vos_u32 dstPid,app_u16 uiCupId);

	//处理子动作响应
    vos_u32 _procDwnRes(vos_msg_header_stru* pResMsg);

	//针余量检测
	vos_u32 _singleNdlMarginScan(vos_u32 dstPid);

	//存储余量扫描结果信息
	vos_u32 _putResultIntoBuffer(vos_msg_header_stru* pResMsg);

	//处理单杯扫描结束响应
	vos_u32 _procOneCupOverRes(vos_msg_header_stru* pResMsg);
	
	//处理结果
	vos_u32 _procResult(vos_u32 ulDskPid,vos_u32 ulCupId,vos_u32 ulDepth);

	//处理超时
	vos_u32 _procTimeOut(vos_msg_header_stru* pResMsg);

	//处理停止请求
	vos_u32 _procStopReq();

	//处理异常
	vos_u32 _procAbnormal();

private:
	vos_u32 m_InulTimer;     //定时器
	vos_u32 m_OutulTimer;     //定时器
	vos_u32 m_uiSrcPid;
	vos_u32 m_uiOutCupId;
	vos_u32 m_uiInCupId;
	vos_u32 m_runState;
	vos_u32 m_resetNum;
	vos_u32 m_testState;
    queue<vos_u32> m_stOuterCups;
	queue<vos_u32> m_stInnerCups;
	STResMarginScan  m_stScanResult;

    vos_u32 m_r1DiskCup;
    vos_u32 m_r2DiskCup;
};

#endif


