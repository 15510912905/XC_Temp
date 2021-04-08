#ifndef _vos_configure_h_
#define _vos_configure_h_    
/******************************************************************************

                  版权所有 (C), 2013-2023, 四川新成生物科技有限公司

 ******************************************************************************
  文 件 名   : vos_configure.h
  版 本 号   : 初稿
  作    者   : Albort,Feng
  生成日期   : 2013年8月26日
  最近修改   :
  功能描述   : vos系统性能配置文件
  函数列表   :
  修改历史   :
  1.日    期   : 2013年8月26日
    作    者   : Albort,Feng
    修改内容   : 创建文件

******************************************************************************/
//vos平台支持最大定时器数
#define VOS_MAX_SUPPORT_TIMER_NUM   (1024)

//vos系统支持最大链表数配置
#define VOS_MAX_SUPPORT_LIST_NUM    (256)

//vos系统支持最大线程数
#define VOS_MAX_SUPPORT_THREAD_NUM  (256)

//vos系统内存泄漏检查周期,单位为秒
#define VOS_CHECK_LEAKMEM_PERIOD    (1)

//vos系统内存泄漏检查上限,即未使用检查次数到达上限后内存被主动释放
#define VOS_CHECK_LEAKMEM_UPPER_THR  (3)

///vos系统告警告警后处理线程周期(单位:毫秒)
#define VOS_ALARM_THREADPROC_PERIOD (1000)

//vos系统故障默认检查周期(单位毫秒)
#define VOS_CHECK_LEAKFAULT_PERIOD  (100)

//vos系统FID模块中能够注册的PID模块最大数,只能是100的整数
#define VOS_MAX_PID_NUM_PER_FID      (100)

//vos系统支持的最多FID数
#define VOS_MAX_SUPPORT_FID_NUM      (16)

//vos系统FID线程栈大小，8M
#define VOS_FID_DEFAULT_STACK_SIZE   (8*1024*1024)

//vos系统线程默认大小
#define VOS_DEFAULT_THREAD_STATCK_SIZE (2 * 1024*1024)

//vos系统内存检测是否支持可重入操作
#define VOS_SUPPORT_CHECK_MEM_MULTI_OP (1)

//vos系统FID线程创建后的出事状态0:挂起, 1:运行
#define VOS_FID_THREAD_INIT_STATUS  (1)

//vos系统单条日志最大长度
#define VOS_MAX_SINGLE_LOG_LENGTH   (1024)

//vos系统每个FID模块日志缓存支持的最大日志条数
#define VOS_MAX_SUPPORT_LOG_NUM_PER_FID (1024)

//vos系统每个FID日志内存大小
#define VOS_LOG_MEM_SIZE_PER_FID    (VOS_MAX_SUPPORT_LOG_NUM_PER_FID * VOS_MAX_SINGLE_LOG_LENGTH)

//vos系统日志文件大小打包门限
#define VOS_PACK_LOG_FILE_THR       (512*1024)

//vos系统服务端通信端口号定义
#define VOS_SERVER_SOCKET_PORT_ID   (20000)

//vos系统最大级联中位机个数定义
#define VOS_MAX_MID_MACHIN_NUM      (10)

//告警名称大小
#define VOS_ALARMNAME_SIZE          (64)

//告警下属个数
#define VOS_SUBFAULT_NUM            (20) 

//定时器模块定时精度(毫秒)
#define VOS_TIMER_ACCURACY          (100)

//堆内存保护隔离长度
#define VOS_PROTECT_HEAP_MEM_LEN    (16)
#endif
