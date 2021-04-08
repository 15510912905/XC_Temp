#ifndef _vos_pid_h_
#define _vos_pid_h_    
/******************************************************************************

                  ��Ȩ���� (C), 2013-2023, �Ĵ��³�����Ƽ����޹�˾

 ******************************************************************************
  �� �� ��   : vos_pid.h
  �� �� ��   : ����
  ��    ��   : Albort,Feng
  ��������   : 2013��9��9��
  ����޸�   :
  ��������   : ���ƽ̨PIDģ��ͷ�ļ�����
  �����б�   :
  �޸���ʷ   :
  1.��    ��   : 2013��9��9��
    ��    ��   : Albort,Feng
    �޸�����   : �����ļ�

******************************************************************************/
#include "vos_basictypedefine.h"
#include "vos_messagecenter.h"


//��ƷFID����
typedef enum
{
    app_self_net_fid = 400,                       //�����߳�
    app_self_oam_fid = 500,					//��ƷOAMģ��FID(500)
    app_self_log_fid = 600,                      //��־�߳�
    app_resource_schedule_fid = 700,		//��Դ����ģ��FID(700)    
    app_dwnmach_cmd_center_fid = 800,  //��λ���������ģ��FID(800)
    app_subsys_mng_fid = 900,              //��ϵͳ�߳�FID��900��
    app_task_result_fid = 1000,              //�������߳�ID(1000)
    app_auto_test_fid = 1200,
}app_interior_fid_enum;

//ҵ��ģ��PID����
typedef enum
{
    Pid_DebugTty = 505,                //ģ����Դ���PID
    Pid_ReportStatus = 512,           //��λ��״̬�ϱ�����PID
    Pid_Maintain = 520,                 //ά��ģ��PID
    Pid_LoopTest = 523,    //�ƶ��豸����ģ��PID
    Pid_DwnMachUart = 801,        //��λ�����ڹ���ģ��PID
    Pid_SimulatorTest = 1201,       //ģ�����
}app_interior_business_pid_enum;

typedef vos_u32 (*vos_pidrcvmsgfunc)(vos_msg_header_stru*);

#ifdef __cplusplus
extern "C" {
#endif
/*****************************************************************************
 �� �� ��  : vos_registerpid
 ��������  : PIDģ��ע��ӿ�
 �������  : vos_u32 ulfid
             vos_u32 ulpid
             vos_pidrcvmsgfunc prcvmsgfunc
 �������  : ��
 �� �� ֵ  : vos_u32
 
 �޸���ʷ      :
  1.��    ��   : 2013��9��9��
    ��    ��   : Albort,Feng

    �޸�����   : �����ɺ���

*****************************************************************************/
vos_u32 vos_registerpid(vos_u32 ulfid, vos_u32 ulpid, vos_pidrcvmsgfunc prcvmsgfunc);

/*****************************************************************************
 �� �� ��  : vos_utreleasepid
 ��������  : �ͷ�PIDģ��
 �������  : vos_u32 ulpid
 �������  : ��
 �� �� ֵ  : vos_u32
 
 �޸���ʷ      :
  1.��    ��   : 2013��9��11��
    ��    ��   : Albort,Feng

    �޸�����   : �����ɺ���

*****************************************************************************/
vos_u32 vos_utreleasepid(vos_u32 ulpid);

/*****************************************************************************
 �� �� ��  : vos_utreleaseallpidoffid
 ��������  : �ͷ�һ��FIDģ�������е�PID
 �������  : vos_u32 ulfid
 �������  : ��
 �� �� ֵ  : void
 
 �޸���ʷ      :
  1.��    ��   : 2013��9��11��
    ��    ��   : Albort,Feng

    �޸�����   : �����ɺ���

*****************************************************************************/
void vos_utreleaseallpidoffid(vos_u32 ulfid);

#ifdef __cplusplus
}
#endif

class CCallBack
{
public:
    CCallBack(vos_u32 ulpid);
    virtual ~CCallBack();

    virtual vos_u32 CallBack(vos_msg_header_stru* pMsg) = 0;

    vos_u32 RegCallBack(vos_u32 ulfid);

protected:
    vos_u32 m_ulFid;
    vos_u32 m_ulPid;
};
vos_u32 vos_registerCallBack(vos_u32 ulfid, vos_u32 ulpid, CCallBack * pBack);

#endif
