#ifndef _app_debugcmd_h_
#define _app_debugcmd_h_
/******************************************************************************

                  版权所有 (C), 2001-2100, 四川省新健康成股份有限公司


 ******************************************************************************
  文 件 名   : app_debugcmd.h
  版 本 号   : 初稿
  作    者   : Albort,Feng
  生成日期   : 2014年7月9日
  最近修改   :
  功能描述   : 中下位机调试命令实现头文件
  函数列表   :
  修改历史   :
  1.日    期   : 2014年7月9日
    作    者   : Albort,Feng
    修改内容   : 创建文件

******************************************************************************/


#ifdef __cplusplus
    extern "C" {
#endif
/*****************************************************************************
 函 数 名  : app_initdebugcmdmd
 功能描述  : 调试命令模块初始化函数
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
vos_u32 app_initdebugcmdmd(void);

vos_u32 app_tasktest(char* tasknum);

void app_sendMidRunStateToDwnmach(vos_u32 endwnmachname, cmd_simplecmd_stru* cmdinfo);

vos_u32 app_updateapp(char* pucserverip);

vos_u32 app_debugequipmentquery(char * para0, char * para1);
#ifdef __cplusplus
}
#endif

#endif
