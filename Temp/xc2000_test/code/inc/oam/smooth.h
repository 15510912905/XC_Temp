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

	//����ƽ̹����������
	vos_u32 _procSmoothReq(vos_msg_header_stru * pReqMsg);

	//����λ��Ӧ
	vos_u32 _procResetRes();

	//����AD�ɼ�
	vos_u32 _openAdStart();

	//�ر�AD�ɼ�
	vos_u32 _closeAdStart();

	//ת����Ӧ��
	vos_u32 _rttReacDsk();

	//������λ��������Ӧ
	vos_u32 _procDwnActRes();

	//����ADֵ�ϱ�
	vos_u32 _procAdRes();

	//��ʱ
	vos_u32 _timeDelay(vos_u32 timeLen);
	
	//�ϱ��ɼ�����λ��
	vos_u32 _sendToUpmach();


private:
	vos_u32 m_srcPid;
	vos_u32 m_circleNum;
	vos_u32 m_cupId;
	vos_u32 m_busy;
	STResSmoothAreaTest m_stTestResResult;
};

#endif

