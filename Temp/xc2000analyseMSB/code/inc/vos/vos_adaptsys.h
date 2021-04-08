#ifndef _vos_adaptsys_h_
#define _vos_adaptsys_h_    
/******************************************************************************

                  ��Ȩ���� (C), 2013-2023, �Ĵ��³�����Ƽ����޹�˾

 ******************************************************************************
  �� �� ��   : vos_adaptsys.h
  �� �� ��   : ����
  ��    ��   : Albort,Feng
  ��������   : 2013��8��28��
  ����޸�   :
  ��������   : ����ϵͳ����ģ��ͷ�ļ�
  �����б�   :
  �޸���ʷ   :
  1.��    ��   : 2013��8��28��
    ��    ��   : Albort,Feng
    �޸�����   : �����ļ�

******************************************************************************/
#include "vos_configure.h"
#include "vos_basictypedefine.h"
#include <sys/time.h>

#define NAME_SIZE  (256)
#define VOS_EXIST_THREAD_CODE (0x00f0f0f0)
#define VOS_MAX_IP_BUF_LEN    (24)

typedef vos_u32 (*vos_pthreadfunc)(void*);
typedef void (*enventcallback)(void);

typedef vos_u32 vos_tid;
typedef int vos_socket;

typedef enum
{
    vos_thread_init_status_susppend = 0,  //�̴߳��������
    vos_thread_init_status_running  = 1,  //�̴߳���������

    vos_thread_init_status_butt
}vos_thread_init_status_enum;

typedef enum
{
    vos_thread_sched_normal = 0,  //��ͨ�̵߳��Ȳ���
    vos_thread_sched_rtos   = 1,  //ʵʱ�̵߳��Ȳ���
}vos_thread_sched_policy;

//�����߳����ȼ�
typedef enum
{
    vos_thread_pri_lowest = 0,
    vos_thread_pri_1,
    vos_thread_pri_2,
    vos_thread_pri_3,
    vos_thread_pri_4,
    vos_thread_pri_5,
    vos_thread_pri_6,
    vos_thread_pri_7,
    vos_thread_pri_8,
    vos_thread_pri_9,
    vos_thread_pri_10,
    vos_thread_pri_11,
    vos_thread_pri_12,
    vos_thread_pri_13,
    vos_thread_pri_14,
    vos_thread_pri_15,
    vos_thread_pri_16,
    vos_thread_pri_17,
    vos_thread_pri_18,
    vos_thread_pri_19,
    vos_thread_pri_20,
    vos_thread_pri_21,
    vos_thread_pri_22,
    vos_thread_pri_23,
    vos_thread_pri_24,
    vos_thread_pri_25,
    vos_thread_pri_26,
    vos_thread_pri_27,
    vos_thread_pri_28,
    vos_thread_pri_29,
    vos_thread_pri_30,
    vos_thread_pri_31,
    vos_thread_pri_32,
    vos_thread_pri_33,
    vos_thread_pri_34,
    vos_thread_pri_35,
    vos_thread_pri_36,
    vos_thread_pri_37,
    vos_thread_pri_38,
    vos_thread_pri_39,
    vos_thread_pri_40,
    vos_thread_pri_41,
    vos_thread_pri_42,
    vos_thread_pri_43,
    vos_thread_pri_44,
    vos_thread_pri_45,
    vos_thread_pri_46,
    vos_thread_pri_47,
    vos_thread_pri_48,
    vos_thread_pri_49,
    vos_thread_pri_50,
    vos_thread_pri_51,
    vos_thread_pri_52,
    vos_thread_pri_53,
    vos_thread_pri_54,
    vos_thread_pri_55,
    vos_thread_pri_56,
    vos_thread_pri_57,
    vos_thread_pri_58,
    vos_thread_pri_59,
    vos_thread_pri_60,
    vos_thread_pri_61,
    vos_thread_pri_62,
    vos_thread_pri_63,
    vos_thread_pri_64,
    vos_thread_pri_65,
    vos_thread_pri_66,
    vos_thread_pri_67,
    vos_thread_pri_68,
    vos_thread_pri_69,
    vos_thread_pri_70,
    vos_thread_pri_71,
    vos_thread_pri_72,
    vos_thread_pri_73,
    vos_thread_pri_74,
    vos_thread_pri_75,
    vos_thread_pri_76,
    vos_thread_pri_77,
    vos_thread_pri_78,
    vos_thread_pri_79,
    vos_thread_pri_80,
    vos_thread_pri_81,
    vos_thread_pri_82,
    vos_thread_pri_83,
    vos_thread_pri_84,
    vos_thread_pri_85,
    vos_thread_pri_86,
    vos_thread_pri_87,
    vos_thread_pri_88,
    vos_thread_pri_89,
    vos_thread_pri_90,
    vos_thread_pri_91,
    vos_thread_pri_92,
    vos_thread_pri_93,
    vos_thread_pri_94,
    vos_thread_pri_95,
    vos_thread_pri_96,
    vos_thread_pri_97,
    vos_thread_pri_98,
    vos_thread_pri_highest,

    vos_thread_pri_butt
}vos_thread_pri_enum;
typedef vos_u32 vos_thread_pri_enum_u32;

