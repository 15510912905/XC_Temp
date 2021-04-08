#ifndef _vos_mngmemory_h_
#define _vos_mngmemory_h_   

#include "vos_basictypedefine.h"

//�����ڴ�����
typedef enum
{
    vos_mem_type_static  = 0,  //��̬�ڴ�,��ʱ��δʹ��ϵͳ�����ͷ��ڴ�
    vos_mem_type_dynamic = 1,  //��̬�ڴ�,��ʱ��δʹ��ϵͳ���Զ��ͷ��ڴ�

    vos_mem_type_butt
}vos_mem_type_enum;
typedef vos_u32 vos_mem_type_enum_u32;

#define DELETE_MEMEORY(POINT) \
    if (NULL != POINT) \
            { \
        vos_freememory(POINT);\
        POINT = NULL; \
            }

#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************************
 �� �� ��  : vos_mallocmemory
 ��������  : �����ڴ�ӿ�
 �������  : vos_u32 ulmemsize
             vos_u32 enmemtype
 �������  : void** ppoutmem
 �� �� ֵ  : vos_u32
 
 �޸���ʷ      :
  1.��    ��   : 2013��8��27��
    ��    ��   : Albort,Feng

    �޸�����   : �����ɺ���

*****************************************************************************/
vos_u32 vos_mallocmemory(vos_u32 ulmemsize, vos_u32 enmemtype,
                         void** ppoutmem);

/*****************************************************************************
 �� �� ��  : vos_freememory
 ��������  : �ͷ��ڴ�
 �������  : void* pmemaddr
 �������  : ��
 �� �� ֵ  : void
 
 �޸���ʷ      :
  1.��    ��   : 2013��8��28��
    ��    ��   : Albort,Feng

    �޸�����   : �����ɺ���

*****************************************************************************/
void vos_freememory(void* pmemaddr);

/*****************************************************************************
 �� �� ��  : vos_getusedmemsize
 ��������  : ��ȡvos�������ʹ���ڴ��С
 �������  : void
 �������  : ��
 �� �� ֵ  : vos_u32
 ���ú���  : 
 ��������  : 
 
 �޸���ʷ      :
  1.��    ��   : 2014��8��7��
    ��    ��   : Albort,Feng
    �޸�����   : �����ɺ���

*****************************************************************************/
vos_u32 vos_getusedmemsize(void);

/*****************************************************************************
 �� �� ��  : vos_calccrc32
 ��������  : ����һ���ڴ�32bit��CRCֵ
 �������  : char* pmemaddr
             vos_u32 ulmemsize
 �������  : vos_u32* poutcrc
 �� �� ֵ  : vos_u32
 
 �޸���ʷ      :
  1.��    ��   : 2013��8��28��
    ��    ��   : Albort,Feng

    �޸�����   : �����ɺ���

*****************************************************************************/
vos_u32 vos_calccrc32(char* pmemaddr, vos_u32 ulmemsize, vos_u32* poutcrc);

//��ʼ���ڴ����ģ��
vos_u32 vos_initmemorymd(void);

#ifdef __cplusplus
}
#endif

#endif