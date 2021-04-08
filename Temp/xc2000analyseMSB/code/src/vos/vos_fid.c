/******************************************************************************

                  版权所有 (C), 2013-2023, 四川新成生物科技有限公司

 ******************************************************************************
  文 件 名   : vos_fid.c
  版 本 号   : 初稿
  作    者   : Albort,Feng
  生成日期   : 2013年9月5日
  最近修改   :
  功能描述   : 平台FID模块功能实现文件
  函数列表   :
  修改历史   :
  1.日    期   : 2013年9月5日
    作    者   : Albort,Feng
    修改内容   : 创建文件

******************************************************************************/
#include "vos_basictypedefine.h"
#include "vos_configure.h"
#include "vos_errorcode.h"
#include "vos_adaptsys.h"
#include "vos_mngmemory.h"
#include "vos_messagecenter.h"
#include "vos_selfmessagecenter.h"
#include "vos_selfpid.h"
#include "vos_selffid.h"
#include "vos_fid.h"
#include "vos_init.h"
#include "vos_log.h"
#include <pthread.h>

#define VOS_MAX_FID(fidbase)  ((fidbase) + (VOS_MAX_SUPPORT_FID_NUM - 1)* VOS_MAX_PID_NUM_PER_FID)
#define VOS_MIN_FID(fidbase)  (fidbase)

//FID存在标志枚举定义
typedef enum
{
    vos_fid_register_yes = 0,  //FID已被注册
    vos_fid_register_no  = 1,  //FID未被注册

    vos_fid_register_butt
}vos_fid_register_enum;
typedef vos_u32 vos_fid_register_enum_u32;

//FID模块FID线程信息结构
typedef struct
{
    vos_u32 ulfid;
    vos_multi_thread_para_stru stthdinfo;
}vos_fid_thd_info_stru;

//FID模块线程信息管理结构
typedef struct
{
    vos_fid_thd_info_stru astthdinfo[VOS_MAX_SUPPORT_FID_NUM];
}vos_mng_fid_thd_info_stru;

