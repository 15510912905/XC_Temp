#ifndef _vos_messagecenter_h_
#define _vos_messagecenter_h_   
/******************************************************************************

                  版权所有 (C), 2013-2023, 四川新成生物科技有限公司

 ******************************************************************************
  文 件 名   : vos_messagecenter.h
  版 本 号   : 初稿
  作    者   : Albort,Feng
  生成日期   : 2013年9月5日
  最近修改   :
  功能描述   : 平台消息中心头文件
  函数列表   :
  修改历史   :
  1.日    期   : 2013年9月5日
    作    者   : Albort,Feng
    修改内容   : 创建文件

******************************************************************************/
#include "vos_basictypedefine.h"

//消息中心统一定义的消息头
#define vos_msg_header    \
    vos_u32 usmsgloadlen; \
    vos_u32 ulsrcpid; \
    vos_u32 uldstpid; \
    vos_u32 usmsgtype; \
    vos_u32 usrsv

#pragma warning(disable:4200)
//vos系统消息结构定义
typedef struct
{
    vos_u32 usmsgloadlen;            //消息中心统一定义的消息头
    vos_u32 ulsrcpid;
    vos_u32 uldstpid;
    vos_u32 usmsgtype;    
    vos_u32 usrsv;
    char  aucmsgload[0];     //消息负载
}vos_msg_header_stru;
#pragma warning(default:)

#define vos_calcvosmsgldlen(msgstru) (sizeof(msgstru) - sizeof(vos_msg_header_stru))

#ifdef __cplusplus
extern "C" {
#endif
/*****************************************************************************
 函 数 名  : vos_mallocmsgbuffer
 功能描述  : 申请发送消息的缓存，消息发送者不用关心该内存的释放，消息内存由
             平台自己释放
 输入参数  : vos_u32 ulmsglength (消息长度包括消息头)   
 输出参数  : void** ppoutbuf
 返 回 值  : vos_u32
 
 修改历史      :
  1.日    期   : 2013年9月5日
    作    者   : Albort,Feng

    修改内容   : 新生成函数

*****************************************************************************/
vos_u32 vos_mallocmsgbuffer(vos_u32 ulmsglength, void** ppoutbuf);

/*****************************************************************************
 函 数 名  : vos_freemsgbuffer
 功能描述  : 释放消息内存，ringbuf中的位置在完成接收消息后就已被清空
 输入参数  : vos_msg_header_stru* pmsgbuf
 输出参数  : 无
 返 回 值  : vos_u32
 
 修改历史      :
  1.日    期   : 2013年9月6日
    作    者   : Albort,Feng

    修改内容   : 新生成函数

*****************************************************************************/
vos_u32 vos_freemsgbuffer(vos_msg_header_stru* pmsgbuf);

/*****************************************************************************
 函 数 名  : vos_sendmsg
 功能描述  : 消息发送接口，本接口发送的消息缓存必须由vos_mallocmsgbuffer申请
             而来，否则不给于发送
 输入参数  : vos_msg_header_stru* pstmsgbuf
 输出参数  : 无
 返 回 值  : vos_u32
 
 修改历史      :
  1.日    期   : 2013年9月6日
    作    者   : Albort,Feng

    修改内容   : 新生成函数

*****************************************************************************/
vos_u32 vos_sendmsg(vos_msg_header_stru* pstmsgbuf, bool bOuter = false);

#ifdef __cplusplus
}
#endif

#endif

