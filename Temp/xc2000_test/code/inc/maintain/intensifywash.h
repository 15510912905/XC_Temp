#ifndef __INTENSIFY_WASH_H__
#define __INTENSIFY_WASH_H__

#include "vos_pid.h"

#include "msg.h"

class CIntensifyWash : public CCallBack
{
public:
    CIntensifyWash(vos_u32 ulPid);
    ~CIntensifyWash();

public:
    virtual vos_u32 CallBack(vos_msg_header_stru* pMsg);

private:
    //��������
	vos_u32 _procReq(vos_msg_header_stru* pMsg);

    //�ϱ����
    vos_u32 _reportRes(vos_u32 ulResult);

    //����λ��Ӧ
    vos_u32 _proResetRes(vos_msg_header_stru* pMsg);

    void _procMoveSmpDsk();
    void _procMoveReagDsk(app_u16 usCupId, app_u16 usDstPos, vos_u32 ulDstPid);

    //������ִ����Ӧ
    vos_u32 _procActRes(vos_msg_header_stru* pMsg);

    //����ǿ����ϴ����
    vos_u32 _sendIntensifyWash(vos_u32 ulDstPid);

    //����ִ�н��
    vos_u32 _procActResult();

private:
    vos_u32 m_ulTimer;     //��ʱ��
    vos_u32 m_ulResNum; //��Ӧ���صĸ���
    vos_u32 m_ulSrcPid; //ԴPid
    vos_u32 m_ulStep;
    bool   signDsk;
};

#endif