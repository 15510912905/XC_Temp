#ifndef _vos_socketcommu_h_
#define _vos_socketcommu_h_        
/******************************************************************************

                  版权所有 (C), 2013-2023, 四川新成生物科技有限公司

 ******************************************************************************
  文 件 名   : vos_selfsocketcommu.h
  版 本 号   : 初稿
  作    者   : Albort,Feng
  生成日期   : 2013年11月20日
  最近修改   :
  功能描述   : vos消息socket通信头文件
  函数列表   :
  修改历史   :
  1.日    期   : 2013年11月20日
    作    者   : Albort,Feng
    修改内容   : 创建文件

******************************************************************************/
#include "vos_configure.h"
#include "vos_basictypedefine.h"

#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************************
 函 数 名  : vos_disconnectsocket
 功能描述  : 断开socket连接
 输入参数  : vos_u32 ulsocketipaddr
 输出参数  : 无
 返 回 值  : vos_u32
 
 修改历史      :
  1.日    期   : 2014年2月26日
    作    者   : Albort,Feng

    修改内容   : 新生成函数

*****************************************************************************/
vos_u32 vos_disconnectsocket();

vos_u32 vos_receiveclientsockthdenter(void* ppara);
#ifdef __cplusplus
}
#endif

#endif

