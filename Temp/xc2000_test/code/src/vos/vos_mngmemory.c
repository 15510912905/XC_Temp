/******************************************************************************

                  版权所有 (C), 2013-2023, 四川新成生物科技有限公司

 ******************************************************************************
  文 件 名   : vos_mngmemory.c
  版 本 号   : 初稿
  作    者   : Albort,Feng
  生成日期   : 2013年8月26日
  最近修改   :
  功能描述   : 内存管理系统功能实现文件
  函数列表   :
  修改历史   :
  1.日    期   : 2013年8月26日
    作    者   : Albort,Feng
    修改内容   : 创建文件

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

//crc32生成多项式
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
 函 数 名  : vos_initcrc32table
 功能描述  : 初始化crc32表
 输入参数  : void
 输出参数  : 无
 返 回 值  : vos_u32
 
 修改历史      :
  1.日    期   : 2013年8月28日
    作    者   : Albort,Feng

    修改内容   : 新生成函数

*****************************************************************************/
vos_u32 vos_initcrc32table(void)
{
    char  aucbuf[8];
    vos_u32 uldata;
    vos_u32 uloopout,uloopin,ulhighbit;
    vos_u32 ulreg     = 0;
    vos_u32 ulsumpoly = 0;

    /*创建每字节的crc32表以加快每次计算一块内存的crc效率*/
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
            // 计算步骤1
            ulsumpoly = ulreg & 0xff000000;
            for(uloopin = 0; uloopin < 8; uloopin++)
            {
                ulhighbit = ulsumpoly&0x80000000; // 测试reg最高位
                ulsumpoly <<= 1;
                
                if(ulhighbit)
                {
                    ulsumpoly = (ulsumpoly ^ VOS_POLYNOMIAL);
                }
            }
            
            // 计算步骤2
            ulreg = (ulreg<<8) |aucbuf[uloopout];
            ulreg = ulreg ^ ulsumpoly;
        }

        *(g_pulcrc32table + uldata) = ulreg;
    }
    
    return VOS_OK;
}

/*****************************************************************************
 函 数 名  : vos_calccrc32
 功能描述  : 计算一块内存32bit的CRC值
 输入参数  : char* pmemaddr
             vos_u32 ulmemsize
 输出参数  : vos_u32* poutcrc
 返 回 值  : vos_u32
 
 修改历史      :
  1.日    期   : 2013年8月28日
    作    者   : Albort,Feng

    修改内容   : 新生成函数

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
        /*要移出的字节的值*/
        ultmpval = (ulcrc32 >> 24) & 0x000000ff;

        /*根据移出的字节的值查表*/
        ultabcrc = *(g_pulcrc32table + ultmpval);

        /*新移进来的字节值*/
        ucmemval = *(pmemaddr + ulmemindex);

        /*将新移进来的字节值添在寄存器末字节中*/
        ulcrc32  = (ulcrc32 << 8) | ucmemval;

        /*将寄存器与查出的值进行xor运算*/
        ulcrc32  = ulcrc32 ^ ultabcrc;
        
        ulmemindex += 1;
    }

    *poutcrc = ulcrc32;
    
    return VOS_OK;
}

