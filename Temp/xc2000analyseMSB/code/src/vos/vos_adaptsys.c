/******************************************************************************

                  版权所有 (C), 2013-2023, 四川新成生物科技有限公司

 ******************************************************************************
  文 件 名   : vos_adaptsys.c
  版 本 号   : 初稿
  作    者   : Albort,Feng
  生成日期   : 2013年8月28日
  最近修改   :
  功能描述   : 操作系统适配功能实现文件
  函数列表   :
  修改历史   :
  1.日    期   : 2013年8月28日
    作    者   : Albort,Feng
    修改内容   : 创建文件

******************************************************************************/
#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include <string.h>
#include <time.h>
#include "vos_configure.h"
#include <sys/time.h>
//#include <zlib.h>
#include <sys/stat.h>
#include <pthread.h>
#include <limits.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <ifaddrs.h>
#include <dirent.h>
#include <errno.h>
#include <signal.h>
#include "vos_basictypedefine.h"
#include "vos_errorcode.h"
#include "vos_init.h"
#include "vos_mngmemory.h"
#include "vos_adaptsys.h"
#include "vos_log.h"

#define VOS_TID_MAP_TO_INDEX(vostid) ((vostid) - 1)
#define VOS_INDEX_MAP_TO_TID(index)  ((index) + 1)
#define VOS_HOST_IP_LENGTH           (32)
#define VOS_IPV4_BYTE_LENGTH         (4)
#define VOS_FBUF_LEN (1024*1024*2)
#define VOS_CBUF_LEN (1024*1024*2)

//平台线程信息结构体
typedef struct
{
    vos_tid vostid;
    void* mutex;    
    pthread_t systid;
    vos_i32   slflag;
    pthread_cond_t runsignal; //线程运行信号

    vos_pthreadfunc callbackfunc;
    void* usrinpara;
}vos_thread_info_stru;

//平台线程管理结构
typedef struct
{
    vos_thread_info_stru astthreadinfo[VOS_MAX_SUPPORT_THREAD_NUM];
    vos_u32 ultotalthrdnum;
}vos_mng_thread_stru;


