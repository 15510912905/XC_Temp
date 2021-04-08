/******************************************************************************

                  ��Ȩ���� (C), 2013-2023, �Ĵ��³�����Ƽ����޹�˾

 ******************************************************************************
  �� �� ��   : vos_timer.c
  �� �� ��   : ����
  ��    ��   : zhangchaoyong
  ��������   : 2014��1��6��
  ����޸�   :
  ��������   : vos ��ʱ��ʵ��
  �����б�   :
  �޸���ʷ   :
  1.��    ��   : 2014��1��6��
    ��    ��   : zhangchaoyong
    �޸�����   : �����ļ�

******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "vos_configure.h"
#include "vos_adaptsys.h"
#include "vos_errorcode.h"
#include "vos_messagecenter.h"
#include "vos_log.h"
#include "vos_mnglist.h"
#include "vos_init.h"
#include "vos_timer.h"

#define VOS_TIMER_ID_BASE_VAR  (1000)

//��ʱ��״̬
typedef enum
{
    vos_timer_status_active   = 1, //�
    vos_timer_status_inactive = 2, //�ǻ
}vos_timer_status_enum;
typedef vos_u32 vos_timer_status_enum_u32;

//��ʱ�����ݽṹ
typedef struct
{
    vos_u32 ultimerid;                       //��ʱ��ID
    vos_u32 ulcounter;                       //��ʱ����
    vos_u32 status;                          //��ʱ��״̬
    vos_u32 entimertype;                     //��ʱ������
    vos_u32 ultimelen;                       //��ʱʱ�䳤��(����)
    vos_u32 ulsrcpid;                        //������ʱ��ϢPID
    vos_u32 aucdstpid[VOS_MAX_PID_TIMER];    //��ʱ��Ŀ��PID
    char auctimername[VOS_TIMERNAME_LEN];  //��ʱ������
}vos_timer_info_stru;

//��ʱ��id����ṹ
typedef struct
{
    vos_u32 ultimerid;
    char  ucusedflag;
    char  aucrsv[3];
}vos_mng_timer_id_stru;

#ifdef __cplusplus
extern "C" {
#endif 

//��ʱ������ID
STATIC vos_listid  g_timerlistid = VOS_INVALID_U32; 
STATIC void* g_ptimermdmutex = VOS_NULL;

STATIC vos_mng_timer_id_stru* g_psttimeridmng = VOS_NULL;

/*****************************************************************************
 �� �� ��  : vos_distributetimerid
 ��������  : ���䶨ʱ��id����
 �������  : void
 �������  : ��
 �� �� ֵ  : vos_u32 (��ʱ��id��,��һ��С��VOS_MAX_SUPPORT_TIMER_NUM)
 ���ú���  : 
 ��������  : 
 
 �޸���ʷ      :
  1.��    ��   : 2014��9��3��
    ��    ��   : Albort,Feng
    �޸�����   : �����ɺ���

*****************************************************************************/
vos_u32 vos_distributetimerid(void)
{
    vos_u32 ulloopvar = 0;
    vos_u32 ultimerid = VOS_MAX_SUPPORT_TIMER_NUM + 1;

    vos_mng_timer_id_stru* pstmvpointer = VOS_NULL;
    vos_obtainmutex(g_ptimermdmutex);
    for (ulloopvar = 0; VOS_MAX_SUPPORT_TIMER_NUM > ulloopvar; ulloopvar++)
    {
        pstmvpointer = (vos_mng_timer_id_stru*)(((char*)g_psttimeridmng)
                     + (sizeof(vos_mng_timer_id_stru) * ulloopvar));
               
        if (VOS_NO == pstmvpointer->ucusedflag)
        {
            pstmvpointer->ucusedflag = VOS_YES;

            ultimerid = pstmvpointer->ultimerid;

            break;
        }
    }
    vos_releasemutex(g_ptimermdmutex);
    
    return ultimerid;
}

