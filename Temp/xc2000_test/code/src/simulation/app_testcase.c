#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include "vos_pid.h"
#include "app_msgtypedefine.h"
#include "app_errorcode.h"
#include "app_debugtty.h"
#include "xc8002_middwnmachshare.h"
#include "app_msg.h"
#include "vos_log.h"
#include "alarm.h"
#include "app_simulation.h"
#include "app_debugcmd.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

//定义下位机故障上报
typedef struct
{
    vos_u32 faultid;
    vos_u32 boardid;
    vos_u32 deviceid;
}app_dwnmach_faultreport_stru;

extern app_u16 g_usdeclinehigh;                 //可配置的上报余量值
app_dwnmach_faultreport_stru* g_pstfaultidmng = VOS_NULL;  //故障faultid; boardid; deviceid管理

#define FAULTCOUNT (50) //可以注册的故障数
#define FUNCCOUNT (10)  //可以注册的测试函数数


#define FAULTIDMATCH(N,faultid,boardid,deviceid) \
    if ((N) < FAULTCOUNT) \
    {\
        g_pstfaultidmng[(N)].faultid  = faultid; \
        g_pstfaultidmng[(N)].boardid  = boardid; \
        g_pstfaultidmng[(N)].deviceid = deviceid; \
    }


#define RegisterFaultFc(n) \
    if(VOS_OK == ulRunRslt) \
    { \
        ulRunRslt = app_registerFaultFc(stFaultFc[(n)]); \
    } \
    else \
    { \
        printf("app_registerFaultFc failed! \n"); \
    }
    
typedef vos_u32 (*TESTFUN)(void* pstpara);

//测试函数管理
typedef struct
{
    app_msgtype_enum msgtype;
    TESTFUN fc;
}app_test_func_mng_stru;

app_test_func_mng_stru* g_psttestfunmng = VOS_NULL;


vos_u32 app_testReagSysR1Vrtfault()
{
//      app_faultFc_mng_stru stFaultFc[] =
//      {
//          { cmd_auto_wash_ndl_exe_washing, 5, INVALID_FAULTID, app_constructSingleCmdFault },
//          { cmd_r1_reagent_ndl_absorb_reagent, 2, INVALID_FAULTID, app_constructSingleCmdFault },
//      };
//      
//     app_cleanFaultFc();
//     RegisterFaultFc(0);

    char tasknum[] = "10";

    return app_tasktest(tasknum);
}

vos_u32 app_testReagSysR1DischPosFault()
{
    vos_u32 ulRunRslt = VOS_OK;
    app_faultFc_mng_stru stFaultFc[] =
    {
        { cmd_r1_reagent_ndl_vdecline, 3, INVALID_FAULTID, VOS_NULL, app_constructRemainFault},
    };

    app_cleanFaultFc();
    RegisterFaultFc(0);
    return VOS_OK;
}


/*****************************************************************************
 函 数 名  : app_simulFaultReport
 功能描述  : 模拟上报故障
 输入参数  : faultid
 输出参数  : 无
 返 回 值  : vos_u32
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2016年1月12日
    作    者   : zxc
    修改内容   : 新生成函数

*****************************************************************************/
vos_u32 app_simuReportFault(vos_u32 faultid)
{
    vos_u32 loop = 0;
    vos_u32 ulRunRslt = VOS_OK;

    for (loop = 0; loop < FAULTCOUNT; loop++)
    {
        if (g_pstfaultidmng[loop].faultid == faultid)
        {
            app_dwnmach_faultreport_stru* pstfaultidmng = (app_dwnmach_faultreport_stru*)&(g_pstfaultidmng[loop]);
            g_pAlarm->GenerateAlarm(faultid, pstfaultidmng->boardid, pstfaultidmng->deviceid);
            break;
        }
    }
    return ulRunRslt;
}


