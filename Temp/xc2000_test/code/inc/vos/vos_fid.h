#ifndef _vos_fid_h_
#define _vos_fid_h_       
/******************************************************************************

                  版权所有 (C), 2013-2023, 四川新成生物科技有限公司

 ******************************************************************************
  文 件 名   : vos_fid.h
  版 本 号   : 初稿
  作    者   : Albort,Feng
  生成日期   : 2013年9月5日
  最近修改   :
  功能描述   : 平台FID模块头文件
  函数列表   :
  修改历史   :
  1.日    期   : 2013年9月5日
    作    者   : Albort,Feng
    修改内容   : 创建文件

******************************************************************************/
#include "vos_basictypedefine.h"

#ifdef __cplusplus
extern "C" {
#endif

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
vos_u32 vos_registerfid(vos_u32 ulfidpri, vos_u32 ulfid);

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
void vos_utreleasefid(vos_u32 ulfid);

//获取当前线程的FID
vos_u32 vos_getfid();

#ifdef __cplusplus
}
#endif

#endif