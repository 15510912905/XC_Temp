/******************************************************************************

                  版权所有 (C), 2013-2023, 四川新成生物科技有限公司

 ******************************************************************************
  文 件 名   : vos_timer.c
  版 本 号   : 初稿
  作    者   : zhangchaoyong
  生成日期   : 2014年1月6日
  最近修改   :
  功能描述   : vos 定时器实现
  函数列表   :
  修改历史   :
  1.日    期   : 2014年1月6日
    作    者   : zhangchaoyong
    修改内容   : 创建文件

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

//定时器状态
typedef enum
{
    vos_timer_status_active   = 1, //活动
    vos_timer_status_inactive = 2, //非活动
}vos_timer_status_enum;
typedef vos_u32 vos_timer_status_enum_u32;

//定时器数据结构
typedef struct
{
    vos_u32 ultimerid;                       //定时器ID
    vos_u32 ulcounter;                       //定时计数
    vos_u32 status;                          //定时器状态
    vos_u32 entimertype;                     //定时器类型
    vos_u32 ultimelen;                       //定时时间长度(毫秒)
    vos_u32 ulsrcpid;                        //触发定时消息PID
    vos_u32 aucdstpid[VOS_MAX_PID_TIMER];    //定时器目标PID
    char auctimername[VOS_TIMERNAME_LEN];  //定时器名称
}vos_timer_info_stru;

//定时器id管理结构
typedef struct
{
    vos_u32 ultimerid;
    char  ucusedflag;
    char  aucrsv[3];
}vos_mng_timer_id_stru;

#ifdef __cplusplus
extern "C" {
#endif 

//定时器链表ID
STATIC vos_listid  g_timerlistid = VOS_INVALID_U32; 
STATIC void* g_ptimermdmutex = VOS_NULL;

STATIC vos_mng_timer_id_stru* g_psttimeridmng = VOS_NULL;

/*****************************************************************************
 函 数 名  : vos_distributetimerid
 功能描述  : 分配定时器id函数
 输入参数  : void
 输出参数  : 无
 返 回 值  : vos_u32 (定时器id号,它一定小于VOS_MAX_SUPPORT_TIMER_NUM)
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2014年9月3日
    作    者   : Albort,Feng
    修改内容   : 新生成函数

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
 函 数 名  : vos_destroytimerid
 功能描述  : 摧毁定时器id
 输入参数  : vos_u32 ultimerid
 输出参数  : 无
 返 回 值  : void
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2014年9月3日
    作    者   : Albort,Feng
    修改内容   : 新生成函数

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
 函 数 名  : vos_checktimerisexist
 功能描述  : 判断定时器是否存在
 输入参数  : char *pctimername
 输出参数  : 无
 返 回 值  : vos_u32 (VOS_YES: 指定名称的定时器已创建,VOS_NO:指定的定时器未创建)
 
 修改历史      :
  1.日    期   : 2014年1月13日
    作    者   : zhangchaoyong

    修改内容   : 新生成函数

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
 函 数 名  : vos_createtimer
 功能描述  : 新建一个定时器
 输入参数  : vos_timer_reg_stru *regstru
              vos_u32 *ultimedid
 输出参数  : 无
 返 回 值  : vos_u32
 
 修改历史      :
  1.日    期   : 2014年1月6日
    作    者   : zhangchaoyong

    修改内容   : 新生成函数

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
 函 数 名  : vos_deletetimer
 功能描述  : 删除一个定时器
 输入参数  : vos_u32 ultimerid
 输出参数  : 无
 返 回 值  : vos_u32 
 
 修改历史      :
  1.日    期   : 2014年1月6日
    作    者   : zhangchaoyong

    修改内容   : 新生成函数

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
 函 数 名  : vos_starttimer
 功能描述  : 启动一个定时器
 输入参数  : vos_u32 ultimerid
 输出参数  : 无
 返 回 值  : vos_u32 
 
 修改历史      :
  1.日    期   : 2014年1月6日
    作    者   : zhangchaoyong

    修改内容   : 新生成函数

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
 函 数 名  : vos_stoptimer
 功能描述  : 停止一个定时器
 输入参数  : vos_u32 ultimerid
 输出参数  : 无
 返 回 值  : vos_u32 
 
 修改历史      :
  1.日    期   : 2014年1月6日
    作    者   : zhangchaoyong

    修改内容   : 新生成函数

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
 函 数 名  : vos_settimertimelen
 功能描述  : 设置定时器定时长度
 输入参数  : vos_u32 ultimerid
              vos_u32 ulmillisecond
 输出参数  : 无
 返 回 值  : vos_u32
 
 修改历史      :
  1.日    期   : 2014年1月6日
    作    者   : zhangchaoyong

    修改内容   : 新生成函数

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
 函 数 名  : vos_printalltimerinfo
 功能描述  : 打印所有定时器信息
 输入参数  : void
 输出参数  : 无
 返 回 值  : void
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2014年9月4日
    作    者   : Albort,Feng
    修改内容   : 新生成函数

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
 函 数 名  : vos_gettimerreginfo
 功能描述  : 获取定时器注册信息
 输入参数  : vos_u32 ultimerid
             vos_timer_reg_stru* pouttimerinfo
 输出参数  : 无
 返 回 值  : vos_u32
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2014年9月17日
    作    者   : Albort,Feng
    修改内容   : 新生成函数

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
 函 数 名  : vos_gettimerrunedtimelen
 功能描述  : 获取定时器已运行时间长度
 输入参数  : vos_u32 ultimerid
 输出参数  : 无
 返 回 值  : vos_u32 (定时器已运行时间长度,单位为100ms)
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2014年9月17日
    作    者   : Albort,Feng
    修改内容   : 新生成函数

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
 函 数 名  : vos_gettimerstatus
 功能描述  : 获取指定定时器的当前状态
 输入参数  : vos_u32 ultimerid
             vos_u32* pulouttimerstatus
 输出参数  : 无
 返 回 值  : vos_u32
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2015年5月15日
    作    者   : Albort,Feng
    修改内容   : 新生成函数

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
 函 数 名  : vos_basetimerproc
 功能描述  : 定时模块公共基准定时器处理函数
 输入参数  : void
 输出参数  : 无
 返 回 值  : void
 
 修改历史      :
  1.日    期   : 2014年1月8日
    作    者   : zhangchaoyong

    修改内容   : 新生成函数

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

        //定时未到
        if (pstnode->ulcounter < pstnode->ultimelen)
        {
            pstnode = VOS_NULL;
            
            (void)vos_getcurrentopnode(g_timerlistid, (void **)(&pstnode));
            
            continue;
        }
                
        //定时到,发送定时超时消息
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
        
        //一次性定时器设置为非活动
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
 函 数 名  : vos_init_timermd
 功能描述  : 初始化定时器模块
 输入参数  : void
 输出参数  : 无
 返 回 值  : vos_u32 （VOS_OK：初始化成功；其他：初始化失败错误码）
 
 修改历史      :
  1.日    期   : 2014年1月6日
    作    者   : zhangchaoyong

    修改内容   : 新生成函数

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


