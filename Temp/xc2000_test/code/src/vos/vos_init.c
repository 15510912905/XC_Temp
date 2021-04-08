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
 函 数 名  : vos_getworkpath
 功能描述  : 获取软件系统工作路径
 输入参数  : char* pucoutpath
 输出参数  : 无
 返 回 值  : void
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2014年7月31日
    作    者   : Albort,Feng
    修改内容   : 新生成函数

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
 函 数 名  : vos_initplatform
 功能描述  : 初始化平台
 输入参数  : void
 输出参数  : 无
 返 回 值  : vos_u32
 
 修改历史      :
  1.日    期   : 2013年9月3日
    作    者   : Albort,Feng

    修改内容   : 新生成函数

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
 函 数 名  : vos_stopplatform
 功能描述  : 停止VOS平台
 输入参数  : void
 输出参数  : 无
 返 回 值  : void
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2014年7月29日
    作    者   : Albort,Feng
    修改内容   : 新生成函数

*****************************************************************************/
void vos_stopplatform(void)
{
    return;
}

/*****************************************************************************
 函 数 名  : vos_startplatform
 功能描述  : 启动平台
 输入参数  : vos_u32 uluppermonitorip
 输出参数  : 无
 返 回 值  : vos_u32
 
 修改历史      :
  1.日    期   : 2013年9月3日
    作    者   : Albort,Feng

    修改内容   : 新生成函数

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

