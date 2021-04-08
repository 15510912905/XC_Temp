/******************************************************************************

                  版权所有 (C), 2013-2023, 四川新成生物科技有限公司

 ******************************************************************************
  文 件 名   : vos_mnglist.c
  版 本 号   : 初稿
  作    者   : Albort,Feng
  生成日期   : 2013年8月26日
  最近修改   :
  功能描述   : 通用链表管理功能实现文件
  函数列表   :
  修改历史   :
  1.日    期   : 2013年8月26日
    作    者   : Albort,Feng
    修改内容   : 创建文件

******************************************************************************/
#include <stdlib.h>
#include <memory.h>
#include "vos_basictypedefine.h"
#include "vos_configure.h"
#include "vos_errorcode.h"
#include "vos_adaptsys.h"
#include "vos_log.h"
#include "vos_init.h"
#include "vos_mnglist.h"

//通用链表节点结构
typedef struct _vos_listnode_
{
    struct _vos_listnode_* pprenode;
    struct _vos_listnode_* pnextnode;

    vos_u32 ulnodekeyword; //用于链表排序的关键字
    
    char aucloadbuf[0];
}vos_listnode_stru;

//通用链表结构
typedef struct
{
    vos_listnode_stru* phead;
    vos_listnode_stru* ptail;

    vos_listnode_stru* pcurnode;

    vos_u32 ulnodesize;
    vos_u32 ullistlength;
}vos_list_stru;

#define vos_mem_map_to_node_addr(addr) ((vos_listnode_stru*)(((char*)(addr))\
                                      - (sizeof(vos_u32) + sizeof(char*) * 2)))


