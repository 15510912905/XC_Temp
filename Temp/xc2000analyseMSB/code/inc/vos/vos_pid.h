#ifndef _vos_pid_h_
#define _vos_pid_h_    
/******************************************************************************

                  版权所有 (C), 2013-2023, 四川新成生物科技有限公司

 ******************************************************************************
  文 件 名   : vos_pid.h
  版 本 号   : 初稿
  作    者   : Albort,Feng
  生成日期   : 2013年9月9日
  最近修改   :
  功能描述   : 软件平台PID模块头文件定义
  函数列表   :
  修改历史   :
  1.日    期   : 2013年9月9日
    作    者   : Albort,Feng
    修改内容   : 创建文件

******************************************************************************/
#include "vos_basictypedefine.h"
#include "vos_messagecenter.h"


//产品FID定义
typedef enum
{
    app_self_net_fid = 400,                       //心跳线程
    app_self_oam_fid = 500,					//产品OAM模块FID(500)
    app_self_log_fid = 600,                      //日志线程
    app_resource_schedule_fid = 700,		//资源调度模块FID(700)    
    app_dwnmach_cmd_center_fid = 800,  //下位机命令管理模块FID(800)
    app_subsys_mng_fid = 900,              //子系统线程FID（900）
    app_task_result_fid = 1000,              //任务结果线程ID(1000)
    app_auto_test_fid = 1200,
}app_interior_fid_enum;

//业务模块PID定义
typedef enum
{
    Pid_DebugTty = 505,                //模拟调试串口PID
    Pid_ReportStatus = 512,           //下位机状态上报处理PID
    Pid_Maintain = 520,                 //维护模块PID
    Pid_LoopTest = 523,    //移动设备交互模块PID
    Pid_DwnMachUart = 801,        //下位机串口管理模块PID
    Pid_SimulatorTest = 1201,       //模拟测试
}app_interior_business_pid_enum;

typedef vos_u32 (*vos_pidrcvmsgfunc)(vos_msg_header_stru*);

#ifdef __cplusplus
extern "C" {
#endif
/*****************************************************************************
 函 数 名  : vos_registerpid
 功能描述  : PID模块注册接口
 输入参数  : vos_u32 ulfid
             vos_u32 ulpid
             vos_pidrcvmsgfunc prcvmsgfunc
 输出参数  : 无
 返 回 值  : vos_u32
 
 修改历史      :
  1.日    期   : 2013年9月9日
    作    者   : Albort,Feng

    修改内容   : 新生成函数

*****************************************************************************/
vos_u32 vos_registerpid(vos_u32 ulfid, vos_u32 ulpid, vos_pidrcvmsgfunc prcvmsgfunc);

/*****************************************************************************
 函 数 名  : vos_utreleasepid
 功能描述  : 释放PID模块
 输入参数  : vos_u32 ulpid
 输出参数  : 无
 返 回 值  : vos_u32
 
 修改历史      :
  1.日    期   : 2013年9月11日
    作    者   : Albort,Feng

    修改内容   : 新生成函数

*****************************************************************************/
vos_u32 vos_utreleasepid(vos_u32 ulpid);

/*****************************************************************************
 函 数 名  : vos_utreleaseallpidoffid
 功能描述  : 释放一个FID模块中所有的PID
 输入参数  : vos_u32 ulfid
 输出参数  : 无
 返 回 值  : void
 
 修改历史      :
  1.日    期   : 2013年9月11日
    作    者   : Albort,Feng

    修改内容   : 新生成函数

*****************************************************************************/
void vos_utreleaseallpidoffid(vos_u32 ulfid);

#ifdef __cplusplus
}
#endif

class CCallBack
{
public:
    CCallBack(vos_u32 ulpid);
    virtual ~CCallBack();

    virtual vos_u32 CallBack(vos_msg_header_stru* pMsg) = 0;

    vos_u32 RegCallBack(vos_u32 ulfid);

protected:
    vos_u32 m_ulFid;
    vos_u32 m_ulPid;
};
vos_u32 vos_registerCallBack(vos_u32 ulfid, vos_u32 ulpid, CCallBack * pBack);

#endif
