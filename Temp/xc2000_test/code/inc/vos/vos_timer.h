#ifndef _vos_timer_h_
#define _vos_timer_h_ 
/******************************************************************************

                  ��Ȩ���� (C), 2013-2023, �Ĵ��³�����Ƽ����޹�˾

 ******************************************************************************
  �� �� ��   : vos_timer.h
  �� �� ��   : ����
  ��    ��   : zhangchaoyong
  ��������   : 2014��1��6��
  ����޸�   :
  ��������   : vos d��ʱ��������ݽṹ����������
  �����б�   :
  �޸���ʷ   :
  1.��    ��   : 2014��1��6��
    ��    ��   : zhangchaoyong
    �޸�����   : �����ļ�

******************************************************************************/
#include "vos_basictypedefine.h"
#include "vos_messagecenter.h"

#define VOS_TIMERNAME_LEN   (64)
#define VOS_MAX_PID_TIMER   (10)
#define  vos_pidmsgtype_timer_timeout  0x000a   //��ʱ��ʱ
//��ʱ������
typedef enum
{
    vos_timer_type_once  = 1,//һ���Զ�ʱ��
    vos_timer_type_loop  = 2,//����Զ�ʱ��
}vos_timer_type_enum;

typedef vos_u32 vos_timer_type_enum_u32;

//��ʱ�������ṹ��
typedef struct 
{
    vos_u32 entimertype;                     //��ʱ������
    vos_u32 ultimelen;                       //��ʱʱ�䳤��(��λ:0.1��)
    vos_u32 ulsrcpid;                        //������ʱ��ϢPID
    vos_u32 aucdstpid[VOS_MAX_PID_TIMER];    //��ʱ��Ŀ��PID
    char  auctimername[VOS_TIMERNAME_LEN]; //��ʱ������
}vos_timer_reg_stru;

//��ʱ����Ϣ��Ϣ
typedef struct vos_timermsg
{
    vos_msg_header;
    vos_u32 ultimerid;                      //��ʱ��ID
    char  auctimername[VOS_TIMERNAME_LEN];//��ʱ������
}vos_timeroutmsg_stru;

#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************************
 �� �� ��  : vos_createtimer
 ��������  : �½�һ����ʱ��
 �������  : vos_timer_reg_stru *regstru
              vos_u32 *ultimedid
 �������  : ��
 �� �� ֵ  : vos_u32
 
 �޸���ʷ      :
  1.��    ��   : 2014��1��6��
    ��    ��   : zhangchaoyong

    �޸�����   : �����ɺ���

*****************************************************************************/
vos_u32 vos_createtimer(vos_timer_reg_stru *regstru, vos_u32 *ptimerid);

/*****************************************************************************
 �� �� ��  : vos_deletetimer
 ��������  : ɾ��һ����ʱ��
 �������  : vos_u32 ultimerid
 �������  : ��
 �� �� ֵ  : vos_u32 
 
 �޸���ʷ      :
  1.��    ��   : 2014��1��6��
    ��    ��   : zhangchaoyong

    �޸�����   : �����ɺ���

*****************************************************************************/
vos_u32  vos_deletetimer(vos_u32 ultimerid);

/*****************************************************************************
 �� �� ��  : vos_starttimer
 ��������  : ����һ����ʱ��
 �������  : vos_u32 ultimerid
 �������  : ��
 �� �� ֵ  : vos_u32 
 
 �޸���ʷ      :
  1.��    ��   : 2014��1��6��
    ��    ��   : zhangchaoyong

    �޸�����   : �����ɺ���

*****************************************************************************/
vos_u32  vos_starttimer(vos_u32 ultimerid);

/*****************************************************************************
 �� �� ��  : vos_stoptimer
 ��������  : ֹͣһ����ʱ��
 �������  : vos_u32 ultimerid
 �������  : ��
 �� �� ֵ  : vos_u32 
 
 �޸���ʷ      :
  1.��    ��   : 2014��1��6��
    ��    ��   : zhangchaoyong

    �޸�����   : �����ɺ���

*****************************************************************************/
vos_u32 vos_stoptimer(vos_u32 ultimerid);

