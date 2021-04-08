/******************************************************************************

                  ��Ȩ���� (C), 2013-2023, �Ĵ��³�����Ƽ����޹�˾

 ******************************************************************************
  �� �� ��   : vos_mngmemory.c
  �� �� ��   : ����
  ��    ��   : Albort,Feng
  ��������   : 2013��8��26��
  ����޸�   :
  ��������   : �ڴ����ϵͳ����ʵ���ļ�
  �����б�   :
  �޸���ʷ   :
  1.��    ��   : 2013��8��26��
    ��    ��   : Albort,Feng
    �޸�����   : �����ļ�

******************************************************************************/
#include <stdlib.h>
#include <memory.h>
#include "vos_configure.h"
#include "vos_basictypedefine.h"
#include "vos_errorcode.h"
#include "vos_adaptsys.h"
#include "vos_mnglist.h"
#include "vos_log.h"
#include "vos_init.h"
#include "vos_mngmemory.h"

#define VOS_CRC_TABLE_LEN       (256)

//crc32���ɶ���ʽ
#define VOS_POLYNOMIAL        (0x04c11db7)

typedef struct
{
    vos_u32 ulmemaddr;
    vos_u32 ulmemsize;
    vos_u32 enmemtype;
    vos_u32 ulmemcrc;
    vos_u32 ulunchgcnt;
}vos_mng_mem_stru;