/*****************************************************************************
 �� �� ��  : vos_destroytimerid
 ��������  : �ݻٶ�ʱ��id
 �������  : vos_u32 ultimerid
 �������  : ��
 �� �� ֵ  : void
 ���ú���  : 
 ��������  : 
 
 �޸���ʷ      :
  1.��    ��   : 2014��9��3��
    ��    ��   : Albort,Feng
    �޸�����   : �����ɺ���

*****************************************************************************/
void vos_destroytimerid(vos_u32 ultimerid)
{
    vos_u32 ulloopvar = 0;

    vos_mng_timer_id_stru* pstmvpointer = VOS_NULL;

    if (VOS_MAX_SUPPORT_TIMER_NUM <= ultimerid)
    {
        return;
    }
    vos_obtainmutex(g_ptimermdmutex);
    pstmvpointer = (vos_mng_timer_id_stru*)(((char*)g_psttimeridmng)
                 + (sizeof(vos_mng_timer_id_stru) * ultimerid));
    
    if (ultimerid == pstmvpointer->ultimerid)
    {
        pstmvpointer->ucusedflag = VOS_NO;
        vos_releasemutex(g_ptimermdmutex);
        return;
    }
    
    for (ulloopvar = 0; VOS_MAX_SUPPORT_TIMER_NUM > ulloopvar; ulloopvar++)
    {
        pstmvpointer = (vos_mng_timer_id_stru*)(((char*)g_psttimeridmng)
                     + (sizeof(vos_mng_timer_id_stru) * ulloopvar));
        
        if (ultimerid == pstmvpointer->ultimerid)
        {
            pstmvpointer->ucusedflag = VOS_NO;

            break;
        }
    }
    vos_releasemutex(g_ptimermdmutex);
    return;
}

/*****************************************************************************
 �� �� ��  : vos_checktimerisexist
 ��������  : �ж϶�ʱ���Ƿ����
 �������  : char *pctimername
 �������  : ��
 �� �� ֵ  : vos_u32 (VOS_YES: ָ�����ƵĶ�ʱ���Ѵ���,VOS_NO:ָ���Ķ�ʱ��δ����)
 
 �޸���ʷ      :
  1.��    ��   : 2014��1��13��
    ��    ��   : zhangchaoyong

    �޸�����   : �����ɺ���

*****************************************************************************/
vos_u32 vos_checktimerisexist(char *pctimername)
{
    vos_u32 ulreturn = VOS_OK;
    
    vos_timer_info_stru *pstnode = VOS_NULL;

    if (VOS_NULL == pctimername)
    {
        RETURNERRNO(vos_timer_errorcode_base, vos_para_null_err);
    }
    vos_obtainmutex(g_ptimermdmutex);
    (void)vos_listseek(g_timerlistid, vos_list_offset_head);
    (void)vos_getcurrentopnode(g_timerlistid, (void **)(&pstnode));

    while (VOS_NULL != pstnode)
    {
        ulreturn = strcmp((char *)(pstnode->auctimername), (char*)pctimername);
        if (VOS_OK == ulreturn)
        {
            break;
        }

        pstnode = VOS_NULL;
        
        (void)vos_getcurrentopnode(g_timerlistid, (void**)(&pstnode));
    }
    vos_releasemutex(g_ptimermdmutex);
    if (VOS_NULL != pstnode)
    {
        return VOS_YES;
    }

    return VOS_NO;
}

/*****************************************************************************
 �� �� ��  : vos_createtimer
 ��������  : �½�һ����ʱ��
 �������  : vos_timer_reg_stru *regstru
              vos_u32 *ultimedid
 �������  : ��
 �� �� ֵ  : vos_u32
 
 �޸���ʷ      :
  1.��    ��   : 2014��1��6��
    ��    ��   : zhangchaoyong

    �޸�����   : �����ɺ���

*****************************************************************************/
vos_u32 vos_createtimer(vos_timer_reg_stru *regstru, vos_u32 *ptimerid)
{
    vos_u32 ulreturn  = VOS_OK;
    vos_u32 ultimerid = 0;
    
    vos_timer_info_stru *pstnode = VOS_NULL;

    if ((VOS_NULL == regstru) || (VOS_NULL == ptimerid))
    {
       RETURNERRNO(vos_timer_errorcode_base, vos_para_null_err);
    }
    
    ulreturn = vos_checktimerisexist(regstru->auctimername);
    if (VOS_YES == ulreturn)
    {
        RETURNERRNO(vos_timer_errorcode_base, vos_timername_repeat_err);
    }

    ultimerid = vos_distributetimerid();
    if (VOS_MAX_SUPPORT_TIMER_NUM <= ultimerid)
    {
        RETURNERRNO(vos_timer_errorcode_base, vos_timer_id_used_over_error);
    }
    vos_obtainmutex(g_ptimermdmutex);
    ulreturn = vos_mallocnode(g_timerlistid, vos_list_sort_fifo_mode, ultimerid, (void **)&pstnode);
    if (VOS_OK != ulreturn)
    {
        vos_releasemutex(g_ptimermdmutex);
        WRITE_ERR_LOG("Call vos_mallocnode failed(0x%x)\r\n", ulreturn);
        return ulreturn;
    }
    
    memset(pstnode, 0x00, sizeof(vos_timer_info_stru));
    
    pstnode->status = vos_timer_status_inactive;

    pstnode->ultimerid   = ultimerid;
    pstnode->ulcounter   = 0;
    pstnode->ulsrcpid    = regstru->ulsrcpid;
    pstnode->ultimelen   = regstru->ultimelen;
    pstnode->entimertype = regstru->entimertype;
    
    memcpy((char*)(pstnode->auctimername), (char*)(regstru->auctimername), (strlen((const char*)regstru->auctimername) + 1));
    memcpy(pstnode->aucdstpid, regstru->aucdstpid, (sizeof(vos_u32)*VOS_MAX_PID_TIMER));

    *ptimerid = ultimerid;
    vos_releasemutex(g_ptimermdmutex);
    return VOS_OK;
}

