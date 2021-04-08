/******************************************************************************

                  ��Ȩ���� (C), 2013-2023, �Ĵ��³�����Ƽ����޹�˾

 ******************************************************************************
  �� �� ��   : vos_mnglist.c
  �� �� ��   : ����
  ��    ��   : Albort,Feng
  ��������   : 2013��8��26��
  ����޸�   :
  ��������   : ͨ�����������ʵ���ļ�
  �����б�   :
  �޸���ʷ   :
  1.��    ��   : 2013��8��26��
    ��    ��   : Albort,Feng
    �޸�����   : �����ļ�

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

//ͨ������ڵ�ṹ
typedef struct _vos_listnode_
{
    struct _vos_listnode_* pprenode;
    struct _vos_listnode_* pnextnode;

    vos_u32 ulnodekeyword; //������������Ĺؼ���
    
    char aucloadbuf[0];
}vos_listnode_stru;

//ͨ������ṹ
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
 �� �� ��  : vos_initlistmd
 ��������  : ����ģ���ʼ���ӿ�
 �������  : void
 �������  : ��
 �� �� ֵ  : vos_u32 (VOS_OK:��ʼ���ɹ�,vos_err:��ʼ��ʧ��)
 
 �޸���ʷ      :
  1.��    ��   : 2013��8��26��
    ��    ��   : Albort,Feng

    �޸�����   : �����ɺ���

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
 �� �� ��  : vos_createlist
 ��������  : ����һ��������
 �������  : vos_u32 ullistnodesize
 �������  : vos_listid* poutid
 �� �� ֵ  : vos_u32
 
 �޸���ʷ      :
  1.��    ��   : 2013��8��26��
    ��    ��   : Albort,Feng

    �޸�����   : �����ɺ���

*****************************************************************************/
vos_u32 vos_createlist(vos_u32 ullistnodesize, vos_listid* poutid)
{
    vos_u32 ulloopnum = 0;
    vos_u32 ulmemsize = 0;
    
    vos_list_stru* ptmpmem = VOS_NULL;
    
    /*begin:�����Ч�Լ��*/
    if (0 == ullistnodesize)
    {
        RETURNERRNO(vos_mnglist_errorcode_base, vos_para_invalid_err);
    }
    if (VOS_NULL == poutid)
    {
        RETURNERRNO(vos_mnglist_errorcode_base, vos_para_null_err);
    }
    /*end:�����Ч�Լ��*/
    
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
        ulmemsize += (4 - (ullistnodesize & 0x00000003)); //ϵͳ����4�ֽ��ڴ����
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
 �� �� ��  : vos_getlistbyid
 ��������  : ��������id�õ�����
 �������  : vos_listid listid
 �������  : vos_list_stru** ppoutlist
 �� �� ֵ  : vos_u32
 
 �޸���ʷ      :
  1.��    ��   : 2013��8��26��
    ��    ��   : Albort,Feng

    �޸�����   : �����ɺ���

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
 �� �� ��  : vos_getnode
 ��������  : �ɹؼ��ֻ�ȡ�ڵ�
 �������  : vos_listid listid
             vos_u32 ulkeyword
 �������  : void** ppout
 �� �� ֵ  : vos_u32
 
 �޸���ʷ      :
  1.��    ��   : 2013��8��26��
    ��    ��   : Albort,Feng

    �޸�����   : �����ɺ���

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

    /*��֤������Ϊ��ż��ʱѭ������ȷ*/
    ulloopupperthd = (plist->ullistlength >> 1)
                   + (plist->ullistlength & 0x00000001);
    
    paftermv = plist->phead;
    pfrontmv = plist->ptail;

    /*��������ֻ��ѭ��һ��������*/
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
 �� �� ��  : vos_getinsertpointerbykeyword
 ��������  : �ɲ���ڵ�ؼ��ֻ�ȡ�����
 �������  : vos_list_stru* plist
             vos_u32 ulinsertkeyword
 �������  : ��
 �� �� ֵ  : vos_listnode_stru*
 
 �޸���ʷ      :
  1.��    ��   : 2013��8��26��
    ��    ��   : Albort,Feng

    �޸�����   : �����ɺ���

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

    /*�²���ڵ�ؼ���С�ڵ��ڱ�ͷ�Ĺؼ���,�ڱ�ͷǰ�����½ڵ�*/
    if (ulinsertkeyword <= plist->phead->ulnodekeyword)
    {
        return plist->phead;
    }

    /*�²���ڵ�ؼ��ִ��ڵ��ڱ�β�Ĺؼ���,�ڱ�β������½ڵ�*/
    if (ulinsertkeyword >= plist->ptail->ulnodekeyword)
    {
        return plist->ptail;
    }

    /*��֤������Ϊ��ż��ʱѭ������ȷ*/
    ulloopupperthd = (plist->ullistlength >> 1)
                   + (plist->ullistlength & 0x00000001);
    
    paftermv = plist->phead;
    pfrontmv = plist->ptail;

    /*��������ֻ��ѭ��һ��������*/
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
 �� �� ��  : vos_insertnodeafterspsnode
 ��������  : ��ָ���ڵ������µĽڵ�
 �������  : vos_list_stru* plist
             vos_listnode_stru* pspsnode
             vos_listnode_stru* pnewnode
 �������  : ��
 �� �� ֵ  : void
 
 �޸���ʷ      :
  1.��    ��   : 2013��8��26��
    ��    ��   : Albort,Feng

    �޸�����   : �����ɺ���

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
 �� �� ��  : vos_insertnodefrontspsnode
 ��������  : ��ָ���ڵ�ǰ�����µĽڵ�
 �������  : vos_list_stru* plist
             vos_listnode_stru* pspsnode
             vos_listnode_stru* pnewnode
 �������  : ��
 �� �� ֵ  : void
 
 �޸���ʷ      :
  1.��    ��   : 2013��8��26��
    ��    ��   : Albort,Feng

    �޸�����   : �����ɺ���

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
 �� �� ��  : vos_insertnodebyorder
 ��������  : �������������ڵ�
 �������  : vos_listid listid
             vos_listnode_stru* pnode
 �������  : ��
 �� �� ֵ  : void
 
 �޸���ʷ      :
  1.��    ��   : 2013��8��26��
    ��    ��   : Albort,Feng

    �޸�����   : �����ɺ���

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
    
    /*��ָ���ڵ������µĽڵ�*/
    if (pnode->ulnodekeyword > pinsertpointer->ulnodekeyword)
    {
        vos_insertnodeafterspsnode(plist, pinsertpointer, pnode);
    }else //��ָ���ڵ�ǰ�����µĽڵ�
    {
        vos_insertnodefrontspsnode(plist, pinsertpointer, pnode);
    }

    plist->ullistlength += 1;
    
    return;
}

