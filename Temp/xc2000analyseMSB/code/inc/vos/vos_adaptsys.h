#ifndef _vos_adaptsys_h_
#define _vos_adaptsys_h_    
/******************************************************************************

                  版权所有 (C), 2013-2023, 四川新成生物科技有限公司

 ******************************************************************************
  文 件 名   : vos_adaptsys.h
  版 本 号   : 初稿
  作    者   : Albort,Feng
  生成日期   : 2013年8月28日
  最近修改   :
  功能描述   : 操作系统适配模块头文件
  函数列表   :
  修改历史   :
  1.日    期   : 2013年8月28日
    作    者   : Albort,Feng
    修改内容   : 创建文件

******************************************************************************/
#include "vos_configure.h"
#include "vos_basictypedefine.h"
#include <sys/time.h>

#define NAME_SIZE  (256)
#define VOS_EXIST_THREAD_CODE (0x00f0f0f0)
#define VOS_MAX_IP_BUF_LEN    (24)

typedef vos_u32 (*vos_pthreadfunc)(void*);
typedef void (*enventcallback)(void);

typedef vos_u32 vos_tid;
typedef int vos_socket;

typedef enum
{
    vos_thread_init_status_susppend = 0,  //线程创建后挂起
    vos_thread_init_status_running  = 1,  //线程创建后运行

    vos_thread_init_status_butt
}vos_thread_init_status_enum;

typedef enum
{
    vos_thread_sched_normal = 0,  //普通线程调度策略
    vos_thread_sched_rtos   = 1,  //实时线程调度策略
}vos_thread_sched_policy;

//定义线程优先级
typedef enum
{
    vos_thread_pri_lowest = 0,
    vos_thread_pri_1,
    vos_thread_pri_2,
    vos_thread_pri_3,
    vos_thread_pri_4,
    vos_thread_pri_5,
    vos_thread_pri_6,
    vos_thread_pri_7,
    vos_thread_pri_8,
    vos_thread_pri_9,
    vos_thread_pri_10,
    vos_thread_pri_11,
    vos_thread_pri_12,
    vos_thread_pri_13,
    vos_thread_pri_14,
    vos_thread_pri_15,
    vos_thread_pri_16,
    vos_thread_pri_17,
    vos_thread_pri_18,
    vos_thread_pri_19,
    vos_thread_pri_20,
    vos_thread_pri_21,
    vos_thread_pri_22,
    vos_thread_pri_23,
    vos_thread_pri_24,
    vos_thread_pri_25,
    vos_thread_pri_26,
    vos_thread_pri_27,
    vos_thread_pri_28,
    vos_thread_pri_29,
    vos_thread_pri_30,
    vos_thread_pri_31,
    vos_thread_pri_32,
    vos_thread_pri_33,
    vos_thread_pri_34,
    vos_thread_pri_35,
    vos_thread_pri_36,
    vos_thread_pri_37,
    vos_thread_pri_38,
    vos_thread_pri_39,
    vos_thread_pri_40,
    vos_thread_pri_41,
    vos_thread_pri_42,
    vos_thread_pri_43,
    vos_thread_pri_44,
    vos_thread_pri_45,
    vos_thread_pri_46,
    vos_thread_pri_47,
    vos_thread_pri_48,
    vos_thread_pri_49,
    vos_thread_pri_50,
    vos_thread_pri_51,
    vos_thread_pri_52,
    vos_thread_pri_53,
    vos_thread_pri_54,
    vos_thread_pri_55,
    vos_thread_pri_56,
    vos_thread_pri_57,
    vos_thread_pri_58,
    vos_thread_pri_59,
    vos_thread_pri_60,
    vos_thread_pri_61,
    vos_thread_pri_62,
    vos_thread_pri_63,
    vos_thread_pri_64,
    vos_thread_pri_65,
    vos_thread_pri_66,
    vos_thread_pri_67,
    vos_thread_pri_68,
    vos_thread_pri_69,
    vos_thread_pri_70,
    vos_thread_pri_71,
    vos_thread_pri_72,
    vos_thread_pri_73,
    vos_thread_pri_74,
    vos_thread_pri_75,
    vos_thread_pri_76,
    vos_thread_pri_77,
    vos_thread_pri_78,
    vos_thread_pri_79,
    vos_thread_pri_80,
    vos_thread_pri_81,
    vos_thread_pri_82,
    vos_thread_pri_83,
    vos_thread_pri_84,
    vos_thread_pri_85,
    vos_thread_pri_86,
    vos_thread_pri_87,
    vos_thread_pri_88,
    vos_thread_pri_89,
    vos_thread_pri_90,
    vos_thread_pri_91,
    vos_thread_pri_92,
    vos_thread_pri_93,
    vos_thread_pri_94,
    vos_thread_pri_95,
    vos_thread_pri_96,
    vos_thread_pri_97,
    vos_thread_pri_98,
    vos_thread_pri_highest,

    vos_thread_pri_butt
}vos_thread_pri_enum;
typedef vos_u32 vos_thread_pri_enum_u32;

