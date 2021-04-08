#ifndef __BCSCAN_H__
#define __BCSCAN_H__

#include <queue>
#include <map>
#include <vector>
#include "upmsg.pb.h"

#include "vos_pid.h"
#include "xc8002_middwnmachshare.h"

using namespace std;

#define  BCSCAN_TIME_LEN 100
#define  BCSCAN_INTERVAL_TIME_LEN 1000

typedef enum
{
	INNERDSK_BCSCAN_POS = 39,
	OUTERDSK_BCSCAN_POS = 51,
}BcScanPreparePos;

//定义测试状态
typedef enum{
	STATE_BCSCAN_IDLE        =0,   //空闲
    STATE_BCSCAN_RESET_NDL   =1,   //复位针
    STATE_BCSCAN_RESET_DSK   =2,   //复位盘
    STATE_BCSCAN_TEST_OVER   =3,   //测试完成
     
    STATE_BCSCAN_BUFF,
}bcScanStateEunm;

class CBcScan : public CCallBack
{
public:
    CBcScan(vos_u32 ulPid);

public:
	virtual vos_u32 CallBack(vos_msg_header_stru* pMsg);

private:

	//处理条码扫描请求
	vos_u32 _procBcScanReq(vos_msg_header_stru * pReqMsg);

	//处理复位响应
	vos_u32 _procResetRes(vos_msg_header_stru* pMsg);
	
	//处理针复位响应
	vos_u32 _procResetNdlRes();

	//处理盘复位响应
	vos_u32 _procResetDskRes();

	//试剂盘预备动作
    vos_u32 _reagDiskRtt(vos_u32 uiDskId);

	//执行试剂扫描
	vos_u32 _exeReagBcScan(vos_u32 uiDskId,vos_u32 uiCupId);

	//执行样本扫描
	vos_u32 _exeSmpBcScan(vos_u32 uiDskId,vos_u32 uiCupId);

	//处理扫描响应
	vos_u32 _procDwnRes(vos_msg_header_stru* pResMsg);

	//处理试剂扫描响应
	vos_u32 _procReagBcScanRes(app_dwn_barcode_tlv_stru* pBcInfo);

	//处理样本扫描响应
	vos_u32 _procSmpBcScanRes(app_dwn_barcode_tlv_stru* pBcInfo);  

	//处理停止请求
	vos_u32 _procStopReq();
	
	//处理超时
	vos_u32 _procTimeOut(vos_msg_header_stru* pResMsg);

	//处理异常
	vos_u32 _procAbnormal();

private:
	vos_u32 m_reagTimer;
	vos_u32 m_smpTimer;
	vos_u32 m_SrcPid;
	vos_u32 m_testState;
	vos_u32 m_resetNum;
	vos_u32 m_runState;
    vector<STCupPos> m_reagCupInfo;
	vector<STCupPos> m_smpCupInfo;
	STResBarcodeScan  m_stScanResult;
};

#endif