#ifdef __cplusplus
extern "C" {
#endif

STATIC vos_mng_thread_stru* g_pstmngthread = VOS_NULL;
STATIC char* g_pucfbuf   = VOS_NULL;
STATIC char* g_puccbuf   = VOS_NULL;
STATIC vos_u32 g_ulfbuflen = VOS_CBUF_LEN;
STATIC vos_u32 g_ulcbuflen = VOS_CBUF_LEN;

/*****************************************************************************
 函 数 名  : itoa
 功能描述  : 整型转字符串
 输入参数  : int value
             char *str
             int radix 
 输出参数  : 无
 返 回 值  : char *
 
 修改历史      :
  1.日    期   : 2013年12月5日
    作    者   : zhangchaoyong

    修改内容   : 新生成函数
  2.日    期   : 2014年08月07日
    作    者   : Albort.Feng
    修改内容   : 解决内存泄漏问题
*****************************************************************************/
char *itoa(vos_i32 value, char *str, vos_u32 radix)    
{
    char* pstr = VOS_NULL;
    
    switch (radix)
    {
        case 2://转换成二进制字符串数据
            break;

        case 8://转换成八进制字符串数据
            sprintf(str, "0%o", (unsigned int)value);
            pstr = str;
            break;

        case 10://转换成十进制字符串数据
            sprintf(str, "%d", (int)value);
            pstr = str;
            break;
            
        case 16://转换成十六进制字符串数据
            sprintf(str, "0x%x", (unsigned int)value);
            pstr = str;
            break;

        default:
            break;
    }
    
    return pstr;
}

/*****************************************************************************
 函 数 名  : vos_initadaptmd
 功能描述  : 适配层初始化接口
 输入参数  : void
 输出参数  : 无
 返 回 值  : vos_u32
 
 修改历史      :
  1.日    期   : 2013年9月18日
    作    者   : Albort,Feng

    修改内容   : 新生成函数

*****************************************************************************/
vos_u32 vos_initadaptmd(void)
{
    vos_u32 ulloopvar = 0;

    g_ulfbuflen = VOS_CBUF_LEN;
    g_ulcbuflen = VOS_CBUF_LEN;
    
    g_pucfbuf = (char*)malloc(VOS_FBUF_LEN);
    if (VOS_NULL == g_pucfbuf)
    {
        RETURNERRNO(vos_adaptsys_errorcode_base, vos_malloc_mem_err);
    }

    g_puccbuf = (char*)malloc(VOS_CBUF_LEN);
    if (VOS_NULL == g_puccbuf)
    {
        free(g_pucfbuf);
        g_pucfbuf = VOS_NULL;
        
        RETURNERRNO(vos_adaptsys_errorcode_base, vos_malloc_mem_err);
    }
    
    g_pstmngthread = (vos_mng_thread_stru*)malloc(sizeof(vos_mng_thread_stru));
    if (VOS_NULL == g_pstmngthread)
    {
        free(g_pucfbuf);
        free(g_puccbuf);
        g_pucfbuf = VOS_NULL;
        g_puccbuf = VOS_NULL;
        
        RETURNERRNO(vos_adaptsys_errorcode_base, vos_malloc_mem_err);
    }

    for (ulloopvar = 0; VOS_MAX_SUPPORT_THREAD_NUM > ulloopvar; ulloopvar++)
    {
        g_pstmngthread->astthreadinfo[ulloopvar].vostid = VOS_INVALID_U32;
        g_pstmngthread->astthreadinfo[ulloopvar].systid = VOS_INVALID_U32;
        g_pstmngthread->astthreadinfo[ulloopvar].mutex        = VOS_NULL;
        g_pstmngthread->astthreadinfo[ulloopvar].callbackfunc = VOS_NULL;
        g_pstmngthread->astthreadinfo[ulloopvar].usrinpara    = VOS_NULL;
    }

    g_pstmngthread->ultotalthrdnum = 0;
        
    return VOS_OK;
}

/*****************************************************************************
 函 数 名  : vos_distributetid
 功能描述  : 分配线程id
 输入参数  : void
 输出参数  : 无
 返 回 值  : vos_tid
 
 修改历史      :
  1.日    期   : 2013年9月18日
    作    者   : Albort,Feng

    修改内容   : 新生成函数

*****************************************************************************/
vos_tid vos_distributetid(void)
{
    vos_tid vostid    = VOS_INVALID_U32;
    vos_u32 ulloopvar = 0;

    for (ulloopvar = 0; VOS_MAX_SUPPORT_THREAD_NUM > ulloopvar; ulloopvar++)
    {
        if (VOS_INVALID_U32 == g_pstmngthread->astthreadinfo[ulloopvar].vostid)
        {
            vostid = VOS_INDEX_MAP_TO_TID(ulloopvar);
            g_pstmngthread->astthreadinfo[ulloopvar].vostid = vostid;
            
            break;
        }
    }

    return vostid;
}

/*****************************************************************************
 函 数 名  : vos_releasetid
 功能描述  : 释放vos平台分配的tid
 输入参数  : vos_tid vostid
 输出参数  : 无
 返 回 值  : void
 
 修改历史      :
  1.日    期   : 2013年9月18日
    作    者   : Albort,Feng

    修改内容   : 新生成函数

*****************************************************************************/
void vos_releasetid(vos_tid vostid)
{
    vos_thread_info_stru* pstthdlocation = VOS_NULL;
    
    if (VOS_MAX_SUPPORT_THREAD_NUM < vostid)
    {
        return;
    }

    pstthdlocation = &(g_pstmngthread->astthreadinfo[VOS_TID_MAP_TO_INDEX(vostid)]);

    pstthdlocation->vostid = VOS_INVALID_U32;
    pstthdlocation->systid = VOS_INVALID_U32;
    pstthdlocation->callbackfunc = VOS_NULL;

    pthread_mutex_unlock((pthread_mutex_t *)(pstthdlocation->mutex));
    pthread_mutex_destroy((pthread_mutex_t *)(pstthdlocation->mutex));

    if (VOS_NULL != pstthdlocation->usrinpara)
    {
        vos_freememory(pstthdlocation->usrinpara);
        pstthdlocation->usrinpara = VOS_NULL;
    }
    
    return;
}

/*****************************************************************************
 函 数 名  : vos_mallocmutex
 功能描述  : 申请一个互斥信号量
 输入参数  : void
 输出参数  : 无
 返 回 值  : void*
 
 修改历史      :
  1.日    期   : 2013年10月22日
    作    者   : Albort,Feng

    修改内容   : 新生成函数

*****************************************************************************/
void* vos_mallocmutex(void)
{
    void* pnewmutex = VOS_NULL;
    pnewmutex = (void*)malloc(sizeof(pthread_mutex_t));
    return pnewmutex;
}

/*****************************************************************************
 函 数 名  : vos_freemutex
 功能描述  : 释放mutx内存
 输入参数  : void* pmutex
 输出参数  : 无
 返 回 值  : void
 
 修改历史      :
  1.日    期   : 2013年11月20日
    作    者   : Albort,Feng

    修改内容   : 新生成函数

*****************************************************************************/
void vos_freemutex(void* pmutex)
{
    if (VOS_NULL == pmutex)
    {
        return;
    }
    
    free(pmutex);
    
    return;
}

/*****************************************************************************
 函 数 名  : vos_createmutex
 功能描述  : 创建互斥信号量
 输入参数  : void* poutmutex
 输出参数  : 无
 返 回 值  : vos_u32
 
 修改历史      :
  1.日    期   : 2013年9月16日
    作    者   : Albort,Feng

    修改内容   : 新生成函数

*****************************************************************************/
vos_u32 vos_createmutex(void* poutmutex)
{
    if (VOS_NULL == poutmutex)
    {
        RETURNERRNO(vos_adaptsys_errorcode_base,vos_para_null_err);
    }

    pthread_mutexattr_t attr;
    
    if(VOS_OK != pthread_mutexattr_init(&attr))
    {
        RETURNERRNO(vos_adaptsys_errorcode_base,vos_createmutex_error);
    }

    #ifdef COMPILER_IS_ARM_LINUX_GCC
    if (VOS_OK != pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_PRIVATE))
    {
        RETURNERRNO(vos_adaptsys_errorcode_base,vos_createmutex_error);
    }

    if (VOS_OK != pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_ADAPTIVE_NP))
    {
        RETURNERRNO(vos_adaptsys_errorcode_base, vos_createmutex_error);
    }
    #else
    if (VOS_OK != pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_PRIVATE))
    {
        RETURNERRNO(vos_adaptsys_errorcode_base,vos_createmutex_error);
    }
    
    if (VOS_OK != pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE_NP))
    {
        RETURNERRNO(vos_adaptsys_errorcode_base, vos_createmutex_error);
    }
    #endif
    
    if(VOS_OK != pthread_mutex_init((pthread_mutex_t *)poutmutex,&attr))
    {
        RETURNERRNO(vos_adaptsys_errorcode_base, vos_createmutex_error);
    }
    
    pthread_mutexattr_destroy(&attr);
    
    return VOS_OK;
}