#define vos_createthread1(a)   vos_createthreadex(a, vos_thread_sched_normal)
#define vos_createthread2(a,b) vos_createthreadex(a,b)

#define GET_2TH_ARG(arg1, arg2, arg3,...) arg3
#define FUN_MACRO_CHOOSER(...) GET_2TH_ARG(__VA_ARGS__, \
                                 vos_createthread2, vos_createthread1, )
 
#define vos_createthread(...) FUN_MACRO_CHOOSER(__VA_ARGS__)(__VA_ARGS__)

//ƽ̨�������̺߳��������ṹ����
typedef struct
{
    vos_u32 ulthreadpri;       //�߳����ȼ�(1~99,����Խ�����ȼ�Խ��)
    vos_u32 ulstacksize;       //�̶߳�ջ��С
    vos_tid ulthrdid;          //�����ɹ���������߳�id
    vos_pthreadfunc pthrdfunc; //�̻߳ص�
    void* pthrdinpara;     //�̺߳����������
    vos_u32   arglen;          //�߳�����������ȼ�pthrdinpara�ĳ���
    vos_u32   eninitstatus;    //�̴߳������״̬
    vos_u32 systid;          //�������߳�ID
}vos_multi_thread_para_stru;

#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************************
 �� �� ��  : vos_mallocmutex
 ��������  : ����һ�������ź���
 �������  : void
 �������  : ��
 �� �� ֵ  : void*
 
 �޸���ʷ      :
  1.��    ��   : 2013��10��22��
    ��    ��   : Albort,Feng

    �޸�����   : �����ɺ���

*****************************************************************************/
void* vos_mallocmutex(void);

/*****************************************************************************
 �� �� ��  : vos_freemutex
 ��������  : �ͷ�mutx�ڴ�
 �������  : void* pmutex
 �������  : ��
 �� �� ֵ  : void
 
 �޸���ʷ      :
  1.��    ��   : 2013��11��20��
    ��    ��   : Albort,Feng

    �޸�����   : �����ɺ���

*****************************************************************************/
void vos_freemutex(void* pmutex);

/*****************************************************************************
 �� �� ��  : vos_checktid_valid
 ��������  :  ������� ��Ч�Լ��
 �������  : vos_tid vostid
             
 �������  : ��
 �� �� ֵ  : vos_u32
 
 �޸���ʷ      :
  1.��    ��   : 2013��9��22��
    ��    ��   : zhangchaoyong

    �޸�����   : �����ɺ���

*****************************************************************************/
vos_u32 vos_checktid_valid( vos_tid vostid );

