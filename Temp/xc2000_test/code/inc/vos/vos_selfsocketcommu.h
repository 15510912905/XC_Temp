#ifndef _vos_selfsocketcommu_h_
#define _vos_selfsocketcommu_h_        
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
#include "vos_basictypedefine.h"
#include "vos_messagecenter.h"

#define VOS_MAX_SOCK_RCV_LEN   (1024)

//�����ͷ�socket��Ϣ�ṹ
typedef struct
{
    vos_msg_header;
    vos_u32 ulipaddr;
}vos_release_socket_req_msg_stru;

#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************************
 �� �� ��  : vos_msgsocketsend
 ��������  : vos��Ϣsocket���ͽӿ�
 �������  : vos_msg_header_stru* pstvosmsg
 �������  : ��
 �� �� ֵ  : vos_u32
 
 �޸���ʷ      :
  1.��    ��   : 2013��11��25��
    ��    ��   : Albort,Feng

    �޸�����   : �����ɺ���

*****************************************************************************/
vos_u32 vos_msgsocketsend(vos_msg_header_stru* pstvosmsg);

/*****************************************************************************
 �� �� ��  : vos_initsockcommumd
 ��������  : socketͨ��ģ���ʼ���ӿ�
 �������  : void
 �������  : ��
 �� �� ֵ  : vos_u32
 
 �޸���ʷ      :
  1.��    ��   : 2013��11��20��
    ��    ��   : Albort,Feng

    �޸�����   : �����ɺ���

*****************************************************************************/
vos_u32 vos_initsockcommumd();


#ifdef __cplusplus
}
#endif

#endif