/*****************************************************************************
 函 数 名  : vos_deletemutex
 功能描述  : 删除互斥信号量
 输入参数  : void* pmutex
 输出参数  : 无
 返 回 值  : vos_u32
 
 修改历史      :
  1.日    期   : 2013年9月23日
    作    者   : Albort,Feng

    修改内容   : 新生成函数

*****************************************************************************/
vos_u32 vos_deletemutex(void* pmutex)
{
    if (VOS_NULL == pmutex)
    {
        RETURNERRNO(vos_adaptsys_errorcode_base, vos_para_null_err);
    }
    pthread_mutex_destroy((pthread_mutex_t *)pmutex);
    return VOS_OK;
}

/*****************************************************************************
 函 数 名  : vos_obtainmutex
 功能描述  : 获取互斥信号量
 输入参数  : void* pmutex
 输出参数  : 无
 返 回 值  : void
 
 修改历史      :
  1.日    期   : 2013年9月16日
    作    者   : Albort,Feng

    修改内容   : 新生成函数

*****************************************************************************/
void vos_obtainmutex(void* pmutex)
{
    if (VOS_NULL == pmutex)
    {
        return;
    }

    pthread_mutex_lock((pthread_mutex_t *)pmutex);
    return;
}

/*****************************************************************************
 函 数 名  : vos_releasemutex
 功能描述  : 释放互斥信号量
 输入参数  : void* pmutex
 输出参数  : 无
 返 回 值  : void
 
 修改历史      :
  1.日    期   : 2013年9月16日
    作    者   : Albort,Feng

    修改内容   : 新生成函数

*****************************************************************************/
void vos_releasemutex(void* pmutex)
{
    if (VOS_NULL == pmutex)
    {
        return;
    }
    pthread_mutex_unlock((pthread_mutex_t *)pmutex);
    return;
}