#ifdef __cplusplus
extern "C" {
#endif

//FID线程句柄管理
STATIC vos_mng_fid_thd_info_stru* g_pstmngfidthdinfo = VOS_NULL;
STATIC void* g_pfidmdmutex = VOS_NULL;

/*****************************************************************************
 函 数 名  : vos_fidmaptoindex
 功能描述  : FID到索引的映射
 输入参数  : vos_u32 ulfid
 输出参数  : 无
 返 回 值  : vos_u32
 
 修改历史      :
  1.日    期   : 2013年9月6日
    作    者   : Albort,Feng

    修改内容   : 新生成函数

*****************************************************************************/
vos_u32 vos_fidmaptoindex(vos_u32 ulfid)
{
    vos_u32 ulloopvar = 0;

    vos_obtainmutex(g_pfidmdmutex);
    
    for (ulloopvar = 0; VOS_MAX_SUPPORT_FID_NUM > ulloopvar; ulloopvar++)
    {
        if (ulfid == g_pstmngfidthdinfo->astthdinfo[ulloopvar].ulfid)
        {
            break;
        }
    }

    vos_releasemutex(g_pfidmdmutex);
    
    return ulloopvar;
}

/*****************************************************************************
 函 数 名  : vos_checkfidregistered
 功能描述  : 检查FID是否已被注册
 输入参数  : vos_u32 ulfid
 输出参数  : 无
 返 回 值  : vos_u32
 
 修改历史      :
  1.日    期   : 2013年10月24日
    作    者   : Albort,Feng

    修改内容   : 新生成函数

*****************************************************************************/
vos_u32 vos_checkfidregistered(vos_u32 ulfid)
{
    vos_u32 ulfididx = 0;

    ulfididx = vos_fidmaptoindex(ulfid);

    if (VOS_MAX_SUPPORT_FID_NUM <= ulfididx)
    {
        return vos_fid_register_no;
    }

    return vos_fid_register_yes;
}

/*****************************************************************************
 函 数 名  : vos_pidmaptofid
 功能描述  : PID到FID的映射
 输入参数  : vos_u32 ulpid
 输出参数  : 无
 返 回 值  : vos_u32
 
 修改历史      :
  1.日    期   : 2013年9月9日
    作    者   : Albort,Feng

    修改内容   : 新生成函数

*****************************************************************************/
vos_u32 vos_pidmaptofid(vos_u32 ulpid)
{
    return (((vos_u32)(ulpid / VOS_MAX_PID_NUM_PER_FID)) * VOS_MAX_PID_NUM_PER_FID);
}

/*****************************************************************************
 函 数 名  : vos_checkfidvalidity
 功能描述  : FID号合法性检查
 输入参数  : vos_u32 ulfid
 输出参数  : 无
 返 回 值  : vos_u32
 
 修改历史      :
  1.日    期   : 2013年9月6日
    作    者   : Albort,Feng

    修改内容   : 新生成函数

*****************************************************************************/
vos_u32 vos_checkfidvalidity(vos_u32 ulfid)
{
    vos_u32 ulrunrslt = vos_fid_register_yes;
    
    if (0 != (ulfid % VOS_MAX_PID_NUM_PER_FID))
    {
        RETURNERRNO(vos_fid_errorcode_base, vos_fid_invalid_error);
    }

    ulrunrslt = vos_checkfidregistered(ulfid);
    if (vos_fid_register_no == ulrunrslt)
    {
        RETURNERRNO(vos_fid_errorcode_base, vos_fid_unreg_error);
    }
    
    return VOS_OK;
}

/*****************************************************************************
 函 数 名  : vos_getfidthreadinfo
 功能描述  : 获取FID线程信息
 输入参数  : vos_u32 ulfid
 输出参数  : vos_multi_thread_para_stru** ppoutthdinfo
 返 回 值  : vos_u32
 
 修改历史      :
  1.日    期   : 2013年9月6日
    作    者   : Albort,Feng

    修改内容   : 新生成函数

*****************************************************************************/
vos_u32 vos_getfidthreadinfo(vos_u32 ulfid,
                             vos_multi_thread_para_stru** ppoutthdinfo)
{
    vos_u32 ulrunrslt = VOS_OK;
    vos_u32 ulindex   = VOS_INVALID_U32;
    
    if (VOS_NULL == ppoutthdinfo)
    {
        RETURNERRNO(vos_fid_errorcode_base, vos_para_null_err);
    }

    /*检查FID合法性,不合法则说明本条消息需要发送到中位机*/
    ulrunrslt = vos_checkfidvalidity(ulfid);
    if (VOS_OK != ulrunrslt)
    {
        WRITE_ERR_LOG(
                    "call vos_checkfidvalidity failed(0x%x)\r\n",
                    ulrunrslt);
                    
        return ulrunrslt;
    }

    ulindex = vos_fidmaptoindex(ulfid);
    if (VOS_MAX_SUPPORT_FID_NUM <= ulindex)
    {
        RETURNERRNO(vos_fid_errorcode_base, vos_array_index_error);
    }

    vos_obtainmutex(g_pfidmdmutex);
    
    *ppoutthdinfo = &(g_pstmngfidthdinfo->astthdinfo[ulindex].stthdinfo);
    
    vos_releasemutex(g_pfidmdmutex);
    
    return VOS_OK;
}

/*****************************************************************************
 函 数 名  : vos_fidthreadenter
 功能描述  : 每个FID模块线程入口,负责该FID内各个PID模块消息分发
 输入参数  : void* ppara
 输出参数  : 无
 返 回 值  : vos_u32
 
 修改历史      :
  1.日    期   : 2013年9月6日
    作    者   : Albort,Feng

    修改内容   : 新生成函数

*****************************************************************************/
vos_u32 vos_fidthreadenter(void* ppara)
{
    vos_u32 ulrunrslt = VOS_OK;
    vos_u32 ulselffid = VOS_INVALID_FID;

    vos_msg_header_stru*        pstmsgbuf  = VOS_NULL;
    vos_multi_thread_para_stru* pstthdinfo = VOS_NULL;

    ulselffid = *((vos_u32*)ppara);

    ulrunrslt = vos_receivemsg(ulselffid , &pstmsgbuf);
    if ((VOS_OK != ulrunrslt) || (VOS_NULL == pstmsgbuf))
    {//邮箱队列中没有消息,当前线程自动挂起
        pstthdinfo = VOS_NULL;
        
        (void)vos_getfidthreadinfo(ulselffid, &pstthdinfo);
        if (VOS_NULL != pstthdinfo)
        {
            (void)vos_suspendthread(pstthdinfo->ulthrdid);
        }
    }else
    {
        /*消息分发到相应的PID模块中*/
        (void)vos_deliverymsgtopid(pstmsgbuf);

        /*消息处理完后释放消息内存*/
        (void)vos_freemsgbuffer(pstmsgbuf);
    }

    pstmsgbuf = VOS_NULL;
    
    return VOS_OK;
}

/*****************************************************************************
 函 数 名  : vos_initfidmd
 功能描述  : FID模块初始化接口
 输入参数  : void
 输出参数  : 无
 返 回 值  : vos_u32
 
 修改历史      :
  1.日    期   : 2013年9月6日
    作    者   : Albort,Feng

    修改内容   : 新生成函数

*****************************************************************************/
vos_u32 vos_initfidmd(void)
{
    vos_u32 ulrunrslt = VOS_OK;
    vos_u32 ulloopvar = 0;

    vos_fid_thd_info_stru* pmvpointer = VOS_NULL;

    g_pfidmdmutex = vos_mallocmutex();
    if (VOS_NULL == g_pfidmdmutex)
    {
        RETURNERRNO(vos_fid_errorcode_base, vos_malloc_mutex_error);
    }
    
    ulrunrslt = vos_createmutex(g_pfidmdmutex);
    if (VOS_OK != ulrunrslt)
    {
        vos_freemutex(g_pfidmdmutex);
        g_pfidmdmutex = VOS_NULL;
        
        return ulrunrslt;
    }

    ulrunrslt = vos_mallocmemory(sizeof(vos_mng_fid_thd_info_stru),
                                 vos_mem_type_static,
                                 (void**)&g_pstmngfidthdinfo);
    if (VOS_OK != ulrunrslt)
    {
        vos_deletemutex(g_pfidmdmutex);
        vos_freemutex(g_pfidmdmutex);
        g_pfidmdmutex = VOS_NULL;
        
        return ulrunrslt;
    }

    for (ulloopvar = 0; VOS_MAX_SUPPORT_FID_NUM > ulloopvar; ulloopvar++)
    {
        pmvpointer = &(g_pstmngfidthdinfo->astthdinfo[ulloopvar]);
        
        pmvpointer->ulfid = VOS_INVALID_FID;

        pmvpointer->stthdinfo.pthrdfunc    = vos_fidthreadenter;
        pmvpointer->stthdinfo.pthrdinpara  = VOS_NULL;
        pmvpointer->stthdinfo.ulstacksize  = VOS_FID_DEFAULT_STACK_SIZE;
        pmvpointer->stthdinfo.ulthrdid     = VOS_INVALID_U32;
        pmvpointer->stthdinfo.ulthreadpri  = 0;
        pmvpointer->stthdinfo.arglen       = 0;
        pmvpointer->stthdinfo.eninitstatus = VOS_FID_THREAD_INIT_STATUS;
    }

    return VOS_OK;
}

/*****************************************************************************
 函 数 名  : vos_registerfid
 功能描述  : 注册FID模块的接口
 输入参数  : vos_u32 ulfidpri (1~99)
             vos_u32 ulfid
 输出参数  : 无
 返 回 值  : vos_u32
 
 修改历史      :
  1.日    期   : 2013年9月5日
    作    者   : Albort,Feng

    修改内容   : 新生成函数

*****************************************************************************/
vos_u32 vos_registerfid(vos_u32 ulfidpri, vos_u32 ulfid)
{
    vos_u32 ulrunrslt = VOS_OK;
    vos_u32 ulloopvar = 0;

    vos_fid_thd_info_stru* pstmove = VOS_NULL;

    /*FID必须是其能注册最大PID数的整数倍*/
    if (0 != (ulfid % VOS_MAX_PID_NUM_PER_FID))
    {
        RETURNERRNO(vos_fid_errorcode_base, vos_fid_invalid_error);
    }

    ulrunrslt = vos_checkfidregistered(ulfid);
    if (vos_fid_register_yes == ulrunrslt)
    {
        RETURNERRNO(vos_fid_errorcode_base, vos_fid_registered_error);
    }

    /*找到还未被注册的位置*/
    vos_obtainmutex(g_pfidmdmutex);
    
    for (ulloopvar = 0; VOS_MAX_SUPPORT_FID_NUM > ulloopvar; ulloopvar++)
    {
        pstmove = &(g_pstmngfidthdinfo->astthdinfo[ulloopvar]);
        
        if (VOS_INVALID_FID == pstmove->ulfid)
        {
            break;
        }
    }

    vos_releasemutex(g_pfidmdmutex);

    if (VOS_MAX_SUPPORT_FID_NUM <= ulloopvar)
    {
        RETURNERRNO(vos_fid_errorcode_base, vos_fid_reach_max_error);
    }
        
    pstmove->ulfid = ulfid;

    pstmove->stthdinfo.ulthreadpri = ulfidpri;
    pstmove->stthdinfo.pthrdinpara = (void*)(&ulfid);
    pstmove->stthdinfo.arglen      = sizeof(ulfid);

    /*创建该FID模块的VOS消息邮箱*/
    ulrunrslt = vos_createmsgmailbox(ulfid);
    if (VOS_OK != ulrunrslt)
    {
        WRITE_ERR_LOG("call vos_createmsgmailbox failed(0x%x)\r\n", ulrunrslt);
        pstmove->ulfid = VOS_INVALID_FID;        
        pstmove->stthdinfo.pthrdinpara = VOS_NULL;
        pstmove->stthdinfo.ulthreadpri = 0;
        pstmove->stthdinfo.arglen      = 0;
        return ulrunrslt;
    }

    /*创建该FID模块的PID注册信息管理存储区*/
    ulrunrslt = vos_mallocpidreginfomem(ulfid);
    if (VOS_OK != ulrunrslt)
    {
        WRITE_ERR_LOG("call vos_mallocpidreginfomem failed(0x%x)\r\n", ulrunrslt);
        vos_destroymsgmailbox(ulfid);
        pstmove->ulfid = VOS_INVALID_FID;        
        pstmove->stthdinfo.pthrdinpara = VOS_NULL;
        pstmove->stthdinfo.ulthreadpri = 0;
        pstmove->stthdinfo.arglen      = 0;        
        return ulrunrslt;
    }

    /*创建该FID模块线程*/
    if (vos_thread_pri_70 <= pstmove->stthdinfo.ulthreadpri)
    {
        ulrunrslt = vos_createthread(&(pstmove->stthdinfo),
                                     vos_thread_sched_rtos);
    }else
    {
        ulrunrslt = vos_createthread(&(pstmove->stthdinfo));
    }

    if (VOS_OK != ulrunrslt)
    {
        vos_destroymsgmailbox(ulfid);
        vos_freepidreginfomem(ulfid);
        
        pstmove->ulfid = VOS_INVALID_FID;
        
        pstmove->stthdinfo.pthrdinpara = VOS_NULL;
        pstmove->stthdinfo.ulthreadpri = 0;
        pstmove->stthdinfo.arglen      = 0;

        WRITE_ERR_LOG("call vos_createthread failed(0x%x)\r\n", ulrunrslt);

        return ulrunrslt;
    }
    
    return VOS_OK;
}

/*****************************************************************************
 函 数 名  : vos_utreleasefid
 功能描述  : 释放FID模块
 输入参数  : vos_u32 ulfid
 输出参数  : 无
 返 回 值  : void
 
 修改历史      :
  1.日    期   : 2013年9月11日
    作    者   : Albort,Feng

    修改内容   : 新生成函数

*****************************************************************************/
void vos_utreleasefid(vos_u32 ulfid)
{
    vos_u32 ulloopvar = 0;

    vos_fid_thd_info_stru* pstmove = VOS_NULL;

    vos_destroymsgmailbox(ulfid);
    vos_utreleaseallpidoffid(ulfid);

    vos_obtainmutex(g_pfidmdmutex);
    
    for (ulloopvar = 0; VOS_MAX_SUPPORT_FID_NUM > ulloopvar; ulloopvar++)
    {
        pstmove = &(g_pstmngfidthdinfo->astthdinfo[ulloopvar]);
        if (ulfid == pstmove->ulfid)
        {
            break;
        }
    }
    
    vos_releasemutex(g_pfidmdmutex);

    if (VOS_MAX_SUPPORT_FID_NUM <= ulloopvar)
    {
        return;
    }
    
    vos_terminatethread(pstmove->stthdinfo.ulthrdid);

    pstmove->ulfid                 = VOS_INVALID_FID;
    pstmove->stthdinfo.pthrdinpara = VOS_NULL;
    pstmove->stthdinfo.ulstacksize = VOS_FID_DEFAULT_STACK_SIZE;
    pstmove->stthdinfo.ulthrdid    = VOS_INVALID_U32;
    pstmove->stthdinfo.ulthreadpri = 0;
    pstmove->stthdinfo.arglen      = 0;
    
    return;
}

//获取Fid
vos_u32 vos_getfid()
{
    vos_u32 fid = 0;
    vos_u32 ulloopvar = 0;
    vos_u32 tid = (vos_u32)pthread_self();
    vos_obtainmutex(g_pfidmdmutex);
    for (ulloopvar = 0; VOS_MAX_SUPPORT_FID_NUM > ulloopvar; ulloopvar++)
    {
        if (tid == g_pstmngfidthdinfo->astthdinfo[ulloopvar].stthdinfo.systid)
        {
            fid = g_pstmngfidthdinfo->astthdinfo[ulloopvar].ulfid;
            break;
        }
    }
    vos_releasemutex(g_pfidmdmutex);
    return fid;
}


#ifdef __cplusplus
}
#endif
