#ifndef __APPUTILS_H__
#define __APPUTILS_H__


#include "app_innermsg.h"
#include "vos_timer.h"

//中位机状态定义
//!!!注意: 这里都是平台定义的中位机状态，应用层在定义自己的状态时不允许与此重复!!!
//!!!如果不明白如何定义及调用，请咨询唐衡!!!
typedef enum
{
    //连接和空闲态
    vos_midmachine_state_standby = 1,  //待机状态

    //任务运行态
    vos_midmachine_state_testing = 11,  //正在测试状态
    vos_midmachine_state_wait_pause = 12,  //等待暂停
    vos_midmachine_state_pause = 13, //中位机处于暂停状态

    //杂项运行态
    vos_midmachine_state_resetting = 21, //中位机复位中
    vos_midmachine_state_rewarming = 22, //中位机复温中
    vos_midmachine_state_intensify_wash = 23, //强化清洗中
    vos_midmachine_state_air_out = 24, //排空气中
    vos_midmachine_state_wash_rctcup = 25, //反应杯清洗中
    vos_midmachine_state_margin_scan = 26, //余量扫描测试中
    vos_midmachine_state_bcscan = 27, //条码扫描测试中
    vos_midmachine_state_darkcurrent = 28, //暗电流测试中
    vos_midmachine_state_precise_add = 29, //精准加液测试中
    vos_midmachine_state_dwncmd = 30,      //命令执行中
    vos_midmachine_state_light_check = 31, //反应杯清洗中
    vos_midmachine_state_cup_blank = 32, //反应杯清洗中

    
    //异常态
    vos_midmachine_state_reseted_err = 51, //中位机复位失败
    vos_midmachine_state_rewarming_err = 52, //中位机复温失败
    vos_midmachine_state_intensify_wash_err = 53, //强化清洗失败
    vos_midmachine_state_air_out_err = 54, //排空气失败
    vos_midmachine_state_midmach_err = 55,  //中位机异常
    vos_midmachine_state_margin_scan_err = 56, //余量扫描测试失败
    vos_midmachine_state_bcscan_err = 57, //条码扫描测试失败
    vos_midmachine_state_darkcurrent_err = 58, //暗电流测试失败
    vos_midmachine_state_precise_add_err = 59, //精准加液测试失败
}vos_midmachine_state_enum;

//定义模块级串口打印开关位图
typedef enum
{
    ps_dwnCmd       = 0,
    ps_commonSys    = 1,
    ps_cmdCenter    = 2,
    ps_stateMach    = 3,
    ps_mnguartcomm  = 4,
    ps_oam          = 5,
    ps_reactsys     = 6,
    ps_reagentsys   = 7,
    ps_samplesys    = 8,
    ps_rscsch       = 9,
    ps_taskmng      = 10,
    ps_darkCurrent  = 11,
    ps_oammng       = 12,
    ps_addReag      = 13,
    ps_addSample    = 14,
	ps_uartLink		= 15,
	ps_appDrv		= 16,
    ps_schCup       = 17,
    ps_bcscan       = 18,
	ps_ADval        = 19,
	ps_singleCmd    = 20,
    ps_lightCheck   = 21,
    ps_reportData   = 22,
	ps_washCup      = 23,
	ps_compensation = 24,
    ps_dwnFault     = 25,
    ps_midFault     = 26,
	ps_crossWash    = 27,
    ps_maitain      = 28,
    ps_equipment    = 29,
    ps_cmdSch       = 30,
    ps_can          = 31,
    ps_autoload = 32,
    //非应用程序模块打印开关
    ps_ebiDrv = 33,
    ps_all = 34
}app_md_print_switch_enum;

const vos_u32 g_ResetTimeLen = 500;

//开启p04
void OPEN_P04(vos_u32 srcpid);

//关闭p04
void CLOSE_P04(vos_u32 srcpid);

//初始化p04,防止g_P04Flag变量异常导致p04失控
void INITP04(vos_u32 srcpid, vos_u16 pNum = Press_our);

//整机复位
vos_u32 app_reqMachReset(vos_u32 ulSrcPid);

//复位
vos_u32 app_reqReset(vos_u32 ulSrcPid, vos_u32 ulDstPid, vos_u32 ulRes);

//设置动作头信息
void app_setActHead(msg_stActHead * pActHead, app_u16 actType, app_u16 cupId, app_u16 devType, app_u16 singRes);

//上报中位机状态
void app_reportmidmachstatus(vos_u32 ulStatus, vos_u32 ulPid = -1, vos_u32 ulLeftTime = 0);

//请求下位机单一命令
vos_u32 app_reqDwmCmd(vos_u32 srcPid, vos_u32 dstPid, app_u16 actType, vos_u32 cmdLen, char* cmdPara, app_u16 singRes);

void app_setioctlvalue(vos_u32 ctrlkey, vos_u32 ctrlval);

void app_print(vos_u32 printswich, const char* msg, ...);

long getTime_ms();

void app_smpLightCtl(vos_u32 srcpid, char ctrl);

void app_adfpgapowerswitch(vos_u32 ulpowermode); //打开采样开关

vos_u32 staticTestAD();//静态测光

vos_u32 dynamicTsetAD();//动态测光

vos_u32 SampMixWashCtrl(vos_u32 srcPid, vos_u32 on_off);

vos_u32 ReagMixWashCtrl(vos_u32 srcPid, vos_u32 on_off);

#ifndef Edition_A
vos_u32 MixWashCtrl(vos_u32 srcPid, vos_u32 dstPid, vos_u32 on_off);
#endif

void DiskPosToCup(vos_u32 dikPos, app_u16 & disk, app_u16 & cup);

void CupToDiskPos(app_u16 disk, app_u16 cup, vos_u32 &diskPos);
#endif
