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


//�������״̬
typedef enum{
	STATE_TEST_IDLE        			=0,   //����
    STATE_CLOSE_LIGHT      			=1,   //�ع�Դ��
    STATE_OPEN_LIGHT       			=2,   //����Դ��
    STATE_DARKCURRENT_TEST_OVER     =3,   //���Խ���
     
    STATE_TEST_BUFF,
}darkCurrentStateEunm;

class CDarkCurrent : public CCallBack
{
public:
    CDarkCurrent(vos_u32 ulPid);

public:
	virtual vos_u32 CallBack(vos_msg_header_stru* pMsg);

private:

	//����������������
	vos_u32 _procDarkCurrentReq(vos_msg_header_stru * pReqMsg);
	
	//���ڹ�Դ�ƹ�ǿ
	vos_u32 _controlLight(vos_u32 intensity);

	//������λ��������Ӧ
	vos_u32 _procDwnActRes(vos_msg_header_stru * pResMsg);

	//����ص���Ӧ
	vos_u32 _procCloseLightRes();

	//��������Ӧ
	vos_u32 _procOpenLightRes();
	
	//��������������
	vos_u32 _enDarkCurrentTest();

	//����ADֵ�ϱ�
	vos_u32 _procAdRes(vos_msg_header_stru * pResMsg);

	//��ʱ100ms
	vos_u32 _timeDelay(vos_u32 timeLen);
	
	//�ϱ��ɼ�����λ��
	vos_u32 _sendToUpmach();

	//����ֹͣ����
	vos_u32 _procStopReq();

	//����ʱ
	vos_u32 _procTimeOut(vos_msg_header_stru* pResMsg);

	//��ȡ��������Ϣ
	vos_u32 _prodpValue(vos_msg_header_stru * pResMsg);
private:
	vos_u32 m_timerId;
	vos_u32 m_srcPid;
	int m_testCount;
	vos_u32 m_testState;
	STResDarkCurrentTest m_stTestResResult;
};

#endif

