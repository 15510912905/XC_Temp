/******************************************************************************

                  版权所有 (C), 2013-2023, 四川新成生物科技有限公司

 ******************************************************************************
  文 件 名   : vos_messagecenter.c
  版 本 号   : 初稿
  作    者   : Albort,Feng
  生成日期   : 2013年9月5日
  最近修改   :
  功能描述   : 平台消息中心功能实现文件
  函数列表   :
  修改历史   :
  1.日    期   : 2013年9月5日
    作    者   : Albort,Feng
    修改内容   : 创建文件

******************************************************************************/
#include <stdlib.h>
#include <memory.h>
#include "vos_configure.h"
#include "vos_basictypedefine.h"
#include "vos_errorcode.h"
#include "vos_adaptsys.h"
#include "vos_mngmemory.h"
#include "vos_selffid.h"
#include "vos_selfpid.h"
#include "vos_selfmessagecenter.h"
#include "vos_selfsocketcommu.h"
#include "vos_messagecenter.h"
#include "vos_init.h"
#include "vos_log.h"
#include "socketmonitor.h"

#define VOS_MAX_RING_BUF_PER_PID (VOS_MAX_PID_NUM_PER_FID << 1)
#define VOS_MAX_MNG_MSG_BUF_ADDR_LEN ((VOS_MAX_RING_BUF_PER_PID) * (VOS_MAX_SUPPORT_FID_NUM))
#define VOS_MAX_SUBDEVIDE_SCH_MSGBUF_RCD (16)

//消息邮箱ring buffer结构
typedef struct
{
    vos_u32 ulfid;            //该ring buffer属于的FID
    vos_u32 ulrmailboxindex;  //读邮箱id
    vos_u32 ulwmailboxindex;  //写邮箱id
    vos_msg_header_stru* pringbuf[VOS_MAX_RING_BUF_PER_PID];
}vos_mailbox_ringbuffer_stru;

//消息邮箱ring buffer管理结构
typedef struct
{
    vos_mailbox_ringbuffer_stru* apstmngringbuf[VOS_MAX_SUPPORT_FID_NUM];
}vos_mng_mailbox_ringbuffer_stru;

//消息内存地址管理结构
typedef struct
{
    vos_msg_header_stru* amsgbufaddr[VOS_MAX_MNG_MSG_BUF_ADDR_LEN];
}vos_mng_msgbuf_addr_stru;