#define vos_createthread1(a)   vos_createthreadex(a, vos_thread_sched_normal)
#define vos_createthread2(a,b) vos_createthreadex(a,b)

#define GET_2TH_ARG(arg1, arg2, arg3,...) arg3
#define FUN_MACRO_CHOOSER(...) GET_2TH_ARG(__VA_ARGS__, \
                                 vos_createthread2, vos_createthread1, )
 
#define vos_createthread(...) FUN_MACRO_CHOOSER(__VA_ARGS__)(__VA_ARGS__)

//平台创建多线程函数参数结构定义
typedef struct
{
    vos_u32 ulthreadpri;       //线程优先级(1~99,数字越大优先级越大)
    vos_u32 ulstacksize;       //线程堆栈大小
    vos_tid ulthrdid;          //创建成功后输出的线程id
    vos_pthreadfunc pthrdfunc; //线程回调
    void* pthrdinpara;     //线程函数输入参数
    vos_u32   arglen;          //线程输入参数长度即pthrdinpara的长度
    vos_u32   eninitstatus;    //线程创建后的状态
    vos_u32 systid;          //真正的线程ID
}vos_multi_thread_para_stru;

#ifdef __cplusplus
extern "C" {
#endif

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
void* vos_mallocmutex(void);

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
void vos_freemutex(void* pmutex);

/*****************************************************************************
 函 数 名  : vos_checktid_valid
 功能描述  :  输入参数 有效性检查
 输入参数  : vos_tid vostid
             
 输出参数  : 无
 返 回 值  : vos_u32
 
 修改历史      :
  1.日    期   : 2013年9月22日
    作    者   : zhangchaoyong

    修改内容   : 新生成函数

*****************************************************************************/
vos_u32 vos_checktid_valid( vos_tid vostid );

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
vos_u32 vos_createmutex(void* poutmutex);

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
vos_u32 vos_deletemutex(void* pmutex);

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
void vos_obtainmutex(void* pmutex);

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
void vos_releasemutex(void* pmutex);

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
vos_u32 vos_setthreadpriority(vos_tid vostid, vos_u32 ulpri);

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
vos_u32 vos_suspendthread(vos_tid vostid);

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
vos_u32 vos_resumethread(vos_tid vostid);

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
void vos_threadsleep(vos_u32 ulmillisecond);

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
vos_u32 vos_createthreadex(vos_multi_thread_para_stru* pstthdpara, vos_u32 enpolicy);

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
void vos_terminatethread(vos_tid vostid);

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
void vos_closesocket(vos_socket ulsock);

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
vos_u32 vos_socksend(vos_socket ulsock, char* pcnt, vos_u32 uldtlen);

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
vos_u32 vos_sockreceive(vos_socket ulsock, char* pucbuf, vos_u32 ulbuflen, vos_u32* pulrcvlen);

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
char* vos_filterpathforfile(char* pucallpath);

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
vos_u32  vos_createenventtimer(vos_u32 ulmillisecond,enventcallback callfunc);

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
vos_u32 vos_getthreadtotalnum(void);

//适配层初始化接口
vos_u32 vos_initadaptmd(void);

#ifdef __cplusplus
}
#endif

#endif