/*****************************************************************************
 �� �� ��  : vos_insertnodebyfifo
 ��������  : �����Ƚ��ȳ���˳�����ڵ㣬��������β�����½ڵ�
 �������  : vos_list_stru* plist
             vos_listnode_stru* pnode
 �������  : ��
 �� �� ֵ  : void
 
 �޸���ʷ      :
  1.��    ��   : 2013��9��6��
    ��    ��   : Albort,Feng

    �޸�����   : �����ɺ���

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
 �� �� ��  : vos_insertnodebylifo
 ��������  : ���պ���ȳ��ķ�ʽ�����½ڵ�,��������ͷ�����½ڵ�
 �������  : ��
 �������  : ��
 �� �� ֵ  : void
 ���ú���  : 
 ��������  : 
 
 �޸���ʷ      :
  1.��    ��   : 2014��7��10��
    ��    ��   : Albort,Feng
    �޸�����   : �����ɺ���

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
 �� �� ��  : vos_deletenode
 ��������  : ɾ���ɹؼ���ָ���Ľڵ�
 �������  : vos_listid listid
             vos_u32 ulkeyword
 �������  : ��
 �� �� ֵ  : vos_u32
 
 �޸���ʷ      :
  1.��    ��   : 2013��8��26��
    ��    ��   : Albort,Feng

    �޸�����   : �����ɺ���

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

    /*��ɾ���ڵ�Ϊ����β*/
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

    /*��ɾ���ڵ�Ϊ����ͷ*/
    if (plist->phead == pspsnode)
    {
        plist->phead = pspsnode->pnextnode;
        
        plist->phead->pprenode = VOS_NULL;
        
        free(pspsnode);
        
        return VOS_OK;
    }
    
    /*��ɾ���ڵ�Ϊ�м�ڵ�*/
    pspsnode->pprenode->pnextnode = pspsnode->pnextnode;
    pspsnode->pnextnode->pprenode = pspsnode->pprenode;
    
    free(pspsnode);
    
    return VOS_OK;
}

/*****************************************************************************
 �� �� ��  : vos_deletelist
 ��������  : ɾ����������
 �������  : vos_listid listid
 �������  : ��
 �� �� ֵ  : vos_u32
 
 �޸���ʷ      :
  1.��    ��   : 2013��8��27��
    ��    ��   : Albort,Feng

    �޸�����   : �����ɺ���

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
 �� �� ��  : vos_mallocnode
 ��������  : ����һ������ڵ�
 �������  : vos_listid listid
             vos_u32 ensortmd  (��Ĭ�Ϸ�ʽ�µ�������Ҫ������Чkeyword)vos_list_sort_default_mode
             vos_u32 ulkeyword 
 �������  : void** ppoutmem
 �� �� ֵ  : void*
 
 �޸���ʷ      :
  1.��    ��   : 2013��8��26��
    ��    ��   : Albort,Feng

    �޸�����   : �����ɺ���

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
    {//��Ĭ������ʽ�¼��ָ���ؼ��ֵĽ���Ƿ��Ѿ�����
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

    /*Ĭ���ǰ����ڴ��ַ������������*/
    pnewnode->ulnodekeyword = (vos_u32)(pnewnode->aucloadbuf);
    
    if (vos_list_sort_default_mode != ensortmd)
    {
        pnewnode->ulnodekeyword = ulkeyword;
    }
    
    if (vos_list_sort_fifo_mode == ensortmd)
    {//�½ڵ��������β
        vos_insertnodebyfifo(plist, pnewnode);
    }else if (vos_list_sort_lifo_mode == ensortmd)
    {//�½ڵ��������ͷ
        vos_insertnodebylifo(plist, pnewnode);
    }else
    {//�½ڵ㰴�չؼ��ִ�С˳���������
        vos_insertnodebyorder(plist, pnewnode);
    }

    /*ֻ�÷����û���������ݽṹ��ʼ��ַ,�Ӷ���������ṹ*/
    *ppoutmem = (void*)(&(pnewnode->aucloadbuf));
    
    return VOS_OK;
}

