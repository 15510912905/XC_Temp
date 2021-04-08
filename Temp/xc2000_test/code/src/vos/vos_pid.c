/******************************************************************************

                  版权所有 (C), 2013-2023, 四川新成生物科技有限公司

 ******************************************************************************
  文 件 名   : vos_pid.c
  版 本 号   : 初稿
  作    者   : Albort,Feng
  生成日期   : 2013年9月9日
  最近修改   :
  功能描述   : PID模块功能定义文件
  函数列表   :
  修改历史   :
  1.日    期   : 2013年9月9日
    作    者   : Albort,Feng
    修改内容   : 创建文件

******************************************************************************/
#include <memory.h>
#include "vos_basictypedefine.h"
#include "vos_configure.h"
#include "vos_errorcode.h"
#include "vos_mngmemory.h"
#include "vos_messagecenter.h"
#include "vos_selffid.h"
#include "vos_selfpid.h"
#include "vos_pid.h"
#include "vos_init.h"
#include "vos_log.h"
#include "upmsg.pb.h"

/*在每个FID阶段要出去FID号本身后最大PID数由系统配置减一*/
#define VOS_SELF_MAX_PID_NUM_PER_FID  (VOS_MAX_PID_NUM_PER_FID - 1)

//PID存在标志枚举定义
typedef enum
{
    vos_pid_register_yes = 0,  //PID已被注册
    vos_pid_register_no  = 1,  //PID未被注册

    vos_pid_register_butt
}vos_pid_register_enum;
typedef vos_u32 vos_pid_register_enum_u32;


//PID注册信息管理结构
typedef struct
{
    vos_pid_register_info_stru* apidreginfo[VOS_MAX_SUPPORT_FID_NUM];
}vos_mng_pid_reginfo_stru;

