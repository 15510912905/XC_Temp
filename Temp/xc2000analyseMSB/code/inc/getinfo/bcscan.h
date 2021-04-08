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

//�������״̬
typedef enum{
	STATE_BCSCAN_IDLE        =0,   //����
    STATE_BCSCAN_RESET_NDL   =1,   //��λ��
    STATE_BCSCAN_RESET_DSK   =2,   //��λ��
    STATE_BCSCAN_TEST_OVER   =3,   //�������
     
    STATE_BCSCAN_BUFF,
}bcScanStateEunm;

class CBcScan : public CCallBack
{
public:
    CBcScan(vos_u32 ulPid);

public:
	virtual vos_u32 CallBack(vos_msg_header_stru* pMsg);

private:

	//��������ɨ������
	vos_u32 _procBcScanReq(vos_msg_header_stru * pReqMsg);

	//����λ��Ӧ
	vos_u32 _procResetRes(vos_msg_header_stru* pMsg);
	
	//�����븴λ��Ӧ
	vos_u32 _procResetNdlRes();

	//�����̸�λ��Ӧ
	vos_u32 _procResetDskRes();

	//�Լ���Ԥ������
    vos_u32 _reagDiskRtt(vos_u32 uiDskId);

	//ִ���Լ�ɨ��
	vos_u32 _exeReagBcScan(vos_u32 uiDskId,vos_u32 uiCupId);

	//ִ������ɨ��
	vos_u32 _exeSmpBcScan(vos_u32 uiDskId,vos_u32 uiCupId);

	//����ɨ����Ӧ
	vos_u32 _procDwnRes(vos_msg_header_stru* pResMsg);

	//�����Լ�ɨ����Ӧ
	vos_u32 _procReagBcScanRes(app_dwn_barcode_tlv_stru* pBcInfo);

	//��������ɨ����Ӧ
	vos_u32 _procSmpBcScanRes(app_dwn_barcode_tlv_stru* pBcInfo);  

	//����ֹͣ����
	vos_u32 _procStopReq();
	
	//����ʱ
	vos_u32 _procTimeOut(vos_msg_header_stru* pResMsg);

	//�����쳣
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