#ifdef __cplusplus
extern "C" {
#endif

STATIC void* g_pmutexformngmsgbuflist = VOS_NULL;
STATIC vos_mng_mailbox_ringbuffer_stru* g_pstmngringbuf    = VOS_NULL;
STATIC vos_mng_msgbuf_addr_stru*        g_pstmngmsgbufaddr = VOS_NULL;

/*****************************************************************************
 函 数 名  : vos_initmsgcentermd
 功能描述  : 消息中心模块初始化接口
 输入参数  : void
 输出参数  : 无
 返 回 值  : vos_u32
 
 修改历史      :
  1.日    期   : 2013年9月5日
    作    者   : Albort,Feng

    修改内容   : 新生成函数

*****************************************************************************/
vos_u32 vos_initmsgcentermd(void)
{
    vos_u32 ulrunrslt = VOS_OK;
    vos_u32 ulmemsize = 0;
    vos_u32 ulloopvar = 0;

    ulmemsize = sizeof(vos_mng_mailbox_ringbuffer_stru);
    ulrunrslt = vos_mallocmemory(ulmemsize, vos_mem_type_static,
                                 (void**)&g_pstmngringbuf);
    if (VOS_OK != ulrunrslt)
    {
        return ulrunrslt;
    }

    for (ulloopvar = 0; VOS_MAX_SUPPORT_FID_NUM > ulloopvar; ulloopvar++)
    {
        g_pstmngringbuf->apstmngringbuf[ulloopvar] = VOS_NULL;
    }

    ulmemsize = sizeof(vos_mng_msgbuf_addr_stru);
    ulrunrslt = vos_mallocmemory(ulmemsize, vos_mem_type_static,
                                 (void**)&g_pstmngmsgbufaddr);
    if (VOS_OK != ulrunrslt)
    {
        vos_freememory((void*)g_pstmngringbuf);
        g_pstmngringbuf = VOS_NULL;
        
        return ulrunrslt;
    }

    for (ulloopvar = 0; VOS_MAX_MNG_MSG_BUF_ADDR_LEN > ulloopvar; ulloopvar++)
    {
        g_pstmngmsgbufaddr->amsgbufaddr[ulloopvar] = VOS_NULL;
    }

    g_pmutexformngmsgbuflist = vos_mallocmutex();
    
    ulrunrslt = vos_createmutex(g_pmutexformngmsgbuflist);
    if (VOS_OK != ulrunrslt)
    {
        vos_freemutex(g_pmutexformngmsgbuflist);
        vos_freememory((void*)g_pstmngmsgbufaddr);
        vos_freememory((void*)g_pstmngringbuf);

        g_pmutexformngmsgbuflist = VOS_NULL;
        g_pstmngmsgbufaddr       = VOS_NULL;
        g_pstmngringbuf          = VOS_NULL;

        return ulrunrslt;
    }
    
    return VOS_OK;
}

/*****************************************************************************
 函 数 名  : vos_createmsgmailbox
 功能描述  : 根据FID为该FID模块创建消息邮箱链表
 输入参数  : vos_u32 ulfid
 输出参数  : 无
 返 回 值  : vos_u32
 
 修改历史      :
  1.日    期   : 2013年9月6日
    作    者   : Albort,Feng

    修改内容   : 新生成函数

*****************************************************************************/
vos_u32 vos_createmsgmailbox(vos_u32 ulfid)
{
    vos_u32 ulrunrslt = VOS_OK;
    vos_u32 ulindex   = VOS_INVALID_U32;
    vos_u32 ulloopvar = 0;

    vos_mailbox_ringbuffer_stru* pstbuf = VOS_NULL;

    ulindex = vos_fidmaptoindex(ulfid);
    if (VOS_MAX_SUPPORT_FID_NUM <= ulindex)
    {
        RETURNERRNO(vos_msgcenter_errorcode_base, vos_array_index_error);
    }

    vos_obtainmutex(g_pmutexformngmsgbuflist);

    if (VOS_NULL != g_pstmngringbuf->apstmngringbuf[ulindex])
    {//已经创建无需重复创建
        vos_releasemutex(g_pmutexformngmsgbuflist);
        return VOS_OK;
    }

    vos_releasemutex(g_pmutexformngmsgbuflist);

    ulrunrslt = vos_mallocmemory(sizeof(vos_mailbox_ringbuffer_stru),
                                 vos_mem_type_static, (void**)&pstbuf);
    if (VOS_OK != ulrunrslt)
    {
        return ulrunrslt;
    }
    
    pstbuf->ulfid           = ulfid;
    pstbuf->ulrmailboxindex = 0;
    pstbuf->ulwmailboxindex = 0;

    for (ulloopvar = 0; VOS_MAX_RING_BUF_PER_PID > ulloopvar; ulloopvar++)
    {
        pstbuf->pringbuf[ulloopvar] = VOS_NULL;
    }

    vos_obtainmutex(g_pmutexformngmsgbuflist);
    
    g_pstmngringbuf->apstmngringbuf[ulindex] = pstbuf;

    vos_releasemutex(g_pmutexformngmsgbuflist);
    
    return VOS_OK;
}

/*****************************************************************************
 函 数 名  : vos_destroymsgmailbox
 功能描述  : 摧毁邮箱
 输入参数  : vos_u32 ulfid
 输出参数  : 无
 返 回 值  : void
 
 修改历史      :
  1.日    期   : 2013年9月17日
    作    者   : Albort,Feng

    修改内容   : 新生成函数

*****************************************************************************/
void vos_destroymsgmailbox(vos_u32 ulfid)
{
    vos_u32 ulrunrslt = VOS_OK;
    vos_u32 ulindex   = VOS_INVALID_U32;
    vos_u32 ulloopvar = 0;

    /*检查FID合法性,不合法则说明本条消息需要发送到中位机*/
    ulrunrslt = vos_checkfidvalidity(ulfid);
    if (VOS_OK != ulrunrslt)
    {
        return;
    }

    ulindex = vos_fidmaptoindex(ulfid);
    if (VOS_NULL == g_pstmngringbuf->apstmngringbuf[ulindex])
    {//已经释放无需重复释放
        return;
    }

    /*释放邮箱前将其存在的消息分发出去*/
    for (ulloopvar = 0; VOS_MAX_RING_BUF_PER_PID > ulloopvar; ulloopvar++)
    {
        if (VOS_NULL != g_pstmngringbuf->apstmngringbuf[ulindex]->pringbuf[ulloopvar])
        {
            (void)vos_deliverymsgtopid(g_pstmngringbuf->apstmngringbuf[ulindex]->pringbuf[ulloopvar]);
            (void)vos_freemsgbuffer(g_pstmngringbuf->apstmngringbuf[ulindex]->pringbuf[ulloopvar]);

            g_pstmngringbuf->apstmngringbuf[ulindex]->pringbuf[ulloopvar] = VOS_NULL;
        }
    }

    vos_freememory((void*)(g_pstmngringbuf->apstmngringbuf[ulindex]));

    g_pstmngringbuf->apstmngringbuf[ulindex] = VOS_NULL;

    return;
}

/*****************************************************************************
 函 数 名  : vos_savemsgbuf
 功能描述  : 保存消息缓存
 输入参数  : vos_u32 ulfid
             vos_msg_header_stru* pmsgbuf
 输出参数  : 无
 返 回 值  : vos_u32
 
 修改历史      :
  1.日    期   : 2013年9月16日
    作    者   : Albort,Feng

    修改内容   : 新生成函数

*****************************************************************************/
vos_u32 vos_savemsgbuf(vos_u32 ulfid, vos_msg_header_stru* pmsgbuf)
{
    vos_u32 ulindex = 0;

    vos_mailbox_ringbuffer_stru* pstmailbox = VOS_NULL;

    ulindex    = vos_fidmaptoindex(ulfid);
    pstmailbox = g_pstmngringbuf->apstmngringbuf[ulindex];
    if (VOS_NULL == pstmailbox)
    {
        RETURNERRNO(vos_msgcenter_errorcode_base, vos_uncrt_mailbox_error);
    }
    
    if (ulfid != pstmailbox->ulfid)
    {
        RETURNERRNO(vos_msgcenter_errorcode_base, vos_mailbox_confusion_error);
    }

    vos_obtainmutex(g_pmutexformngmsgbuflist);

    if (VOS_MAX_RING_BUF_PER_PID <= pstmailbox->ulwmailboxindex)
    {
        pstmailbox->ulwmailboxindex = 0;
    }
    
    if (VOS_NULL != pstmailbox->pringbuf[pstmailbox->ulwmailboxindex])
    {
        vos_releasemutex(g_pmutexformngmsgbuflist);
        WRITE_ERR_LOG("Fid=%lu消息队列满\n", pstmailbox->ulfid);
        vos_msg_header_stru* pTmpMsg = NULL;
        vos_u32 ulrunrslt = vos_receivemsg(pstmailbox->ulfid, &pTmpMsg);
        while ((VOS_OK == ulrunrslt) && (NULL != pTmpMsg))
        {
            WRITE_ERR_LOG("消息队列数据：srcpid=%lu,dstpid=%lu,msgtype=%lu\n", pTmpMsg->ulsrcpid, pTmpMsg->uldstpid, pTmpMsg->usmsgtype);
            vos_freemsgbuffer(pTmpMsg);
            pTmpMsg = NULL;
            ulrunrslt = vos_receivemsg(pstmailbox->ulfid, &pTmpMsg);
        }
        RETURNERRNO(vos_msgcenter_errorcode_base, vos_mailbox_full_error);
    }
    if (SPid_Log != pmsgbuf->uldstpid)
    {
        WRITE_TRACE_LOG(" SendMsg WIndex=%lu,pid:%lu-%lu,msgtype=%lu\n", pstmailbox->ulwmailboxindex, pmsgbuf->ulsrcpid, pmsgbuf->uldstpid, pmsgbuf->usmsgtype);
    }
    pstmailbox->pringbuf[pstmailbox->ulwmailboxindex] = pmsgbuf;
    pstmailbox->ulwmailboxindex                      += 1;
    if (VOS_MAX_RING_BUF_PER_PID <= pstmailbox->ulwmailboxindex)
    {
        pstmailbox->ulwmailboxindex = 0;
    }

    vos_releasemutex(g_pmutexformngmsgbuflist);

    return VOS_OK;
}

/*****************************************************************************
 函 数 名  : vos_getspslocationbyaddr
 功能描述  : 根据地址获取记录消息地址的位置
 输入参数  : vos_msg_header_stru*   pstaddr
             vos_msg_header_stru*** pppoutlocation
 输出参数  : 无
 返 回 值  : void
 
 修改历史      :
  1.日    期   : 2013年9月18日
    作    者   : Albort,Feng

    修改内容   : 新生成函数

*****************************************************************************/
void vos_getspslocationbyaddr(vos_msg_header_stru*   pstaddr,
                                  vos_msg_header_stru*** pppoutlocation )
{
    vos_u32 uloutloop      = 0;
    vos_u32 ulinloop       = 0;
    vos_u32 ulindex        = 0;
    vos_u32 ulsixteenthlen = (VOS_MAX_MNG_MSG_BUF_ADDR_LEN >> 4);

    *pppoutlocation = VOS_NULL;
    
    for (uloutloop = 0; ulsixteenthlen > uloutloop; uloutloop++)
    {
        for (ulinloop = 0; VOS_MAX_SUBDEVIDE_SCH_MSGBUF_RCD > ulinloop; ulinloop++)
        {
            ulindex = ulsixteenthlen * ulinloop + uloutloop;
            
            if (pstaddr == g_pstmngmsgbufaddr->amsgbufaddr[ulindex])
            {
                *pppoutlocation = &(g_pstmngmsgbufaddr->amsgbufaddr[ulindex]);
                break;
            }
        }

        if (VOS_MAX_SUBDEVIDE_SCH_MSGBUF_RCD > ulinloop)
        {
            break;
        }
    }
    
    return;
}

/*****************************************************************************
 函 数 名  : vos_recordmsgbuf
 功能描述  : 记录申请的消息内存地址
 输入参数  : vos_msg_header_stru* pmsgbuf
 输出参数  : 无
 返 回 值  : vos_u32
 
 修改历史      :
  1.日    期   : 2013年9月17日
    作    者   : Albort,Feng

    修改内容   : 新生成函数

*****************************************************************************/
vos_u32 vos_recordmsgbuf(vos_msg_header_stru* pmsgbuf)
{
    vos_msg_header_stru** ppstlocation = VOS_NULL;

    vos_obtainmutex(g_pmutexformngmsgbuflist);

    vos_getspslocationbyaddr(VOS_NULL, &ppstlocation);

    if (VOS_NULL == ppstlocation)
    {
        vos_releasemutex(g_pmutexformngmsgbuflist);
    
        RETURNERRNO(vos_msgcenter_errorcode_base, vos_mailbox_full_error);
    }

    *ppstlocation = pmsgbuf;

    vos_releasemutex(g_pmutexformngmsgbuflist);

    return VOS_OK;
}

/*****************************************************************************
 函 数 名  : vos_deletemsgrecord
 功能描述  : 将消息缓存删除
 输入参数  : vos_msg_header_stru* pmsgbuf
 输出参数  : 无
 返 回 值  : void
 
 修改历史      :
  1.日    期   : 2013年9月16日
    作    者   : Albort,Feng

    修改内容   : 新生成函数

*****************************************************************************/
void vos_deletemsgrecord(vos_msg_header_stru* pmsgbuf)
{
    vos_msg_header_stru** ppstlocation = VOS_NULL;
    
    vos_obtainmutex(g_pmutexformngmsgbuflist);

    /*获取pmsgbuf所在位置*/
    vos_getspslocationbyaddr(pmsgbuf, &ppstlocation);

    if (VOS_NULL != ppstlocation)
    {
        *ppstlocation = VOS_NULL;
    }
    
    vos_releasemutex(g_pmutexformngmsgbuflist);
}

/*****************************************************************************
 函 数 名  : vos_checkmsgbufvalid
 功能描述  : 检测消息内存合法性
 输入参数  : vos_msg_header_stru* pmsgbuf
 输出参数  : 无
 返 回 值  : vos_u32
 
 修改历史      :
  1.日    期   : 2013年9月16日
    作    者   : Albort,Feng

    修改内容   : 新生成函数

*****************************************************************************/
vos_u32 vos_checkmsgbufvalid(vos_msg_header_stru* pmsgbuf)
{
    vos_msg_header_stru** ppstlocation = VOS_NULL;

    /*获取pmsgbuf所在位置*/
    vos_getspslocationbyaddr(pmsgbuf, &ppstlocation);

    if (VOS_NULL == ppstlocation)
    {//消息内存不是通过消息中心接口申请的
        RETURNERRNO(vos_msgcenter_errorcode_base, vos_msgbuf_invalid_error);
    }
    
    return VOS_OK;
}

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
vos_u32 vos_mallocmsgbuffer(vos_u32 ulmsglength, void** ppoutbuf)
{
    vos_u32 ulrunrslt = VOS_OK;
   
    vos_msg_header_stru* psttmpmem = VOS_NULL;

    if (0 == ulmsglength)
    {
        RETURNERRNO(vos_msgcenter_errorcode_base, vos_para_invalid_err);
    }
    if (VOS_NULL == ppoutbuf)
    {
        RETURNERRNO(vos_msgcenter_errorcode_base, vos_para_null_err);
    }

    *ppoutbuf = VOS_NULL;
    
    ulrunrslt = vos_mallocmemory(ulmsglength, vos_mem_type_static,
                                 (void**)&psttmpmem);
    if (VOS_OK != ulrunrslt)
    {
        return ulrunrslt;
    }

    ulrunrslt = vos_recordmsgbuf(psttmpmem);
    if (VOS_OK != ulrunrslt)
    {
        vos_freememory((void*)psttmpmem);
        return ulrunrslt;
    }

    psttmpmem->uldstpid = VOS_INVALID_PID;
    psttmpmem->ulsrcpid = VOS_INVALID_PID;
    
    *ppoutbuf = (void*)psttmpmem;
    
    return VOS_OK;
}

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
vos_u32 vos_freemsgbuffer(vos_msg_header_stru* pmsgbuf)
{
    vos_u32 ulrunrslt = VOS_OK;

    if (VOS_NULL == pmsgbuf)
    {
        RETURNERRNO(vos_msgcenter_errorcode_base, vos_para_null_err);
    }

    ulrunrslt = vos_checkmsgbufvalid(pmsgbuf);
    if (VOS_OK != ulrunrslt)
    {
        return ulrunrslt;
    }
    
    /*删除消息缓存管理结构中的记录*/
    (void)vos_deletemsgrecord(pmsgbuf);

    /*邮箱中的消息在分发时已被清空,这里直接删除发送消息的内存即可*/
    vos_freememory((void*)pmsgbuf);
    
    return VOS_OK;
}

/*****************************************************************************
 函 数 名  : vos_mountmsgtorcvmailbox
 功能描述  : 将消息挂接到目的接收消息邮箱中
 输入参数  : vos_msg_header_stru* pstmsgbuf
 输出参数  : 无
 返 回 值  : vos_u32
 
 修改历史      :
  1.日    期   : 2013年11月21日
    作    者   : Albort,Feng

    修改内容   : 新生成函数

*****************************************************************************/
vos_u32 vos_mountmsgtorcvmailbox(vos_msg_header_stru* pstmsgbuf)
{
    vos_u32  ulrunrslt = VOS_OK;
    vos_u32  ulfid     = VOS_INVALID_FID;
    vos_u32  ulpid     = VOS_INVALID_PID;

    vos_multi_thread_para_stru* pstthdinfo = VOS_NULL;
    
    ulpid = pstmsgbuf->uldstpid;
    ulfid = vos_pidmaptofid(ulpid);

    /*检查FID合法性,不合法则说明本条消息需要发送到中位机*/
    ulrunrslt = vos_checkfidvalidity(ulfid);
    if (VOS_OK != ulrunrslt)
    {//目的PID非法
        /*删除发送消息的内存*/
        vos_freemsgbuffer(pstmsgbuf);
        RETURNERRNO(vos_msgcenter_errorcode_base, vos_dst_pid_error);
    }

    /*获取目标FID模块线程信息*/
    ulrunrslt = vos_getfidthreadinfo(ulfid, &pstthdinfo);
    if (VOS_OK != ulrunrslt)
    {
        /*删除发送消息的内存*/
        vos_freememory((void*)pstmsgbuf);
        return ulrunrslt;
    }

    ulrunrslt = vos_savemsgbuf(ulfid, pstmsgbuf);
    if (VOS_OK != ulrunrslt)
    {
        /*删除发送消息的内存*/
        vos_freememory((void*)pstmsgbuf);
        return ulrunrslt;
    }
    
    /*向目标FID模块发送线程唤醒消息*/
    ulrunrslt = vos_resumethread(pstthdinfo->ulthrdid);
    if (VOS_OK != ulrunrslt)
    {
        return ulrunrslt;
    }
   
    return VOS_OK;
}

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
vos_u32 vos_sendmsg(vos_msg_header_stru* pstmsgbuf, bool bOuter)
{
    vos_u32 ulrunrslt = VOS_OK;
    if (VOS_NULL == pstmsgbuf)
    {
        WRITE_ERR_LOG("vos_sendmsg msg is null\n");
        RETURNERRNO(vos_msgcenter_errorcode_base, vos_para_null_err);
    }

    ulrunrslt = vos_checkmsgbufvalid(pstmsgbuf);
    if (VOS_OK != ulrunrslt)
    {
        //消息内存不合法,不是通过消息中心申请到的
        WRITE_ERR_LOG("vos_checkmsgbufvalid error=%lu\n", ulrunrslt);
        return ulrunrslt;
    }
  
    if (bOuter)
    {
        ulrunrslt = vos_socket_connect_error;
        //目的IP地址与源IP地址不同说明不是同一个系统内部模块之间的消息/*调用socket发送接口*/
        if (g_pMonitor->IsConnect())
        {
            ulrunrslt = vos_msgsocketsend(pstmsgbuf);
            if (VOS_OK != ulrunrslt)
            {
                WRITE_ERR_LOG("vos_msgsocketsend error=%lx, msgtype=%lu\n", ulrunrslt, pstmsgbuf->usmsgtype);
            }
        }
        /*删除发送消息的内存*/
        vos_freemsgbuffer(pstmsgbuf);
        return ulrunrslt;
    }
    if (VOS_INVALID_PID != pstmsgbuf->uldstpid)
    {
        ulrunrslt = vos_mountmsgtorcvmailbox(pstmsgbuf);
        if (VOS_OK != ulrunrslt)
        {   
            WRITE_ERR_LOG("vos_mountmsgtorcvmailbox failed(0x%x),pid=%lu,msg=%lu\n", ulrunrslt, pstmsgbuf->uldstpid, pstmsgbuf->usmsgtype);
            vos_freemsgbuffer(pstmsgbuf);
            return ulrunrslt;
        }

        return VOS_OK;
    }
    
    //注意:上面的分支各自有return处理，如果走到这个地方，无论如何都需要释放掉内存了
    vos_freemsgbuffer(pstmsgbuf);    
    return VOS_OK;
}

/*****************************************************************************
 函 数 名  : vos_receivemsg
 功能描述  : FID模块接收消息接口
 输入参数  : vos_u32 ulfid
 输出参数  : vos_msg_header_stru** ppoutmsg
 返 回 值  : vos_u32
 
 修改历史      :
  1.日    期   : 2013年9月6日
    作    者   : Albort,Feng

    修改内容   : 新生成函数

*****************************************************************************/
vos_u32 vos_receivemsg(vos_u32 ulfid, vos_msg_header_stru** ppoutmsg)
{
    vos_u32 ulrunrslt   = VOS_OK;
    vos_u32 ulindex     = 0;
    vos_mailbox_ringbuffer_stru* pstmailbox = VOS_NULL;

    ulrunrslt = vos_checkfidvalidity(ulfid);
    if (VOS_OK != ulrunrslt)
    {
        WRITE_ERR_LOG("call vos_checkfidvalidity failed(0x%x)\r\n", ulrunrslt);
        RETURNERRNO(vos_msgcenter_errorcode_base, vos_fid_invalid_error);
    }
    
    if (VOS_NULL == ppoutmsg)
    {
        RETURNERRNO(vos_msgcenter_errorcode_base, vos_para_null_err);
    }

    *ppoutmsg = VOS_NULL;
    
    ulindex    = vos_fidmaptoindex(ulfid);
    pstmailbox = g_pstmngringbuf->apstmngringbuf[ulindex];
    if (VOS_NULL == pstmailbox)
    {
        RETURNERRNO(vos_msgcenter_errorcode_base, vos_uncrt_mailbox_error);
    }
    
    if (ulfid != pstmailbox->ulfid)
    {
        RETURNERRNO(vos_msgcenter_errorcode_base, vos_mailbox_confusion_error);
    }

    vos_obtainmutex(g_pmutexformngmsgbuflist);

    if (VOS_MAX_RING_BUF_PER_PID <= pstmailbox->ulrmailboxindex)
    {
        pstmailbox->ulrmailboxindex = 0;
    }
    
    *ppoutmsg = pstmailbox->pringbuf[pstmailbox->ulrmailboxindex];

    vos_releasemutex(g_pmutexformngmsgbuflist);
    
    if (VOS_NULL == *ppoutmsg)
    {
        return VOS_OK;
    }

    /*消息接收到后清空消息邮箱*/
    pstmailbox->pringbuf[pstmailbox->ulrmailboxindex] = VOS_NULL;

    pstmailbox->ulrmailboxindex += 1;

    return VOS_OK;
}

#ifdef __cplusplus
}
#endif
