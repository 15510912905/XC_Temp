#ifndef __DWM_CMD_H__
#define __DWM_CMD_H__

#include "vos_pid.h"

#include "msg.h"
#include "app_comstructdefine.h"

class CDwnCmd : public CCallBack
{
    typedef void(CDwnCmd::*ReqAct)(STReqActCmd *pReq);
    typedef void(CDwnCmd::*ProcResFuc)(app_u32 subSys, app_com_dev_act_result_stru* pRes);

public:
    CDwnCmd(vos_u32 ulPid);
    ~CDwnCmd();
    
public:
    virtual vos_u32 CallBack(vos_msg_header_stru* pMsg);
        
private:
    //����������
    vos_u32 _procActCmdReq(vos_msg_header_stru* pMsg);
    vos_u32 _procQueryVres();
    vos_u32 _procResVers(vos_msg_header_stru* pMsg);

    vos_u32 _procTimeOut(vos_msg_header_stru* pMsg);
    
    //����΢������
    void _reqTrimOffset(STReqActCmd* pReq);  

    //�����������
    void _reqSaveOffset(STReqActCmd* pReq);

    //�����������
    void _reqSaveMotor(STReqActCmd* pReq);

    void _reqSetWTemp(STReqActCmd* pReq);
    //������λ����ѯ
    void _reqDwnQuery(STReqActCmd* pReq);

    //������ˮƽ�˶�
    void _reqNdlMove(STReqActCmd* pReq);

    //������ˮƽת��
    void _reqDskHrotate(STReqActCmd* pReq);
    
    //�����봹ֱ�˶�
    void _reqNdlVrt(STReqActCmd* pReq);
    //
    void _reqReset(STReqActCmd* pReq);
    void _reqDskMove(STReqActCmd* pReq);
    void _reqMove(STReqActCmd* pReq);
    void _reqMix(STReqActCmd* pReq);
    void _reqWsah(STReqActCmd* pReq);
    void _reqLightCtl(STReqActCmd* pReq);
    void _reqMachineDebug(STReqActCmd* pReq);
	void _reqTempSwitch(STReqActCmd* pReq);
    vos_u32 _sendVersions();
    void _reqBcScan(STReqActCmd* pReq);
    vos_u32 _procTempeQuery();
	vos_u32 _procTempeADQuery();
    void _reqPVCtrl(STReqActCmd* pReq);
    void _reqLiquidDBG(STReqActCmd* pReq);
    vos_u32 _procADtest(vos_msg_header_stru* pMsg);
    void _reqSetFlState(STReqActCmd* pReq);
    void _reqPumpCtrl(STReqActCmd* pReq);
	void _reqOlnyMix(STReqActCmd* pReq);

    void _reqSetRTemp(STReqActCmd* pReq);
    vos_u32 _simulStaticTestAD();//ģ�ⷵ�ؾ�̬�ɹ�ֵ
    void _procResMsg(vos_msg_header_stru* pMsg);
    void _procReactTemp(app_u32 subSys, app_com_dev_act_result_stru* pRes);
    void _procWaterTemp(app_u32 subSys, app_com_dev_act_result_stru* pRes);
    void _procShowWave(vos_msg_header_stru* pMsg);
private:
    map<app_u16, ReqAct>  m_reqFucs;     //����������������    
    vos_u32 m_ulQueryTimer;
    vos_u32 m_ulCmdTimer;
    STVersions m_versions;
    std::map<app_u32, std::map<app_u32, ProcResFuc> > m_mResFuncs;

};








#endif

