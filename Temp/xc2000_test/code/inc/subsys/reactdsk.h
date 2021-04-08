#ifndef __REACT_DSK_H__
#define __REACT_DSK_H__

#include "subsys.h"
#include "app_downmsg.h"
#include <map>

class CReactDsk : public CSubSys
{
public:
    CReactDsk(vos_u32 ulPid);
    ~CReactDsk();

private:
    //��ʼ����������
    void _initParseFuc();

    //�����Զ���ϴ����
    vos_u32 _parseReset(msg_stComDevActInfo* pComAct);
    vos_u32 _parseMove(msg_stComDevActInfo* pComAct);
    vos_u32 _parseLight(msg_stComDevActInfo* pComAct);

    vos_u32 _parseAd(msg_stComDevActInfo* pComAct);
        
    //����΢������
    vos_u32 _parseTrimOffset(msg_stComDevActInfo* pComAct);

    //������ʱ��ת��
    vos_u32 _parseDskHrotate(msg_stComDevActInfo* pComAct);  
};

#endif