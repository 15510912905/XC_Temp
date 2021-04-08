/******************************************************************************

                  ��Ȩ���� (C), 2013-2023, �Ĵ��³�����Ƽ����޹�˾

 ******************************************************************************
  �� �� ��   : vos_messagecenter.c
  �� �� ��   : ����
  ��    ��   : Albort,Feng
  ��������   : 2013��9��5��
  ����޸�   :
  ��������   : ƽ̨��Ϣ���Ĺ���ʵ���ļ�
  �����б�   :
  �޸���ʷ   :
  1.��    ��   : 2013��9��5��
    ��    ��   : Albort,Feng
    �޸�����   : �����ļ�

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

//��Ϣ����ring buffer�ṹ
typedef struct
{
    vos_u32 ulfid;            //��ring buffer���ڵ�FID
    vos_u32 ulrmailboxindex;  //������id
    vos_u32 ulwmailboxindex;  //д����id
    vos_msg_header_stru* pringbuf[VOS_MAX_RING_BUF_PER_PID];
}vos_mailbox_ringbuffer_stru;

//��Ϣ����ring buffer����ṹ
typedef struct
{
    vos_mailbox_ringbuffer_stru* apstmngringbuf[VOS_MAX_SUPPORT_FID_NUM];
}vos_mng_mailbox_ringbuffer_stru;

//��Ϣ�ڴ��ַ����ṹ
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
 �� �� ��  : vos_initmsgcentermd
 ��������  : ��Ϣ����ģ���ʼ���ӿ�
 �������  : void
 �������  : ��
 �� �� ֵ  : vos_u32
 
 �޸���ʷ      :
  1.��    ��   : 2013��9��5��
    ��    ��   : Albort,Feng

    �޸�����   : �����ɺ���

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
 �� �� ��  : vos_createmsgmailbox
 ��������  : ����FIDΪ��FIDģ�鴴����Ϣ��������
 �������  : vos_u32 ulfid
 �������  : ��
 �� �� ֵ  : vos_u32
 
 �޸���ʷ      :
  1.��    ��   : 2013��9��6��
    ��    ��   : Albort,Feng

    �޸�����   : �����ɺ���

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
    {//�Ѿ����������ظ�����
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
 �� �� ��  : vos_destroymsgmailbox
 ��������  : �ݻ�����
 �������  : vos_u32 ulfid
 �������  : ��
 �� �� ֵ  : void
 
 �޸���ʷ      :
  1.��    ��   : 2013��9��17��
    ��    ��   : Albort,Feng

    �޸�����   : �����ɺ���

*****************************************************************************/
void vos_destroymsgmailbox(vos_u32 ulfid)
{
    vos_u32 ulrunrslt = VOS_OK;
    vos_u32 ulindex   = VOS_INVALID_U32;
    vos_u32 ulloopvar = 0;

    /*���FID�Ϸ���,���Ϸ���˵��������Ϣ��Ҫ���͵���λ��*/
    ulrunrslt = vos_checkfidvalidity(ulfid);
    if (VOS_OK != ulrunrslt)
    {
        return;
    }

    ulindex = vos_fidmaptoindex(ulfid);
    if (VOS_NULL == g_pstmngringbuf->apstmngringbuf[ulindex])
    {//�Ѿ��ͷ������ظ��ͷ�
        return;
    }

    /*�ͷ�����ǰ������ڵ���Ϣ�ַ���ȥ*/
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
 �� �� ��  : vos_savemsgbuf
 ��������  : ������Ϣ����
 �������  : vos_u32 ulfid
             vos_msg_header_stru* pmsgbuf
 �������  : ��
 �� �� ֵ  : vos_u32
 
 �޸���ʷ      :
  1.��    ��   : 2013��9��16��
    ��    ��   : Albort,Feng

    �޸�����   : �����ɺ���

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
        WRITE_ERR_LOG("Fid=%lu��Ϣ������\n", pstmailbox->ulfid);
        vos_msg_header_stru* pTmpMsg = NULL;
        vos_u32 ulrunrslt = vos_receivemsg(pstmailbox->ulfid, &pTmpMsg);
        while ((VOS_OK == ulrunrslt) && (NULL != pTmpMsg))
        {
            WRITE_ERR_LOG("��Ϣ�������ݣ�srcpid=%lu,dstpid=%lu,msgtype=%lu\n", pTmpMsg->ulsrcpid, pTmpMsg->uldstpid, pTmpMsg->usmsgtype);
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
 �� �� ��  : vos_getspslocationbyaddr
 ��������  : ���ݵ�ַ��ȡ��¼��Ϣ��ַ��λ��
 �������  : vos_msg_header_stru*   pstaddr
             vos_msg_header_stru*** pppoutlocation
 �������  : ��
 �� �� ֵ  : void
 
 �޸���ʷ      :
  1.��    ��   : 2013��9��18��
    ��    ��   : Albort,Feng

    �޸�����   : �����ɺ���

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
 �� �� ��  : vos_recordmsgbuf
 ��������  : ��¼�������Ϣ�ڴ��ַ
 �������  : vos_msg_header_stru* pmsgbuf
 �������  : ��
 �� �� ֵ  : vos_u32
 
 �޸���ʷ      :
  1.��    ��   : 2013��9��17��
    ��    ��   : Albort,Feng

    �޸�����   : �����ɺ���

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
 �� �� ��  : vos_deletemsgrecord
 ��������  : ����Ϣ����ɾ��
 �������  : vos_msg_header_stru* pmsgbuf
 �������  : ��
 �� �� ֵ  : void
 
 �޸���ʷ      :
  1.��    ��   : 2013��9��16��
    ��    ��   : Albort,Feng

    �޸�����   : �����ɺ���

*****************************************************************************/
void vos_deletemsgrecord(vos_msg_header_stru* pmsgbuf)
{
    vos_msg_header_stru** ppstlocation = VOS_NULL;
    
    vos_obtainmutex(g_pmutexformngmsgbuflist);

    /*��ȡpmsgbuf����λ��*/
    vos_getspslocationbyaddr(pmsgbuf, &ppstlocation);

    if (VOS_NULL != ppstlocation)
    {
        *ppstlocation = VOS_NULL;
    }
    
    vos_releasemutex(g_pmutexformngmsgbuflist);
}

/*****************************************************************************
 �� �� ��  : vos_checkmsgbufvalid
 ��������  : �����Ϣ�ڴ�Ϸ���
 �������  : vos_msg_header_stru* pmsgbuf
 �������  : ��
 �� �� ֵ  : vos_u32
 
 �޸���ʷ      :
  1.��    ��   : 2013��9��16��
    ��    ��   : Albort,Feng

    �޸�����   : �����ɺ���

*****************************************************************************/
vos_u32 vos_checkmsgbufvalid(vos_msg_header_stru* pmsgbuf)
{
    vos_msg_header_stru** ppstlocation = VOS_NULL;

    /*��ȡpmsgbuf����λ��*/
    vos_getspslocationbyaddr(pmsgbuf, &ppstlocation);

    if (VOS_NULL == ppstlocation)
    {//��Ϣ�ڴ治��ͨ����Ϣ���Ľӿ������
        RETURNERRNO(vos_msgcenter_errorcode_base, vos_msgbuf_invalid_error);
    }
    
    return VOS_OK;
}

/*****************************************************************************
 �� �� ��  : vos_mallocmsgbuffer
 ��������  : ���뷢����Ϣ�Ļ��棬��Ϣ�����߲��ù��ĸ��ڴ���ͷţ���Ϣ�ڴ���
             ƽ̨�Լ��ͷ�
 �������  : vos_u32 ulmsglength (��Ϣ���Ȱ�����Ϣͷ)   
 �������  : void** ppoutbuf
 �� �� ֵ  : vos_u32
 
 �޸���ʷ      :
  1.��    ��   : 2013��9��5��
    ��    ��   : Albort,Feng

    �޸�����   : �����ɺ���

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
 �� �� ��  : vos_freemsgbuffer
 ��������  : �ͷ���Ϣ�ڴ棬ringbuf�е�λ������ɽ�����Ϣ����ѱ����
 �������  : vos_msg_header_stru* pmsgbuf
 �������  : ��
 �� �� ֵ  : vos_u32
 
 �޸���ʷ      :
  1.��    ��   : 2013��9��6��
    ��    ��   : Albort,Feng

    �޸�����   : �����ɺ���

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
    
    /*ɾ����Ϣ�������ṹ�еļ�¼*/
    (void)vos_deletemsgrecord(pmsgbuf);

    /*�����е���Ϣ�ڷַ�ʱ�ѱ����,����ֱ��ɾ��������Ϣ���ڴ漴��*/
    vos_freememory((void*)pmsgbuf);
    
    return VOS_OK;
}

/*****************************************************************************
 �� �� ��  : vos_mountmsgtorcvmailbox
 ��������  : ����Ϣ�ҽӵ�Ŀ�Ľ�����Ϣ������
 �������  : vos_msg_header_stru* pstmsgbuf
 �������  : ��
 �� �� ֵ  : vos_u32
 
 �޸���ʷ      :
  1.��    ��   : 2013��11��21��
    ��    ��   : Albort,Feng

    �޸�����   : �����ɺ���

*****************************************************************************/
vos_u32 vos_mountmsgtorcvmailbox(vos_msg_header_stru* pstmsgbuf)
{
    vos_u32  ulrunrslt = VOS_OK;
    vos_u32  ulfid     = VOS_INVALID_FID;
    vos_u32  ulpid     = VOS_INVALID_PID;

    vos_multi_thread_para_stru* pstthdinfo = VOS_NULL;
    
    ulpid = pstmsgbuf->uldstpid;
    ulfid = vos_pidmaptofid(ulpid);

    /*���FID�Ϸ���,���Ϸ���˵��������Ϣ��Ҫ���͵���λ��*/
    ulrunrslt = vos_checkfidvalidity(ulfid);
    if (VOS_OK != ulrunrslt)
    {//Ŀ��PID�Ƿ�
        /*ɾ��������Ϣ���ڴ�*/
        vos_freemsgbuffer(pstmsgbuf);
        RETURNERRNO(vos_msgcenter_errorcode_base, vos_dst_pid_error);
    }

    /*��ȡĿ��FIDģ���߳���Ϣ*/
    ulrunrslt = vos_getfidthreadinfo(ulfid, &pstthdinfo);
    if (VOS_OK != ulrunrslt)
    {
        /*ɾ��������Ϣ���ڴ�*/
        vos_freememory((void*)pstmsgbuf);
        return ulrunrslt;
    }

    ulrunrslt = vos_savemsgbuf(ulfid, pstmsgbuf);
    if (VOS_OK != ulrunrslt)
    {
        /*ɾ��������Ϣ���ڴ�*/
        vos_freememory((void*)pstmsgbuf);
        return ulrunrslt;
    }
    
    /*��Ŀ��FIDģ�鷢���̻߳�����Ϣ*/
    ulrunrslt = vos_resumethread(pstthdinfo->ulthrdid);
    if (VOS_OK != ulrunrslt)
    {
        return ulrunrslt;
    }
   
    return VOS_OK;
}

/*****************************************************************************
 �� �� ��  : vos_sendmsg
 ��������  : ��Ϣ���ͽӿڣ����ӿڷ��͵���Ϣ���������vos_mallocmsgbuffer����
             ���������򲻸��ڷ���
 �������  : vos_msg_header_stru* pstmsgbuf
 �������  : ��
 �� �� ֵ  : vos_u32
 
 �޸���ʷ      :
  1.��    ��   : 2013��9��6��
    ��    ��   : Albort,Feng

    �޸�����   : �����ɺ���

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
        //��Ϣ�ڴ治�Ϸ�,����ͨ����Ϣ�������뵽��
        WRITE_ERR_LOG("vos_checkmsgbufvalid error=%lu\n", ulrunrslt);
        return ulrunrslt;
    }
  
    if (bOuter)
    {
        ulrunrslt = vos_socket_connect_error;
        //Ŀ��IP��ַ��ԴIP��ַ��ͬ˵������ͬһ��ϵͳ�ڲ�ģ��֮�����Ϣ/*����socket���ͽӿ�*/
        if (g_pMonitor->IsConnect())
        {
            ulrunrslt = vos_msgsocketsend(pstmsgbuf);
            if (VOS_OK != ulrunrslt)
            {
                WRITE_ERR_LOG("vos_msgsocketsend error=%lx, msgtype=%lu\n", ulrunrslt, pstmsgbuf->usmsgtype);
            }
        }
        /*ɾ��������Ϣ���ڴ�*/
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
    
    //ע��:����ķ�֧������return��������ߵ�����ط���������ζ���Ҫ�ͷŵ��ڴ���
    vos_freemsgbuffer(pstmsgbuf);    
    return VOS_OK;
}

/*****************************************************************************
 �� �� ��  : vos_receivemsg
 ��������  : FIDģ�������Ϣ�ӿ�
 �������  : vos_u32 ulfid
 �������  : vos_msg_header_stru** ppoutmsg
 �� �� ֵ  : vos_u32
 
 �޸���ʷ      :
  1.��    ��   : 2013��9��6��
    ��    ��   : Albort,Feng

    �޸�����   : �����ɺ���

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

    /*��Ϣ���յ��������Ϣ����*/
    pstmailbox->pringbuf[pstmailbox->ulrmailboxindex] = VOS_NULL;

    pstmailbox->ulrmailboxindex += 1;

    return VOS_OK;
}

#ifdef __cplusplus
}
#endif
