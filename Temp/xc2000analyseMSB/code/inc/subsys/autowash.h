#ifndef __AUTO_WASH_H__
#define __AUTO_WASH_H__

#include "subsys.h"
#include "app_downmsg.h"
#include <map>

class CAutoWash : public CSubSys
{
public:
    CAutoWash(vos_u32 ulPid, vos_u32 ulDstSubsys);
    ~CAutoWash();

private:
    //��ʼ����������
    void _initParseFuc();

    //�����Զ���ϴ����
    vos_u32 _parseAutoWash(msg_stComDevActInfo* pComAct);
    
    void _sendConsum(msg_stAutoWash * pDwnCmd);
    //����΢������
    vos_u32 _parseTrimOffset(msg_stComDevActInfo* pComAct);

    vos_u32 _parseNdlVrt(msg_stComDevActInfo* pComAct);
 
	//������ѹ�ÿ���
	vos_u32 _ParseCtrlP04(msg_stComDevActInfo* pComAct);
    //�÷�����
    vos_u32 _parsePVCtrl(msg_stComDevActInfo* pComAct);
    //Һ·����������
    vos_u32 _parseLiquidsCheck(msg_stComDevActInfo* pComAct);
    //Һ·����
    vos_u32 _parseLiquidDBG(msg_stComDevActInfo* pComAct);
    //���ø���״̬
    vos_u32 _parseSetFlState(msg_stComDevActInfo* pComAct);

	vos_u32 _parseSleep(msg_stComDevActInfo* pComAct);
    //״̬�л�
    vos_u32 _parseSwitch(msg_stComDevActInfo* pComAct);
};

#endif
