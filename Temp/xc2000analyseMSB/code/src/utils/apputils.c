#include "apputils.h"
#include "app_innermsg.h"
#include "app_msg.h"
#include "vos_pid.h"
#include "typedefine.pb.h"
#include "app_comstructdefine.h"
#include "upmsg.pb.h"
#include <map>
#include <sys/ioctl.h>
#include "app_debugtty.h"
#include <stdarg.h>
#include "vos_log.h"
#include "app_comstructdefine.h"
#include "inituart.h"
#include <time.h>
#include "task.h"

using namespace std;

#define APP_MAX_PRINT_BUF_LENGTH       (512)

vos_u32 g_P04Flag = 0;   //p04(齿轮增压泵控制标示)当为0时关泵
extern map<app_u16, app_u16> g_uartHdl;
STATIC vos_u32 g_ulmdprintswitch;

void OPEN_P04(vos_u32 srcpid)
{
    if (0 == g_P04Flag)
    {
        msg_PVCtrl stPara;
        stPara.operateType = 1;
        stPara.PVNum = (srcpid == SPid_SmpNdl || srcpid == SPid_R1 || srcpid == SPid_R2) ? Press_our : Press_in;
        app_reqDwmCmd(srcpid, SPid_Press, Act_PVCtrl, sizeof(stPara), (char*)&stPara, APP_NO);
    }
    g_P04Flag++;
}

//关闭p04
void CLOSE_P04(vos_u32 srcpid) 
{
    if (0 < g_P04Flag)
    {
        g_P04Flag--;
        if (0 == g_P04Flag)
        {
            msg_PVCtrl stPara;
            stPara.operateType = 0;
            stPara.PVNum = (srcpid == SPid_SmpNdl || srcpid == SPid_R1 || srcpid == SPid_R2) ? Press_our : Press_in;
            app_reqDwmCmd(srcpid, SPid_Press, Act_PVCtrl, sizeof(stPara), (char*)&stPara, APP_NO);
        }
    }
}

//初始化p04,防止g_P04Flag变量异常导致p04失控
void INITP04(vos_u32 srcpid, vos_u16 pNum /*= Press_our*/)
{
    if (0 != g_P04Flag)
    {
        g_P04Flag = 0;
        msg_PVCtrl stPara;
        stPara.operateType = 0;
        stPara.PVNum = pNum;
        app_reqDwmCmd(srcpid, SPid_Press, Act_PVCtrl, sizeof(stPara), (char*)&stPara, APP_NO);
    }
}

void app_smpLightCtl(vos_u32 srcpid, char ctrl)
{
    static char g_smpLightState = samp_key_led_off;
    //控制态等于原状态，不处理;原状态是亮，不进入闪。
    if (ctrl != g_smpLightState && !(ctrl == samp_key_led_flicker && g_smpLightState == samp_key_led_on))
    {
        g_smpLightState = ctrl;
        msg_sample_lightCtl lightCtl;
        lightCtl.ctl = ctrl;
        app_reqDwmCmd(srcpid, SPid_SmpDsk, Act_SmpLightCtrl, sizeof(lightCtl), (char *)&lightCtl, APP_NO);
    }
}

vos_u32 app_reqMachReset(vos_u32 ulSrcPid)
{
    STReqReset rPara;
    rPara.set_bupper(false);
    return sendInnerMsg(SPid_Reset, ulSrcPid, MSG_ReqReset, &rPara);
}

vos_u32 app_reqReset(vos_u32 ulSrcPid,vos_u32 ulDstPid,vos_u32 ulRes)
{
    vos_u32 ulResult = VOS_OK;

    msg_stComDevActReq* pPara = VOS_NULL;
    vos_u32 ulParaLen = sizeof(msg_stComDevActReq) + sizeof(msg_stComDevActInfo);
    ulResult = app_mallocBuffer(ulParaLen, (void**)(&pPara));
    if (VOS_OK != ulResult)
    {
        WRITE_ERR_LOG("Call app_mallocBuffer() failed(0x%x) \n", ulResult);
        return ulResult;
    }
    pPara->usActNum = 1;
    msg_stComDevActInfo * pActInfo = (msg_stComDevActInfo*)(&(pPara->astActInfo[0]));
    pActInfo->ulDataLen = sizeof(msg_stActHead);
    pActInfo->stActHead.usActType = Act_Reset;
    pActInfo->stActHead.usSingleRes = ulRes;

    msg_stHead stHead;
    app_constructMsgHead(&stHead, ulDstPid, ulSrcPid, MSG_ReqReset);
    return app_sendMsg(&stHead, pPara, ulParaLen);
}