#ifdef __cplusplus
extern "C" {
#endif

STATIC vos_list_stru* g_apmnglist[VOS_MAX_SUPPORT_LIST_NUM] = {VOS_NULL};

/*****************************************************************************
 函 数 名  : vos_initlistmd
 功能描述  : 链表模块初始化接口
 输入参数  : void
 输出参数  : 无
 返 回 值  : vos_u32 (VOS_OK:初始化成功,vos_err:初始化失败)
 
 修改历史      :
  1.日    期   : 2013年8月26日
    作    者   : Albort,Feng

    修改内容   : 新生成函数

*****************************************************************************/
vos_u32 vos_initlistmd(void)
{
    vos_u32 ulloopnum = 0;
    
    while (VOS_MAX_SUPPORT_LIST_NUM > ulloopnum)
    {
        g_apmnglist[ulloopnum] = VOS_NULL;
        
        ulloopnum += 1;
    }
    
    return VOS_OK;
}

/*****************************************************************************
 函 数 名  : vos_createlist
 功能描述  : 创建一个新链表
 输入参数  : vos_u32 ullistnodesize
 输出参数  : vos_listid* poutid
 返 回 值  : vos_u32
 
 修改历史      :
  1.日    期   : 2013年8月26日
    作    者   : Albort,Feng

    修改内容   : 新生成函数

*****************************************************************************/
vos_u32 vos_createlist(vos_u32 ullistnodesize, vos_listid* poutid)
{
    vos_u32 ulloopnum = 0;
    vos_u32 ulmemsize = 0;
    
    vos_list_stru* ptmpmem = VOS_NULL;
    
    /*begin:入参有效性检测*/
    if (0 == ullistnodesize)
    {
        RETURNERRNO(vos_mnglist_errorcode_base, vos_para_invalid_err);
    }
    if (VOS_NULL == poutid)
    {
        RETURNERRNO(vos_mnglist_errorcode_base, vos_para_null_err);
    }
    /*end:入参有效性检测*/
    
    while (ulloopnum < VOS_MAX_SUPPORT_LIST_NUM)
    {
        if (VOS_NULL == g_apmnglist[ulloopnum])
        {
            break;
        }

        ulloopnum += 1;
    }

    *poutid = VOS_INVALID_U32;
    
    if (VOS_MAX_SUPPORT_LIST_NUM <= ulloopnum)
    {
        RETURNERRNO(vos_mnglist_errorcode_base, vos_list_num_max_err);
    }

    ptmpmem = (vos_list_stru*)malloc(sizeof(vos_list_stru));
    if (VOS_NULL == ptmpmem)
    {
        RETURNERRNO(vos_mnglist_errorcode_base, vos_malloc_mem_err);
    }
    
    ulmemsize = ullistnodesize + sizeof(vos_listnode_stru);
    if (0 != (ullistnodesize & 0x00000003))
    {
        ulmemsize += (4 - (ullistnodesize & 0x00000003)); //系统按照4字节内存对齐
    }

    ptmpmem->phead    = VOS_NULL;
    ptmpmem->ptail    = VOS_NULL;
    ptmpmem->pcurnode = VOS_NULL;

    ptmpmem->ulnodesize   = ulmemsize;
    ptmpmem->ullistlength = 0;
    
    g_apmnglist[ulloopnum] = ptmpmem;
    *poutid                = ulloopnum;
    
    return VOS_OK;
}

/*****************************************************************************
 函 数 名  : vos_getlistbyid
 功能描述  : 根据链表id得到链表
 输入参数  : vos_listid listid
 输出参数  : vos_list_stru** ppoutlist
 返 回 值  : vos_u32
 
 修改历史      :
  1.日    期   : 2013年8月26日
    作    者   : Albort,Feng

    修改内容   : 新生成函数

*****************************************************************************/
STATIC vos_u32 vos_getlistbyid(vos_listid listid, vos_list_stru** ppoutlist)
{
    if (VOS_MAX_SUPPORT_LIST_NUM <= listid)
    {
        RETURNERRNO(vos_mnglist_errorcode_base, vos_list_id_invalid_err);
    }
    if (VOS_NULL == ppoutlist)
    {
        RETURNERRNO(vos_mnglist_errorcode_base, vos_para_null_err);
    }
    
    if (VOS_NULL == g_apmnglist[listid])
    {
        RETURNERRNO(vos_mnglist_errorcode_base, vos_list_no_create_err);
    }
    
    *ppoutlist = g_apmnglist[listid];
    
    return VOS_OK;
}

/*****************************************************************************
 函 数 名  : vos_getnode
 功能描述  : 由关键字获取节点
 输入参数  : vos_listid listid
             vos_u32 ulkeyword
 输出参数  : void** ppout
 返 回 值  : vos_u32
 
 修改历史      :
  1.日    期   : 2013年8月26日
    作    者   : Albort,Feng

    修改内容   : 新生成函数

*****************************************************************************/
vos_u32 vos_getnode(vos_listid listid, vos_u32 ulkeyword, void** ppout)
{
    vos_u32 ulsearchstep   = 0;
    vos_u32 ulloopupperthd = 0;
    vos_u32 ulrunrslt      = VOS_OK;

    vos_list_stru*     plist    = VOS_NULL;
    vos_listnode_stru* pfrontmv = VOS_NULL;
    vos_listnode_stru* paftermv = VOS_NULL;
    vos_listnode_stru* pspsnode = VOS_NULL;

    if (VOS_NULL == ppout)
    {
        RETURNERRNO(vos_mnglist_errorcode_base, vos_para_null_err);
    }
    
    ulrunrslt = vos_getlistbyid(listid, &plist);
    if (VOS_OK != ulrunrslt)
    {
        return ulrunrslt;
    }

    /*保证链表长度为奇偶数时循环都正确*/
    ulloopupperthd = (plist->ullistlength >> 1)
                   + (plist->ullistlength & 0x00000001);
    
    paftermv = plist->phead;
    pfrontmv = plist->ptail;

    /*整个链表只用循环一半链表长度*/
    while (ulsearchstep < ulloopupperthd)
    {
        if (ulkeyword == paftermv->ulnodekeyword)
        {
            pspsnode = paftermv;
            break;
        }
        
        if (ulkeyword == pfrontmv->ulnodekeyword)
        {
            pspsnode = pfrontmv;
            break;
        }

        paftermv = paftermv->pnextnode;
        pfrontmv = pfrontmv->pprenode;

        ulsearchstep += 1;
    }

    *ppout = VOS_NULL;
    if (VOS_NULL == pspsnode)
    { 
        RETURNERRNO(vos_mnglist_errorcode_base, vos_list_no_sps_node_err);
    }
    
    *ppout = (void*)(pspsnode->aucloadbuf);
        
    return VOS_OK;
}

/*****************************************************************************
 函 数 名  : vos_getinsertpointerbykeyword
 功能描述  : 由插入节点关键字获取插入点
 输入参数  : vos_list_stru* plist
             vos_u32 ulinsertkeyword
 输出参数  : 无
 返 回 值  : vos_listnode_stru*
 
 修改历史      :
  1.日    期   : 2013年8月26日
    作    者   : Albort,Feng

    修改内容   : 新生成函数

*****************************************************************************/
STATIC vos_listnode_stru* vos_getinsertpointerbykeyword(vos_list_stru* plist,
                                                        vos_u32 ulinsertkeyword)
{
    vos_u32 ulsearchstep   = 0;
    vos_u32 ulloopupperthd = 0;
    
    vos_listnode_stru* pfrontmv = VOS_NULL;
    vos_listnode_stru* paftermv = VOS_NULL;
    vos_listnode_stru* pspsnode = VOS_NULL;

    if(0 == plist->ullistlength)
    {
        return VOS_NULL;
    }

    /*新插入节点关键字小于等于表头的关键字,在表头前插入新节点*/
    if (ulinsertkeyword <= plist->phead->ulnodekeyword)
    {
        return plist->phead;
    }

    /*新插入节点关键字大于等于表尾的关键字,在表尾后插入新节点*/
    if (ulinsertkeyword >= plist->ptail->ulnodekeyword)
    {
        return plist->ptail;
    }

    /*保证链表长度为奇偶数时循环都正确*/
    ulloopupperthd = (plist->ullistlength >> 1)
                   + (plist->ullistlength & 0x00000001);
    
    paftermv = plist->phead;
    pfrontmv = plist->ptail;

    /*整个链表只用循环一半链表长度*/
    while (ulsearchstep < ulloopupperthd)
    {
        if (ulinsertkeyword < paftermv->ulnodekeyword)
        {
            pspsnode = paftermv;
            break;
        }

        if (ulinsertkeyword > pfrontmv->ulnodekeyword)
        {
            pspsnode = pfrontmv;
            break;
        }
        
        paftermv = paftermv->pnextnode;
        pfrontmv = pfrontmv->pprenode;

        ulsearchstep += 1;
    }

    if (VOS_NULL == pspsnode)
    {
        if ((VOS_NULL != paftermv) && (ulinsertkeyword < paftermv->ulnodekeyword))
        {
            pspsnode = paftermv;
        }else if ((VOS_NULL != paftermv) && (ulinsertkeyword > paftermv->ulnodekeyword))
        {
            pspsnode = pfrontmv;
        }
    }
    
    return pspsnode;
}

/*****************************************************************************
 函 数 名  : vos_insertnodeafterspsnode
 功能描述  : 在指定节点后插入新的节点
 输入参数  : vos_list_stru* plist
             vos_listnode_stru* pspsnode
             vos_listnode_stru* pnewnode
 输出参数  : 无
 返 回 值  : void
 
 修改历史      :
  1.日    期   : 2013年8月26日
    作    者   : Albort,Feng

    修改内容   : 新生成函数

*****************************************************************************/
STATIC void vos_insertnodeafterspsnode(vos_list_stru* plist,
                                           vos_listnode_stru* pspsnode,
                                           vos_listnode_stru* pnewnode)
{
    pnewnode->pnextnode = pspsnode->pnextnode;
    pnewnode->pprenode  = pspsnode;

    if (VOS_NULL == pspsnode->pnextnode)
    {
        pspsnode->pnextnode = pnewnode;

        plist->ptail = pnewnode;

        return;
    }

    pspsnode->pnextnode->pprenode = pnewnode;
    pspsnode->pnextnode = pnewnode;
    
    return;
}

/*****************************************************************************
 函 数 名  : vos_insertnodefrontspsnode
 功能描述  : 在指定节点前插入新的节点
 输入参数  : vos_list_stru* plist
             vos_listnode_stru* pspsnode
             vos_listnode_stru* pnewnode
 输出参数  : 无
 返 回 值  : void
 
 修改历史      :
  1.日    期   : 2013年8月26日
    作    者   : Albort,Feng

    修改内容   : 新生成函数

*****************************************************************************/
STATIC void vos_insertnodefrontspsnode(vos_list_stru* plist,
                                           vos_listnode_stru* pspsnode,
                                           vos_listnode_stru* pnewnode)
{
    pnewnode->pprenode  = pspsnode->pprenode;
    pnewnode->pnextnode = pspsnode;

    if (VOS_NULL == pspsnode->pprenode)
    {
        pspsnode->pprenode = pnewnode;

        plist->phead = pnewnode;

        return;
    }

    pspsnode->pprenode->pnextnode = pnewnode;
    pspsnode->pprenode = pnewnode;
    
    return;
}

/*****************************************************************************
 函 数 名  : vos_insertnodebyorder
 功能描述  : 按序向链表插入节点
 输入参数  : vos_listid listid
             vos_listnode_stru* pnode
 输出参数  : 无
 返 回 值  : void
 
 修改历史      :
  1.日    期   : 2013年8月26日
    作    者   : Albort,Feng

    修改内容   : 新生成函数

*****************************************************************************/
STATIC void vos_insertnodebyorder(vos_list_stru* plist,
                                      vos_listnode_stru* pnode)
{
    vos_listnode_stru* pinsertpointer = VOS_NULL;

    if ((VOS_NULL == plist) || (VOS_NULL == pnode))
    {
        return;
    }

    plist->pcurnode = pnode;
    
    pnode->pnextnode = VOS_NULL;
    pnode->pprenode  = VOS_NULL;
    
    if (0 == plist->ullistlength)
    {
        plist->phead = pnode;
        plist->ptail = pnode;
        
        plist->ullistlength += 1;
        
        return;
    }
    
    pinsertpointer = vos_getinsertpointerbykeyword(plist, pnode->ulnodekeyword);
    if (VOS_NULL == pinsertpointer)
    {
        return;
    }
    
    /*在指定节点后插入新的节点*/
    if (pnode->ulnodekeyword > pinsertpointer->ulnodekeyword)
    {
        vos_insertnodeafterspsnode(plist, pinsertpointer, pnode);
    }else //在指定节点前插入新的节点
    {
        vos_insertnodefrontspsnode(plist, pinsertpointer, pnode);
    }

    plist->ullistlength += 1;
    
    return;
}

/*****************************************************************************
 函 数 名  : vos_insertnodebyfifo
 功能描述  : 按照先将先出的顺序插入节点，即在链表尾插入新节点
 输入参数  : vos_list_stru* plist
             vos_listnode_stru* pnode
 输出参数  : 无
 返 回 值  : void
 
 修改历史      :
  1.日    期   : 2013年9月6日
    作    者   : Albort,Feng

    修改内容   : 新生成函数

*****************************************************************************/
STATIC void vos_insertnodebyfifo(vos_list_stru* plist,
                                     vos_listnode_stru* pnode)
{
    if ((VOS_NULL == plist) || (VOS_NULL == pnode))
    {
        return;
    }

    plist->pcurnode = pnode;
    
    pnode->pnextnode = VOS_NULL;
    pnode->pprenode  = VOS_NULL;
    
    if (0 == plist->ullistlength)
    {
        plist->phead = pnode;
        plist->ptail = pnode;
        
        plist->ullistlength += 1;
        
        return;
    }

    pnode->pprenode = plist->ptail;
    
    plist->ptail->pnextnode = pnode;
    plist->ptail = pnode;

    plist->ullistlength += 1;
    
    return;
}

/*****************************************************************************
 函 数 名  : vos_insertnodebylifo
 功能描述  : 按照后进先出的方式插入新节点,即在链表头插入新节点
 输入参数  : 无
 输出参数  : 无
 返 回 值  : void
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2014年7月10日
    作    者   : Albort,Feng
    修改内容   : 新生成函数

*****************************************************************************/
STATIC void vos_insertnodebylifo(vos_list_stru* plist,
                                     vos_listnode_stru* pnode)
{
    if ((VOS_NULL == plist) || (VOS_NULL == pnode))
    {
        return;
    }

    plist->pcurnode = pnode;
    
    pnode->pnextnode = VOS_NULL;
    pnode->pprenode  = VOS_NULL;
    
    if (0 == plist->ullistlength)
    {
        plist->phead = pnode;
        plist->ptail = pnode;
        
        plist->ullistlength += 1;
        
        return;
    }

    pnode->pnextnode = plist->phead;
    
    plist->phead->pprenode = pnode;
    plist->phead = pnode;

    plist->ullistlength += 1;

    return;
}

/*****************************************************************************
 函 数 名  : vos_deletenode
 功能描述  : 删除由关键字指定的节点
 输入参数  : vos_listid listid
             vos_u32 ulkeyword
 输出参数  : 无
 返 回 值  : vos_u32
 
 修改历史      :
  1.日    期   : 2013年8月26日
    作    者   : Albort,Feng

    修改内容   : 新生成函数

*****************************************************************************/
vos_u32 vos_deletenode(vos_listid listid, vos_u32 ulkeyword)
{
    vos_u32 ulrunrslt = VOS_OK;
    char* puctmpbuf = VOS_NULL;
    
    vos_list_stru*     plist    = VOS_NULL;
    vos_listnode_stru* pspsnode = VOS_NULL;

    ulrunrslt = vos_getlistbyid(listid, &plist);
    if (VOS_OK != ulrunrslt)
    {
        return ulrunrslt;
    }
    
    (void)vos_getnode(listid, ulkeyword, (void**)&puctmpbuf);
    if (VOS_NULL == puctmpbuf)
    {
        RETURNERRNO(vos_mnglist_errorcode_base, vos_list_no_sps_node_err);
    }
    
    pspsnode = vos_mem_map_to_node_addr(puctmpbuf);

    if (1 == plist->ullistlength)
    {
        plist->pcurnode = VOS_NULL;
        plist->phead    = VOS_NULL;
        plist->ptail    = VOS_NULL;
        plist->ullistlength = 0;

        free(pspsnode);
        return VOS_OK;
    }

    plist->ullistlength -= 1;

    /*待删除节点为链表尾*/
    if (plist->ptail == pspsnode)
    {
        if (pspsnode == plist->pcurnode)
        {
            plist->pcurnode = pspsnode->pprenode;
        }
        
        plist->ptail = pspsnode->pprenode;

        plist->ptail->pnextnode = VOS_NULL;

        free(pspsnode);
        return VOS_OK;
    }

    if (pspsnode == plist->pcurnode)
    {
        plist->pcurnode = pspsnode->pnextnode;
    }

    /*待删除节点为链表头*/
    if (plist->phead == pspsnode)
    {
        plist->phead = pspsnode->pnextnode;
        
        plist->phead->pprenode = VOS_NULL;
        
        free(pspsnode);
        
        return VOS_OK;
    }
    
    /*待删除节点为中间节点*/
    pspsnode->pprenode->pnextnode = pspsnode->pnextnode;
    pspsnode->pnextnode->pprenode = pspsnode->pprenode;
    
    free(pspsnode);
    
    return VOS_OK;
}

/*****************************************************************************
 函 数 名  : vos_deletelist
 功能描述  : 删除整个链表
 输入参数  : vos_listid listid
 输出参数  : 无
 返 回 值  : vos_u32
 
 修改历史      :
  1.日    期   : 2013年8月27日
    作    者   : Albort,Feng

    修改内容   : 新生成函数

*****************************************************************************/
vos_u32 vos_deletelist(vos_listid listid)
{
    vos_listnode_stru* pdelnode = VOS_NULL;
    vos_listnode_stru* pmvnode  = VOS_NULL;

    if (VOS_MAX_SUPPORT_LIST_NUM <= listid)
    {
        RETURNERRNO(vos_mnglist_errorcode_base, vos_list_id_invalid_err);
    }
    
    if (VOS_NULL == g_apmnglist[listid])
    {
        RETURNERRNO(vos_mnglist_errorcode_base, vos_list_no_create_err);
    }

    pmvnode = g_apmnglist[listid]->phead;
    while(VOS_NULL != pmvnode)
    {
        pdelnode = pmvnode;
        pmvnode  = pmvnode->pnextnode;

        free(pdelnode);
    }
    
    free(g_apmnglist[listid]);
    g_apmnglist[listid] = VOS_NULL;
    
    return VOS_OK;
}

/*****************************************************************************
 函 数 名  : vos_mallocnode
 功能描述  : 申请一个链表节点
 输入参数  : vos_listid listid
             vos_u32 ensortmd  (非默认方式下调用者需要输入有效keyword)vos_list_sort_default_mode
             vos_u32 ulkeyword 
 输出参数  : void** ppoutmem
 返 回 值  : void*
 
 修改历史      :
  1.日    期   : 2013年8月26日
    作    者   : Albort,Feng

    修改内容   : 新生成函数

*****************************************************************************/
vos_u32 vos_mallocnode(vos_listid listid, vos_u32 ensortmd, vos_u32 ulkeyword,
                       void** ppoutmem)
{
    vos_u32 ulrunrslt = VOS_OK;
    
    vos_list_stru*     plist    = VOS_NULL;
    vos_listnode_stru* pnewnode = VOS_NULL;

    if (VOS_NULL == ppoutmem)
    {
        RETURNERRNO(vos_mnglist_errorcode_base, vos_para_null_err);
    }

    if (vos_list_sort_default_mode != ensortmd)
    {//非默认排序方式下检查指定关键字的结点是否已经存在
        ulrunrslt = vos_getnode(listid, ulkeyword, (void**)(&pnewnode));
        if (VOS_NULL != pnewnode)
        {
            RETURNERRNO(vos_mnglist_errorcode_base,vos_list_exist_sps_node_err);
        }
    }
    
    ulrunrslt = vos_getlistbyid(listid, &plist);
    if (VOS_OK != ulrunrslt)
    {
        return ulrunrslt;
    }

    pnewnode = (vos_listnode_stru*)malloc(plist->ulnodesize);
    if (VOS_NULL == pnewnode)
    {
        RETURNERRNO(vos_mnglist_errorcode_base, vos_malloc_mem_err);
    }

    memset(pnewnode, 0, plist->ulnodesize);

    /*默认是按照内存地址进行链表排序*/
    pnewnode->ulnodekeyword = (vos_u32)(pnewnode->aucloadbuf);
    
    if (vos_list_sort_default_mode != ensortmd)
    {
        pnewnode->ulnodekeyword = ulkeyword;
    }
    
    if (vos_list_sort_fifo_mode == ensortmd)
    {//新节点插在链表尾
        vos_insertnodebyfifo(plist, pnewnode);
    }else if (vos_list_sort_lifo_mode == ensortmd)
    {//新节点插在链表头
        vos_insertnodebylifo(plist, pnewnode);
    }else
    {//新节点按照关键字大小顺序插入链表
        vos_insertnodebyorder(plist, pnewnode);
    }

    /*只用返回用户定义的数据结构起始地址,从而隐藏链表结构*/
    *ppoutmem = (void*)(&(pnewnode->aucloadbuf));
    
    return VOS_OK;
}

/*****************************************************************************
 函 数 名  : vos_listseek
 功能描述  : 设置链表当前操作位置
 输入参数  : vos_listid listid
             vos_i32 sloffset
 输出参数  : 无
 返 回 值  : vos_u32
 
 修改历史      :
  1.日    期   : 2013年8月27日
    作    者   : Albort,Feng

    修改内容   : 新生成函数

*****************************************************************************/
vos_u32 vos_listseek(vos_listid listid, vos_i32 sloffset)
{
    vos_u32 ulloopnum      = 0;
    vos_u32 ulloopupperthd = 0;
    vos_u32 ulrunrslt      = VOS_OK;
    
    vos_list_stru* plist = VOS_NULL;
    
    ulrunrslt = vos_getlistbyid(listid, &plist);
    if (VOS_OK != ulrunrslt)
    {
        return ulrunrslt;
    }

    if (((vos_i32)vos_list_offset_head) == sloffset)
    {
        plist->pcurnode = plist->phead;
        return VOS_OK;
    }

    if (((vos_i32)vos_list_offset_tail) == sloffset)
    {
        plist->pcurnode = plist->ptail;
        return VOS_OK;
    }

    /*链表当前操作位置向前移*/
    if (0 > sloffset)
    {
        ulloopupperthd = 0 - sloffset;
        ulloopnum      = 0;
        while(ulloopnum < ulloopupperthd)
        {
            if ((VOS_NULL == plist->pcurnode)
             || (VOS_NULL == plist->pcurnode->pprenode))
            {
                break;
            }
            
            plist->pcurnode = plist->pcurnode->pprenode;
            
            ulloopnum += 1;
        }
        return VOS_OK;
    }

    /*链表当前操作位置向后移*/
    ulloopupperthd = (vos_u32)sloffset;
    ulloopnum      = 0;
    while(ulloopnum < ulloopupperthd)
    {
        if ((VOS_NULL == plist->pcurnode)
         || (VOS_NULL == plist->pcurnode->pnextnode))
        {
            break;
        }
        
        plist->pcurnode = plist->pcurnode->pnextnode;
        
        ulloopnum += 1;
    }
    
    return VOS_OK;
}

/*****************************************************************************
 函 数 名  : vos_getlistlength
 功能描述  : 获取链表长度
 输入参数  : vos_listid listid
             vos_u32 *plistlength
 输出参数  : 无
 返 回 值  : vos_u32(VOS_OK:成功获取链表长度，其他:获取链表失败)
 
 修改历史      :
  1.日    期   : 2014年3月4日
    作    者   : zhangchaoyong

    修改内容   : 新生成函数

*****************************************************************************/
vos_u32 vos_getlistlength(vos_listid listid, vos_u32 *plistlength)
{
    vos_u32 ulrunrslt      = VOS_OK;
    vos_list_stru* plist = VOS_NULL;

    if (VOS_NULL == plistlength)
    {
        RETURNERRNO(vos_mnglist_errorcode_base, vos_para_null_err);
    }

    *plistlength = VOS_INVALID_U32;
    
    ulrunrslt = vos_getlistbyid(listid, &plist);
    if (VOS_OK != ulrunrslt)
    {
        return ulrunrslt;
    }

    *plistlength = plist->ullistlength;

    return VOS_OK;
}

/*****************************************************************************
 函 数 名  : vos_getlistnodesize
 功能描述  : 获取链表节点大小
 输入参数  : vos_listid listid
             vos_u32 *plistnodesize
 输出参数  : 无
 返 回 值  : vos_u32(VOS_OK:成功获取链表长度，其他:获取链表失败)
 
 修改历史      :
  1.日    期   : 2014年3月4日
    作    者   : zhangchaoyong

    修改内容   : 新生成函数

*****************************************************************************/
vos_u32 vos_getlistnodesize(vos_listid listid, vos_u32 *plistnodesize)
{
    vos_u32 ulrunrslt      = VOS_OK;
    vos_list_stru* plist = VOS_NULL;

    if (VOS_NULL == plistnodesize)
    {
        RETURNERRNO(vos_mnglist_errorcode_base, vos_para_null_err);
    }

    *plistnodesize = VOS_INVALID_U32;
    
    ulrunrslt = vos_getlistbyid(listid, &plist);
    if (VOS_OK != ulrunrslt)
    {
        return ulrunrslt;
    }

    *plistnodesize = plist->ulnodesize;

    return VOS_OK;
}

/*****************************************************************************
 函 数 名  : vos_getlistheadnode
 功能描述  : 获取链表头结点
 输入参数  : vos_listid listid
             void** ppoutnode
 输出参数  : 无
 返 回 值  : vos_u32
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2014年5月31日
    作    者   : Albort,Feng
    修改内容   : 新生成函数

*****************************************************************************/
vos_u32 vos_getlistheadnode(vos_listid listid, void** ppoutnode)
{
    vos_u32 ulrunrslt = VOS_OK;

    vos_list_stru* plist = VOS_NULL;
    
    if (VOS_NULL == ppoutnode)
    {
        RETURNERRNO(vos_mnglist_errorcode_base, vos_para_null_err);
    }
    
    ulrunrslt = vos_getlistbyid(listid, &plist);
    if (VOS_OK != ulrunrslt)
    {
        return ulrunrslt;
    }

    *ppoutnode = VOS_NULL;

    if (VOS_NULL != plist->phead)
    {
        *ppoutnode = (void*)(plist->phead->aucloadbuf);
    }
    
    return VOS_OK;
}

/*****************************************************************************
 函 数 名  : vos_getlisttailnode
 功能描述  : 获取链表尾结点
 输入参数  : vos_listid listid
             void** ppoutnode
 输出参数  : 无
 返 回 值  : vos_u32
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2014年5月31日
    作    者   : Albort,Feng
    修改内容   : 新生成函数

*****************************************************************************/
vos_u32 vos_getlisttailnode(vos_listid listid, void** ppoutnode)
{
    vos_u32 ulrunrslt = VOS_OK;

    vos_list_stru* plist = VOS_NULL;
    
    if (VOS_NULL == ppoutnode)
    {
        RETURNERRNO(vos_mnglist_errorcode_base, vos_para_null_err);
    }
    
    ulrunrslt = vos_getlistbyid(listid, &plist);
    if (VOS_OK != ulrunrslt)
    {
        return ulrunrslt;
    }

    *ppoutnode = VOS_NULL;

    if (VOS_NULL != plist->ptail)
    {
        *ppoutnode = (void*)(plist->ptail->aucloadbuf);
    }
    
    return VOS_OK;
}


/*****************************************************************************
 函 数 名  : vos_getcurrentopnode
 功能描述  : 获取当前操作位置节点
 输入参数  : vos_listid listid
 输出参数  : void** ppoutnode
 返 回 值  : vos_u32
 
 修改历史      :
  1.日    期   : 2013年8月27日
    作    者   : Albort,Feng

    修改内容   : 新生成函数

*****************************************************************************/
vos_u32 vos_getcurrentopnode(vos_listid listid, void** ppoutnode)
{
    vos_u32 ulrunrslt = VOS_OK;
    
    vos_list_stru* plist = VOS_NULL;

    if (VOS_NULL == ppoutnode)
    {
        RETURNERRNO(vos_mnglist_errorcode_base, vos_para_null_err);
    }
    
    ulrunrslt = vos_getlistbyid(listid, &plist);
    if (VOS_OK != ulrunrslt)
    {
        return ulrunrslt;
    }

    *ppoutnode = VOS_NULL;

    /*已经到链表尾部*/
    if (VOS_NULL == plist->pcurnode)
    {
        return VOS_OK;
    }
    
    *ppoutnode = (void*)(plist->pcurnode->aucloadbuf);
    
    /*已经到链表尾部*/
    if (VOS_NULL == plist->pcurnode)
    {
        return VOS_OK;
    }
    
    plist->pcurnode = plist->pcurnode->pnextnode;
  
    return VOS_OK;
}

/*****************************************************************************
 函 数 名  : vos_getnextnode
 功能描述  : 获取链表下一个节点
 输入参数  : vos_listid listid
             void** ppoutnode
 输出参数  : 无
 返 回 值  : vos_u32
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2014年7月9日
    作    者   : Albort,Feng
    修改内容   : 新生成函数

*****************************************************************************/
vos_u32 vos_getnextnode(vos_listid listid, void** ppoutnode)
{
    vos_u32 ulrunrslt = VOS_OK;
    
    vos_list_stru* plist = VOS_NULL;

    if (VOS_NULL == ppoutnode)
    {
        RETURNERRNO(vos_mnglist_errorcode_base, vos_para_null_err);
    }
    
    ulrunrslt = vos_getlistbyid(listid, &plist);
    if (VOS_OK != ulrunrslt)
    {
        return ulrunrslt;
    }

    *ppoutnode = VOS_NULL;

    /*已经到链表尾部*/
    if (VOS_NULL == plist->pcurnode)
    {
        return VOS_OK;
    }
    
    *ppoutnode = (void*)(plist->pcurnode->aucloadbuf);

    plist->pcurnode = plist->pcurnode->pnextnode;
    
    return VOS_OK;
}

/*****************************************************************************
 函 数 名  : vos_getprenode
 功能描述  : 获取链表前一个节点
 输入参数  : vos_listid listid
             void** ppoutnode
 输出参数  : 无
 返 回 值  : vos_u32
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2014年7月9日
    作    者   : Albort,Feng
    修改内容   : 新生成函数

*****************************************************************************/
vos_u32 vos_getprenode(vos_listid listid, void** ppoutnode)
{
    vos_u32 ulrunrslt = VOS_OK;
    
    vos_list_stru* plist = VOS_NULL;

    if (VOS_NULL == ppoutnode)
    {
        RETURNERRNO(vos_mnglist_errorcode_base, vos_para_null_err);
    }
    
    ulrunrslt = vos_getlistbyid(listid, &plist);
    if (VOS_OK != ulrunrslt)
    {
        return ulrunrslt;
    }

    *ppoutnode = VOS_NULL;

    /*已经到链表头部*/
    if (VOS_NULL == plist->pcurnode)
    {
        return VOS_OK;
    }

    *ppoutnode = (void*)(plist->pcurnode->aucloadbuf);

    plist->pcurnode = plist->pcurnode->pprenode;
    
    return VOS_OK;
}

#ifdef __cplusplus
}
#endif

