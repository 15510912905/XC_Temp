#ifndef _app_debugtty_h_
#define _app_debugtty_h_

#include "vos_basictypedefine.h"

typedef vos_u32  (*pdebugcmdfunc)(char*, char*, char*, char*, char*, char*,
                                    char*, char*, char*, char*, char*, char*,
                                    char*, char*, char*, char*, char*, char*,
                                    char*, char*, char*, char*, char*, char*,
                                    char*, char*, char*, char*, char*, char*);
typedef void (*pshowcmdhelp)(void);


#ifdef __cplusplus
    extern "C" {
#endif

/*****************************************************************************
 函 数 名  : app_strnumToIntnum
 功能描述  : 字符串数字转换成整形数字
 输入参数  : char* pucstrnum
             vos_u32* pulintnum
 输出参数  : 无
 返 回 值  : vos_u32
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2014年7月31日
    作    者   : Albort,Feng
    修改内容   : 新生成函数

*****************************************************************************/
vos_u32 app_strnumToIntnum(char* pucstrnum, vos_u32* pulintnum);

/*****************************************************************************
 函 数 名  : app_registerDebugCmdfunc
 功能描述  : 注册维护函数
 输入参数  : char* pcmdfuncaddr
             pdebugcmdfunc pfuncaddr
             pshowcmdhelp phfuncaddr
             vos_u32 ulparanum
 输出参数  : 无
 返 回 值  : void
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2014年7月9日
    作    者   : Albort,Feng
    修改内容   : 新生成函数

*****************************************************************************/
void app_registerDebugCmdfunc(char* pucfuncname, pdebugcmdfunc pcmdfuncaddr,
                                pshowcmdhelp phfuncaddr, vos_u32 ulparanum);

/*****************************************************************************
 函 数 名  : app_initDebugttymd
 功能描述  : 维护模块初始化函数
 输入参数  : void
 输出参数  : 无
 返 回 值  : vos_u32
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2014年7月9日
    作    者   : Albort,Feng
    修改内容   : 新生成函数

*****************************************************************************/
vos_u32 app_initDebugttymd(void);

#ifdef __cplusplus
}
#endif

#endif
