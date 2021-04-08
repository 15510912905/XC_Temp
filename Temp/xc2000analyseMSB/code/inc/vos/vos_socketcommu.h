#ifndef _vos_socketcommu_h_
#define _vos_socketcommu_h_        
/******************************************************************************

                  ��Ȩ���� (C), 2013-2023, �Ĵ��³�����Ƽ����޹�˾

 ******************************************************************************
  �� �� ��   : vos_selfsocketcommu.h
  �� �� ��   : ����
  ��    ��   : Albort,Feng
  ��������   : 2013��11��20��
  ����޸�   :
  ��������   : vos��Ϣsocketͨ��ͷ�ļ�
  �����б�   :
  �޸���ʷ   :
  1.��    ��   : 2013��11��20��
    ��    ��   : Albort,Feng
    �޸�����   : �����ļ�

******************************************************************************/
#include "vos_configure.h"
#include "vos_basictypedefine.h"

#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************************
 �� �� ��  : vos_disconnectsocket
 ��������  : �Ͽ�socket����
 �������  : vos_u32 ulsocketipaddr
 �������  : ��
 �� �� ֵ  : vos_u32
 
 �޸���ʷ      :
  1.��    ��   : 2014��2��26��
    ��    ��   : Albort,Feng

    �޸�����   : �����ɺ���

*****************************************************************************/
vos_u32 vos_disconnectsocket();

vos_u32 vos_receiveclientsockthdenter(void* ppara);
#ifdef __cplusplus
}
#endif

#endif

