#ifndef _app_debugcmd_h_
#define _app_debugcmd_h_
/******************************************************************************

                  ��Ȩ���� (C), 2001-2100, �Ĵ�ʡ�½����ɹɷ����޹�˾


 ******************************************************************************
  �� �� ��   : app_debugcmd.h
  �� �� ��   : ����
  ��    ��   : Albort,Feng
  ��������   : 2014��7��9��
  ����޸�   :
  ��������   : ����λ����������ʵ��ͷ�ļ�
  �����б�   :
  �޸���ʷ   :
  1.��    ��   : 2014��7��9��
    ��    ��   : Albort,Feng
    �޸�����   : �����ļ�

******************************************************************************/


#ifdef __cplusplus
    extern "C" {
#endif
/*****************************************************************************
 �� �� ��  : app_initdebugcmdmd
 ��������  : ��������ģ���ʼ������
 �������  : void
 �������  : ��
 �� �� ֵ  : vos_u32
 ���ú���  : 
 ��������  : 
 
 �޸���ʷ      :
  1.��    ��   : 2014��7��9��
    ��    ��   : Albort,Feng
    �޸�����   : �����ɺ���

*****************************************************************************/
vos_u32 app_initdebugcmdmd(void);

vos_u32 app_tasktest(char* tasknum);

void app_sendMidRunStateToDwnmach(vos_u32 endwnmachname, cmd_simplecmd_stru* cmdinfo);

vos_u32 app_updateapp(char* pucserverip);

vos_u32 app_debugequipmentquery(char * para0, char * para1);
#ifdef __cplusplus
}
#endif

#endif