/*****************************************************************************
 �� �� ��  : vos_createmutex
 ��������  : ���������ź���
 �������  : void* poutmutex
 �������  : ��
 �� �� ֵ  : vos_u32
 
 �޸���ʷ      :
  1.��    ��   : 2013��9��16��
    ��    ��   : Albort,Feng

    �޸�����   : �����ɺ���

*****************************************************************************/
vos_u32 vos_createmutex(void* poutmutex);

/*****************************************************************************
 �� �� ��  : vos_deletemutex
 ��������  : ɾ�������ź���
 �������  : void* pmutex
 �������  : ��
 �� �� ֵ  : vos_u32
 
 �޸���ʷ      :
  1.��    ��   : 2013��9��23��
    ��    ��   : Albort,Feng

    �޸�����   : �����ɺ���

*****************************************************************************/
vos_u32 vos_deletemutex(void* pmutex);

/*****************************************************************************
 �� �� ��  : vos_obtainmutex
 ��������  : ��ȡ�����ź���
 �������  : void* pmutex
 �������  : ��
 �� �� ֵ  : void
 
 �޸���ʷ      :
  1.��    ��   : 2013��9��16��
    ��    ��   : Albort,Feng

    �޸�����   : �����ɺ���

*****************************************************************************/
void vos_obtainmutex(void* pmutex);

/*****************************************************************************
 �� �� ��  : vos_releasemutex
 ��������  : �ͷŻ����ź���
 �������  : void* pmutex
 �������  : ��
 �� �� ֵ  : void
 
 �޸���ʷ      :
  1.��    ��   : 2013��9��16��
    ��    ��   : Albort,Feng

    �޸�����   : �����ɺ���

*****************************************************************************/
void vos_releasemutex(void* pmutex);

/*****************************************************************************
 �� �� ��  : vos_setthreadpriority
 ��������  : �����߳����ȼ�
 �������  : vos_tid vostid
             vos_u32 ulpri
 �������  : ��
 �� �� ֵ  : vos_u32
 
 �޸���ʷ      :
  1.��    ��   : 2013��9��2��
    ��    ��   : Albort,Feng

    �޸�����   : �����ɺ���

*****************************************************************************/
vos_u32 vos_setthreadpriority(vos_tid vostid, vos_u32 ulpri);

/*****************************************************************************
 �� �� ��  : vos_suspendthread
 ��������  : �����߳̽ӿں���
 �������  : vos_tid vostid
 �������  : ��
 �� �� ֵ  : vos_u32
 
 �޸���ʷ      :
  1.��    ��   : 2013��9��6��
    ��    ��   : Albort,Feng

    �޸�����   : �����ɺ���

*****************************************************************************/
vos_u32 vos_suspendthread(vos_tid vostid);

/*****************************************************************************
 �� �� ��  : vos_resumethread
 ��������  : �ָ��߳�����
 �������  : vos_tid vostid
 �������  : ��
 �� �� ֵ  : vos_u32
 
 �޸���ʷ      :
  1.��    ��   : 2013��9��2��
    ��    ��   : Albort,Feng

    �޸�����   : �����ɺ���

*****************************************************************************/
vos_u32 vos_resumethread(vos_tid vostid);

/*****************************************************************************
 �� �� ��  : vos_threadsleep
 ��������  : �����߳�˯��ָ��ʱ�䣨��λ�����룩
 �������  : vos_u32 ulmillisecond
 �������  : ��
 �� �� ֵ  : void
 
 �޸���ʷ      :
  1.��    ��   : 2013��9��3��
    ��    ��   : Albort,Feng

    �޸�����   : �����ɺ���

*****************************************************************************/
void vos_threadsleep(vos_u32 ulmillisecond);

/*****************************************************************************
 �� �� ��  : vos_createthreadex
 ��������  : �����߳�
 �������  : vos_multi_thread_para_stru* pstthdpara
             char* pucpolicy
 �������  : ��
 �� �� ֵ  : vos_u32
 
 �޸���ʷ      :
  1.��    ��   : 2013��8��28��
    ��    ��   : Albort,Feng

    �޸�����   : �����ɺ���

*****************************************************************************/
vos_u32 vos_createthreadex(vos_multi_thread_para_stru* pstthdpara, vos_u32 enpolicy);

