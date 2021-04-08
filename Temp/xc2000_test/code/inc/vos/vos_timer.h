#ifndef _vos_timer_h_
#define _vos_timer_h_ 
/******************************************************************************

                  版权所有 (C), 2013-2023, 四川新成生物科技有限公司

 ******************************************************************************
  文 件 名   : vos_timer.h
  版 本 号   : 初稿
  作    者   : zhangchaoyong
  生成日期   : 2014年1月6日
  最近修改   :
  功能描述   : vos d定时器相关数据结构及方法定义
  函数列表   :
  修改历史   :
  1.日    期   : 2014年1月6日
    作    者   : zhangchaoyong
    修改内容   : 创建文件

******************************************************************************/
#include "vos_basictypedefine.h"
#include "vos_messagecenter.h"

#define VOS_TIMERNAME_LEN   (64)
#define VOS_MAX_PID_TIMER   (10)
#define  vos_pidmsgtype_timer_timeout  0x000a   //定时超时
//定时器类型
typedef enum
{
    vos_timer_type_once  = 1,//一次性定时器
    vos_timer_type_loop  = 2,//多次性定时器
}vos_timer_type_enum;

typedef vos_u32 vos_timer_type_enum_u32;

//定时器创建结构体
typedef struct 
{
    vos_u32 entimertype;                     //定时器类型
    vos_u32 ultimelen;                       //定时时间长度(单位:0.1秒)
    vos_u32 ulsrcpid;                        //触发定时消息PID
    vos_u32 aucdstpid[VOS_MAX_PID_TIMER];    //定时器目标PID
    char  auctimername[VOS_TIMERNAME_LEN]; //定时器名称
}vos_timer_reg_stru;

//定时器消息信息
typedef struct vos_timermsg
{
    vos_msg_header;
    vos_u32 ultimerid;                      //定时器ID
    char  auctimername[VOS_TIMERNAME_LEN];//定时器名称
}vos_timeroutmsg_stru;

#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************************
 函 数 名  : vos_createtimer
 功能描述  : 新建一个定时器
 输入参数  : vos_timer_reg_stru *regstru
              vos_u32 *ultimedid
 输出参数  : 无
 返 回 值  : vos_u32
 
 修改历史      :
  1.日    期   : 2014年1月6日
    作    者   : zhangchaoyong

    修改内容   : 新生成函数

*****************************************************************************/
vos_u32 vos_createtimer(vos_timer_reg_stru *regstru, vos_u32 *ptimerid);

/*****************************************************************************
 函 数 名  : vos_deletetimer
 功能描述  : 删除一个定时器
 输入参数  : vos_u32 ultimerid
 输出参数  : 无
 返 回 值  : vos_u32 
 
 修改历史      :
  1.日    期   : 2014年1月6日
    作    者   : zhangchaoyong

    修改内容   : 新生成函数

*****************************************************************************/
vos_u32  vos_deletetimer(vos_u32 ultimerid);

/*****************************************************************************
 函 数 名  : vos_starttimer
 功能描述  : 启动一个定时器
 输入参数  : vos_u32 ultimerid
 输出参数  : 无
 返 回 值  : vos_u32 
 
 修改历史      :
  1.日    期   : 2014年1月6日
    作    者   : zhangchaoyong

    修改内容   : 新生成函数

*****************************************************************************/
vos_u32  vos_starttimer(vos_u32 ultimerid);

/*****************************************************************************
 函 数 名  : vos_stoptimer
 功能描述  : 停止一个定时器
 输入参数  : vos_u32 ultimerid
 输出参数  : 无
 返 回 值  : vos_u32 
 
 修改历史      :
  1.日    期   : 2014年1月6日
    作    者   : zhangchaoyong

    修改内容   : 新生成函数

*****************************************************************************/
vos_u32 vos_stoptimer(vos_u32 ultimerid);

/*****************************************************************************
 函 数 名  : vos_checktimerisexist
 功能描述  : 判断定时器是否存在
 输入参数  : char *pctimername
 输出参数  : 无
 返 回 值  : vos_u32 (VOS_YES: 指定名称的定时器已创建,VOS_NO:指定的定时器未创建)
 
 修改历史      :
  1.日    期   : 2014年1月13日
    作    者   : zhangchaoyong

    修改内容   : 新生成函数

*****************************************************************************/
vos_u32 vos_checktimerisexist(char *timername);

/*****************************************************************************
 函 数 名  : vos_settimertimelen
 功能描述  : 设置定时器定时长度
 输入参数  : vos_u32 ultimerid
              vos_u32 ulmillisecond
 输出参数  : 无
 返 回 值  : vos_u32
 
 修改历史      :
  1.日    期   : 2014年1月6日
    作    者   : zhangchaoyong

    修改内容   : 新生成函数

*****************************************************************************/
vos_u32 vos_settimertimelen(vos_u32 ultimerid,vos_u32 ulmillisecond);

/*****************************************************************************
 函 数 名  : vos_gettimerreginfo
 功能描述  : 获取定时器注册信息
 输入参数  : vos_u32 ultimerid
             vos_timer_reg_stru* pouttimerinfo
 输出参数  : 无
 返 回 值  : vos_u32
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2014年9月17日
    作    者   : Albort,Feng
    修改内容   : 新生成函数

*****************************************************************************/
vos_u32 vos_gettimerreginfo(vos_u32 ultimerid, vos_timer_reg_stru* pouttimerinfo);

/*****************************************************************************
 函 数 名  : vos_gettimerrunedtimelen
 功能描述  : 获取定时器已运行时间长度
 输入参数  : vos_u32 ultimerid
 输出参数  : 无
 返 回 值  : vos_u32 (定时器已运行时间长度,单位为100ms)
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2014年9月17日
    作    者   : Albort,Feng
    修改内容   : 新生成函数

*****************************************************************************/
vos_u32 vos_gettimerrunedtimelen(vos_u32 ultimerid);

/*****************************************************************************
 函 数 名  : vos_gettimerstatus
 功能描述  : 获取指定定时器的当前状态
 输入参数  : vos_u32 ultimerid
             vos_u32* pulouttimerstatus
 输出参数  : 无
 返 回 值  : vos_u32
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2015年5月15日
    作    者   : Albort,Feng
    修改内容   : 新生成函数

*****************************************************************************/
vos_u32 vos_gettimerstatus(vos_u32 ultimerid, vos_u32* pulouttimerstatus);

/*****************************************************************************
 函 数 名  : vos_printalltimerinfo
 功能描述  : 打印所有定时器信息
 输入参数  : void
 输出参数  : 无
 返 回 值  : void
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2014年9月4日
    作    者   : Albort,Feng
    修改内容   : 新生成函数

*****************************************************************************/
void vos_printalltimerinfo(void);

//创建定时器
vos_u32 createTimer(vos_u32 ulPid, const char* strName, vos_u32 ulTimeLen = 0, vos_u32 ulTimerType = vos_timer_type_once);

//开始定时器
vos_u32 startTimer(vos_u32 ulTimer, vos_u32 ulTimeLen);

//初始化定时器模块
vos_u32 vos_init_timermd(void);

#ifdef __cplusplus
}
#endif
#endif
