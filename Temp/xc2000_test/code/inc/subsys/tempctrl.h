#ifndef __TEMP_CTRL_H__
#define __TEMP_CTRL_H__

#include "subsys.h"
#include "app_downmsg.h"
#include <map>

class CTempCtrl : public CSubSys
{
public:
    CTempCtrl(vos_u32 ulPid, vos_u32 ulDstSubsys);
    ~CTempCtrl();

private:
    //��ʼ����������
    void _initParseFuc();

    //�����Զ���ϴ����
    vos_u32 _parseTempSwitch(msg_stComDevActInfo* pComAct);

    //�����¶Ȳ�ѯ
    vos_u32 _parseTempQuery(msg_stComDevActInfo* pComAct);

	//�¿�˯��
	vos_u32 _preseTempSleep(msg_stComDevActInfo* pComAct);
    vos_u32 _parseSetWTemp(msg_stComDevActInfo* pComAct);
    vos_u32 _parseSetRTemp(msg_stComDevActInfo* pComAct);
    //�¿ؿ���
    vos_u32 _parseTempControl(msg_stComDevActInfo* pCom);
};

#endif