/*****************************************************************************
 �� �� ��  : vos_deletetimer
 ��������  : ɾ��һ����ʱ��
 �������  : vos_u32 ultimerid
 �������  : ��
 �� �� ֵ  : vos_u32 
 
 �޸���ʷ      :
  1.��    ��   : 2014��1��6��
    ��    ��   : zhangchaoyong

    �޸�����   : �����ɺ���

*****************************************************************************/
vos_u32 vos_deletetimer(vos_u32 ultimerid)
{
    vos_u32 ulreturn = VOS_OK;
    
    if (VOS_MAX_SUPPORT_TIMER_NUM <= ultimerid)
    {
        RETURNERRNO(vos_timer_errorcode_base,vos_timer_id_invalid_error);
    }
    vos_obtainmutex(g_ptimermdmutex);
    ulreturn = vos_deletenode(g_timerlistid, ultimerid);
    if (VOS_OK != ulreturn)
    {
        WRITE_ERR_LOG("Call vos_deletenode failed(0x%x)\r\n", ulreturn);
    }
    vos_releasemutex(g_ptimermdmutex);
    vos_destroytimerid(ultimerid);
    
    return VOS_OK;
}

/*****************************************************************************
 �� �� ��  : vos_starttimer
 ��������  : ����һ����ʱ��
 �������  : vos_u32 ultimerid
 �������  : ��
 �� �� ֵ  : vos_u32 
 
 �޸���ʷ      :
  1.��    ��   : 2014��1��6��
    ��    ��   : zhangchaoyong

    �޸�����   : �����ɺ���

*****************************************************************************/
vos_u32 vos_starttimer(vos_u32 ultimerid)
{
    vos_u32 ulreturn = VOS_OK;
    
    vos_timer_info_stru *pstnode = VOS_NULL;

    if (VOS_MAX_SUPPORT_TIMER_NUM <= ultimerid)
    {
        WRITE_ERR_LOG("call vos_starttimer failed\n");
        RETURNERRNO(vos_timer_errorcode_base,vos_timer_id_invalid_error);
    }
    vos_obtainmutex(g_ptimermdmutex);
    ulreturn = vos_getnode(g_timerlistid, ultimerid, (void**)(&pstnode));
    if (VOS_OK != ulreturn)
    {
        vos_releasemutex(g_ptimermdmutex);
        WRITE_ERR_LOG("call vos_starttimer failed(%lx)\n", ulreturn);

        RETURNERRNO(vos_timer_errorcode_base, vos_timer_not_find_error);
    }
    
    pstnode->status    = vos_timer_status_active;
    pstnode->ulcounter = 0;

    vos_releasemutex(g_ptimermdmutex);
    return VOS_OK;
}