/*****************************************************************************
 函 数 名  : vos_setthreadpriority
 功能描述  : 设置线程优先级
 输入参数  : vos_tid vostid
             vos_u32 ulpri
 输出参数  : 无
 返 回 值  : vos_u32
 
 修改历史      :
  1.日    期   : 2013年9月2日
    作    者   : Albort,Feng

    修改内容   : 新生成函数

*****************************************************************************/
vos_u32 vos_setthreadpriority(vos_tid vostid, vos_u32 ulpri)
{
    vos_u32 ulrunrslt = VOS_OK;
    
    struct sched_param param;
    #ifndef COMPILER_IS_ARM_LINUX_GCC
    pthread_attr_t attr;
    #else
    vos_thread_info_stru* pstthdlocation = VOS_NULL;
    vos_i32 policy = 0;
    #endif

    if (VOS_OK != vos_checktid_valid(vostid))
    {
        RETURNERRNO(vos_adaptsys_errorcode_base, vos_para_invalid_err);
    }

    #ifndef COMPILER_IS_ARM_LINUX_GCC
    ulrunrslt = pthread_attr_getschedparam(&attr, &param);
    if(VOS_OK !=ulrunrslt)
    {
        RETURNERRNO(vos_adaptsys_errorcode_base, vos_get_threadpri_error);
    }

    param.sched_priority = ulpri;

    ulrunrslt = pthread_attr_setschedparam(&attr, &param);
    if(VOS_OK !=ulrunrslt)
    {
        RETURNERRNO(vos_adaptsys_errorcode_base, vos_set_threadpri_error);
    }
    #else
    pstthdlocation = &(g_pstmngthread->astthreadinfo[VOS_TID_MAP_TO_INDEX(vostid)]);
    
    ulrunrslt = pthread_getschedparam(pstthdlocation->systid,(int *)&policy, &param);
    if(VOS_OK !=ulrunrslt)
    {
        RETURNERRNO(vos_adaptsys_errorcode_base, vos_get_threadpri_error);
    }
    
    param.sched_priority = ulpri;
    policy = SCHED_FIFO;
    
    //设置线程优先级
    ulrunrslt = pthread_setschedparam(pstthdlocation->systid, policy, &param);
    if(VOS_OK !=ulrunrslt)
    {
        RETURNERRNO(vos_adaptsys_errorcode_base, vos_set_threadpri_error);
    }
    #endif
    
    return VOS_OK;
}

/*****************************************************************************
 函 数 名  : vos_suspendthread
 功能描述  : 挂起线程接口函数
 输入参数  : vos_tid vostid
 输出参数  : 无
 返 回 值  : vos_u32
 
 修改历史      :
  1.日    期   : 2013年9月6日
    作    者   : Albort,Feng

    修改内容   : 新生成函数

*****************************************************************************/
vos_u32 vos_suspendthread(vos_tid vostid)
{
    vos_u32 ulrunrslt = VOS_OK;

    vos_thread_info_stru* pstthdlocation = VOS_NULL;

    ulrunrslt = vos_checktid_valid(vostid);
    if (VOS_OK != ulrunrslt)
    {
        WRITE_WARN_LOG("vos_checktid_valid failed,ulrunrslt=%d, vostid=%d\n", ulrunrslt, vostid);
        RETURNERRNO(vos_adaptsys_errorcode_base, vos_para_invalid_err);
    }

    pstthdlocation = &(g_pstmngthread->astthreadinfo[VOS_TID_MAP_TO_INDEX(vostid)]);
    
    ulrunrslt = pthread_mutex_lock((pthread_mutex_t *)(pstthdlocation->mutex));
    if (VOS_OK != ulrunrslt)
    {
		WRITE_ERR_LOG("pthread_mutex_lock failed,ulrunrslt=(%d)\n",ulrunrslt);
        RETURNERRNO(vos_adaptsys_errorcode_base, vos_suspend_thread_error);
    }

    pstthdlocation->slflag -= 1;

    pthread_mutex_unlock((pthread_mutex_t *)(pstthdlocation->mutex));
    return VOS_OK;
}

/*****************************************************************************
 函 数 名  : vos_resumethread
 功能描述  : 恢复线程运行
 输入参数  : vos_tid vostid
 输出参数  : 无
 返 回 值  : vos_u32
 
 修改历史      :
  1.日    期   : 2013年9月2日
    作    者   : Albort,Feng

    修改内容   : 新生成函数

*****************************************************************************/
vos_u32 vos_resumethread(vos_tid vostid)
{
    vos_u32 ulrunrslt = VOS_OK;

    vos_thread_info_stru* pstthdlocation = VOS_NULL;

    ulrunrslt = vos_checktid_valid(vostid);

    if (VOS_OK != ulrunrslt)
    {
        RETURNERRNO(vos_adaptsys_errorcode_base, vos_para_invalid_err);
    }

    pstthdlocation = &(g_pstmngthread->astthreadinfo[VOS_TID_MAP_TO_INDEX(vostid)]);

    ulrunrslt = pthread_mutex_lock((pthread_mutex_t *)(pstthdlocation->mutex));
    if (VOS_OK != ulrunrslt)
    {
        RETURNERRNO(vos_adaptsys_errorcode_base, vos_suspend_thread_error);
    }

    pstthdlocation->slflag += 1;

    pthread_cond_signal(&(pstthdlocation->runsignal));
    
    pthread_mutex_unlock((pthread_mutex_t *)(pstthdlocation->mutex));
   
    return VOS_OK;
}

