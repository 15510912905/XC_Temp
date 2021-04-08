#ifndef _app_debugtty_h_
#define _app_debugtty_h_

#include "vos_basictypedefine.h"

typedef vos_u32  (*pdebugcmdfunc)(char*, char*, char*, char*, char*, char*,
                                    char*, char*, char*, char*, char*, char*,
                                    char*, char*, char*, char*, char*, char*,
                                    char*, char*, char*, char*, char*, char*,
                                    char*, char*, char*, char*, char*, char*);
typedef void (*pshowcmdhelp)(void);


#ifdef __cplusplus
    extern "C" {
#endif

/*****************************************************************************
 �� �� ��  : app_strnumToIntnum
 ��������  : �ַ�������ת������������
 �������  : char* pucstrnum
             vos_u32* pulintnum
 �������  : ��
 �� �� ֵ  : vos_u32
 ���ú���  : 
 ��������  : 
 
 �޸���ʷ      :
  1.��    ��   : 2014��7��31��
    ��    ��   : Albort,Feng
    �޸�����   : �����ɺ���

*****************************************************************************/
vos_u32 app_strnumToIntnum(char* pucstrnum, vos_u32* pulintnum);

/*****************************************************************************
 �� �� ��  : app_registerDebugCmdfunc
 ��������  : ע��ά������
 �������  : char* pcmdfuncaddr
             pdebugcmdfunc pfuncaddr
             pshowcmdhelp phfuncaddr
             vos_u32 ulparanum
 �������  : ��
 �� �� ֵ  : void
 ���ú���  : 
 ��������  : 
 
 �޸���ʷ      :
  1.��    ��   : 2014��7��9��
    ��    ��   : Albort,Feng
    �޸�����   : �����ɺ���

*****************************************************************************/
void app_registerDebugCmdfunc(char* pucfuncname, pdebugcmdfunc pcmdfuncaddr,
                                pshowcmdhelp phfuncaddr, vos_u32 ulparanum);

/*****************************************************************************
 �� �� ��  : app_initDebugttymd
 ��������  : ά��ģ���ʼ������
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
vos_u32 app_initDebugttymd(void);

#ifdef __cplusplus
}
#endif

#endif