/*****************************************************************************
 �� �� ��  : vos_stoptimer
 ��������  : ֹͣһ����ʱ��
 �������  : vos_u32 ultimerid
 �������  : ��
 �� �� ֵ  : vos_u32 
 
 �޸���ʷ      :
  1.��    ��   : 2014��1��6��
    ��    ��   : zhangchaoyong

    �޸�����   : �����ɺ���

*****************************************************************************/
vos_u32 vos_stoptimer(vos_u32 ultimerid)
{
    vos_u32 ulreturn = VOS_OK;
    
    vos_timer_info_stru *pstnode = VOS_NULL;

    if (VOS_MAX_SUPPORT_TIMER_NUM <= ultimerid)
    {
        RETURNERRNO(vos_timer_errorcode_base,vos_timer_id_invalid_error);
    }
    
    vos_obtainmutex(g_ptimermdmutex);
    
    ulreturn = vos_getnode(g_timerlistid, ultimerid, (void **)(&pstnode));
    if (VOS_OK != ulreturn)
    {
        vos_releasemutex(g_ptimermdmutex);
        WRITE_ERR_LOG("call vos_getnode failed(0x%x)\r\n", ulreturn);
        RETURNERRNO(vos_timer_errorcode_base, vos_timer_not_find_error);
    }
    
    pstnode->status = vos_timer_status_inactive;
    
    vos_releasemutex(g_ptimermdmutex);
    return VOS_OK;
}

/*****************************************************************************
 �� �� ��  : vos_settimertimelen
 ��������  : ���ö�ʱ����ʱ����
 �������  : vos_u32 ultimerid
              vos_u32 ulmillisecond
 �������  : ��
 �� �� ֵ  : vos_u32
 
 �޸���ʷ      :
  1.��    ��   : 2014��1��6��
    ��    ��   : zhangchaoyong

    �޸�����   : �����ɺ���

*****************************************************************************/
vos_u32 vos_settimertimelen(vos_u32 ultimerid, vos_u32 ulmillisecond)
{
    vos_u32 ulreturn = VOS_OK;
    
    vos_timer_info_stru *pstnode = VOS_NULL;

    if (VOS_MAX_SUPPORT_TIMER_NUM <= ultimerid)
    {
        RETURNERRNO(vos_timer_errorcode_base,vos_timer_id_invalid_error);
    }
    vos_obtainmutex(g_ptimermdmutex);
    ulreturn = vos_getnode(g_timerlistid, ultimerid, (void**)(&pstnode));
    if (VOS_OK != ulreturn)
    {
        vos_releasemutex(g_ptimermdmutex);
        WRITE_ERR_LOG("call vos_getnode failed(0x%x)\r\n", ulreturn);
        RETURNERRNO(vos_timer_errorcode_base, vos_timer_not_find_error);
    }

    pstnode->ultimelen = ulmillisecond;
    pstnode->ulcounter = 0;
    vos_releasemutex(g_ptimermdmutex);
    return VOS_OK;
}

/*****************************************************************************
 �� �� ��  : vos_printalltimerinfo
 ��������  : ��ӡ���ж�ʱ����Ϣ
 �������  : void
 �������  : ��
 �� �� ֵ  : void
 ���ú���  : 
 ��������  : 
 
 �޸���ʷ      :
  1.��    ��   : 2014��9��4��
    ��    ��   : Albort,Feng
    �޸�����   : �����ɺ���

*****************************************************************************/
void vos_printalltimerinfo(void)
{
    vos_u32 ulloopvar = 0;
    
    vos_timer_info_stru *pstnode = VOS_NULL;
    vos_obtainmutex(g_ptimermdmutex);
    (void)vos_listseek(g_timerlistid, vos_list_offset_head);
    (void)vos_getcurrentopnode(g_timerlistid, (void**)(&pstnode));
    
    while(VOS_NULL != pstnode)
    {
        printf("\r Tmname= %s, tmid= %u, tmtype= %u, tmstatus= %u, curtm= %u, tmlen= %u, rcvpid= ",
               (const char*)(pstnode->auctimername), (int)(pstnode->ultimerid),
               (int)(pstnode->entimertype),(int)(pstnode->status),
               (int)(pstnode->ulcounter),(int)(pstnode->ultimelen));

        for (ulloopvar = 0; VOS_MAX_PID_TIMER > ulloopvar; ulloopvar++)
        {
            if (0 != pstnode->aucdstpid[ulloopvar])
            {
                printf("%u, ", (unsigned int)(pstnode->aucdstpid[ulloopvar]));
            }
        }

        printf("\n");
        
        pstnode = VOS_NULL;
        (void)vos_getcurrentopnode(g_timerlistid, (void **)(&pstnode));
    }
    vos_releasemutex(g_ptimermdmutex);
    return;
}

