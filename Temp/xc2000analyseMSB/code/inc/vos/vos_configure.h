#ifndef _vos_configure_h_
#define _vos_configure_h_    
/******************************************************************************

                  ��Ȩ���� (C), 2013-2023, �Ĵ��³�����Ƽ����޹�˾

 ******************************************************************************
  �� �� ��   : vos_configure.h
  �� �� ��   : ����
  ��    ��   : Albort,Feng
  ��������   : 2013��8��26��
  ����޸�   :
  ��������   : vosϵͳ���������ļ�
  �����б�   :
  �޸���ʷ   :
  1.��    ��   : 2013��8��26��
    ��    ��   : Albort,Feng
    �޸�����   : �����ļ�

******************************************************************************/
//vosƽ̨֧�����ʱ����
#define VOS_MAX_SUPPORT_TIMER_NUM   (1024)

//vosϵͳ֧���������������
#define VOS_MAX_SUPPORT_LIST_NUM    (256)

//vosϵͳ֧������߳���
#define VOS_MAX_SUPPORT_THREAD_NUM  (256)

//vosϵͳ�ڴ�й©�������,��λΪ��
#define VOS_CHECK_LEAKMEM_PERIOD    (1)

//vosϵͳ�ڴ�й©�������,��δʹ�ü������������޺��ڴ汻�����ͷ�
#define VOS_CHECK_LEAKMEM_UPPER_THR  (3)

///vosϵͳ�澯�澯�����߳�����(��λ:����)
#define VOS_ALARM_THREADPROC_PERIOD (1000)

//vosϵͳ����Ĭ�ϼ������(��λ����)
#define VOS_CHECK_LEAKFAULT_PERIOD  (100)

//vosϵͳFIDģ�����ܹ�ע���PIDģ�������,ֻ����100������
#define VOS_MAX_PID_NUM_PER_FID      (100)

//vosϵͳ֧�ֵ����FID��
#define VOS_MAX_SUPPORT_FID_NUM      (16)

//vosϵͳFID�߳�ջ��С��8M
#define VOS_FID_DEFAULT_STACK_SIZE   (8*1024*1024)

//vosϵͳ�߳�Ĭ�ϴ�С
#define VOS_DEFAULT_THREAD_STATCK_SIZE (2 * 1024*1024)

//vosϵͳ�ڴ����Ƿ�֧�ֿ��������
#define VOS_SUPPORT_CHECK_MEM_MULTI_OP (1)

//vosϵͳFID�̴߳�����ĳ���״̬0:����, 1:����
#define VOS_FID_THREAD_INIT_STATUS  (1)

//vosϵͳ������־��󳤶�
#define VOS_MAX_SINGLE_LOG_LENGTH   (1024)

//vosϵͳÿ��FIDģ����־����֧�ֵ������־����
#define VOS_MAX_SUPPORT_LOG_NUM_PER_FID (1024)

//vosϵͳÿ��FID��־�ڴ��С
#define VOS_LOG_MEM_SIZE_PER_FID    (VOS_MAX_SUPPORT_LOG_NUM_PER_FID * VOS_MAX_SINGLE_LOG_LENGTH)

//vosϵͳ��־�ļ���С�������
#define VOS_PACK_LOG_FILE_THR       (512*1024)

//vosϵͳ�����ͨ�Ŷ˿ںŶ���
#define VOS_SERVER_SOCKET_PORT_ID   (20000)

//vosϵͳ�������λ����������
#define VOS_MAX_MID_MACHIN_NUM      (10)

//�澯���ƴ�С
#define VOS_ALARMNAME_SIZE          (64)

//�澯��������
#define VOS_SUBFAULT_NUM            (20) 

//��ʱ��ģ�鶨ʱ����(����)
#define VOS_TIMER_ACCURACY          (100)

//���ڴ汣�����볤��
#define VOS_PROTECT_HEAP_MEM_LEN    (16)
#endif