void app_setActHead(msg_stActHead * pActHead, app_u16 actType, app_u16 cupId, app_u16 devType, app_u16 singRes)
{
    pActHead->usActType = actType;
    pActHead->usRctCupId = cupId;
    pActHead->usDevType = devType;
    pActHead->usSingleRes = singRes;
    pActHead->ulActIndex = 0;
}

void app_reportmidmachstatus(vos_u32 ulStatus, vos_u32 ulPid, vos_u32 ulLeftTime)
{
    STNotifyMachineStatus stStatus;
    stStatus.set_uimachstatus(ulStatus);
    stStatus.set_uilefttime(ulLeftTime);

    if (VOS_OK != sendInnerMsg(SPid_SocketMonitor, ulPid, MSG_NotifyMachineStatus, &stStatus))
    {
        WRITE_ERR_LOG("call SendInnerParaMsg failed\n");
    }
}

vos_u32 app_reqDwmCmd(vos_u32 srcPid, vos_u32 dstPid, app_u16 actType, vos_u32 cmdLen, char* cmdPara, app_u16 singRes)
{
    vos_u32 ulResult = VOS_OK;
    msg_stComDevActReq * pPara = VOS_NULL;
    vos_u32 ulParaLen = sizeof(msg_stComDevActReq) + sizeof(msg_stComDevActInfo) + cmdLen;
    
    ulResult = app_mallocBuffer(ulParaLen, (void**)(&pPara));
    if (VOS_OK != ulResult)
    {
        WRITE_ERR_LOG("Call app_mallocBuffer() failed(%d)\n", ulResult);
        return ulResult;
    }
    
    pPara->usActNum = 1;
    msg_stComDevActInfo * pActInfo = (msg_stComDevActInfo *)pPara->astActInfo;
    pActInfo->ulDataLen = sizeof(msg_stActHead) + cmdLen;
    app_setActHead(&pActInfo->stActHead, actType, 0, 0, singRes);
    if(cmdLen != 0 && cmdPara != VOS_NULL)
    {
        memcpy(pActInfo->aucPara, cmdPara, cmdLen);
    }
    
    msg_stHead stHead;
    app_constructMsgHead(&stHead, dstPid, srcPid, app_com_device_req_exe_act_msg_type);

    WRITE_INFO_LOG("ActReq: acttype=%lu,dstpid=%lu,res=%u\n", actType, dstPid, singRes);
    return app_sendMsg(&stHead, pPara, ulParaLen);
}

//设置ioctrl值
void app_setioctlvalue(vos_u32 ctrlkey, vos_u32 ctrlval)
{
    app_i32 fpgahdl = APP_NULL_HANDLE;

    fpgahdl = g_uartHdl[xc8002_dwnmach_name_fpga_ad];
    if (APP_NULL_HANDLE == fpgahdl)
    {
        WRITE_ERR_LOG("[Error] fpgahdl = NULL. \n");
        return;
    }
    ioctl(fpgahdl, ctrlkey, &ctrlval);

    WRITE_INFO_LOG("app_setioctlvalue %d %d\n", (int)ctrlkey, (int)ctrlval);
    return;
}

void app_print(vos_u32 printswich, const char* msg, ...)
{
    if ((ps_all == printswich)||(1 == ((g_ulmdprintswitch >> printswich) & 0x00000001))) 
    { 
        char  aucdatabuf[APP_MAX_PRINT_BUF_LENGTH] = { '\0' };
        va_list ag;
        va_start(ag, msg);
        vsprintf(aucdatabuf, msg, ag);
        printf("%s", (char *)aucdatabuf);
        fflush(stdout);
        va_end(ag);
    } 
}

long getTime_ms()
{
    timeval tv;
    gettimeofday(&tv, NULL);
    return ((tv.tv_sec%1000)*1000 + tv.tv_usec/1000);
}

void app_adfpgapowerswitch(vos_u32 ulpowermode)
{
    return;
    app_i32 fpgahdl = APP_NULL_HANDLE;
    vos_u32 ulmode = ulpowermode;

    fpgahdl = g_uartHdl[xc8002_dwnmach_name_fpga_ad];

    ioctl(fpgahdl, AD_START, &ulmode);
}

