#ifndef _vos_init_h_
#define _vos_init_h_   
/******************************************************************************

                  ��Ȩ���� (C), 2013-2023, �Ĵ��³�����Ƽ����޹�˾

 ******************************************************************************
  �� �� ��   : vos_init.h
  �� �� ��   : ����
  ��    ��   : Albort,Feng
  ��������   : 2013��9��3��
  ����޸�   :
  ��������   : ƽ̨��ʼ��ͷ�ļ�����
  �����б�   :
  �޸���ʷ   :
  1.��    ��   : 2013��9��3��
    ��    ��   : Albort,Feng
    �޸�����   : �����ļ�

******************************************************************************/
#include "vos_basictypedefine.h"

/*****************************************************************************
 �� �� ��  : vos_getworkpath
 ��������  : ��ȡ����ϵͳ����·��
 �������  : char* pucoutpath
 �������  : ��
 �� �� ֵ  : void
 ���ú���  : 
 ��������  : 
 
 �޸���ʷ      :
  1.��    ��   : 2014��7��31��
    ��    ��   : Albort,Feng
    �޸�����   : �����ɺ���

*****************************************************************************/
void vos_getworkpath(char* pucoutpath);
/*****************************************************************************
 �� �� ��  : vos_startplatform
 ��������  : ����ƽ̨
 �������  : vos_u32 uluppermonitorip
 �������  : ��
 �� �� ֵ  : vos_u32
 
 �޸���ʷ      :
  1.��    ��   : 2013��9��3��
    ��    ��   : Albort,Feng

    �޸�����   : �����ɺ���

*****************************************************************************/
vos_u32 vos_startplatform();

#endif