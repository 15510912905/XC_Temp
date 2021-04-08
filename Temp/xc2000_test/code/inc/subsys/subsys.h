#ifndef __SUBSYS_H__
#define __SUBSYS_H__

#include "app_innermsg.h"
#include "app_comstructdefine.h"
#include <queue>
#include "vos_pid.h"
#include "typedefine.pb.h"

using namespace std;

//����ִ�ж���״̬�����ݽṹ
typedef struct
{
    vos_u32 ulSrcIp;            //����ԴIP
    vos_u32 ulSrcPid;          //����ԴPID
    app_u16 usResMsgType;       //��Ӧ��Ϣ����
}StMsgHead;

//ע���Ӷ������ݽṹ
typedef struct
{
    msg_stActHead stActHead;
    app_u16 usRsltInfoLen;
    char  aucActRsltInfo[APP_MAX_CMD_RSLT_INFO_LEN];
    vos_u32 ulCmdInfoLen;    //������Ϣstcmdinfo�ĳ���
    char stDwnCmd[0];
}STActInfo;

class CSubSys : public CCallBack
{
protected:
    typedef vos_u32(CSubSys::*ParseAct)(msg_stComDevActInfo* pComAct);
public:
    CSubSys(vos_u32 ulPid, vos_u32 ulDstSubsys);
    virtual ~CSubSys();

public:   
    //�ص�����
    virtual vos_u32 CallBack(vos_msg_header_stru* pMsg);

    //��������
    vos_u32 ProcReqMsg(vos_msg_header_stru* pMsg);

	//��λ����ѯ
    vos_u32 ParseDwnQuery(msg_stComDevActInfo* pComAct);
    
    //�����л�����״̬
    vos_u32 ParseMachineDBG(msg_stComDevActInfo* pComAct);
	
	//��λ�����ϻָ�
	vos_u32 ParseDwnFault(msg_stComDevActInfo* pComAct);

	//��λ��״̬�·�����λ��
	vos_u32 ParseMidState(msg_stComDevActInfo* pComAct);
	
    //�����λ����־
    vos_u32 ParseBBCLEAR(msg_stComDevActInfo* pComAct);

    vos_u32 ParseSingleCMD(msg_stComDevActInfo* pComAct);

    vos_u32 ParseNdlReset(msg_stComDevActInfo* pComAct);
protected:
    //��ʼ��ִ�ж���
    STActInfo * InitActInfo(msg_stActHead& stActHead, vos_u32 cmdLen);

    //����΢������
    vos_u32 ParseTrimOffset(msg_stComDevActInfo* pComAct, vos_u32 cmd);

    //���ò�������
    vos_u32 ParseSaveOffset(msg_stComDevActInfo* pComAct);

    //������λ��
    vos_u32 ParseUpdateDwn(msg_stComDevActInfo* pComAct);

    //��λ�����
    vos_u32 ParseWriteMotor(msg_stComDevActInfo* pComAct);
	
	//����FPGA
    vos_u32 ParseUpFpga(msg_stComDevActInfo* pComAct, vos_u32 cmd);

private:
    //����ִ�ж���
    vos_u32 ParseActInfo(vos_msg_header_stru* pMsg);

    //������Ϣ
    vos_u32 _pushMsgToBuf(vos_msg_header_stru* pMsg);

    //ȡ��������Ϣ
    char* _popMsgFromBuf();

    //ִ��������Ϣ
    vos_u32 _procReqMsg();

    //���������������
    vos_u32 _sendCmdToDwnmach();

    //���������
    vos_u32 _sendCmdAct(STActInfo * pActInfo);

    //�ϱ���Ӧ��Ϣ
    vos_u32 _reportRes(STActInfo* pActInfo, vos_u32 ulResult);

    //��ʼ������
    void _initialize();

    //��������������Ӧ
    vos_u32 _procResMsg(vos_msg_header_stru* pMsg);

    //�����ǰ��Ϣ
    void _clearCurAct();

    //������Ϣͷ
    void _parseMsgHead(vos_msg_header_stru* pMsg);

    //�����Ϣ����
    void _clearMsgBuf();

    bool _checkAct(app_u32 subSys, app_u32 actType);
    void _setAct(app_u32 subSys, app_u32 actType, bool bAct);
protected:
    map<app_u16, ParseAct>  m_parseFucs;     //����������������
    map<app_u16, app_u16> m_mActCMD;

private:
    vos_u32 m_ulDstSubsys;  //Ŀ����ϵͳ
    vos_u32 m_ulBusy; //�Ƿ����ڴ�����Ϣ
    vos_u32 m_ulTimer;     //��ʱ��

    StMsgHead m_stHead; //���ڴ������Ϣͷ
    queue<char *> m_stCurAct; //���ڴ������Ϣ����
    queue<char *> m_stMsgBuf;  //��Ϣ����

    static bool m_bReactDisk;
    static bool m_bAutoWash;
    static bool m_bRmix;
    static bool m_bSMix;
};

#endif
