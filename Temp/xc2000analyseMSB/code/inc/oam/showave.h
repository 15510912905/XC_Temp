#ifndef __SHOWAVE_H__
#define __SHOWAVE_H__

#include "vos_pid.h"
#include "upmsg.pb.h"
#include "xc8002_middwnmachshare.h"
#include <map>

using namespace std;

#define WAVE_SMP_SIZE (30)

//����ʾ���������ϱ�
typedef struct
{
    app_u16 cmd;              //�ϱ�����ؼ���:0xff04
	app_u16 usstatustype; //״̬����  
    vos_u32 wave_smp[WAVE_SMP_SIZE];
} xc8002_wave_smp_report_stru;

class CShowave : public CCallBack
{
public:
    CShowave(vos_u32 ulPid);

public:
	virtual vos_u32 CallBack(vos_msg_header_stru* pMsg);

private:
	//����˿��л�����
	vos_u32 _procChangePort(vos_msg_header_stru * pReqMsg);
		
	//��������ʾ��������
	vos_u32 _procShowaveReq(vos_msg_header_stru * pReqMsg);

	//��������ʾ��
	vos_u32 _ctrlShowave(STReqShowWave * pReq);

	//ֹͣ����
	vos_u32 _procStopReq();

	//�·���������
	vos_u32 _sendToDwn(vos_u32 dstPid, vos_u32 dwnChannel, vos_u32 switchState);

	//�����ź�ֵ�ϱ�
	vos_u32 _procSigValReport(vos_msg_header_stru * pResMsg);
	
	//�ϱ��ɼ�����λ��
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