/*****************************************************************************
 函 数 名  : vos_threadsleep
 功能描述  : 调用线程睡眠指定时间（单位：毫秒）
 输入参数  : vos_u32 ulmillisecond
 输出参数  : 无
 返 回 值  : void
 
 修改历史      :
  1.日    期   : 2013年9月3日
    作    者   : Albort,Feng

    修改内容   : 新生成函数

*****************************************************************************/
void vos_threadsleep(vos_u32 ulmillisecond)
{
    struct timespec req, rem;
    
    req.tv_sec = ulmillisecond/1000;
    req.tv_nsec = (ulmillisecond % 1000) * 1000 *1000;
    nanosleep(&req, &rem);
}

/*****************************************************************************
 函 数 名  : vos_comthreadenter
 功能描述  : 公共线程入口函数
 输入参数  : void* para
 输出参数  : 无
 返 回 值  : vos_u32
 
 修改历史      :
  1.日    期   : 2013年9月18日
    作    者   : Albort,Feng

    修改内容   : 新生成函数

*****************************************************************************/
vos_u32 vos_comthreadenter(void* para)
{
    vos_tid vostid    = *((vos_tid*)para);
    vos_u32 ulrunrslt = VOS_OK;

    vos_thread_info_stru* pthdlocation = VOS_NULL;

    if (VOS_MAX_SUPPORT_THREAD_NUM < vostid)
    {
        RETURNERRNO(vos_adaptsys_errorcode_base, vos_para_invalid_err);
    }

    pthdlocation = &(g_pstmngthread->astthreadinfo[VOS_TID_MAP_TO_INDEX(vostid)]);

    while (1)
    {
        vos_obtainmutex(pthdlocation->mutex);
        while (0 >= pthdlocation->slflag)
        {
            pthread_cond_wait(&(pthdlocation->runsignal), (pthread_mutex_t*)(pthdlocation->mutex));
        }
        vos_releasemutex(pthdlocation->mutex);
        
        if (VOS_NULL != pthdlocation->callbackfunc)
        {
            ulrunrslt = pthdlocation->callbackfunc(pthdlocation->usrinpara);
            if (VOS_EXIST_THREAD_CODE == ulrunrslt)
            {
                break;
            }
        }
    }

    /*线程主动退出前释放相关资源*/
    vos_releasetid(vostid);

    g_pstmngthread->ultotalthrdnum -= 1;
    
    return VOS_OK;
}