#ifdef __cplusplus
extern "C" {
#endif

STATIC vos_mng_pid_reginfo_stru* g_pstmngpidreginfo = VOS_NULL;
STATIC void* g_pmutexforpidmng = VOS_NULL;

/*****************************************************************************
 函 数 名  : vos_getsavepidinfobufheadforfid
 功能描述  : 获取指定FID模块的PID信息缓存首地址,调用接口务必保证FID正确
 输入参数  : vos_u32 ulfid
 输出参数  : 无
 返 回 值  : vos_pid_register_info_stru*
 
 修改历史      :
  1.日    期   : 2013年9月11日
    作    者   : Albort,Feng

    修改内容   : 新生成函数

*****************************************************************************/
vos_pid_register_info_stru* vos_getsavepidinfobufheadforfid(vos_u32 ulfid)
{
    vos_u32 ulrunrslt = VOS_OK;
    vos_u32 ulindex   = 0;

    ulrunrslt = vos_checkfidvalidity(ulfid);
    if (VOS_OK != ulrunrslt)
    {
        WRITE_ERR_LOG("call vos_checkfidvalidity failed(0x%x)\r\n", ulrunrslt);
        return VOS_NULL;
    }
    
    ulindex = vos_fidmaptoindex(ulfid);

    /*取得当前FID模块存储PID注册信息的首地址*/
    return (g_pstmngpidreginfo->apidreginfo[ulindex]);
}

/*****************************************************************************
 函 数 名  : vos_mallocpidreginfomem
 功能描述  : 为一个FID申请保存PID注册信息的内存
 输入参数  : vos_u32 ulfid
 输出参数  : 无
 返 回 值  : vos_u32
 
 修改历史      :
  1.日    期   : 2013年9月10日
    作    者   : Albort,Feng

    修改内容   : 新生成函数

*****************************************************************************/
vos_u32 vos_mallocpidreginfomem(vos_u32 ulfid)
{
    vos_u32 ulindex   = 0;
    vos_u32 ulmemsize = 0;
    vos_u32 ulrunrslt = VOS_OK;
    vos_u32 ulloopvar = 0;
    
    vos_pid_register_info_stru* psttmpbuf = VOS_NULL;

    ulindex = vos_fidmaptoindex(ulfid);

    if (VOS_MAX_SUPPORT_FID_NUM <= ulindex)
    {
        RETURNERRNO(vos_pid_errorcode_base, vos_array_index_error);
    }
    
    vos_obtainmutex(g_pmutexforpidmng);
    
    if (VOS_NULL != g_pstmngpidreginfo->apidreginfo[ulindex])
    {//已经申请不需重复申请
        vos_releasemutex(g_pmutexforpidmng);
        
        return VOS_OK;
    }

    vos_releasemutex(g_pmutexforpidmng);

    ulmemsize = VOS_SELF_MAX_PID_NUM_PER_FID * sizeof(vos_pid_register_info_stru);
    ulrunrslt = vos_mallocmemory(ulmemsize, vos_mem_type_static,
                                 (void**)&psttmpbuf);
    if (VOS_OK != ulrunrslt)
    {
        WRITE_ERR_LOG(
                    "call vos_mallocmemory failed(0x%x)\r\n",
                    ulrunrslt);
        return ulrunrslt;
    }

    for (ulloopvar = 0; VOS_SELF_MAX_PID_NUM_PER_FID > ulloopvar; ulloopvar++)
    {
        (psttmpbuf + ulloopvar)->ulpid         = VOS_INVALID_PID;
        (psttmpbuf + ulloopvar)->pidrcvmsgfunc = VOS_NULL;
        (psttmpbuf + ulloopvar)->pCallBack = VOS_NULL;
    }

    vos_obtainmutex(g_pmutexforpidmng);
    
    g_pstmngpidreginfo->apidreginfo[ulindex] = psttmpbuf;
    
    vos_releasemutex(g_pmutexforpidmng);
    
    return VOS_OK;
}

/*****************************************************************************
 函 数 名  : vos_freepidreginfomem
 功能描述  : 释放保存PID注册信息的内存
 输入参数  : vos_u32 ulfid
 输出参数  : 无
 返 回 值  : void
 
 修改历史      :
  1.日    期   : 2013年9月17日
    作    者   : Albort,Feng

    修改内容   : 新生成函数

*****************************************************************************/
void vos_freepidreginfomem(vos_u32 ulfid)
{
    vos_u32 ulrunrslt = VOS_OK;
    vos_u32 ulindex   = 0;

    ulrunrslt = vos_checkfidvalidity(ulfid);
    if (VOS_OK != ulrunrslt)
    {
        WRITE_ERR_LOG(
                    "call vos_checkfidvalidity failed(0x%x)\r\n",
                    ulrunrslt);
        return;
    }

    ulindex = vos_fidmaptoindex(ulfid);
    if (VOS_MAX_SUPPORT_FID_NUM <= ulindex)
    {
        return;
    }

    vos_obtainmutex(g_pmutexforpidmng);
    
    if (VOS_NULL == g_pstmngpidreginfo->apidreginfo[ulindex])
    {//已经释放无需重复释放
        vos_releasemutex(g_pmutexforpidmng);
        
        return;
    }

    vos_freememory((void*)(g_pstmngpidreginfo->apidreginfo[ulindex]));

    g_pstmngpidreginfo->apidreginfo[ulindex] = VOS_NULL;

    vos_releasemutex(g_pmutexforpidmng);
    
    return;
}


/*****************************************************************************
 函 数 名  : vos_initpidmd
 功能描述  : PID模块初始化接口
 输入参数  : void
 输出参数  : 无
 返 回 值  : vos_u32
 
 修改历史      :
  1.日    期   : 2013年9月9日
    作    者   : Albort,Feng

    修改内容   : 新生成函数

*****************************************************************************/
vos_u32 vos_initpidmd(void)
{
    vos_u32 ulrunrslt = VOS_OK;
    vos_u32 ulmemsize = 0;
    vos_u32 ulloopvar = 0;

    g_pmutexforpidmng = vos_mallocmutex();
    if (VOS_NULL == g_pmutexforpidmng)
    {
        RETURNERRNO(vos_pid_errorcode_base,vos_malloc_mutex_error);
    }

    ulrunrslt = vos_createmutex(g_pmutexforpidmng);
    if (VOS_OK != ulrunrslt)
    {
        vos_freemutex(g_pmutexforpidmng);
        g_pmutexforpidmng = VOS_NULL;
        
        return ulrunrslt;
    }
    
    ulmemsize = sizeof(vos_mng_pid_reginfo_stru);
    ulrunrslt = vos_mallocmemory(ulmemsize, vos_mem_type_static,
                                 (void**)&g_pstmngpidreginfo);
    if (VOS_OK != ulrunrslt)
    {   
        vos_deletemutex(g_pmutexforpidmng);
        vos_freemutex(g_pmutexforpidmng);
        g_pmutexforpidmng = VOS_NULL;
        
        return ulrunrslt;
    }

    for (ulloopvar = 0; VOS_MAX_SUPPORT_FID_NUM > ulloopvar; ulloopvar++)
    {
        g_pstmngpidreginfo->apidreginfo[ulloopvar] = VOS_NULL;
    }
    
    return VOS_OK;
}

/*****************************************************************************
 函 数 名  : vos_checkpidregistered
 功能描述  : 检查PID是否已被注册
 输入参数  : vos_u32 ulfid
             vos_u32 ulpid
 输出参数  : 无
 返 回 值  : vos_u32
 
 修改历史      :
  1.日    期   : 2013年10月24日
    作    者   : Albort,Feng

    修改内容   : 新生成函数

*****************************************************************************/
vos_u32 vos_checkpidregistered(vos_u32 ulfid, vos_u32 ulpid)
{
    vos_u32 ulloopvar = 0;
    
    vos_pid_register_info_stru* pstmove = VOS_NULL;

    /*获取属于当前FID存储PID注册信息的存储区*/
    pstmove = vos_getsavepidinfobufheadforfid(ulfid);
    for (ulloopvar = 0; VOS_SELF_MAX_PID_NUM_PER_FID > ulloopvar; ulloopvar++)
    {
        if (ulpid == pstmove->ulpid)
        {
            break;
        }

        pstmove += 1;
    }

    if (VOS_SELF_MAX_PID_NUM_PER_FID <= ulloopvar)
    {
        return vos_pid_register_no;
    }

    return vos_pid_register_yes;
}
vos_u32 vos_checkFidAndPid(vos_u32 ulfid, vos_u32 ulpid)
{
    vos_u32 ulrunrslt = VOS_OK;
    ulrunrslt = vos_checkfidvalidity(ulfid);
    if (VOS_OK != ulrunrslt)
    {
        WRITE_ERR_LOG("call vos_checkfidvalidity failed(0x%x)\r\n", ulrunrslt);
        return ulrunrslt;
    }

    /*检查PID是否合法*/
    if ((ulpid < ulfid) || (ulpid >= (ulfid + VOS_MAX_PID_NUM_PER_FID)))
    {
        RETURNERRNO(vos_pid_errorcode_base, vos_pid_invalid_error);
    }

    ulrunrslt = vos_checkpidregistered(ulfid, ulpid);
    if (vos_pid_register_yes == ulrunrslt)
    {
        RETURNERRNO(vos_pid_errorcode_base, vos_pid_invalid_error);
    }
    return VOS_OK;
}
/*****************************************************************************
函 数 名  : vos_getpidreginfo
功能描述  : 获取PID注册信息
输入参数  : vos_u32 ulfid
vos_u32 ulpid
输出参数  : 无
返 回 值  : vos_pid_register_info_stru*

修改历史      :
1.日    期   : 2013年10月24日
作    者   : Albort,Feng

修改内容   : 新生成函数

*****************************************************************************/
vos_pid_register_info_stru* vos_getpidreginfo(vos_u32 ulfid, vos_u32 ulpid)
{
    vos_u32 ulloopvar = 0;

    vos_pid_register_info_stru* pstmove = VOS_NULL;

    pstmove = vos_getsavepidinfobufheadforfid(ulfid);
    if (VOS_NULL == pstmove)
    {
        return VOS_NULL;
    }

    for (ulloopvar = 0; VOS_SELF_MAX_PID_NUM_PER_FID > ulloopvar; ulloopvar++)
    {
        if (ulpid == pstmove->ulpid)
        {
            break;
        }

        pstmove += 1;
    }

    if (VOS_SELF_MAX_PID_NUM_PER_FID <= ulloopvar)
    {
        return VOS_NULL;
    }

    return pstmove;
}

/*****************************************************************************
 函 数 名  : vos_deliverymsgtopid
 功能描述  : 消息分发，即根据消息目的PID将消息投递给相应的PID模块
 输入参数  : vos_msg_header_stru* pstmsg
 输出参数  : 无
 返 回 值  : vos_u32
 
 修改历史      :
  1.日    期   : 2013年9月6日
    作    者   : Albort,Feng

    修改内容   : 新生成函数

*****************************************************************************/
vos_u32 vos_deliverymsgtopid(vos_msg_header_stru* pstmsg)
{
    vos_u32 ulrunrslt = VOS_OK;
    vos_u32 ulfid     = VOS_INVALID_FID;

    vos_pid_register_info_stru* pstpidreginf = VOS_NULL;

    if (VOS_NULL == pstmsg)
    {
        RETURNERRNO(vos_pid_errorcode_base, vos_para_null_err);
    }

    ulfid     = vos_pidmaptofid(pstmsg->uldstpid);
    ulrunrslt = vos_checkfidvalidity(ulfid);
    if (VOS_OK != ulrunrslt)
    {
        WRITE_ERR_LOG(
                    "call vos_checkfidvalidity failed(0x%x)\r\n",
                    ulrunrslt);
        RETURNERRNO(vos_pid_errorcode_base, vos_fid_invalid_error);
    }

    /*获取目标PID注册信息*/
    pstpidreginf = vos_getpidreginfo(ulfid, pstmsg->uldstpid);

    /*判断PID是否注册以及是否注册消息接收回调*/
    if ((VOS_NULL == pstpidreginf))
    {
        RETURNERRNO(vos_pid_errorcode_base, vos_pid_unreg_error);
    }
    if (VOS_NULL != pstpidreginf->pidrcvmsgfunc)
    {
        /*将消息提交给目标PID模块*/
        ulrunrslt = pstpidreginf->pidrcvmsgfunc(pstmsg);
    }
    else if (VOS_NULL != pstpidreginf->pCallBack)
    {

        pstpidreginf->pCallBack->CallBack(pstmsg);
    }
    else
    {
        RETURNERRNO(vos_pid_errorcode_base, vos_pid_unreg_error);
    }

    return ulrunrslt;
}

/*****************************************************************************
 函 数 名  : vos_checkpidvalidity
 功能描述  : PID有效性校验
 输入参数  : vos_u32 ulpid
 输出参数  : 无
 返 回 值  : vos_u32
 
 修改历史      :
  1.日    期   : 2013年9月11日
    作    者   : Albort,Feng

    修改内容   : 新生成函数

*****************************************************************************/
vos_u32 vos_checkpidvalidity(vos_u32 ulpid)
{
    vos_u32 ulfid     = VOS_INVALID_FID;
    vos_u32 ulrunrslt = VOS_OK;

    if (VOS_INVALID_PID == ulpid)
    {
        RETURNERRNO(vos_pid_errorcode_base, vos_pid_invalid_error);
    }
    
    ulfid = vos_pidmaptofid(ulpid);
    ulrunrslt = vos_checkfidvalidity(ulfid);
    if (VOS_OK != ulrunrslt)
    {
        WRITE_ERR_LOG(
                    "call vos_checkfidvalidity failed(0x%x)\r\n",
                    ulrunrslt);
        
        return ulrunrslt;
    }

    ulrunrslt = vos_checkpidregistered(ulfid, ulpid);
    if (vos_pid_register_yes != ulrunrslt)
    {
        RETURNERRNO(vos_pid_errorcode_base, vos_pid_invalid_error);
    }

    return VOS_OK;
}

/*****************************************************************************
 函 数 名  : vos_utreleasepid
 功能描述  : 释放PID模块
 输入参数  : vos_u32 ulpid
 输出参数  : 无
 返 回 值  : vos_u32
 
 修改历史      :
  1.日    期   : 2013年9月11日
    作    者   : Albort,Feng

    修改内容   : 新生成函数

*****************************************************************************/
vos_u32 vos_utreleasepid(vos_u32 ulpid)
{
    vos_u32 ulrunrslt = VOS_OK;
    vos_u32 ulfid     = VOS_INVALID_FID;

    vos_pid_register_info_stru* pstpidreginfo = VOS_NULL;

    ulrunrslt = vos_checkpidvalidity(ulpid);
    if (VOS_OK != ulrunrslt)
    {
        return ulrunrslt;
    }
    
    ulfid     = vos_pidmaptofid(ulpid);
    ulrunrslt = vos_checkfidvalidity(ulfid);
    if (VOS_OK != ulrunrslt)
    {
        return ulrunrslt;
    }

    /*获取PID注册信息*/
    pstpidreginfo = vos_getpidreginfo(ulfid, ulpid);

    pstpidreginfo->pidrcvmsgfunc = VOS_NULL;
    pstpidreginfo->pCallBack = VOS_NULL;
    pstpidreginfo->ulpid         = VOS_INVALID_PID;
   
    return VOS_OK;
}

/*****************************************************************************
 函 数 名  : vos_utreleaseallpidoffid
 功能描述  : 释放一个FID模块中所有的PID
 输入参数  : vos_u32 ulfid
 输出参数  : 无
 返 回 值  : void
 
 修改历史      :
  1.日    期   : 2013年9月11日
    作    者   : Albort,Feng

    修改内容   : 新生成函数

*****************************************************************************/
void vos_utreleaseallpidoffid(vos_u32 ulfid)
{
    vos_freepidreginfomem(ulfid);
    
    return;
}

//PID模块注册接口
vos_u32 vos_registerpid(vos_u32 ulfid, vos_u32 ulpid, vos_pidrcvmsgfunc prcvmsgfunc)
{
    if (VOS_NULL == prcvmsgfunc)
    {
        RETURNERRNO(vos_pid_errorcode_base, vos_para_null_err);
    }

    if (VOS_OK != vos_checkFidAndPid(ulfid, ulpid))
    {
        RETURNERRNO(vos_pid_errorcode_base, vos_pid_invalid_error);
    }

    vos_pid_register_info_stru* pstmove = VOS_NULL;
    pstmove = vos_getpidreginfo(ulfid, VOS_INVALID_PID);
    if (VOS_NULL == pstmove)
    {
        RETURNERRNO(vos_pid_errorcode_base, vos_pid_registered_error);
    }

    pstmove->ulpid = ulpid; //本次注册的PID
    pstmove->pidrcvmsgfunc = prcvmsgfunc;

    return VOS_OK;
}

#ifdef __cplusplus
}
#endif

vos_u32 vos_registerCallBack(vos_u32 ulfid, vos_u32 ulpid, CCallBack * pBack)
{
    if (VOS_NULL == pBack)
    {
        RETURNERRNO(vos_pid_errorcode_base, vos_para_null_err);
    }

    if (VOS_OK != vos_checkFidAndPid(ulfid, ulpid))
    {
        RETURNERRNO(vos_pid_errorcode_base, vos_pid_invalid_error);
    }

    vos_pid_register_info_stru* pstmove = VOS_NULL;
    pstmove = vos_getpidreginfo(ulfid, VOS_INVALID_PID);
    if (VOS_NULL == pstmove)
    {
        RETURNERRNO(vos_pid_errorcode_base, vos_pid_registered_error);
    }

    pstmove->ulpid = ulpid; //本次注册的PID
    pstmove->pCallBack = pBack;

    return VOS_OK;
}

CCallBack::CCallBack(vos_u32 ulpid) :m_ulFid(0), m_ulPid(ulpid)
{

}
CCallBack::~CCallBack()
{

}
vos_u32 CCallBack::RegCallBack(vos_u32 ulfid)
{
    m_ulFid = ulfid;
    return vos_registerCallBack(ulfid, m_ulPid, this);
}