/*****************************************************************************
 �� �� ��  : vos_checktimerisexist
 ��������  : �ж϶�ʱ���Ƿ����
 �������  : char *pctimername
 �������  : ��
 �� �� ֵ  : vos_u32 (VOS_YES: ָ�����ƵĶ�ʱ���Ѵ���,VOS_NO:ָ���Ķ�ʱ��δ����)
 
 �޸���ʷ      :
  1.��    ��   : 2014��1��13��
    ��    ��   : zhangchaoyong

    �޸�����   : �����ɺ���

*****************************************************************************/
vos_u32 vos_checktimerisexist(char *timername);

/*****************************************************************************
 �� �� ��  : vos_settimertimelen
 ��������  : ���ö�ʱ����ʱ����
 �������  : vos_u32 ultimerid
              vos_u32 ulmillisecond
 �������  : ��
 �� �� ֵ  : vos_u32
 
 �޸���ʷ      :
  1.��    ��   : 2014��1��6��
    ��    ��   : zhangchaoyong

    �޸�����   : �����ɺ���

*****************************************************************************/
vos_u32 vos_settimertimelen(vos_u32 ultimerid,vos_u32 ulmillisecond);

/*****************************************************************************
 �� �� ��  : vos_gettimerreginfo
 ��������  : ��ȡ��ʱ��ע����Ϣ
 �������  : vos_u32 ultimerid
             vos_timer_reg_stru* pouttimerinfo
 �������  : ��
 �� �� ֵ  : vos_u32
 ���ú���  : 
 ��������  : 
 
 �޸���ʷ      :
  1.��    ��   : 2014��9��17��
    ��    ��   : Albort,Feng
    �޸�����   : �����ɺ���

*****************************************************************************/
vos_u32 vos_gettimerreginfo(vos_u32 ultimerid, vos_timer_reg_stru* pouttimerinfo);

/*****************************************************************************
 �� �� ��  : vos_gettimerrunedtimelen
 ��������  : ��ȡ��ʱ��������ʱ�䳤��
 �������  : vos_u32 ultimerid
 �������  : ��
 �� �� ֵ  : vos_u32 (��ʱ��������ʱ�䳤��,��λΪ100ms)
 ���ú���  : 
 ��������  : 
 
 �޸���ʷ      :
  1.��    ��   : 2014��9��17��
    ��    ��   : Albort,Feng
    �޸�����   : �����ɺ���

*****************************************************************************/
vos_u32 vos_gettimerrunedtimelen(vos_u32 ultimerid);

/*****************************************************************************
 �� �� ��  : vos_gettimerstatus
 ��������  : ��ȡָ����ʱ���ĵ�ǰ״̬
 �������  : vos_u32 ultimerid
             vos_u32* pulouttimerstatus
 �������  : ��
 �� �� ֵ  : vos_u32
 ���ú���  : 
 ��������  : 
 
 �޸���ʷ      :
  1.��    ��   : 2015��5��15��
    ��    ��   : Albort,Feng
    �޸�����   : �����ɺ���

*****************************************************************************/
vos_u32 vos_gettimerstatus(vos_u32 ultimerid, vos_u32* pulouttimerstatus);

/*****************************************************************************
 �� �� ��  : vos_printalltimerinfo
 ��������  : ��ӡ���ж�ʱ����Ϣ
 �������  : void
 �������  : ��
 �� �� ֵ  : void
 ���ú���  : 
 ��������  : 
 
 �޸���ʷ      :
  1.��    ��   : 2014��9��4��
    ��    ��   : Albort,Feng
    �޸�����   : �����ɺ���

*****************************************************************************/
void vos_printalltimerinfo(void);

//������ʱ��
vos_u32 createTimer(vos_u32 ulPid, const char* strName, vos_u32 ulTimeLen = 0, vos_u32 ulTimerType = vos_timer_type_once);

//��ʼ��ʱ��
vos_u32 startTimer(vos_u32 ulTimer, vos_u32 ulTimeLen);

//��ʼ����ʱ��ģ��
vos_u32 vos_init_timermd(void);

#ifdef __cplusplus
}
#endif
#endif