/*****************************************************************************
 函 数 名  : vos_createthreadex
 功能描述  : 创建线程
 输入参数  : vos_multi_thread_para_stru* pstthdpara
             char* pucpolicy
 输出参数  : 无
 返 回 值  : vos_u32
 
 修改历史      :
  1.日    期   : 2013年8月28日
    作    者   : Albort,Feng

    修改内容   : 新生成函数

*****************************************************************************/
vos_u32 vos_createthreadex(vos_multi_thread_para_stru* pstthdpara, vos_u32 enpolicy)
{
    vos_u32 ulrunrslt = VOS_OK;
    vos_tid vostid    = VOS_INVALID_U32;
    pthread_attr_t attr;
    #ifndef COMPILER_IS_ARM_LINUX_GCC
    struct sched_param param;
    #endif

    vos_thread_info_stru* pstlocation = VOS_NULL;

    if (VOS_NULL == pstthdpara)
    {
        RETURNERRNO(vos_adaptsys_errorcode_base, vos_para_null_err);
    }

    if (VOS_NULL == pstthdpara->pthrdfunc)
    {
        RETURNERRNO(vos_adaptsys_errorcode_base, vos_para_invalid_err);
    }

    vostid = vos_distributetid();
    if (vos_max_crt_thd_error == vos_checktid_valid(vostid))
    { 
        RETURNERRNO(vos_adaptsys_errorcode_base, vos_max_crt_thd_error);
    }

    pstlocation = &(g_pstmngthread->astthreadinfo[VOS_TID_MAP_TO_INDEX(vostid)]);
    
    if (VOS_NULL != pstthdpara->pthrdinpara)
    {
        ulrunrslt = vos_mallocmemory(pstthdpara->arglen, vos_mem_type_static,
                                     &(pstlocation->usrinpara));
        if (VOS_OK != ulrunrslt)
        {
            vos_releasetid(vostid);
            return ulrunrslt;
        }
        
        memcpy(pstlocation->usrinpara, pstthdpara->pthrdinpara, pstthdpara->arglen);
    }
    
    pstlocation->callbackfunc = pstthdpara->pthrdfunc;

    if (0 == pstthdpara->ulstacksize)
    {
        pstthdpara->ulstacksize = VOS_DEFAULT_THREAD_STATCK_SIZE;
    }
    
    pstlocation->mutex = vos_mallocmutex();
    if (VOS_NULL == pstlocation)
    {
        RETURNERRNO(vos_adaptsys_errorcode_base,vos_malloc_mutex_error);
    }
    
    ulrunrslt = vos_createmutex(pstlocation->mutex);
    if (VOS_OK != ulrunrslt)
    {
        return ulrunrslt;
    }
    
    pthread_cond_init(&(pstlocation->runsignal),NULL);

    pstlocation->slflag = 1;
    
    //初始化线程属性
    ulrunrslt = pthread_attr_init(&attr);
    if (VOS_OK != ulrunrslt)
    {
        RETURNERRNO(vos_adaptsys_errorcode_base,vos_init_thread_attr_error);
    }
    
    //设置分离属性,新线程退出时释放所拥有的资源
    ulrunrslt = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    if (VOS_OK != ulrunrslt)
    {
        RETURNERRNO(vos_adaptsys_errorcode_base,vos_set_thread_detach_error);
    }

    if (vos_thread_sched_rtos == enpolicy)
    {//实时线程采用FIFO的方式
        ulrunrslt = pthread_attr_setschedpolicy(&attr, SCHED_FIFO);
        if (VOS_OK != ulrunrslt)
        {
            RETURNERRNO(vos_adaptsys_errorcode_base,vos_set_thread_policy_error);
        }
    }
    
    //设置线程堆空间
    if(PTHREAD_STACK_MIN < pstthdpara->ulstacksize)
    {
        ulrunrslt = pthread_attr_setstacksize(&attr, pstthdpara->ulstacksize);
        if (VOS_OK != ulrunrslt)
        {
            RETURNERRNO(vos_adaptsys_errorcode_base,vos_set_thread_stack_error);
        }
    }
    
    ulrunrslt = pthread_create(&(pstlocation->systid), &attr, (void* (*)(void*))vos_comthreadenter, (void *)&(pstlocation->vostid));
    if(VOS_OK != ulrunrslt)
    {
        vos_releasetid(vostid);
        RETURNERRNO(vos_adaptsys_errorcode_base, vos_create_thread_err);
    }
    
    ulrunrslt = pthread_attr_destroy(&attr);
    if (VOS_OK != ulrunrslt)
    {
        RETURNERRNO(vos_adaptsys_errorcode_base,vos_destory_thread_attr_error);
    }
    
    ulrunrslt = vos_suspendthread(pstlocation->vostid);
    if (VOS_OK != ulrunrslt)
    {
        return ulrunrslt;
    }
    
    /*设置线程优先级*/
    if (vos_thread_sched_rtos == enpolicy)
    {//仅实时线程才需设置优先级
        #ifdef COMPILER_IS_ARM_LINUX_GCC
        ulrunrslt = vos_setthreadpriority(vostid, pstthdpara->ulthreadpri);
        if (VOS_OK != ulrunrslt)
        {
            return ulrunrslt;
        }
        #else
        ulrunrslt = pthread_attr_getschedparam(&attr, &param);
        if(VOS_OK !=ulrunrslt)
        {
            RETURNERRNO(vos_adaptsys_errorcode_base, vos_get_threadpri_error);
        }

        param.sched_priority = pstthdpara->ulthreadpri;

        ulrunrslt = pthread_attr_setschedparam(&attr, &param);
        if(VOS_OK !=ulrunrslt)
        {
            RETURNERRNO(vos_adaptsys_errorcode_base, vos_set_threadpri_error);
        }
        #endif
    }
    
    if (vos_thread_init_status_running == pstthdpara->eninitstatus)
    {
        /*启动线程开始运行*/
        ulrunrslt = vos_resumethread(vostid);
        if (VOS_OK != ulrunrslt)
        {
            return ulrunrslt;
        }
    }
    
    pstthdpara->ulthrdid = vostid;
    pstthdpara->systid = (vos_u32)pstlocation->systid;
    g_pstmngthread->ultotalthrdnum += 1;
    
    return VOS_OK;
}