/*****************************************************************************
 �� �� ��  : vos_gettimerreginfo
 ��������  : ��ȡ��ʱ��ע����Ϣ
 �������  : vos_u32 ultimerid
             vos_timer_reg_stru* pouttimerinfo
 �������  : ��
 �� �� ֵ  : vos_u32
 ���ú���  : 
 ��������  : 
 
 �޸���ʷ      :
  1.��    ��   : 2014��9��17��
    ��    ��   : Albort,Feng
    �޸�����   : �����ɺ���

*****************************************************************************/
vos_u32 vos_gettimerreginfo(vos_u32 ultimerid, vos_timer_reg_stru* pouttimerinfo)
{
    vos_u32 ulreturn = VOS_OK;
    
    vos_timer_info_stru *pstnode = VOS_NULL;

    if ((VOS_MAX_SUPPORT_TIMER_NUM <= ultimerid) || (VOS_NULL == pouttimerinfo))
    {
        RETURNERRNO(vos_timer_errorcode_base,vos_para_invalid_err);
    }
    vos_obtainmutex(g_ptimermdmutex);
    ulreturn = vos_getnode(g_timerlistid, ultimerid, (void**)(&pstnode));
    if (VOS_OK != ulreturn)
    {
        vos_releasemutex(g_ptimermdmutex);
        WRITE_ERR_LOG("call vos_getnode failed(0x%x)\r\n", ulreturn);
        return ulreturn;
    }

    pouttimerinfo->entimertype = pstnode->entimertype;
    pouttimerinfo->ulsrcpid    = pstnode->ulsrcpid;
    pouttimerinfo->ultimelen   = pstnode->ultimelen;

    memcpy((char*)(pouttimerinfo->aucdstpid), (char*)(pstnode->aucdstpid), (sizeof(vos_u32) * VOS_MAX_PID_TIMER));
    memcpy((char*)(pouttimerinfo->auctimername), (char*)(pstnode->auctimername), VOS_TIMERNAME_LEN);

    vos_releasemutex(g_ptimermdmutex);
    return VOS_OK;
}


/*****************************************************************************
 �� �� ��  : vos_gettimerrunedtimelen
 ��������  : ��ȡ��ʱ��������ʱ�䳤��
 �������  : vos_u32 ultimerid
 �������  : ��
 �� �� ֵ  : vos_u32 (��ʱ��������ʱ�䳤��,��λΪ100ms)
 ���ú���  : 
 ��������  : 
 
 �޸���ʷ      :
  1.��    ��   : 2014��9��17��
    ��    ��   : Albort,Feng
    �޸�����   : �����ɺ���

*****************************************************************************/
vos_u32 vos_gettimerrunedtimelen(vos_u32 ultimerid)
{
    vos_u32 ulreturn       = VOS_OK;
    vos_u32 ulrunedtimelen = 0;
    
    vos_timer_info_stru *pstnode = VOS_NULL;

    if (VOS_MAX_SUPPORT_TIMER_NUM <= ultimerid)
    {
        return 0;
    }
    vos_obtainmutex(g_ptimermdmutex);
    ulreturn = vos_getnode(g_timerlistid, ultimerid, (void**)(&pstnode));
    if (VOS_OK != ulreturn)
    {
        vos_releasemutex(g_ptimermdmutex);
        WRITE_ERR_LOG("call vos_getnode failed(0x%x)\r\n", ulreturn);
        return 0;
    }

    ulrunedtimelen = pstnode->ulcounter;

    vos_releasemutex(g_ptimermdmutex);
    return ulrunedtimelen;
}