/*****************************************************************************
 函 数 名  : vos_checkmemory
 功能描述  : 内存检测，未被使用的动态内存在检测次数到达上限时自动释放掉
 输入参数  : void
 输出参数  : 无
 返 回 值  : void
 
 修改历史      :
  1.日    期   : 2013年8月28日
    作    者   : Albort,Feng

    修改内容   : 新生成函数

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
    
    /*每次检查都从链表头开始*/
    (void)vos_listseek(g_ulmngdmemlistid, vos_list_offset_head);
    
    while (1)
    {
        (void)vos_getcurrentopnode(g_ulmngdmemlistid, (void**)&pmngmemnode);
        if (VOS_NULL == pmngmemnode)
        {
            break;
        }
        
        /*只有动态内存才进行内存泄漏检测*/
        (void)vos_calccrc32((char*)pmngmemnode->ulmemaddr,
                                pmngmemnode->ulmemsize, &ulnewcrc);
        /*本次计算的crc与上次计算的crc相同则表面该快内存在检测周期内未使用*/
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

        /*未被使用检测次数达到上限释放该内存*/
        if (VOS_CHECK_LEAKMEM_UPPER_THR <= ultmpunchgcnt)
        {
            g_ulusedmemsize -= pmngmemnode->ulmemsize;

            ulrunrslt = vos_deletenode(g_ulmngdmemlistid, (vos_u32)ultmpmemaddr);
            if (VOS_OK == ulrunrslt)
            {//内存管理中记录的内存地址才不是野指针地址
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
 函 数 名  : vos_memcheckthreadenter
 功能描述  : 内存检查线程入口
 输入参数  : void* pcheckperiod (单位: 秒)
 输出参数  : 无
 返 回 值  : vos_u32
 
 修改历史      :
  1.日    期   : 2013年9月3日
    作    者   : Albort,Feng

    修改内容   : 新生成函数

*****************************************************************************/
vos_u32 vos_memcheckthreadenter(void* pcheckperiod)
{
    vos_u32 ulcheckperiod = 0;  //检查周期,单位: 毫秒

    ulcheckperiod = (*((vos_u32*)pcheckperiod)) * 1000;
    
    vos_checkmemory();

    /*线程休眠指定周期后再次进行内存检查*/
    vos_threadsleep(ulcheckperiod);

    return VOS_OK;
}

/*****************************************************************************
 函 数 名  : vos_initmemorymd
 功能描述  : 初始化内存管理模块
 输入参数  : void
 输出参数  : 无
 返 回 值  : vos_u32 (VOS_OK:初始化成功,vos_err:初始化失败)
 
 修改历史      :
  1.日    期   : 2013年8月27日
    作    者   : Albort,Feng

    修改内容   : 新生成函数

*****************************************************************************/
vos_u32 vos_initmemorymd(void)
{
    vos_u32 ulrunrslt   = VOS_OK;
    vos_u32 ulmemsize   = 0;
    vos_u32 ulchkperiod = VOS_CHECK_LEAKMEM_PERIOD; //内存检查周期为3秒

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
    g_pstmemchkthdinfo->ulstacksize  = 0;  //线程堆栈采用默认大小
    g_pstmemchkthdinfo->ulthreadpri  = 50; //线程优先级采用中等即正常
    g_pstmemchkthdinfo->pthrdinpara  = (void*)&ulchkperiod;
    g_pstmemchkthdinfo->arglen       = sizeof(ulchkperiod);
    g_pstmemchkthdinfo->eninitstatus = vos_thread_init_status_running;

    /*创建内存检查线程*/
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
 函 数 名  : vos_mallocmemory
 功能描述  : 申请内存接口
 输入参数  : vos_u32 ulmemsize
             vos_u32 enmemtype
 输出参数  : void** ppoutmem
 返 回 值  : vos_u32
 
 修改历史      :
  1.日    期   : 2013年8月27日
    作    者   : Albort,Feng

    修改内容   : 新生成函数

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
    {//4字节对齐处理
        ulmemsize += (4 - (ulmemsize & 0x00000003));
    }

    //对内存保护长度
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

    /*在内存管理链表中为新申请内存创建管理节点*/
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
 函 数 名  : vos_freememory
 功能描述  : 释放内存
 输入参数  : void* pmemaddr
 输出参数  : 无
 返 回 值  : void
 
 修改历史      :
  1.日    期   : 2013年8月28日
    作    者   : Albort,Feng

    修改内容   : 新生成函数

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
    {//内存管理中记录的地址才是合法的地址
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
    {//内存管理中记录的地址才是合法的地址
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
 函 数 名  : vos_getusedmemsize
 功能描述  : 获取vos管理的已使用内存大小
 输入参数  : void
 输出参数  : 无
 返 回 值  : vos_u32
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2014年8月7日
    作    者   : Albort,Feng
    修改内容   : 新生成函数

*****************************************************************************/
vos_u32 vos_getusedmemsize(void)
{
    return g_ulusedmemsize;
}

#ifdef __cplusplus
}
#endif