extern vos_u32 g_ultestmode;
vos_u32 staticTestAD()
{
    g_ultestmode = TESTMODE_DEBUG;

    act_stReadOneAD stPara;
    stPara.channel = 0;
    app_reqDwmCmd(Pid_DebugTty, SPid_ADOuter, Act_OneAD, sizeof(stPara), (char*)&stPara, APP_NO);
    app_reqDwmCmd(Pid_DebugTty, SPid_ADInner, Act_OneAD, sizeof(stPara), (char*)&stPara, APP_NO);
    return VOS_OK;
}

vos_u32 dynamicTsetAD()
{
    g_ultestmode = TESTMODE_DEBUG;

    //控制AD采集
    act_stReadAD para;
    para.AdCount = APP_RTT_RCTCUP_PER_PERIOD;
    app_reqDwmCmd(SPid_Task, SPid_ADOuter, Act_Ad, sizeof(para), (char*)&para, APP_NO);
    app_reqDwmCmd(SPid_Task, SPid_ADInner, Act_Ad, sizeof(para), (char*)&para, APP_NO);

    //转盘
    act_stReactsysDskRtt stPara;
    stPara.usRttedCupNum = APP_RTT_RCTCUP_PER_PERIOD;
    stPara.dir = cmd_motor_dir_CLOCKWISE;
    return app_reqDwmCmd(SPid_Task, SPid_ReactDsk, Act_Move, sizeof(act_stReactsysDskRtt), (char*)&stPara, APP_YES);
}

#ifndef Edition_A
vos_u32 MixWashCtrl(vos_u32 srcPid, vos_u32 dstPid, vos_u32 on_off)
{
    static vos_u32 sampmix = APP_CLOSE, regmix = APP_CLOSE;
    msg_PVCtrl PVCtrl;

    if (SPid_SMix == dstPid)
    {
        sampmix = on_off;
    }
    if (SPid_R2Mix == dstPid)
    {
        regmix = on_off;
    }

    if ((APP_OPEN == sampmix) || (APP_OPEN == regmix))
    {
        PVCtrl.operateType = APP_OPEN;
        app_reqDwmCmd(srcPid, SPid_SMix, Act_PVCtrl, sizeof(PVCtrl), (char*)&PVCtrl, APP_NO);
    }
    if ((APP_CLOSE == sampmix) && (APP_CLOSE == regmix))
    {
        PVCtrl.operateType = APP_CLOSE;
        app_reqDwmCmd(srcPid, SPid_SMix, Act_PVCtrl, sizeof(PVCtrl), (char*)&PVCtrl, APP_NO);
    }

    return VOS_OK;
}
#endif // Edition_A


vos_u32 SampMixWashCtrl(vos_u32 srcPid, vos_u32 on_off)
{
	msg_PVCtrl PVCtrl;
	PVCtrl.operateType = on_off;
	app_reqDwmCmd(srcPid, SPid_SMix, Act_PVCtrl, sizeof(PVCtrl), (char*)&PVCtrl, APP_NO);
	return VOS_OK;
}
vos_u32 ReagMixWashCtrl(vos_u32 srcPid, vos_u32 on_off)
{
	msg_PVCtrl PVCtrl;
	PVCtrl.operateType = on_off;
	app_reqDwmCmd(srcPid, SPid_R2Mix, Act_PVCtrl, sizeof(PVCtrl), (char*)&PVCtrl, APP_NO);
	return VOS_OK;
}

void DiskPosToCup(vos_u32 dikPos, app_u16 & disk, app_u16 & cup)
{
    if (dikPos <= 108)
    {
        disk = (dikPos - 1) / 54;//样本针和样本盘最外圈吸样位统一从0开始编号
        cup = (dikPos - 1) % 54 + 1;
    }
    else
    {
        disk = (dikPos - 108 - 1) / 27 + 2;
        cup = (dikPos - 108 - 1) % 27 + 1;
    }
}

void CupToDiskPos(app_u16 disk, app_u16 cup, vos_u32 &diskPos)
{
    diskPos = disk * 54 + cup;
    if (disk >= 2)
    {
        diskPos = 2 * 54 + (disk - 2) * 27 + cup;
    }
}
