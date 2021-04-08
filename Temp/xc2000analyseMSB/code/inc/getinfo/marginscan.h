#ifndef __MARGINSCAN_H__
#define __MARGINSCAN_H__

#include <queue>
#include "upmsg.pb.h"

#include "vos_pid.h"

using namespace std;

#define  MarginScan_TIME_LEN 100

//����ɨ����Ӧ����
typedef struct
{
    app_u16 usemptydepth;  //���½��߶�
    app_u16 usrsv;
}msg_stScanResLoad;

//�������״̬
typedef enum{
	STATE_MARGINSCAN_IDLE        =0,   //����
    STATE_MARGINSCAN_RESET_NDL   =1,   //��λ��
    STATE_MARGINSCAN_RESET_DSK   =2,   //��λ��
    STATE_MARGINSCAN_TEST_OVER   =3,   //�������
     
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
	//��������ɨ������
	vos_u32 _procMarginScanReq(vos_msg_header_stru* pReqMsg);
	
    //����ɨ�豭λ��Ϣ����
    vos_u32 _putCupposIntoBuffer(vos_msg_header_stru* pReqMsg);
	
	//�븴λ
	vos_u32 _ndlReset();

	//����λ��Ӧ
	vos_u32 _procResetRes(vos_msg_header_stru* pMsg);

	//�����븴λ
	vos_u32 _procNdlReset();

	//�����̸�λ
	vos_u32 _procDskReset(vos_msg_header_stru* pMsg);

	//����ת��
	vos_u32 _outDiskRtt();

	//����ת��
	vos_u32 _inDiskRtt();

	//��ת����������
    vos_u32 _singleDiskRtt(vos_u32 dstPid,app_u16 uiCupId);

	//�����Ӷ�����Ӧ
    vos_u32 _procDwnRes(vos_msg_header_stru* pResMsg);

	//���������
	vos_u32 _singleNdlMarginScan(vos_u32 dstPid);

	//�洢����ɨ������Ϣ
	vos_u32 _putResultIntoBuffer(vos_msg_header_stru* pResMsg);

	//������ɨ�������Ӧ
	vos_u32 _procOneCupOverRes(vos_msg_header_stru* pResMsg);
	
	//������
	vos_u32 _procResult(vos_u32 ulDskPid,vos_u32 ulCupId,vos_u32 ulDepth);

	//����ʱ
	vos_u32 _procTimeOut(vos_msg_header_stru* pResMsg);

	//����ֹͣ����
	vos_u32 _procStopReq();

	//�����쳣
	vos_u32 _procAbnormal();

private:
	vos_u32 m_InulTimer;     //��ʱ��
	vos_u32 m_OutulTimer;     //��ʱ��
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