/*****************************************************************************
 �� �� ��  : vos_gettimerstatus
 ��������  : ��ȡָ����ʱ���ĵ�ǰ״̬
 �������  : vos_u32 ultimerid
             vos_u32* pulouttimerstatus
 �������  : ��
 �� �� ֵ  : vos_u32
 ���ú���  : 
 ��������  : 
 
 �޸���ʷ      :
  1.��    ��   : 2015��5��15��
    ��    ��   : Albort,Feng
    �޸�����   : �����ɺ���

*****************************************************************************/
vos_u32 vos_gettimerstatus(vos_u32 ultimerid, vos_u32* pulouttimerstatus)
{
    vos_u32 ulreturn      = VOS_OK;
    vos_u32 ultimerstatus = 0;
    
    vos_timer_info_stru *pstnode = VOS_NULL;

    if ((VOS_MAX_SUPPORT_TIMER_NUM <= ultimerid) || (VOS_NULL == pulouttimerstatus))
    {
        RETURNERRNO(vos_timer_errorcode_base,vos_para_invalid_err);
    }
    
    vos_obtainmutex(g_ptimermdmutex);
    ulreturn = vos_getnode(g_timerlistid, ultimerid, (void**)(&pstnode));
    if (VOS_OK != ulreturn)
    {
        vos_releasemutex(g_ptimermdmutex);
        WRITE_ERR_LOG("call vos_getnode failed(0x%x)\r\n", ulreturn);
        return ulreturn;
    }

    ultimerstatus = pstnode->status;

    vos_releasemutex(g_ptimermdmutex);

    *pulouttimerstatus = ultimerstatus;
    return VOS_OK;
}

/*****************************************************************************
 �� �� ��  : vos_basetimerproc
 ��������  : ��ʱģ�鹫����׼��ʱ��������
 �������  : void
 �������  : ��
 �� �� ֵ  : void
 
 �޸���ʷ      :
  1.��    ��   : 2014��1��8��
    ��    ��   : zhangchaoyong

    �޸�����   : �����ɺ���

*****************************************************************************/
void vos_basetimerproc(void)
{
    vos_u32 ulreturn  = VOS_OK;
    vos_u32 ulloopvar = 0;
    
    vos_timer_info_stru *pstnode = VOS_NULL;
    vos_timeroutmsg_stru *pmsg   = VOS_NULL;
    
    vos_obtainmutex(g_ptimermdmutex);
    (void)vos_listseek(g_timerlistid, vos_list_offset_head);
    (void)vos_getcurrentopnode(g_timerlistid, (void**)(&pstnode));
    
    while(VOS_NULL != pstnode)
    {
        if (vos_timer_status_active != pstnode->status)
        {
            pstnode = VOS_NULL;
            (void)vos_getcurrentopnode(g_timerlistid, (void **)(&pstnode));
            
            continue;
        }

        pstnode->ulcounter += 1;

        //��ʱδ��
        if (pstnode->ulcounter < pstnode->ultimelen)
        {
            pstnode = VOS_NULL;
            
            (void)vos_getcurrentopnode(g_timerlistid, (void **)(&pstnode));
            
            continue;
        }
                
        //��ʱ��,���Ͷ�ʱ��ʱ��Ϣ
        for (ulloopvar = 0; VOS_MAX_PID_TIMER > ulloopvar; ulloopvar++)
        {
            if (0 == pstnode->aucdstpid[ulloopvar])
            {
                continue;
            }
            
            ulreturn = vos_mallocmsgbuffer(sizeof(vos_timeroutmsg_stru), (void **)(&pmsg));
            if (VOS_OK != ulreturn)
            {
                WRITE_ERR_LOG("call vos_mallocmsgbuffer failed(0x%x) when timerid = %u,uldstpid = %u\r\n", ulreturn, pstnode->ultimerid, pstnode->aucdstpid[ulloopvar]);
                
                continue;
            }

            pmsg->uldstpid  = pstnode->aucdstpid[ulloopvar];
            pmsg->ulsrcpid  = pstnode->ulsrcpid;
            pmsg->usmsgtype = vos_pidmsgtype_timer_timeout;
            pmsg->ultimerid = pstnode->ultimerid;
            
            strcpy((char *)(pmsg->auctimername), (char *)(pstnode->auctimername));
            
            pmsg->usmsgloadlen = (vos_u16)(sizeof(vos_timeroutmsg_stru) - sizeof(vos_msg_header_stru));
                                           
            ulreturn = vos_sendmsg((vos_msg_header_stru *)pmsg);
            if (VOS_OK != ulreturn)
            {
                WRITE_ERR_LOG("Call vos_sendmsg failed(0x%x)() when timerid= %u, uldstpid= %u\r\n", ulreturn, pstnode->ultimerid, pstnode->aucdstpid[ulloopvar]);
            }

            pmsg = VOS_NULL;
        }

        pstnode->ulcounter = 0;
        
        //һ���Զ�ʱ������Ϊ�ǻ
        if (vos_timer_type_loop != pstnode->entimertype)
        {
            pstnode->status = vos_timer_status_inactive;
        }

        pstnode = VOS_NULL;
        (void)vos_getcurrentopnode(g_timerlistid, (void **)(&pstnode));
    }
    
    vos_releasemutex(g_ptimermdmutex);
}

