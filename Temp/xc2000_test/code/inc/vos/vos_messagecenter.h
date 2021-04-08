#ifndef _vos_messagecenter_h_
#define _vos_messagecenter_h_   
/******************************************************************************

                  ��Ȩ���� (C), 2013-2023, �Ĵ��³�����Ƽ����޹�˾

 ******************************************************************************
  �� �� ��   : vos_messagecenter.h
  �� �� ��   : ����
  ��    ��   : Albort,Feng
  ��������   : 2013��9��5��
  ����޸�   :
  ��������   : ƽ̨��Ϣ����ͷ�ļ�
  �����б�   :
  �޸���ʷ   :
  1.��    ��   : 2013��9��5��
    ��    ��   : Albort,Feng
    �޸�����   : �����ļ�

******************************************************************************/
#include "vos_basictypedefine.h"

//��Ϣ����ͳһ�������Ϣͷ
#define vos_msg_header    \
    vos_u32 usmsgloadlen; \
    vos_u32 ulsrcpid; \
    vos_u32 uldstpid; \
    vos_u32 usmsgtype; \
    vos_u32 usrsv

#pragma warning(disable:4200)
//vosϵͳ��Ϣ�ṹ����
typedef struct
{
    vos_u32 usmsgloadlen;            //��Ϣ����ͳһ�������Ϣͷ
    vos_u32 ulsrcpid;
    vos_u32 uldstpid;
    vos_u32 usmsgtype;    
    vos_u32 usrsv;
    char  aucmsgload[0];     //��Ϣ����
}vos_msg_header_stru;
#pragma warning(default:)

#define vos_calcvosmsgldlen(msgstru) (sizeof(msgstru) - sizeof(vos_msg_header_stru))

#ifdef __cplusplus
extern "C" {
#endif
/*****************************************************************************
 �� �� ��  : vos_mallocmsgbuffer
 ��������  : ���뷢����Ϣ�Ļ��棬��Ϣ�����߲��ù��ĸ��ڴ���ͷţ���Ϣ�ڴ���
             ƽ̨�Լ��ͷ�
 �������  : vos_u32 ulmsglength (��Ϣ���Ȱ�����Ϣͷ)   
 �������  : void** ppoutbuf
 �� �� ֵ  : vos_u32
 
 �޸���ʷ      :
  1.��    ��   : 2013��9��5��
    ��    ��   : Albort,Feng

    �޸�����   : �����ɺ���

*****************************************************************************/
vos_u32 vos_mallocmsgbuffer(vos_u32 ulmsglength, void** ppoutbuf);

/*****************************************************************************
 �� �� ��  : vos_freemsgbuffer
 ��������  : �ͷ���Ϣ�ڴ棬ringbuf�е�λ������ɽ�����Ϣ����ѱ����
 �������  : vos_msg_header_stru* pmsgbuf
 �������  : ��
 �� �� ֵ  : vos_u32
 
 �޸���ʷ      :
  1.��    ��   : 2013��9��6��
    ��    ��   : Albort,Feng

    �޸�����   : �����ɺ���

*****************************************************************************/
vos_u32 vos_freemsgbuffer(vos_msg_header_stru* pmsgbuf);

/*****************************************************************************
 �� �� ��  : vos_sendmsg
 ��������  : ��Ϣ���ͽӿڣ����ӿڷ��͵���Ϣ���������vos_mallocmsgbuffer����
             ���������򲻸��ڷ���
 �������  : vos_msg_header_stru* pstmsgbuf
 �������  : ��
 �� �� ֵ  : vos_u32
 
 �޸���ʷ      :
  1.��    ��   : 2013��9��6��
    ��    ��   : Albort,Feng

    �޸�����   : �����ɺ���

*****************************************************************************/
vos_u32 vos_sendmsg(vos_msg_header_stru* pstmsgbuf, bool bOuter = false);

#ifdef __cplusplus
}
#endif

#endif