/*****************************************************************************
 函 数 名  : app_procTestResult
 功能描述  : 处理测试结果
 输入参数  : app_msghead * msg
 输出参数  : 无
 返 回 值  : vos_u32
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2016年1月13日
    作    者   : zxc
    修改内容   : 新生成函数

*****************************************************************************/
vos_u32 app_procTestResult(vos_msg_header_stru* pstvosmsg)
{
    vos_u32 ulRunRslt = VOS_OK;
    
    return ulRunRslt;
}


/*****************************************************************************
 函 数 名  : app_cleanTestFun
 功能描述  : 清空测试函数
 输入参数  : void
 输出参数  : 无
 返 回 值  : void
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2016年1月19日
    作    者   : zxc
    修改内容   : 新生成函数

*****************************************************************************/
void app_cleanTestFun( void )
{
    vos_u32 loop = 0;

    for (loop = 0; loop < FUNCCOUNT; loop++)
    {
        g_psttestfunmng[loop].msgtype   = vos_appmsgtype_max;
        g_psttestfunmng[loop].fc        = VOS_NULL;
    }
    
    return;
}

/*****************************************************************************
 函 数 名  : app_registTestFunc
 功能描述  : 注册测试函数
 输入参数  : 
 输出参数  : 无
 返 回 值  : vos_u32
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2016年1月15日
    作    者   : zxc
    修改内容   : 新生成函数

*****************************************************************************/
vos_u32 app_registTestFunc(vos_u32 msgtype, TESTFUN fc)
{
    vos_u32 loop;
    for (loop = 0; loop < FUNCCOUNT; loop++)
    {
        if (g_psttestfunmng[loop].msgtype == vos_appmsgtype_max)
        {
            g_psttestfunmng[loop].msgtype = (app_msgtype_enum)msgtype;
            g_psttestfunmng[loop].fc      = fc;
            return VOS_OK;
        }
    }

    return APP_INVALID_U32;  	
}


/*****************************************************************************
 函 数 名  : app_registerFaultid
 功能描述  : 注册下位机故障id
 输入参数  : void
 输出参数  : 无
 返 回 值  : void
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2016年1月12日
    作    者   : zxc
    修改内容   : 新生成函数

*****************************************************************************/
vos_u32 app_registerFaultid( void )
{
    vos_u32 ulRunRslt = VOS_OK;
        
    //FAULTIDMATCH(0, 0, 0, 0);
    g_pstfaultidmng[0].faultid  = 0; 
    g_pstfaultidmng[0].boardid  = 0; 
    g_pstfaultidmng[0].deviceid = 0;
    //......
    
    return ulRunRslt;
}


/*****************************************************************************
 函 数 名  : app_initTestCase
 功能描述  : 初始化测试用例
 输入参数  : void
 输出参数  : 无
 返 回 值  : vos_u32
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2016年1月12日
    作    者   : zxc
    修改内容   : 新生成函数

*****************************************************************************/
vos_u32 app_initTestCase( void )
{
    vos_u32 ulRunRslt = VOS_OK;

    //分配50个故障注册
    vos_u32 ulmemsize = 0;
    ulmemsize = FAULTCOUNT*sizeof(app_dwnmach_faultreport_stru);   
    ulRunRslt = vos_mallocmemory(ulmemsize, vos_mem_type_static, (void **)(&g_pstfaultidmng));
    if (VOS_OK != ulRunRslt) 
    {
        WRITE_ERR_LOG("Call vos_mallocmemory() failed(0x%x) \r\n",ulRunRslt);
        return ulRunRslt;
    }
    //下位机故障注册
    app_registerFaultid();
    
    //分配10个测试函数注册
    ulmemsize = FUNCCOUNT*sizeof(app_test_func_mng_stru);
    ulRunRslt = vos_mallocmemory(ulmemsize, vos_mem_type_static, (void **)(&g_psttestfunmng));
    if (VOS_OK != ulRunRslt) 
    {
        WRITE_ERR_LOG("Call vos_mallocmemory() failed(0x%x) \r\n",ulRunRslt);
        return ulRunRslt;
    }
    //清空测试函数
    app_cleanTestFun();
    //注册测试函数
    //app_registTestFunc();


    
    return ulRunRslt;
}




#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */
