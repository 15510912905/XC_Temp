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
const vos_u32 g_LampFullBright = 100; //��Դ��ȫ��

/*****************************************************************************
 �� �� ��  : app_terminateprocess
 ��������  : ��ֹAPP�߳�
 �������  : void
 �������  : ��
 �� �� ֵ  : void
 ���ú���  : 
 ��������  : 
 
 �޸���ʷ      :
  1.��    ��   : 2014��7��11��
    ��    ��   : Albort,Feng
    �޸�����   : �����ɺ���

*****************************************************************************/
void app_terminateprocess(void)
{
    g_ulterminateflag = APP_TRUE;
    
    return;
}

/*****************************************************************************
 �� �� ��  : app_getterminateflag
 ��������  : ��ȡ������ֹ��־
 �������  : void
 �������  : ��
 �� �� ֵ  : vos_u32
 ���ú���  : 
 ��������  : 
 
 �޸���ʷ      :
  1.��    ��   : 2014��7��11��
    ��    ��   : Albort,Feng
    �޸�����   : �����ɺ���

*****************************************************************************/
vos_u32 app_getterminateflag(void)
{
    return g_ulterminateflag;
}

vos_u32 app_oamInit()
{
    //��ʼ��OAMģ��
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
 �� �� ��  : main
 ��������  : ��Ʒ��λ��Ӧ�ó������
 �������  : app_i32 argc
             app_i8 *argv[]
 �������  : ��
 �� �� ֵ  : 
 ���ú���  : 
 ��������  : 
 
 �޸���ʷ      :
  1.��    ��   : 2014��4��2��
    ��    ��   : Albort,Feng
    �޸�����   : �����ɺ���

*****************************************************************************/
int main(int argc, char *argv[])
{
    GOOGLE_PROTOBUF_VERIFY_VERSION;
    vos_u32 ulrunrslt  = VOS_OK;
    #ifdef COMPILER_IS_ARM_LINUX_GCC
    system("ulimit -c unlimited");
    vos_threadsleep(2000);
    #endif

    //Ӧ�ó���������������Ŀ¼�˳����ϼ�Ŀ¼
    ulrunrslt = chdir("..");
    if (VOS_OK != ulrunrslt)
    {
        WRITE_ERR_LOG(" Change work path failed\n");
        return APP_COM_ERR;
    }

    //��ʼ����vosƽ̨
    printf(" xc begin to boot vos platform at %s \r\n", GetCurTimeStr());    
    ulrunrslt = vos_startplatform();
    if (VOS_OK != ulrunrslt)
    {
        WRITE_ERR_LOG("call vos_startplatform failed(0x%x) at %s \r\n", ulrunrslt, GetCurTimeStr());
        return ulrunrslt;
    }
    printf(" xc boot vos platform ok at %s\r\n", GetCurTimeStr());

    //��ʼ��ʼ����ƷӦ�ó���
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
    
    //�ȴ���λ����������
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

