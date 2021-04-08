#ifndef __OUTER_DSK_H__
#define __OUTER_DSK_H__

#include "subsys.h"
#include "app_downmsg.h"
#include <map>

class COuterDsk : public CSubSys
{
public:
    COuterDsk(vos_u32 ulPid);
    ~COuterDsk();

private:
    //��ʼ����������
    void _initParseFuc();

    vos_u32 _parseMove(msg_stComDevActInfo* pComAct);
	vos_u32 _parseBcScan(msg_stComDevActInfo* pComAct);
    
    //����΢������
    vos_u32 _parseTrimOffset(msg_stComDevActInfo* pComAct);

    //��������ʱ��ת��
    vos_u32 _parseDskHrotate(msg_stComDevActInfo* pComAct);

	//����ʾ�������ؿ���
	vos_u32 ParseShowave(msg_stComDevActInfo* pComAct);

    //�����߼�
    vos_u32 _parseUpFpga(msg_stComDevActInfo* pComAct);
};

#endif