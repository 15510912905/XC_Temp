#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "vos_configure.h"
#include <unistd.h>
#include "vos_basictypedefine.h"
#include "vos_errorcode.h"
#include "vos_adaptsys.h"
#include "vos_mnglist.h"
#include "vos_mngmemory.h"
#include "vos_selfmessagecenter.h"
#include "vos_selfsocketcommu.h"
#include "vos_selfpid.h"
#include "vos_selffid.h"
#include "vos_timer.h"
#include "vos_fid.h"
#include "vos_init.h"
#include "vos_log.h"
#include "socketmonitor.h"

STATIC char* g_puccurworkpath = VOS_NULL;
extern CSocketMonitor * g_pMonitor;

/*****************************************************************************
 �� �� ��  : vos_getworkpath
 ��������  : ��ȡ���ϵͳ����·��
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
void vos_getworkpath(char* pucoutpath)
{
    if (VOS_NULL == pucoutpath)
    {
        return;
    }
    
    memcpy(pucoutpath, g_puccurworkpath, (strlen(g_puccurworkpath) + 1));
    
    return;
}

/*****************************************************************************
 �� �� ��  : vos_initplatform
 ��������  : ��ʼ��ƽ̨
 �������  : void
 �������  : ��
 �� �� ֵ  : vos_u32
 
 �޸���ʷ      :
  1.��    ��   : 2013��9��3��
    ��    ��   : Albort,Feng

    �޸�����   : �����ɺ���

*****************************************************************************/
vos_u32 vos_initplatform(void)
{
    vos_u32 ulrunrslt = VOS_OK;

    ulrunrslt = vos_initadaptmd();
    if (VOS_OK != ulrunrslt)
    {
        return ulrunrslt;
    }
    
    ulrunrslt = vos_initlistmd();
    if (VOS_OK != ulrunrslt)
    {
        return ulrunrslt;
    }

    ulrunrslt = vos_initmemorymd();
    if (VOS_OK != ulrunrslt)
    {
        return ulrunrslt;
    }

    ulrunrslt = vos_initmsgcentermd();
    if (VOS_OK != ulrunrslt)
    {
        return ulrunrslt;
    }

    ulrunrslt = vos_initfidmd();
    if (VOS_OK != ulrunrslt)
    {
        return ulrunrslt;
    }

    ulrunrslt = vos_initpidmd();
    if (VOS_OK != ulrunrslt)
    {
        return ulrunrslt;
    }

    ulrunrslt = vos_init_timermd();
    if (VOS_OK != ulrunrslt)
    {
        return ulrunrslt;
    }

    ulrunrslt = vos_registerfid(50, app_self_log_fid);
    if (VOS_OK != ulrunrslt)
    {
        return ulrunrslt;
    }

    CLog * pLog = new CLog();
    pLog->RegCallBack(app_self_log_fid);
    ulrunrslt = vos_registerfid(50, app_self_net_fid);
    if (VOS_OK != ulrunrslt)
    {
        return ulrunrslt;
    }
    return VOS_OK;
}

/*****************************************************************************
 �� �� ��  : vos_stopplatform
 ��������  : ֹͣVOSƽ̨
 �������  : void
 �������  : ��
 �� �� ֵ  : void
 ���ú���  : 
 ��������  : 
 
 �޸���ʷ      :
  1.��    ��   : 2014��7��29��
    ��    ��   : Albort,Feng
    �޸�����   : �����ɺ���

*****************************************************************************/
void vos_stopplatform(void)
{
    return;
}

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
vos_u32 vos_startplatform()
{
    vos_u32 ulrunrslt = VOS_OK;
    char* puccurcwd = VOS_NULL;

    g_puccurworkpath = (char*)malloc(VOS_MAX_PATH);
    if (VOS_NULL == g_puccurworkpath)
    {
        RETURNERRNO(VOS_COM_PID,vos_malloc_mem_err);
    }

    memset(g_puccurworkpath, 0x00, VOS_MAX_PATH);

    puccurcwd = getcwd(g_puccurworkpath, VOS_MAX_PATH);
    if (VOS_NULL == puccurcwd)
    {
        free(g_puccurworkpath);
        g_puccurworkpath = VOS_NULL;
        
        RETURNERRNO(VOS_COM_PID,vos_get_curworkpath_error);
    }

    ulrunrslt = vos_initplatform();
    if (VOS_OK != ulrunrslt)
    {
        return ulrunrslt;
    }
    WRITE_INFO_LOG("vos_startpaltform successfull\r\n");
    return VOS_OK;
}