/*****************************************************************************
 函 数 名  : vos_checktid_valid
 功能描述  :  输入参数 有效性检查
 输入参数  : vos_tid tid
             
 输出参数  : 无
 返 回 值  : vos_u32
 
 修改历史      :
  1.日    期   : 2013年9月22日
    作    者   : zhangchaoyong

    修改内容   : 新生成函数

*****************************************************************************/
vos_u32 vos_checktid_valid( vos_tid vostid )
{
    vos_u32 ulresult = VOS_OK;
    vos_thread_info_stru* pstlocation = VOS_NULL;
    
    if (VOS_MAX_SUPPORT_THREAD_NUM < vostid)
    {
        ulresult = vos_max_crt_thd_error;
    }
    
    pstlocation = &(g_pstmngthread->astthreadinfo[VOS_TID_MAP_TO_INDEX(vostid)]);

    if(VOS_NULL == pstlocation->callbackfunc)
    {//已经被注册过
        ulresult = vos_list_id_invalid_err ;
    }
    
    return ulresult;
}


/*****************************************************************************
 函 数 名  : vos_terminatethread
 功能描述  : 终止线程
 输入参数  : vos_tid vostid
 输出参数  : 无
 返 回 值  : void
 
 修改历史      :
  1.日    期   : 2013年9月11日
    作    者   : Albort,Feng

    修改内容   : 新生成函数

*****************************************************************************/
void vos_terminatethread(vos_tid vostid)
{
    vos_thread_info_stru* pstlocation = VOS_NULL;
    
    if (VOS_MAX_SUPPORT_THREAD_NUM < vostid)
    {
        return;
    }
    
    pstlocation = &(g_pstmngthread->astthreadinfo[VOS_TID_MAP_TO_INDEX(vostid)]);
    vos_releasemutex(pstlocation->mutex);
    
    pthread_cancel((pthread_t)&(pstlocation->systid));

    vos_releasetid(vostid);

    g_pstmngthread->ultotalthrdnum -= 1;
    
    return;
}

/*****************************************************************************
 函 数 名  : vos_closesocket
 功能描述  : vos平台关闭socket接口
 输入参数  : vos_socket ulsock
 输出参数  : 无
 返 回 值  : void
 
 修改历史      :
  1.日    期   : 2013年11月20日
    作    者   : Albort,Feng

    修改内容   : 新生成函数

*****************************************************************************/
void vos_closesocket(vos_socket ulsock)
{
    if (VOS_NULL_SOCKET != ulsock)
    {
        shutdown(ulsock, SHUT_RDWR);
        close(ulsock);
    }
}

/*****************************************************************************
 函 数 名  : vos_socksend
 功能描述  : vos平台socket报文发送接口
 输入参数  : vos_socket ulsock
             char* puccnt
             vos_u32 uldtlen
 输出参数  : 无
 返 回 值  : vos_u32
 
 修改历史      :
  1.日    期   : 2013年11月20日
    作    者   : Albort,Feng

    修改内容   : 新生成函数

*****************************************************************************/
vos_u32 vos_socksend(vos_socket ulsock, char* puccnt, vos_u32 uldtlen)
{
    vos_i32 ilsendbytes = 0;
    
    if (VOS_NULL == puccnt)
    {
        RETURNERRNO(vos_adaptsys_errorcode_base, vos_para_null_err);
    }
    
    ilsendbytes = send(ulsock, puccnt, uldtlen, 0);
    
    if (0 >= ilsendbytes&&(vos_u32)ilsendbytes != uldtlen)
    {
        RETURNERRNO(vos_adaptsys_errorcode_base, vos_socket_send_error);
    }
    
    return VOS_OK;
}

/*****************************************************************************
 函 数 名  : vos_sockreceive
 功能描述  : vos平台socket接收报文接口
 输入参数  : vos_socket ulsock
             char* pucbuf
             vos_u32 ulbuflen
             vos_u32* pulrcvlen
 输出参数  : 无
 返 回 值  : vos_u32
 
 修改历史      :
  1.日    期   : 2013年11月20日
    作    者   : Albort,Feng

    修改内容   : 新生成函数

*****************************************************************************/
vos_u32 vos_sockreceive(vos_socket ulsock, char* pucbuf, vos_u32 ulbuflen,
                        vos_u32* pulrcvlen)
{
    vos_i32 ilrcvlen   =  0; 
    if ((VOS_NULL == pucbuf) || (VOS_NULL == pulrcvlen))
    {
        RETURNERRNO(vos_adaptsys_errorcode_base, vos_para_null_err);
    }
    
    ilrcvlen = recv(ulsock, pucbuf, ulbuflen, 0);

    if(0 >= ilrcvlen)
    {
        RETURNERRNO(vos_adaptsys_errorcode_base, vos_socket_receive_error);
    }

    *pulrcvlen = ilrcvlen;
   
    return VOS_OK;
}

