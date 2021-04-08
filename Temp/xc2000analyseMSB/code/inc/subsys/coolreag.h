#ifndef __COOL_REAG_H__
#define __COOL_REAG_H__

#include "subsys.h"
#include "app_downmsg.h"
#include <map>

class CCoolReag : public CSubSys
{
public:
    CCoolReag(vos_u32 ulPid, vos_u32 ulDstSubsys);
    ~CCoolReag();

private:
    //��ʼ����������
    void _initParseFuc();

    //�����¶Ȳ�ѯ
    vos_u32 _parseTempQuery(msg_stComDevActInfo* pComAct);
    //���俪��
    vos_u32 _parseCoolingCtrl(msg_stComDevActInfo* pComAct);
};

#endif
