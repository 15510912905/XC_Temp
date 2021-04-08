#ifndef _vos_selfsocketcommu_h_
#define _vos_selfsocketcommu_h_        
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
#include "vos_basictypedefine.h"
#include "vos_messagecenter.h"

#define VOS_MAX_SOCK_RCV_LEN   (1024)

//定义释放socket消息结构
typedef struct
{
    vos_msg_header;
    vos_u32 ulipaddr;
}vos_release_socket_req_msg_stru;

#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************************
 函 数 名  : vos_msgsocketsend
 功能描述  : vos消息socket发送接口
 输入参数  : vos_msg_header_stru* pstvosmsg
 输出参数  : 无
 返 回 值  : vos_u32
 
 修改历史      :
  1.日    期   : 2013年11月25日
    作    者   : Albort,Feng

    修改内容   : 新生成函数

*****************************************************************************/
vos_u32 vos_msgsocketsend(vos_msg_header_stru* pstvosmsg);

/*****************************************************************************
 函 数 名  : vos_initsockcommumd
 功能描述  : socket通信模块初始化接口
 输入参数  : void
 输出参数  : 无
 返 回 值  : vos_u32
 
 修改历史      :
  1.日    期   : 2013年11月20日
    作    者   : Albort,Feng

    修改内容   : 新生成函数

*****************************************************************************/
vos_u32 vos_initsockcommumd();


#ifdef __cplusplus
}
#endif

#endif