#ifdef __cplusplus
extern "C" {
#endif

#if VOS_SUPPORT_CHECK_MEM_MULTI_OP
STATIC void* g_pmutexformngmemlist = VOS_NULL;
#endif

STATIC vos_listid  g_ulmngsmemlistid = VOS_INVALID_U32;
STATIC vos_listid  g_ulmngdmemlistid = VOS_INVALID_U32;
STATIC vos_u32*    g_pulcrc32table   = VOS_NULL;
STATIC volatile vos_u32 g_ulusedmemsize = 0;
STATIC vos_multi_thread_para_stru* g_pstmemchkthdinfo = VOS_NULL;

/*****************************************************************************
 �� �� ��  : vos_initcrc32table
 ��������  : ��ʼ��crc32��
 �������  : void
 �������  : ��
 �� �� ֵ  : vos_u32
 
 �޸���ʷ      :
  1.��    ��   : 2013��8��28��
    ��    ��   : Albort,Feng

    �޸�����   : �����ɺ���

*****************************************************************************/
vos_u32 vos_initcrc32table(void)
{
    char  aucbuf[8];
    vos_u32 uldata;
    vos_u32 uloopout,uloopin,ulhighbit;
    vos_u32 ulreg     = 0;
    vos_u32 ulsumpoly = 0;

    /*����ÿ�ֽڵ�crc32���Լӿ�ÿ�μ���һ���ڴ��crcЧ��*/
    g_pulcrc32table = (vos_u32*)malloc(VOS_CRC_TABLE_LEN*sizeof(vos_u32));
    if (VOS_NULL == g_pulcrc32table)
    {
        RETURNERRNO(vos_mngmem_errorcode_base, vos_malloc_mem_err);
    }
    memset(g_pulcrc32table, 0x00, (VOS_CRC_TABLE_LEN*sizeof(vos_u32)));
    
    for (uldata = 0; VOS_CRC_TABLE_LEN > uldata; uldata++)
    {
        memset(aucbuf, 0x00, sizeof(aucbuf));
        memcpy(aucbuf, &uldata, sizeof(vos_u32));
        
        for(uloopout = 0; uloopout < 8; uloopout++)
        {
            // ���㲽��1
            ulsumpoly = ulreg & 0xff000000;
            for(uloopin = 0; uloopin < 8; uloopin++)
            {
                ulhighbit = ulsumpoly&0x80000000; // ����reg���λ
                ulsumpoly <<= 1;
                
                if(ulhighbit)
                {
                    ulsumpoly = (ulsumpoly ^ VOS_POLYNOMIAL);
                }
            }
            
            // ���㲽��2
            ulreg = (ulreg<<8) |aucbuf[uloopout];
            ulreg = ulreg ^ ulsumpoly;
        }

        *(g_pulcrc32table + uldata) = ulreg;
    }
    
    return VOS_OK;
}

/*****************************************************************************
 �� �� ��  : vos_calccrc32
 ��������  : ����һ���ڴ�32bit��CRCֵ
 �������  : char* pmemaddr
             vos_u32 ulmemsize
 �������  : vos_u32* poutcrc
 �� �� ֵ  : vos_u32
 
 �޸���ʷ      :
  1.��    ��   : 2013��8��28��
    ��    ��   : Albort,Feng

    �޸�����   : �����ɺ���

*****************************************************************************/
vos_u32 vos_calccrc32(char* pmemaddr, vos_u32 ulmemsize, vos_u32* poutcrc)
{
    vos_u32 ulcrc32    = 0;
    vos_u32 ultabcrc   = 0;
    vos_u32 ulmemindex = 0;
    vos_u32 ultmpval   = 0;
    char  ucmemval   = 0;

    if ((VOS_NULL == pmemaddr) || (VOS_NULL == poutcrc))
    {
        RETURNERRNO(vos_mngmem_errorcode_base, vos_para_null_err);
    }
    
    ulmemindex = 0;
    while (ulmemindex < ulmemsize)
    {
        /*Ҫ�Ƴ����ֽڵ�ֵ*/
        ultmpval = (ulcrc32 >> 24) & 0x000000ff;

        /*�����Ƴ����ֽڵ�ֵ���*/
        ultabcrc = *(g_pulcrc32table + ultmpval);

        /*���ƽ������ֽ�ֵ*/
        ucmemval = *(pmemaddr + ulmemindex);

        /*�����ƽ������ֽ�ֵ���ڼĴ���ĩ�ֽ���*/
        ulcrc32  = (ulcrc32 << 8) | ucmemval;

        /*���Ĵ���������ֵ����xor����*/
        ulcrc32  = ulcrc32 ^ ultabcrc;
        
        ulmemindex += 1;
    }

    *poutcrc = ulcrc32;
    
    return VOS_OK;
}

/*****************************************************************************
 �� �� ��  : vos_checkmemory
 ��������  : �ڴ��⣬δ��ʹ�õĶ�̬�ڴ��ڼ�������������ʱ�Զ��ͷŵ�
 �������  : void
 �������  : ��
 �� �� ֵ  : void
 
 �޸���ʷ      :
  1.��    ��   : 2013��8��28��
    ��    ��   : Albort,Feng

    �޸�����   : �����ɺ���

*****************************************************************************/
void vos_checkmemory(void)
{
    vos_u32 ulnewcrc      = 0;
    vos_u32 ultmpmemaddr  = 0;
    vos_u32 ultmpunchgcnt = 0;
    vos_u32 ulrunrslt     = VOS_OK;
    
    vos_mng_mem_stru* pmngmemnode = VOS_NULL;

    #ifdef VOS_SUPPORT_CHECK_MEM_MULTI_OP
    vos_obtainmutex(g_pmutexformngmemlist);
    #endif
    
    /*ÿ�μ�鶼������ͷ��ʼ*/
    (void)vos_listseek(g_ulmngdmemlistid, vos_list_offset_head);
    
    while (1)
    {
        (void)vos_getcurrentopnode(g_ulmngdmemlistid, (void**)&pmngmemnode);
        if (VOS_NULL == pmngmemnode)
        {
            break;
        }
        
        /*ֻ�ж�̬�ڴ�Ž����ڴ�й©���*/
        (void)vos_calccrc32((char*)pmngmemnode->ulmemaddr,
                                pmngmemnode->ulmemsize, &ulnewcrc);
        /*���μ����crc���ϴμ����crc��ͬ�����ÿ��ڴ��ڼ��������δʹ��*/
        if (ulnewcrc == pmngmemnode->ulmemcrc)
        {
            pmngmemnode->ulunchgcnt += 1;
        }else
        {
            pmngmemnode->ulmemcrc   = ulnewcrc;
            pmngmemnode->ulunchgcnt = 0;
        }

        ultmpmemaddr  = pmngmemnode->ulmemaddr;
        ultmpunchgcnt = pmngmemnode->ulunchgcnt;

        /*δ��ʹ�ü������ﵽ�����ͷŸ��ڴ�*/
        if (VOS_CHECK_LEAKMEM_UPPER_THR <= ultmpunchgcnt)
        {
            g_ulusedmemsize -= pmngmemnode->ulmemsize;

            ulrunrslt = vos_deletenode(g_ulmngdmemlistid, (vos_u32)ultmpmemaddr);
            if (VOS_OK == ulrunrslt)
            {//�ڴ�����м�¼���ڴ��ַ�Ų���Ұָ���ַ
                free((void*)ultmpmemaddr);
            }
        }

        pmngmemnode = VOS_NULL;
    }

    #ifdef VOS_SUPPORT_CHECK_MEM_MULTI_OP
    vos_releasemutex(g_pmutexformngmemlist);
    #endif
    
    return;
}

/*****************************************************************************
 �� �� ��  : vos_memcheckthreadenter
 ��������  : �ڴ����߳����
 �������  : void* pcheckperiod (��λ: ��)
 �������  : ��
 �� �� ֵ  : vos_u32
 
 �޸���ʷ      :
  1.��    ��   : 2013��9��3��
    ��    ��   : Albort,Feng

    �޸�����   : �����ɺ���

*****************************************************************************/
vos_u32 vos_memcheckthreadenter(void* pcheckperiod)
{
    vos_u32 ulcheckperiod = 0;  //�������,��λ: ����

    ulcheckperiod = (*((vos_u32*)pcheckperiod)) * 1000;
    
    vos_checkmemory();

    /*�߳�����ָ�����ں��ٴν����ڴ���*/
    vos_threadsleep(ulcheckperiod);

    return VOS_OK;
}

/*****************************************************************************
 �� �� ��  : vos_initmemorymd
 ��������  : ��ʼ���ڴ����ģ��
 �������  : void
 �������  : ��
 �� �� ֵ  : vos_u32 (VOS_OK:��ʼ���ɹ�,vos_err:��ʼ��ʧ��)
 
 �޸���ʷ      :
  1.��    ��   : 2013��8��27��
    ��    ��   : Albort,Feng

    �޸�����   : �����ɺ���

*****************************************************************************/
vos_u32 vos_initmemorymd(void)
{
    vos_u32 ulrunrslt   = VOS_OK;
    vos_u32 ulmemsize   = 0;
    vos_u32 ulchkperiod = VOS_CHECK_LEAKMEM_PERIOD; //�ڴ�������Ϊ3��

    g_ulusedmemsize = 0;
    
    #if VOS_SUPPORT_CHECK_MEM_MULTI_OP
    g_pmutexformngmemlist = vos_mallocmutex();
    
    ulrunrslt = vos_createmutex(g_pmutexformngmemlist);
    if (VOS_OK != ulrunrslt)
    {
        vos_freemutex(g_pmutexformngmemlist);
        g_pmutexformngmemlist = VOS_NULL;

        return ulrunrslt;
    }
    #endif

    ulmemsize = sizeof(vos_multi_thread_para_stru);
    
    g_pstmemchkthdinfo = (vos_multi_thread_para_stru*)malloc(ulmemsize);
    if (VOS_NULL == g_pstmemchkthdinfo)
    {
        #if VOS_SUPPORT_CHECK_MEM_MULTI_OP
        vos_deletemutex(g_pmutexformngmemlist);
        vos_freemutex(g_pmutexformngmemlist);
        g_pmutexformngmemlist = VOS_NULL;
        #endif
        
        RETURNERRNO(vos_mngmem_errorcode_base, vos_malloc_mem_err);
    }
    
    ulrunrslt = vos_createlist(sizeof(vos_mng_mem_stru), &g_ulmngsmemlistid);
    if (VOS_OK != ulrunrslt)
    {
        free(g_pstmemchkthdinfo);

        #if VOS_SUPPORT_CHECK_MEM_MULTI_OP
        vos_deletemutex(g_pmutexformngmemlist);
        vos_freemutex(g_pmutexformngmemlist);
        g_pmutexformngmemlist = VOS_NULL;
        #endif
        
        return ulrunrslt;
    }

    ulrunrslt = vos_createlist(sizeof(vos_mng_mem_stru), &g_ulmngdmemlistid);
    if (VOS_OK != ulrunrslt)
    {
        free(g_pstmemchkthdinfo);
        vos_deletelist(g_ulmngsmemlistid);

        #if VOS_SUPPORT_CHECK_MEM_MULTI_OP
        vos_deletemutex(g_pmutexformngmemlist);
        vos_freemutex(g_pmutexformngmemlist);
        g_pmutexformngmemlist = VOS_NULL;
        #endif
        
        return ulrunrslt;
    }

    ulrunrslt = vos_initcrc32table();
    if (VOS_OK != ulrunrslt)
    {
        free(g_pstmemchkthdinfo);
        vos_deletelist(g_ulmngsmemlistid);
        vos_deletelist(g_ulmngdmemlistid);

        #if VOS_SUPPORT_CHECK_MEM_MULTI_OP
        vos_deletemutex(g_pmutexformngmemlist);
        vos_freemutex(g_pmutexformngmemlist);
        g_pmutexformngmemlist = VOS_NULL;
        #endif
        
        return ulrunrslt;
    }
    
    g_pstmemchkthdinfo->pthrdfunc    = (vos_pthreadfunc)vos_memcheckthreadenter;
    g_pstmemchkthdinfo->ulthrdid     = 0;
    g_pstmemchkthdinfo->ulstacksize  = 0;  //�̶߳�ջ����Ĭ�ϴ�С
    g_pstmemchkthdinfo->ulthreadpri  = 50; //�߳����ȼ������еȼ�����
    g_pstmemchkthdinfo->pthrdinpara  = (void*)&ulchkperiod;
    g_pstmemchkthdinfo->arglen       = sizeof(ulchkperiod);
    g_pstmemchkthdinfo->eninitstatus = vos_thread_init_status_running;

    /*�����ڴ����߳�*/
    ulrunrslt = vos_createthread(g_pstmemchkthdinfo);
    if (VOS_OK != ulrunrslt)
    {
        free(g_pstmemchkthdinfo);
        vos_deletelist(g_ulmngsmemlistid);
        vos_deletelist(g_ulmngdmemlistid);

        #if VOS_SUPPORT_CHECK_MEM_MULTI_OP
        vos_deletemutex(g_pmutexformngmemlist);
        vos_freemutex(g_pmutexformngmemlist);
        g_pmutexformngmemlist = VOS_NULL;
        #endif
    }
    
    return ulrunrslt;
}

/*****************************************************************************
 �� �� ��  : vos_mallocmemory
 ��������  : �����ڴ�ӿ�
 �������  : vos_u32 ulmemsize
             vos_u32 enmemtype
 �������  : void** ppoutmem
 �� �� ֵ  : vos_u32
 
 �޸���ʷ      :
  1.��    ��   : 2013��8��27��
    ��    ��   : Albort,Feng

    �޸�����   : �����ɺ���

*****************************************************************************/
vos_u32 vos_mallocmemory(vos_u32 ulmemsize, vos_u32 enmemtype,
                         void** ppoutmem)
{
    vos_u32    ulrunrslt  = VOS_OK;
    void*  ptmpmem    = VOS_NULL;
    vos_listid ultmplstid = VOS_INVALID_U32;
    
    vos_mng_mem_stru* pmngmemnode = VOS_NULL;

    if (0 == ulmemsize)
    {
        RETURNERRNO(vos_mngmem_errorcode_base, vos_para_invalid_err);
    }
    
    if (vos_mem_type_butt <= enmemtype)
    {
        RETURNERRNO(vos_mngmem_errorcode_base, vos_no_support_mem_type_err);
    }
    
    if (VOS_NULL == ppoutmem)
    {
        RETURNERRNO(vos_mngmem_errorcode_base, vos_para_null_err);
    }

    *ppoutmem = VOS_NULL;

    if (0 != (ulmemsize & 0x00000003))
    {//4�ֽڶ��봦��
        ulmemsize += (4 - (ulmemsize & 0x00000003));
    }

    //���ڴ汣������
    ulmemsize += VOS_PROTECT_HEAP_MEM_LEN;

    ptmpmem = (void*)malloc(ulmemsize);
    if (VOS_NULL == ptmpmem)
    {
        RETURNERRNO(vos_mngmem_errorcode_base, vos_malloc_mem_err);
    }

    memset(ptmpmem, 0x00, ulmemsize);

    if (vos_mem_type_static == enmemtype)
    {
        ultmplstid = g_ulmngsmemlistid;
    }else
    {
        ultmplstid = g_ulmngdmemlistid;
    }

    #if VOS_SUPPORT_CHECK_MEM_MULTI_OP
    vos_obtainmutex(g_pmutexformngmemlist);
    #endif

    /*���ڴ����������Ϊ�������ڴ洴������ڵ�*/
    ulrunrslt = vos_mallocnode(ultmplstid, vos_list_sort_fifo_mode,
                               (vos_u32)ptmpmem, (void**)&pmngmemnode);
    if (VOS_OK != ulrunrslt)
    {
        #if VOS_SUPPORT_CHECK_MEM_MULTI_OP
        vos_releasemutex(g_pmutexformngmemlist);
        #endif
        
        free(ptmpmem);
        return ulrunrslt;
    }
    
    pmngmemnode->ulmemaddr  = (vos_u32)ptmpmem;
    pmngmemnode->enmemtype  = enmemtype;
    pmngmemnode->ulmemsize  = ulmemsize;
    pmngmemnode->ulunchgcnt = 0;
    
    vos_calccrc32((char*)ptmpmem, ulmemsize, &(pmngmemnode->ulmemcrc));

    *ppoutmem = ptmpmem;

    #if VOS_SUPPORT_CHECK_MEM_MULTI_OP
    vos_releasemutex(g_pmutexformngmemlist);
    #endif

    g_ulusedmemsize += ulmemsize;
    
    return VOS_OK;
}

/*****************************************************************************
 �� �� ��  : vos_freememory
 ��������  : �ͷ��ڴ�
 �������  : void* pmemaddr
 �������  : ��
 �� �� ֵ  : void
 
 �޸���ʷ      :
  1.��    ��   : 2013��8��28��
    ��    ��   : Albort,Feng

    �޸�����   : �����ɺ���

*****************************************************************************/
void vos_freememory(void* pmemaddr)
{
    vos_u32 ulrunrslt = VOS_OK;

    vos_mng_mem_stru* pstnode = VOS_NULL;

    #if VOS_SUPPORT_CHECK_MEM_MULTI_OP
    vos_obtainmutex(g_pmutexformngmemlist);
    #endif

    ulrunrslt = vos_getnode(g_ulmngdmemlistid, (vos_u32)pmemaddr,
                            (void**)(&pstnode));
    if ((VOS_OK == ulrunrslt) && (VOS_NULL != pstnode))
    {
        g_ulusedmemsize -= pstnode->ulmemsize;
    }

    pstnode = VOS_NULL;
    
    ulrunrslt = vos_deletenode(g_ulmngdmemlistid, (vos_u32)pmemaddr);
    if (VOS_OK == ulrunrslt)
    {//�ڴ�����м�¼�ĵ�ַ���ǺϷ��ĵ�ַ
        free(pmemaddr);

        #if VOS_SUPPORT_CHECK_MEM_MULTI_OP
        vos_releasemutex(g_pmutexformngmemlist);
        #endif
        
        return;
    }
    
    ulrunrslt = vos_getnode(g_ulmngsmemlistid, (vos_u32)pmemaddr,
                            (void**)(&pstnode));
    if ((VOS_OK == ulrunrslt) && (VOS_NULL != pstnode))
    {
        g_ulusedmemsize -= pstnode->ulmemsize;
    }

    pstnode = VOS_NULL;
    
    ulrunrslt = vos_deletenode(g_ulmngsmemlistid, (vos_u32)pmemaddr);
    if (VOS_OK == ulrunrslt)
    {//�ڴ�����м�¼�ĵ�ַ���ǺϷ��ĵ�ַ
        free(pmemaddr);

        #if VOS_SUPPORT_CHECK_MEM_MULTI_OP
        vos_releasemutex(g_pmutexformngmemlist);
        #endif
    
        return;
    }

    #if VOS_SUPPORT_CHECK_MEM_MULTI_OP
    vos_releasemutex(g_pmutexformngmemlist);
    #endif
    
    return;
}

/*****************************************************************************
 �� �� ��  : vos_getusedmemsize
 ��������  : ��ȡvos�������ʹ���ڴ��С
 �������  : void
 �������  : ��
 �� �� ֵ  : vos_u32
 ���ú���  : 
 ��������  : 
 
 �޸���ʷ      :
  1.��    ��   : 2014��8��7��
    ��    ��   : Albort,Feng
    �޸�����   : �����ɺ���

*****************************************************************************/
vos_u32 vos_getusedmemsize(void)
{
    return g_ulusedmemsize;
}

#ifdef __cplusplus
}
#endif


