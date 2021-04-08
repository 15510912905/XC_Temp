#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <google/protobuf/stubs/common.h>
#include "app_errorcode.h"
#include "app_msgtypedefine.h"
#include "app_init.h"
#include "app_main.h"
#include "app_msg.h"
#include "vos_init.h"
#include <string>
#include "socketmonitor.h"
#include "oammng.h"
#include "vos_log.h"
#include "vos_selfsocketcommu.h"

using namespace std;

#ifdef __cplusplus
extern "C" {
#endif

STATIC vos_u32 g_ulterminateflag = APP_FALSE;
const vos_u32 g_LampFullBright = 100; //光源灯全亮

/*****************************************************************************
 函 数 名  : app_terminateprocess
 功能描述  : 终止APP线程
 输入参数  : void
 输出参数  : 无
 返 回 值  : void
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2014年7月11日
    作    者   : Albort,Feng
    修改内容   : 新生成函数

*****************************************************************************/
void app_terminateprocess(void)
{
    g_ulterminateflag = APP_TRUE;
    
    return;
}

/*****************************************************************************
 函 数 名  : app_getterminateflag
 功能描述  : 获取进程终止标志
 输入参数  : void
 输出参数  : 无
 返 回 值  : vos_u32
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2014年7月11日
    作    者   : Albort,Feng
    修改内容   : 新生成函数

*****************************************************************************/
vos_u32 app_getterminateflag(void)
{
    return g_ulterminateflag;
}

vos_u32 app_oamInit()
{
    //初始化OAM模块
    COamMng * pOamMng = new COamMng(SPid_Oammng);
    vos_u32 ulrunrslt = pOamMng->RegCallBack(app_self_oam_fid);
    if (VOS_OK != ulrunrslt)
    {
        return ulrunrslt;
    }

    pOamMng->InitMachine();
    return VOS_OK;
}

/*****************************************************************************
 函 数 名  : main
 功能描述  : 产品中位机应用程序入口
 输入参数  : app_i32 argc
             app_i8 *argv[]
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2014年4月2日
    作    者   : Albort,Feng
    修改内容   : 新生成函数

*****************************************************************************/
int main(int argc, char *argv[])
{
    GOOGLE_PROTOBUF_VERIFY_VERSION;
    vos_u32 ulrunrslt  = VOS_OK;
    #ifdef COMPILER_IS_ARM_LINUX_GCC
    system("ulimit -c unlimited");
    vos_threadsleep(2000);
    #endif

    //应用程序运行起来后工作目录退出到上级目录
    ulrunrslt = chdir("..");
    if (VOS_OK != ulrunrslt)
    {
        WRITE_ERR_LOG(" Change work path failed\n");
        return APP_COM_ERR;
    }

    //开始启动vos平台
    printf(" xc begin to boot vos platform at %s \r\n", GetCurTimeStr());    
    ulrunrslt = vos_startplatform();
    if (VOS_OK != ulrunrslt)
    {
        WRITE_ERR_LOG("call vos_startplatform failed(0x%x) at %s \r\n", ulrunrslt, GetCurTimeStr());
        return ulrunrslt;
    }
    printf(" xc boot vos platform ok at %s\r\n", GetCurTimeStr());

    //开始初始化产品应用程序
    printf(" xc begin to boot app at %s \r\n", GetCurTimeStr());    
    ulrunrslt = app_productinit();
    if (VOS_OK != ulrunrslt)
    {
        WRITE_ERR_LOG(" call app_productinit failed(0x%lx)\n", ulrunrslt);
        return ulrunrslt;
    }
    printf("xc boot app ok at %s\n", GetCurTimeStr());
    
    ulrunrslt = app_oamInit();
    if (VOS_OK != ulrunrslt)
    {
        WRITE_ERR_LOG("Call app_oamInit() failed(0x%x)", ulrunrslt);
        return ulrunrslt;
    }
    
    g_pMonitor = new CSocketMonitor(SPid_SocketMonitor);
    g_pMonitor->RegCallBack(app_self_net_fid);

    ulrunrslt = vos_initsockcommumd();
    if (VOS_OK != ulrunrslt)
    {
        return ulrunrslt;
    }

    printf("\r->");
    
    //等待上位机发配任务
    while (1)
    {
        if (APP_TRUE == g_ulterminateflag)
        {
            break;
        }
        vos_threadsleep(10000);
    }
    google::protobuf::ShutdownProtobufLibrary();
    return VOS_OK;
}

#ifdef __cplusplus
}
#endif