/*****************************************************************************
 函 数 名  : vos_filterpathforfile
 功能描述  : 文件路径过滤掉路径接口
 输入参数  : char* pucallpath
 输出参数  : 无
 返 回 值  : char*
 
 修改历史      :
  1.日    期   : 2013年11月20日
    作    者   : Albort,Feng

    修改内容   : 新生成函数

*****************************************************************************/
char* vos_filterpathforfile(char* pucallpath)
{
    vos_u32 ullenght    = 0;
    char  ucsplitchar = '/';
    char* pucfilename = VOS_NULL;

    ullenght    = (vos_u32)strlen((char *)pucallpath);
    pucfilename = pucallpath + ullenght;

    while ((pucfilename != pucallpath) && (ucsplitchar != *pucfilename))
    {
        pucfilename -= 1;
    }

    if (ucsplitchar == *pucfilename)
    {
        pucfilename += 1;
    }

    return pucfilename;
}

/*****************************************************************************
 函 数 名  : vos_linuxtimerroutine
 功能描述  : 基准定时器Linux 处理函数
 输入参数  : vos_i32 signum
             siginfo_t *si
             void *uc
 输出参数  : 无
 返 回 值  : void
 
 修改历史      :
  1.日    期   : 2014年1月8日
    作    者   : zhangchaoyong

    修改内容   : 新生成函数

*****************************************************************************/
void vos_linuxtimerroutine(union sigval v)
{
    enventcallback pcallfunc = VOS_NULL;

    if (VOS_NULL != v.sival_ptr)
    {
        pcallfunc = (enventcallback)(v.sival_ptr);
        pcallfunc();
    }

    return;
}

/*****************************************************************************
 函 数 名  : vos_createenventtimer
 功能描述  : 创建一个事件定时器
 输入参数  : vos_u32 ulmillisecond
             enventcallback callfunc
 输出参数  : 无
 返 回 值  : vos_u32 
 
 修改历史      :
  1.日    期   : 2014年1月13日
    作    者   : zhangchaoyong

    修改内容   : 新生成函数

*****************************************************************************/
vos_u32  vos_createenventtimer(vos_u32 ulmillisecond,enventcallback callfunc)
{
    vos_u32 ulreturn = VOS_OK;
    
    timer_t timerfd;
    struct sigevent sevp;
    struct itimerspec newvalue;
    memset(&sevp, 0x00, sizeof(struct sigevent));
    sevp.sigev_notify = SIGEV_THREAD;
    sevp.sigev_value.sival_ptr = (void*)callfunc;
    sevp.sigev_notify_function = vos_linuxtimerroutine;

    ulreturn = timer_create(CLOCK_REALTIME, &sevp, &timerfd);
    if (VOS_OK != ulreturn)
    {   
        RETURNERRNO(vos_adaptsys_errorcode_base, vos_crt_basetimer_error); 
    }

    memset(&newvalue, 0x00, sizeof(struct itimerspec));
    newvalue.it_value.tv_sec = ulmillisecond/1000;
    newvalue.it_value.tv_nsec = (ulmillisecond%1000)*1000*1000;
    
    newvalue.it_interval.tv_sec = ulmillisecond/1000;
    newvalue.it_interval.tv_nsec = (ulmillisecond%1000)*1000*1000;
    
    if (0 != timer_settime(timerfd, 0, &newvalue, VOS_NULL))
    {
        RETURNERRNO(vos_adaptsys_errorcode_base, vos_setcrt_basetimer_error);
    }
    return VOS_OK;
}

/*****************************************************************************
 函 数 名  : vos_getthreadtotalnum
 功能描述  : 获取线程总数
 输入参数  : void
 输出参数  : 无
 返 回 值  : vos_u32
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2014年8月1日
    作    者   : Albort,Feng
    修改内容   : 新生成函数

*****************************************************************************/
vos_u32 vos_getthreadtotalnum(void)
{
    return g_pstmngthread->ultotalthrdnum;
}

#ifdef __cplusplus
}
#endif


