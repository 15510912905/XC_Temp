#ifndef _vos_selfpid_h_
#define _vos_selfpid_h_       
/******************************************************************************

                  版权所有 (C), 2013-2023, 四川新成生物科技有限公司

 ******************************************************************************
  文 件 名   : vos_selfpid.h
  版 本 号   : 初稿
  作    者   : Albort,Feng
  生成日期   : 2013年9月9日
  最近修改   :
  功能描述   : PID模块仅内部使用的头文件
  函数列表   :
  修改历史   :
  1.日    期   : 2013年9月9日
    作    者   : Albort,Feng
    修改内容   : 创建文件

******************************************************************************/
#include "vos_basictypedefine.h"
#include "vos_messagecenter.h"
#include "vos_pid.h"

//PID注册信息结构定义
typedef struct
{
    vos_u32 ulpid;
    vos_pidrcvmsgfunc pidrcvmsgfunc;
    CCallBack * pCallBack;
}vos_pid_register_info_stru;

#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************************
 函 数 名  : vos_initpidmd
 功能描述  : PID模块初始化接口
 输入参数  : void
 输出参数  : 无
 返 回 值  : vos_u32
 
 修改历史      :
  1.日    期   : 2013年9月9日
    作    者   : Albort,Feng

    修改内容   : 新生成函数

*****************************************************************************/
vos_u32 vos_initpidmd(void);

/*****************************************************************************
 函 数 名  : vos_deliverymsgtopid
 功能描述  : 消息分发，即根据消息目的PID将消息投递给相应的PID模块
 输入参数  : vos_msg_header_stru* pstmsg
 输出参数  : 无
 返 回 值  : vos_u32
 
 修改历史      :
  1.日    期   : 2013年9月6日
    作    者   : Albort,Feng

    修改内容   : 新生成函数

*****************************************************************************/
vos_u32 vos_deliverymsgtopid(vos_msg_header_stru* pstmsg);

/*****************************************************************************
 函 数 名  : vos_checkpidvalidity
 功能描述  : PID有效性校验
 输入参数  : vos_u32 ulpid
 输出参数  : 无
 返 回 值  : vos_u32
 
 修改历史      :
  1.日    期   : 2013年9月11日
    作    者   : Albort,Feng

    修改内容   : 新生成函数

*****************************************************************************/
vos_u32 vos_checkpidvalidity(vos_u32 ulpid);

/*****************************************************************************
 函 数 名  : vos_mallocpidreginfomem
 功能描述  : 为一个FID申请保存PID注册信息的内存
 输入参数  : vos_u32 ulfid
 输出参数  : 无
 返 回 值  : vos_u32
 
 修改历史      :
  1.日    期   : 2013年9月10日
    作    者   : Albort,Feng

    修改内容   : 新生成函数

*****************************************************************************/
vos_u32 vos_mallocpidreginfomem(vos_u32 ulfid);

/*****************************************************************************
 函 数 名  : vos_freepidreginfomem
 功能描述  : 释放保存PID注册信息的内存
 输入参数  : vos_u32 ulfid
 输出参数  : 无
 返 回 值  : void
 
 修改历史      :
  1.日    期   : 2013年9月17日
    作    者   : Albort,Feng

    修改内容   : 新生成函数

*****************************************************************************/
void vos_freepidreginfomem(vos_u32 ulfid);

/*****************************************************************************
 函 数 名  : vos_getsavepidinfobufheadforfid
 功能描述  : 获取指定FID模块的PID信息缓存首地址,调用接口务必保证FID正确
 输入参数  : vos_u32 ulfid
 输出参数  : 无
 返 回 值  : vos_pid_register_info_stru*
 
 修改历史      :
  1.日    期   : 2013年9月11日
    作    者   : Albort,Feng

    修改内容   : 新生成函数

*****************************************************************************/
vos_pid_register_info_stru* vos_getsavepidinfobufheadforfid(vos_u32 ulfid);

#ifdef __cplusplus
}
#endif


#endif