/*****************************************************************************
 �� �� ��  : vos_listseek
 ��������  : ��������ǰ����λ��
 �������  : vos_listid listid
             vos_i32 sloffset
 �������  : ��
 �� �� ֵ  : vos_u32
 
 �޸���ʷ      :
  1.��    ��   : 2013��8��27��
    ��    ��   : Albort,Feng

    �޸�����   : �����ɺ���

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

    /*����ǰ����λ����ǰ��*/
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

    /*����ǰ����λ�������*/
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
 �� �� ��  : vos_getlistlength
 ��������  : ��ȡ������
 �������  : vos_listid listid
             vos_u32 *plistlength
 �������  : ��
 �� �� ֵ  : vos_u32(VOS_OK:�ɹ���ȡ�����ȣ�����:��ȡ����ʧ��)
 
 �޸���ʷ      :
  1.��    ��   : 2014��3��4��
    ��    ��   : zhangchaoyong

    �޸�����   : �����ɺ���

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
 �� �� ��  : vos_getlistnodesize
 ��������  : ��ȡ����ڵ��С
 �������  : vos_listid listid
             vos_u32 *plistnodesize
 �������  : ��
 �� �� ֵ  : vos_u32(VOS_OK:�ɹ���ȡ�����ȣ�����:��ȡ����ʧ��)
 
 �޸���ʷ      :
  1.��    ��   : 2014��3��4��
    ��    ��   : zhangchaoyong

    �޸�����   : �����ɺ���

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
 �� �� ��  : vos_getlistheadnode
 ��������  : ��ȡ����ͷ���
 �������  : vos_listid listid
             void** ppoutnode
 �������  : ��
 �� �� ֵ  : vos_u32
 ���ú���  : 
 ��������  : 
 
 �޸���ʷ      :
  1.��    ��   : 2014��5��31��
    ��    ��   : Albort,Feng
    �޸�����   : �����ɺ���

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
 �� �� ��  : vos_getlisttailnode
 ��������  : ��ȡ����β���
 �������  : vos_listid listid
             void** ppoutnode
 �������  : ��
 �� �� ֵ  : vos_u32
 ���ú���  : 
 ��������  : 
 
 �޸���ʷ      :
  1.��    ��   : 2014��5��31��
    ��    ��   : Albort,Feng
    �޸�����   : �����ɺ���

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
 �� �� ��  : vos_getcurrentopnode
 ��������  : ��ȡ��ǰ����λ�ýڵ�
 �������  : vos_listid listid
 �������  : void** ppoutnode
 �� �� ֵ  : vos_u32
 
 �޸���ʷ      :
  1.��    ��   : 2013��8��27��
    ��    ��   : Albort,Feng

    �޸�����   : �����ɺ���

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

    /*�Ѿ�������β��*/
    if (VOS_NULL == plist->pcurnode)
    {
        return VOS_OK;
    }
    
    *ppoutnode = (void*)(plist->pcurnode->aucloadbuf);
    
    /*�Ѿ�������β��*/
    if (VOS_NULL == plist->pcurnode)
    {
        return VOS_OK;
    }
    
    plist->pcurnode = plist->pcurnode->pnextnode;
  
    return VOS_OK;
}

/*****************************************************************************
 �� �� ��  : vos_getnextnode
 ��������  : ��ȡ������һ���ڵ�
 �������  : vos_listid listid
             void** ppoutnode
 �������  : ��
 �� �� ֵ  : vos_u32
 ���ú���  : 
 ��������  : 
 
 �޸���ʷ      :
  1.��    ��   : 2014��7��9��
    ��    ��   : Albort,Feng
    �޸�����   : �����ɺ���

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

    /*�Ѿ�������β��*/
    if (VOS_NULL == plist->pcurnode)
    {
        return VOS_OK;
    }
    
    *ppoutnode = (void*)(plist->pcurnode->aucloadbuf);

    plist->pcurnode = plist->pcurnode->pnextnode;
    
    return VOS_OK;
}

/*****************************************************************************
 �� �� ��  : vos_getprenode
 ��������  : ��ȡ����ǰһ���ڵ�
 �������  : vos_listid listid
             void** ppoutnode
 �������  : ��
 �� �� ֵ  : vos_u32
 ���ú���  : 
 ��������  : 
 
 �޸���ʷ      :
  1.��    ��   : 2014��7��9��
    ��    ��   : Albort,Feng
    �޸�����   : �����ɺ���

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

    /*�Ѿ�������ͷ��*/
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

