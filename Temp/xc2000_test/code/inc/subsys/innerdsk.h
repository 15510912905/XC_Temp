#ifndef __INNER_DSK_H__
#define __INNER_DSK_H__

#include "subsys.h"
#include "app_downmsg.h"
#include <map>

class CInnerDsk : public CSubSys
{
public:
    CInnerDsk(vos_u32 ulPid);
    ~CInnerDsk();

private:
    //��ʼ����������
    void _initParseFuc();

    vos_u32 _parseMove(msg_stComDevActInfo* pComAct);
    vos_u32 _parseBcScan(msg_stComDevActInfo* pComAct);

    //����΢������
    vos_u32 _parseTrimOffset(msg_stComDevActInfo* pComAct);

    //������ʱ��ת��
    vos_u32 _parseDskHrotate(msg_stComDevActInfo* pComAct);

	//����ʾ�������ؿ���
	vos_u32 ParseShowave(msg_stComDevActInfo* pComAct);
};

#endif