/*****************************************************************************
 �� �� ��  : vos_terminatethread
 ��������  : ��ֹ�߳�
 �������  : vos_tid vostid
 �������  : ��
 �� �� ֵ  : void
 
 �޸���ʷ      :
  1.��    ��   : 2013��9��11��
    ��    ��   : Albort,Feng

    �޸�����   : �����ɺ���

*****************************************************************************/
void vos_terminatethread(vos_tid vostid);

/*****************************************************************************
 �� �� ��  : vos_closesocket
 ��������  : vosƽ̨�ر�socket�ӿ�
 �������  : vos_socket ulsock
 �������  : ��
 �� �� ֵ  : void
 
 �޸���ʷ      :
  1.��    ��   : 2013��11��20��
    ��    ��   : Albort,Feng

    �޸�����   : �����ɺ���

*****************************************************************************/
void vos_closesocket(vos_socket ulsock);

/*****************************************************************************
 �� �� ��  : vos_socksend
 ��������  : vosƽ̨socket���ķ��ͽӿ�
 �������  : vos_socket ulsock
             char* puccnt
             vos_u32 uldtlen
 �������  : ��
 �� �� ֵ  : vos_u32
 
 �޸���ʷ      :
  1.��    ��   : 2013��11��20��
    ��    ��   : Albort,Feng

    �޸�����   : �����ɺ���

*****************************************************************************/
vos_u32 vos_socksend(vos_socket ulsock, char* pcnt, vos_u32 uldtlen);

/*****************************************************************************
 �� �� ��  : vos_sockreceive
 ��������  : vosƽ̨socket���ձ��Ľӿ�
 �������  : vos_socket ulsock
             char* pucbuf
             vos_u32 ulbuflen
             vos_u32* pulrcvlen
 �������  : ��
 �� �� ֵ  : vos_u32
 
 �޸���ʷ      :
  1.��    ��   : 2013��11��20��
    ��    ��   : Albort,Feng

    �޸�����   : �����ɺ���

*****************************************************************************/
vos_u32 vos_sockreceive(vos_socket ulsock, char* pucbuf, vos_u32 ulbuflen, vos_u32* pulrcvlen);

/*****************************************************************************
 �� �� ��  : vos_filterpathforfile
 ��������  : �ļ�·�����˵�·���ӿ�
 �������  : char* pucallpath
 �������  : ��
 �� �� ֵ  : char*
 
 �޸���ʷ      :
  1.��    ��   : 2013��11��20��
    ��    ��   : Albort,Feng

    �޸�����   : �����ɺ���

*****************************************************************************/
char* vos_filterpathforfile(char* pucallpath);

/*****************************************************************************
 �� �� ��  : vos_createenventtimer
 ��������  : ����һ���¼���ʱ��
 �������  : vos_u32 ulmillisecond
             enventcallback callfunc
 �������  : ��
 �� �� ֵ  : vos_u32 
 
 �޸���ʷ      :
  1.��    ��   : 2014��1��13��
    ��    ��   : zhangchaoyong

    �޸�����   : �����ɺ���

*****************************************************************************/
vos_u32  vos_createenventtimer(vos_u32 ulmillisecond,enventcallback callfunc);

/*****************************************************************************
 �� �� ��  : vos_getthreadtotalnum
 ��������  : ��ȡ�߳�����
 �������  : void
 �������  : ��
 �� �� ֵ  : vos_u32
 ���ú���  : 
 ��������  : 
 
 �޸���ʷ      :
  1.��    ��   : 2014��8��1��
    ��    ��   : Albort,Feng
    �޸�����   : �����ɺ���

*****************************************************************************/
vos_u32 vos_getthreadtotalnum(void);

//������ʼ���ӿ�
vos_u32 vos_initadaptmd(void);

#ifdef __cplusplus
}
#endif

#endif
