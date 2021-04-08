#ifndef __OAMMNG_H__
#define __OAMMNG_H__

#include "vos_pid.h"
#include "msg.h"
#include "compensation.pb.h"

#define  TEMPQUERY_TIME_LEN 20

class COamMng : public CCallBack
{
    typedef vos_u32(COamMng::*ProcMsg)(vos_msg_header_stru* pMsg);
    
public:
    COamMng(vos_u32 ulPid);
    ~COamMng();

public:
    //��Ϣ���
    virtual vos_u32 CallBack(vos_msg_header_stru* pMsg);

    //��ʼ���豸
    void InitMachine();

private:
    //��ʼ����Ϣ������
    void _initProcFuc();

    //��ʼ����Ҫ����ֹͣ�����ģ��
    void _initStopPid();

    //ֹͣ��������
    vos_u32 _procStopMach(vos_msg_header_stru* pMsg);

    //������ת�Լ�����
    vos_u32 _procKeyMoveOuterDsk(vos_msg_header_stru* pMsg);

    // ������ת�Լ�����
    vos_u32 _procKeyMoveInnerDsk(vos_msg_header_stru* pMsg);

    //ת���Լ���
    vos_u32 _moveReagDsk(vos_u32 dstPid, char ucCupNum);

    //����ʱ
    vos_u32 _procTimeOut(vos_msg_header_stru* pMsg);

    //�����¶Ȳ�ѯ
    vos_u32 _procQueryTemp(vos_msg_header_stru* pMsg);

    //��ȡ��λ����־
    vos_u32 _procGetMidLog(vos_msg_header_stru* pMsg);

    //����������λ��Ӧ
    vos_u32 _procResetRes(vos_msg_header_stru* pMsg);

    //�򿪹�Դ��
    vos_u32 _setLampBrightness();
    //���¿� �л�����̬
    vos_u32 _openControl();

    //�����·�
    void _setOffset(CMcu *pCMcu);
    //�������ָ�����
    vos_u32 _procCompRecover(vos_msg_header_stru* pMsg);
    vos_u32 _procMotorRecover(vos_msg_header_stru* pMsg);

    void _saveMotor(const STSubSysMotor& pReqMotor);

    //��������������
    vos_u32 _procReqCompBack(vos_msg_header_stru* pMsg);
    vos_u32 _procReqMotorBack(vos_msg_header_stru* pMsg);

    //����������
    vos_u32 _procResCompBack(vos_msg_header_stru* pMsg);
    vos_u32 _procResMotorBack(vos_msg_header_stru* pMsg);

    
    //���Ҳ���
    CMcu* _findCompData(vos_u32 comptype, vos_u32 subsysid);
    //����������
    void _saveOffsetPara(const STOffsetPara & pPara, vos_u32 subsysid);

    //������λ��������Ӧ
    vos_u32 _procDwnRes(vos_msg_header_stru* pMsg);

	vos_u32 _procReactDiskTemp(vos_msg_header_stru* pMsg);

private:
    map<app_u16, ProcMsg>  m_procFucs;     //��Ϣ����������
    vector<vos_u32> m_vStopPids;    //��Ҫֹͣ��ģ���pid

    vos_u32 m_ulSrcPid;
    vos_u32 m_ulSrcIp;
    vos_u32 m_ulInitTimer;
    CMcuList m_compBack;
    CCompSetRes m_compSetRes;
    STMotorList m_motorList;
    int m_waitCount;
public:    
    static vos_u32 m_ulTimer;     //��ʱ��
    static vos_u32 CompBackFlg;
};

#endif