/*****************************************************************************
 �� �� ��  : vos_init_timermd
 ��������  : ��ʼ����ʱ��ģ��
 �������  : void
 �������  : ��
 �� �� ֵ  : vos_u32 ��VOS_OK����ʼ���ɹ�����������ʼ��ʧ�ܴ����룩
 
 �޸���ʷ      :
  1.��    ��   : 2014��1��6��
    ��    ��   : zhangchaoyong

    �޸�����   : �����ɺ���

*****************************************************************************/
vos_u32 vos_init_timermd(void)
{
    vos_u32 ulreturn  = VOS_OK;
    vos_u32 ulmemsize = 0;
    vos_u32 ulloopvar = 0;

    vos_mng_timer_id_stru* pstmvpointer = VOS_NULL;

    g_ptimermdmutex  = vos_mallocmutex();
    ulreturn = vos_createmutex(g_ptimermdmutex);
    if (VOS_OK != ulreturn)
    {
        return ulreturn;
    }

    ulmemsize = (sizeof(vos_mng_timer_id_stru) * VOS_MAX_SUPPORT_TIMER_NUM);

    g_psttimeridmng = (vos_mng_timer_id_stru*)malloc(ulmemsize);
    if (VOS_NULL == g_psttimeridmng)
    {
        vos_deletemutex(g_ptimermdmutex);
        vos_freemutex(g_ptimermdmutex);
        g_ptimermdmutex = VOS_NULL;
        
        RETURNERRNO(vos_timer_errorcode_base,vos_malloc_mem_err);
    }

    for (ulloopvar = 0; VOS_MAX_SUPPORT_TIMER_NUM > ulloopvar; ulloopvar++)
    {
        pstmvpointer = (vos_mng_timer_id_stru*)(((char*)g_psttimeridmng) 
                     + (sizeof(vos_mng_timer_id_stru) * ulloopvar));

        pstmvpointer->ultimerid  = ulloopvar;
        pstmvpointer->ucusedflag = VOS_NO;
    }
    
    ulreturn = vos_createlist(sizeof(vos_timer_info_stru), &g_timerlistid);
    if (VOS_OK != ulreturn)
    {
        vos_deletemutex(g_ptimermdmutex);
        vos_freemutex(g_ptimermdmutex);
        g_ptimermdmutex = VOS_NULL;

        free(g_psttimeridmng);
        g_psttimeridmng = VOS_NULL;
        
        return ulreturn;
    }

    ulreturn = vos_createenventtimer(VOS_TIMER_ACCURACY, vos_basetimerproc);
    if (VOS_OK != ulreturn)
    {
        vos_deletemutex(g_ptimermdmutex);
        vos_freemutex(g_ptimermdmutex);
        g_ptimermdmutex = VOS_NULL;
        
        free(g_psttimeridmng);
        g_psttimeridmng = VOS_NULL;
        
        return ulreturn;
    }
    
    return VOS_OK;
}

vos_u32 createTimer(vos_u32 ulPid, const char* strName, vos_u32 ulTimeLen, vos_u32 ulTimerType)
{
    vos_u32 ulTimer = 0;
    vos_timer_reg_stru stTimer;

    memset((char*)(&stTimer), 0x00, sizeof(stTimer));
    stTimer.aucdstpid[0] = ulPid;
    stTimer.entimertype = ulTimerType;
    stTimer.ultimelen = ulTimeLen;
    sprintf(stTimer.auctimername, "%s", strName);

    if (VOS_OK != vos_createtimer(&stTimer, &ulTimer))
    {
        WRITE_ERR_LOG("vos_createtimer failed pid=%lu name=%s\n", ulPid, strName);
    }
    return ulTimer;
}

vos_u32 startTimer(vos_u32 ulTimer, vos_u32 ulTimeLen)
{
    vos_u32 ulResult = vos_settimertimelen(ulTimer, ulTimeLen);
    if (VOS_OK != ulResult)
    {
        WRITE_ERR_LOG("vos_settimertimelen failed(%lx)\n", ulResult);
        return ulResult;
    }
    return vos_starttimer(ulTimer);
}


#ifdef __cplusplus
}
#endif


