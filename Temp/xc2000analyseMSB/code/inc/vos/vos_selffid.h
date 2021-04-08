#ifndef _vos_selffid_h_
#define _vos_selffid_h_       

/******************************************************************************

                  ��Ȩ���� (C), 2013-2023, �Ĵ��³�����Ƽ����޹�˾

 ******************************************************************************
  �� �� ��   : vos_selffid.h
  �� �� ��   : ����
  ��    ��   : Albort,Feng
  ��������   : 2013��9��6��
  ����޸�   :
  ��������   : FIDģ���ƽ̨�ڲ�ʹ�õ�ͷ�ļ�
  �����б�   :
  �޸���ʷ   :
  1.��    ��   : 2013��9��6��
    ��    ��   : Albort,Feng
    �޸�����   : �����ļ�

******************************************************************************/
#include "vos_basictypedefine.h"
#include "vos_adaptsys.h"

#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************************
 �� �� ��  : vos_fidmaptoindex
 ��������  : FID��������ӳ��
 �������  : vos_u32 ulfid
 �������  : ��
 �� �� ֵ  : vos_u32
 
 �޸���ʷ      :
  1.��    ��   : 2013��9��6��
    ��    ��   : Albort,Feng

    �޸�����   : �����ɺ���

*****************************************************************************/
vos_u32 vos_fidmaptoindex(vos_u32 ulfid);

/*****************************************************************************
 �� �� ��  : vos_pidmaptofid
 ��������  : PID��FID��ӳ��
 �������  : vos_u32 ulpid
 �������  : ��
 �� �� ֵ  : vos_u32
 
 �޸���ʷ      :
  1.��    ��   : 2013��9��9��
    ��    ��   : Albort,Feng

    �޸�����   : �����ɺ���

*****************************************************************************/
vos_u32 vos_pidmaptofid(vos_u32 ulpid);

/*****************************************************************************
 �� �� ��  : vos_checkfidvalidity
 ��������  : FID�źϷ��Լ��
 �������  : vos_u32 ulfid
 �������  : ��
 �� �� ֵ  : vos_u32
 
 �޸���ʷ      :
  1.��    ��   : 2013��9��6��
    ��    ��   : Albort,Feng

    �޸�����   : �����ɺ���

*****************************************************************************/
vos_u32 vos_checkfidvalidity(vos_u32 ulfid);

/*****************************************************************************
 �� �� ��  : vos_initfidmd
 ��������  : FIDģ���ʼ���ӿ�
 �������  : void
 �������  : ��
 �� �� ֵ  : vos_u32
 
 �޸���ʷ      :
  1.��    ��   : 2013��9��6��
    ��    ��   : Albort,Feng

    �޸�����   : �����ɺ���

*****************************************************************************/
vos_u32 vos_initfidmd(void);

/*****************************************************************************
 �� �� ��  : vos_getfidthreadinfo
 ��������  : ��ȡFID�߳���Ϣ
 �������  : vos_u32 ulfid
 �������  : vos_multi_thread_para_stru** ppoutthdinfo
 �� �� ֵ  : vos_u32
 
 �޸���ʷ      :
  1.��    ��   : 2013��9��6��
    ��    ��   : Albort,Feng

    �޸�����   : �����ɺ���

*****************************************************************************/
vos_u32 vos_getfidthreadinfo(vos_u32 ulfid,
                             vos_multi_thread_para_stru** ppoutthdinfo);

#ifdef __cplusplus
}
#endif

#endif