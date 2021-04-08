/******************************************************************************

                  版权所有 (C), 2001-2100, 四川省新健康成股份有限公司

 ******************************************************************************
  文 件 名   : app_debugcmd.c
  版 本 号   : 初稿
  作    者   : Albort,Feng
  生成日期   : 2014年7月9日
  最近修改   :
  功能描述   : 中下位机调试命令实现文件
  函数列表   :
  修改历史   :
  1.日    期   : 2014年7月9日
    作    者   : Albort,Feng
    修改内容   : 创建文件

******************************************************************************/
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <memory.h>
#include <termio.h>
#include <sys/time.h>
#include <time.h>
#include "drv_ebi.h"
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include "xc8002_middwnmachshare.h"
#include "app_xc8002shareinfo.h"
#include "app_errorcode.h"
#include "app_msgtypedefine.h"
#include "app_comstructdefine.h"
#include "app_statusrepdispose.h"
#include "app_debugtty.h"
#include "cmd_coolingreagent.h"
#include "app_maintain.h"
#include "app_msg.h"
#include "app_testcase.h"
#include "app_simulation.h"
#include "app_main.h"
#include "app_msg.h"
#include "upmsg.pb.h"
#include "typedefine.pb.h"
#include "vos_mnglist.h"
#include "vos_mngmemory.h"
#include "vos_errorcode.h"
#include "vos_log.h"
#include "vos_pid.h"
#include "vos_init.h"
#include "vos_timer.h"
#include "app_debugcmd.h"
#include "msg.h"
#include "apputils.h"
#include "inituart.h"
#include "cmd_downmachine.h"
#include "oammng.h"
#include "singledwncmd.h"
#include "uart.h"
#include "alarm.h"
#include "taskmng.h"
#include <map>
#include "socketmonitor.h"
#include "vos_basictypedefine.h"
#include "singledwncmd.h"
#include <math.h>
#include "acthandler.h"
using namespace std;

extern map<app_u16, app_u16> g_uartHdl;

#ifdef __cplusplus
    extern "C" {
#endif


#define DEBUG_PARA_PARSE(NUM) \
    ulrunrslt = app_strnumToIntnum(para##NUM, &cmd.auccmdpara[NUM]); \
    if (VOS_OK != ulrunrslt) \
    { \
        return ulrunrslt; \
    }
    
#define DEBUG_PARA_CHECK \
    if (cmd.ulcmdparanum >= 1){ DEBUG_PARA_PARSE(0);} \
    if (cmd.ulcmdparanum >= 2){ DEBUG_PARA_PARSE(1);} \
    if (cmd.ulcmdparanum >= 3){ DEBUG_PARA_PARSE(2);} \
    if (cmd.ulcmdparanum >= 4){ DEBUG_PARA_PARSE(3);} \
    if (cmd.ulcmdparanum >= 5){ DEBUG_PARA_PARSE(4);} \
    if (cmd.ulcmdparanum >= 6){ DEBUG_PARA_PARSE(5);} \
    if (cmd.ulcmdparanum >= 7){ DEBUG_PARA_PARSE(6);} \
    if (cmd.ulcmdparanum >= 8){ DEBUG_PARA_PARSE(7);} \
    if (cmd.ulcmdparanum >= 9){ DEBUG_PARA_PARSE(8);} \
    if (cmd.ulcmdparanum >= 10){ DEBUG_PARA_PARSE(9);} \
    if (cmd.ulcmdparanum >= 11){ DEBUG_PARA_PARSE(10);} \
    if (cmd.ulcmdparanum >= 12){ DEBUG_PARA_PARSE(11);} \
    if (cmd.ulcmdparanum >= 13){ DEBUG_PARA_PARSE(12);} \
    if (cmd.ulcmdparanum >= 14){ DEBUG_PARA_PARSE(13);}

#define GET_DEBUG_PARA_PARSE(num) \
    ulrunrslt = app_strnumToIntnum(para##num, &para[num]); \
    if (VOS_OK != ulrunrslt) \
    { \
        return ulrunrslt; \
    }
    
#define GET_DEBUG_PARA(n) \
    vos_u32 loop = 0; \
    for(loop = 0; loop<=n; loop++) \
    { \
        GET_DEBUG_PARA_PARSE(n); \
    }

extern vos_u32 g_ultestmode;
vos_u32  g_uldwnmachmask;
app_u16 g_usdeclinehigh;
extern sim_stCondMutex stCondMutex;
extern vos_u32 g_ulmdprintswitch;

typedef struct 
{
	vos_u32	ulnum;
	app_u16 machname;
	vos_u32 frameid;
	app_u16 ulcmd;
	char  ulname[128];  
} update_list_stru;

static const update_list_stru updatelist[] = 
{
	{0, 0x03, 0x0, cmd_sample_dsk_update, 			 "sampdisk.bin"},
	{1, 0x03, 0x0, cmd_sample_ndl_update, 			 "sampndl.bin"},
	{2, 0x03, 0x0, cmd_inner_reagentdisk_update,	 "innerreag.bin"},
	{3, 0x03, 0x0, cmd_r1_ndl_chip_update,			  "r1.bin"},
	{4, 0x03, 0x0, cmd_outer_reagentdisk_update,	  "outreag.bin"},
	{5, 0x03, 0x0, cmd_r2_ndl_update, 				  "r2.bin"},
	{6, 0x03, 0x0, cmd_react_disk_update, 			  "reatdisk.bin"},
	{7, 0x03, 0x0, cmd_auto_wash_update,			  "autowash.bin"},
	{8, 0x03, 0x0, cmd_sample_mix_update, 			  "sampmix.bin"},
	{9, 0x03, 0x0, cmd_reagent_mix_update,			  "reagmix.bin"},
	{10,0x03, 0x0, cmd_temp_control_update,			  "tempctl.bin"},
	{11,0x03, 0x0, cmd_cooling_update,				  "cooling.bin"},
	{0, 0x00, 0x0, cmd_sample_dsk_fpga_update,		  "sampdiskfpga.bin"},
	{4, 0x04, 0x4, cmd_outer_reagentdisk_fpga_update, "reagfpga.bin"},
	{9, 0x09, 0x9, cmd_reagent_mix_fpga_update,		  "washfpga.bin"},
	{15, 0,    0,  0, 								  "midfpga.bin"},
	
};

vos_u32 app_debugsamplemix(char* pactType, char* ppara0, char* ppara1)
{
    vos_u32 actType;
	vos_u32 para0;
	vos_u32 para1;
    
	app_strnumToIntnum(pactType, &actType);
	app_strnumToIntnum(ppara0, &para0);
	app_strnumToIntnum(ppara1, &para1);

	CChooseModeStrategy * pCho = new CChooseModeStrategy;
	pCho->_chooseModeByActtype(SPid_SMix, actType, para0, para1);

	delete pCho;
	pCho = NULL;
    return VOS_OK;
}

void app_showdebugsamplemixhelpinfo(void)
{
    printf("\r\n Command format: app_debugsamplemix {acttype} {para0} \r\n\n");
	
    printf(" acttype: <0>-smpmix reset \r\n");
	printf(" para0  : null\r\n\n\n");
	
    printf(" acttype: <1>-smpmix rtt \r\n");
	printf(" para0  : <0>-wash_pos,  <1>-mix_pos \r\n\n\n");

	printf(" acttype: <12>-smpmix vrt \r\n");
	printf(" para0  : <0>-up,  <1>-down \r\n\n");
    
    printf(" acttype: <30>-smpmix Mix \r\n");
    fflush(stdout);
  
    return;
}

vos_u32 app_debugreagmix(char* pactType, char* ppara0, char* ppara1)
{
    vos_u32 actType;
	vos_u32 para0;
	vos_u32 para1;
	
	app_strnumToIntnum(pactType, &actType);
	app_strnumToIntnum(ppara0, &para0);
	app_strnumToIntnum(ppara1, &para1);

	CChooseModeStrategy * pCho = new CChooseModeStrategy;
	pCho->_chooseModeByActtype(SPid_R2Mix, actType, para0, para1);

	delete pCho;
	pCho = NULL;
    return VOS_OK;
}

void app_showdebugreagmixhelpinfo(void)
{
    printf("\r\n Command format: app_debugreagmix {acttype} {para0} \r\n\n");
	
    printf(" acttype: <0>-reagmix reset \r\n");
	printf(" para0  : null\r\n\n\n");
	
    printf(" acttype: <1>-reagmix rtt \r\n");
	printf(" para0  : <0>-wash_pos,  <1>-mix_pos \r\n\n\n");

	printf(" acttype: <12>-reagmix vrt \r\n");
	printf(" para0  : <0>-up,  <1>-down \r\n\n");
    
    printf(" acttype: <30>-reagmix Mix \r\n");

    fflush(stdout);
  
    return;
}

vos_u32 app_debugsampleneedle(char* pactType, char* ppara0, char* ppara1)
{
	vos_u32 actType;
	vos_u32 para0;
	vos_u32 para1;
	
	app_strnumToIntnum(pactType, &actType);
	app_strnumToIntnum(ppara0, &para0);
	app_strnumToIntnum(ppara1, &para1);

	CChooseModeStrategy * pCho = new CChooseModeStrategy;
	pCho->_chooseModeByActtype(SPid_SmpNdl, actType, para0, para1);

	delete pCho;
	pCho = NULL;
    return VOS_OK;
}

void app_showdebugsampleneedlehelpinfo(void)
{
    printf("\r\n Command format: app_debugsampleneedle {acttype} {para0} {para1}\r\n\n");
	
    printf(" acttype: <0>-smpndl reset \r\n");
	printf(" para0  : null\r\n\n\n");
	
    printf(" acttype: <1>-smpndl rtt \r\n");
	printf(" para0  : <0>-circle1(out),  <1>-circle2,  <2>-circle3,  <3>-circle4,  <4>-circle5,  <6>-wash_pos,  <8>-discharge_pos \r\n\n\n");

	printf(" acttype: <12>-smpndl vrt \r\n");
	printf(" para0  : <0>-up\r\n\n\n");

	printf(" acttype: <12>-smpndl vrt \r\n");
	printf(" para0  : <1>-down \r\n");
	printf(" para1  : <0~350>-liquor volume in reaccup(eg:250 means 25ul) \r\n\n\n");

    printf(" acttype: <24>-in or out \r\n");
    printf(" para0  : dir:<1>-in,<0>-out \r\n");
    printf(" para1  : vol:(0.1ul) \r\n");
    
    fflush(stdout);
  
    return;
}
/*****************************************************************************
 函 数 名  : app_debugsampledisk
 功能描述  : 样本盘转动演示命令
 输入参数  : char* puccmd
             char* pucpara0
             char* pucpara1
 输出参数  : 无
 返 回 值  : vos_u32
 
 修改历史      :
  1.日    期   : 2014年7月11日
    作    者   : Gao.J

    修改内容   : 新生成函数

*****************************************************************************/
vos_u32 app_debugsampledisk(char* pactType, char* ppara0, char* ppara1)
{
    vos_u32 actType;
	vos_u32 para0;
	vos_u32 para1;
	
	app_strnumToIntnum(pactType, &actType);
	app_strnumToIntnum(ppara0, &para0);
	app_strnumToIntnum(ppara1, &para1);

	CChooseModeStrategy * pCho = new CChooseModeStrategy;
	pCho->_chooseModeByActtype(SPid_SmpDsk, actType, para0, para1);

	delete pCho;
	pCho = NULL;
    return VOS_OK;
}
/*****************************************************************************
 函 数 名  : showdebugsamplediskinfo
 功能描述  : 样本盘调试命令帮助信息
 输入参数  : 无
 输出参数  : 无
 返 回 值  :  
 
 修改历史      :
  1.日    期   : 2014年7月11日
    作    者   : Gao.J

    修改内容   : 新生成函数

*****************************************************************************/
void app_showdebugsamplediskhelpinfo( void )
{
	printf("\r\n Command format: app_debugsampledisk {acttype} {para0} {para1} \r\n\n");
	
    printf(" acttype: <0>-smpdsk reset \r\n");
	printf(" para0  : null\r\n");
	printf(" para1  : null\r\n\n\n");
	
    printf(" acttype: <1>-smpdsk rtt \r\n");
	printf(" para0  : <0>-circle1(out),  <1>-circle2,  <2>-circle3,  <3>-circle4,  <4>-circle5 \r\n");
	printf(" para1  : <1~35>-cupid for circle1~3,  <1~25>-cupid for circle4~5 \r\n\n\n");

	printf(" acttype: <8>-smpdsk bcscan rtt \r\n");
	printf(" para0  : <0>-circle1(out),  <1>-circle2,  <2>-circle3,  <3>-circle4,  <4>-circle5 \r\n");
	printf(" para1  : <1~35>-cupid\r\n\n");
    
    printf(" acttype: <28>-light \r\n");
    printf(" para0  : <0>-off,  <1>-on,  <2>-flicker \r\n");

    fflush(stdout);
  
    return;
}

vos_u32 app_debugreag1needle(char* pactType, char* ppara0, char* ppara1)
{
    vos_u32 actType;
	vos_u32 para0;
	vos_u32 para1;
	
	app_strnumToIntnum(pactType, &actType);
	app_strnumToIntnum(ppara0, &para0);
	app_strnumToIntnum(ppara1, &para1);

	CChooseModeStrategy * pCho = new CChooseModeStrategy;
	pCho->_chooseModeByActtype(SPid_R1, actType, para0, para1);

	delete pCho;
	pCho = NULL;
    return VOS_OK;
}

void app_showdebugreag1needlehelpinfo( void )
{
    printf("\r\n Command format: app_debugreag1needle {acttype} {para0} \r\n\n");
	
    printf(" acttype: <0>-r1 reset \r\n");
	printf(" para0  : null\r\n\n\n");
	
    printf(" acttype: <1>-r1 rtt \r\n");
	printf(" para0  : <0>-wash_pos,  <1>-absorb_pos,  <2>-discharge_pos \r\n\n\n");

	printf(" acttype: <12>-r1 vrt \r\n");
	printf(" para0  : <0>-up\r\n\n\n");

	printf(" acttype: <12>-r1 vrt \r\n");
	printf(" para0  : <1>-down \r\n");
	printf(" para1  : <0~3500>-liquor volume in reaccup(eg:2500 means 250ul) \r\n\n");
    
    printf(" acttype: <24>-in or out \r\n");
    printf(" para0  : dir:<1>-in,<0>-out \r\n");
    printf(" para1  : vol:(0.1ul) \r\n");
    fflush(stdout);
  
    return;
}

vos_u32 app_debugreag2needle(char* pactType, char* ppara0, char* ppara1)
{
    vos_u32 actType;
	vos_u32 para0;
	vos_u32 para1;
	
	app_strnumToIntnum(pactType, &actType);
	app_strnumToIntnum(ppara0, &para0);
	app_strnumToIntnum(ppara1, &para1);

	CChooseModeStrategy * pCho = new CChooseModeStrategy;
	pCho->_chooseModeByActtype(SPid_R2, actType, para0, para1);

	delete pCho;
	pCho = NULL;
    return VOS_OK;
}

void app_showdebugreag2needlehelpinfo( void )
{
    printf("\r\n Command format: app_debugreag2needle {acttype} {para0} \r\n\n");
	
    printf(" acttype: <0>-r2 reset \r\n");
	printf(" para0  : null\r\n\n\n");
	
    printf(" acttype: <1>-r2 rtt \r\n");
	printf(" para0  : <0>-wash_pos,  <1>-absorb_pos,  <2>-discharge_pos \r\n\n\n");

	printf(" acttype: <12>-r2 vrt \r\n");
	printf(" para0  : <0>-up\r\n\n\n");

	printf(" acttype: <12>-r2 vrt \r\n");
	printf(" para0  : <1>-down \r\n");
	printf(" para1  : <0~3500>-liquor volume in reaccup(eg:2500 means 250ul) \r\n\n");

    printf(" acttype: <24>-in or out \r\n");
    printf(" para0  : dir:<1>-in,<0>-out \r\n");
    printf(" para1  : vol:(0.1ul) \r\n");
    
    fflush(stdout);
  
    return;
}

vos_u32 app_debugreagoutdisk(char* pactType, char* ppara0, char* ppara1)
{
    vos_u32 actType;
	vos_u32 para0;
	vos_u32 para1;
	
	app_strnumToIntnum(pactType, &actType);
	app_strnumToIntnum(ppara0, &para0);
	app_strnumToIntnum(ppara1, &para1);

	CChooseModeStrategy * pCho = new CChooseModeStrategy;
	pCho->_chooseModeByActtype(SPid_R1Dsk, actType, para0, 0);

	delete pCho;
	pCho = NULL;
    return VOS_OK;
}

void app_showdebugreagoutdiskhelpinfo( void )
{
    printf("\r\n Command format: app_debugreagoutdisk {acttype} {para0} \r\n\n");
	
    printf(" acttype: <0>-outdsk reset \r\n");
	printf(" para0  : null\r\n\n\n");
	
    printf(" acttype: <1>-outdsk rtt \r\n");
	printf(" para0  : <1~65>-cupid rtt to absorb_pos \r\n\n\n");

	printf(" acttype: <8>-outdsk bcscan rtt \r\n");
	printf(" para0  : <1~65>-cupid rtt to bcscan_pos \r\n");
    
    fflush(stdout);
  
    return;
}

vos_u32 app_debugreagindisk(char* pactType, char* ppara0, char* ppara1)
{
    vos_u32 actType;
	vos_u32 para0;
	vos_u32 para1;
	
	app_strnumToIntnum(pactType, &actType);
	app_strnumToIntnum(ppara0, &para0);
	app_strnumToIntnum(ppara1, &para1);

	CChooseModeStrategy * pCho = new CChooseModeStrategy;
	pCho->_chooseModeByActtype(SPid_R2Dsk, actType, para0, 0);

	delete pCho;
	pCho = NULL;
    return VOS_OK;
}

void app_showdebugreagindiskhelpinfo( void )
{
    printf("\r\n Command format: app_debugreagindisk {acttype} {para0} \r\n\n");
	
    printf(" acttype: <0>-indsk reset \r\n");
	printf(" para0  : null\r\n\n\n");
	
    printf(" acttype: <1>-indsk rtt \r\n");
	printf(" para0  : <1~45>-cupid rtt to absorb_pos \r\n\n\n");

	printf(" acttype: <8>-indsk bcscan rtt \r\n");
	printf(" para0  : <1~45>-cupid rtt to bcscan_pos \r\n");
    
    fflush(stdout);
  
    return;
}

vos_u32  app_debugreactiondisk(char* pactType, char* ppara0, char* ppara1)
{
    vos_u32 actType;
	vos_u32 para0;
	vos_u32 para1;
	
	app_strnumToIntnum(pactType, &actType);
	app_strnumToIntnum(ppara0, &para0);
	app_strnumToIntnum(ppara1, &para1);

	CChooseModeStrategy * pCho = new CChooseModeStrategy;
	pCho->_chooseModeByActtype(SPid_ReactDsk, actType, para0, para1);

	delete pCho;
	pCho = NULL;
    return VOS_OK;
}

void app_showdebugreactiondiskhelpinfo( void )
{
    printf("\r\n Command format: app_debugreactiondisk {acttype} {para0} \r\n\n");
	
    printf(" acttype: <0>-reacdsk reset \r\n");
	printf(" para0  : null\r\n\n\n");
	
    printf(" acttype: <1>-reacdsk rtt \r\n");
	printf(" para0  : <1~165>-rtt cupnum(anticlockwise) \r\n");
    
    fflush(stdout);
  
    return;
}

vos_u32 app_debugautowashneedle(char* pactType, char* ppara0, char* ppara1)
{
    vos_u32 actType;
	vos_u32 para0;
	vos_u32 para1;
	
	app_strnumToIntnum(pactType, &actType);
	app_strnumToIntnum(ppara0, &para0);
	app_strnumToIntnum(ppara1, &para1);

	CChooseModeStrategy * pCho = new CChooseModeStrategy;
	pCho->_chooseModeByActtype(SPid_AutoWash, actType, para0, para1);

	delete pCho;
	pCho = NULL;
    return VOS_OK;
}

void  app_showdebugautowashneedlehelpinfo(void)
{
    printf("\r\n Command format: app_debugautowashneedle {acttype} {para0} \r\n\n");
	
    printf(" acttype: <0>-autowash reset \r\n");
	printf(" para0  : null\r\n\n\n");
	
    printf(" acttype: <4>-exe one cycle autowash \r\n");
	printf(" para0  : <0>-no water,  <255>-1~8 needle water \r\n\n\n");

	printf(" acttype: <12>-autowash vrt \r\n");
	printf(" para0  : <0>-up,  <1>-down \r\n");
    
    fflush(stdout);
  
    return;
}

/*****************************************************************************
 函 数 名  : app_commonsyscmd
 功能描述  : 公共系统命令演示
 输入参数  : char* puccmd
 				char* pucpara0
 				char* pucpara1
 输出参数  : 无
 返 回 值  : vos_u32
 
 修改历史      :
  1.日    期   : 2014年8月1日
    作    者   : Gao.J

    修改内容   : 新生成函数

*****************************************************************************/
vos_u32 app_commonsyscmd()
{
    return app_reqMachReset(Pid_DebugTty);
}
/*****************************************************************************
 函 数 名  : app_showcommonsyscmdhelpinfo
 功能描述  : 公共系统组合命令演示
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 
 
 修改历史      :
  1.日    期   : 2014年8月1日
    作    者   : Gao.J

    修改内容   : 新生成函数

*****************************************************************************/
void app_showcommonsyscmdhelpinfo( void )
{
    printf("\r command format: app_commonsyscmd ulcmdtype \r\n\r\n");
    printf(" ulcmdtype:	0x5001 reactionsubsystem reset\r\n");
}
/*****************************************************************************
 函 数 名  : app_updateapp
 功能描述  : app应用程序升级命令
 输入参数  : void
 输出参数  : 无
 返 回 值  : vos_u32
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2014年7月31日
    作    者   : Albort,Feng
    修改内容   : 新生成函数

*****************************************************************************/
vos_u32 app_updateapp(char* pucserverip)
{
    vos_u32 ulrunrslt          = VOS_OK;
    char  aucexetftpcmd[128] = {0};
    char  aucbuf[128]        = {0};
    char  aucbkupzone[16]    = {0};
    char  aucwkpath[128]     = {0};
    char  aucabspath[256]    = {0};
    char* pucrslt = VOS_NULL;
    FILE*   pf      = VOS_NULL;

    vos_getworkpath((char*)aucwkpath);

    sprintf((char*)aucabspath, "%s/pingpang.ini", (char*)aucwkpath);
    
    pf = fopen((char*)aucabspath, "r");
    if (VOS_NULL == pf)
    {
        printf(" open pingpang.ini failed\r\n");
        return (vos_u32)APP_COM_ERR;
    }

    pucrslt = fgets((char*)aucbuf, 128, pf);
    fclose(pf);
    pf = VOS_NULL;

    if (VOS_NULL == pucrslt)
    {
        printf(" read pingpang.ini failed\r\n");
    }
    
    if (strstr((char*)aucbuf, "zone1"))
    {
        sprintf((char*)aucbkupzone, "zone2");
    }else if (strstr((char*)aucbuf, "zone2"))
    {
        sprintf((char*)aucbkupzone, "zone1");
    }else
    {
        printf(" main zone error!\r\n");
        
        return (vos_u32)APP_COM_ERR;
    }

    sprintf((char*)aucexetftpcmd, "rm -r %s/%s/xc_app",
            (char*)aucwkpath, (char*)aucbkupzone);
    
    ulrunrslt = system((char*)aucexetftpcmd);
    
    sprintf((char*)aucexetftpcmd, "tftp -g -r xc_app %s", pucserverip);
    
    #ifdef COMPILER_IS_ARM_LINUX_GCC
    ulrunrslt = system((char*)aucexetftpcmd);
    if (VOS_OK != ulrunrslt)
    {
        printf(" update the app system failed\r\n");
        return (vos_u32)APP_COM_ERR;
    }

    sprintf((char*)aucabspath, "%s/%s", (char*)aucwkpath, (char*)aucbkupzone);

    sprintf((char*)aucexetftpcmd, "mv xc_app %s", (char*)aucabspath);

    ulrunrslt = system((char*)aucexetftpcmd);
    
    sprintf((char*)aucexetftpcmd, "chmod 777 %s/%s/xc_app",
            (char*)aucwkpath, (char*)aucbkupzone);

    ulrunrslt = system((char*)aucexetftpcmd);
    #else
    printf(" %s\r\n", (char*)aucexetftpcmd);
    #endif

    sprintf((char*)aucabspath, "%s/pingpang.ini", (char*)aucwkpath);
    
    pf = fopen((char*)aucabspath, "w+");
    if (VOS_NULL == pf)
    {
        printf(" open pingpang.ini failed\r\n");
        
        return (vos_u32)APP_COM_ERR;
    }
    
    sprintf((char*)aucbuf, "mainzone= %s", (char*)aucbkupzone);
    fputs((char*)aucbuf, pf);
    fflush(pf);
    fclose(pf);
    pf = VOS_NULL;

	SENDREBOOT2DOWNM();
    vos_threadsleep(1000);

    ulrunrslt = system("reboot");
    if (VOS_OK != ulrunrslt)
    {
        printf(" reboot failed\r\n");
        
        return (vos_u32)APP_COM_ERR;
    }
    
    return VOS_OK;
}

/*****************************************************************************
 函 数 名  : app_showupdateappcmdhelpinfo
 功能描述  : 显示升级命令帮助信息
 输入参数  : void
 输出参数  : 无
 返 回 值  : void
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2014年7月31日
    作    者   : Albort,Feng
    修改内容   : 新生成函数

*****************************************************************************/
void app_showupdateappcmdhelpinfo(void)
{
    printf("\r command format: app_updateapp serverip\r\n");
    printf("\r funcation: update the app system\r\n");
    printf("\r serverip : the tftp server's ip address\r\n");
    
    fflush(stdout);
    
    return;
}

/*****************************************************************************
 函 数 名  : app_showthreadsum
 功能描述  : 查看创建线程总数
 输入参数  : void
 输出参数  : 无
 返 回 值  : vos_u32
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2014年8月1日
    作    者   : Albort,Feng
    修改内容   : 新生成函数

*****************************************************************************/
vos_u32 app_showthreadsum(void)
{
    printf("\r Thread sum: %d\r\n", (int)vos_getthreadtotalnum());
    fflush(stdout);
    
    return VOS_OK;
}

/*****************************************************************************
 函 数 名  : app_showshowthreadsum
 功能描述  : 查看线程总数命令的帮助信息
 输入参数  : void
 输出参数  : 无
 返 回 值  : void
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2014年8月1日
    作    者   : Albort,Feng
    修改内容   : 新生成函数

*****************************************************************************/
void app_showshowthreadsum(void)
{
    printf("\r command format: app_showshowthreadsum\r\n");
    printf("\r funcation: look up current thread total number\r\n");
    
    fflush(stdout);
    return;
}

/*****************************************************************************
 函 数 名  : app_version
 功能描述  : 显示版本号命令
 输入参数  : void
 输出参数  : 无
 返 回 值  : vos_u32
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2014年8月5日
    作    者   : Albort,Feng
    修改内容   : 新生成函数

*****************************************************************************/
vos_u32 app_version(void)
{
    char aucvinfo[256] = {0};

    printf("\r App build date: %s  version: %s\r\n", PBUILTTIME, PBUILDVERSION);
    printf("\r %s\r\n", (char*)aucvinfo);
    
    fflush(stdout);
    return VOS_OK;
}

/*****************************************************************************
 函 数 名  : app_showversionhelpinfo
 功能描述  : 显示版本号命令帮助信息
 输入参数  : void
 输出参数  : 无
 返 回 值  : void
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2014年8月5日
    作    者   : Albort,Feng
    修改内容   : 新生成函数

*****************************************************************************/
void app_showversionhelpinfo(void)
{
    printf("\r command format: app_version\r\n");
    printf("\r funcation: query system version infomation\r\n");
    
    fflush(stdout);
    return;
}

/*****************************************************************************
 函 数 名  : app_showusedmemsize
 功能描述  : 查看已使用内存大小
 输入参数  : void
 输出参数  : 无
 返 回 值  : vos_u32
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2014年8月7日
    作    者   : Albort,Feng
    修改内容   : 新生成函数

*****************************************************************************/
vos_u32 app_showusedmemsize( void )
{
    printf("\r Used memory: %d(byte)\r\n", (int)vos_getusedmemsize());
    fflush(stdin);
    
    return VOS_OK;
}

/*****************************************************************************
 函 数 名  : app_showshowusedmemsizehelpinfo
 功能描述  : 显示查看已使用内存命令帮助信息
 输入参数  : void
 输出参数  : 无
 返 回 值  : void
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2014年8月7日
    作    者   : Albort,Feng
    修改内容   : 新生成函数

*****************************************************************************/
void app_showshowusedmemsizehelpinfo(void)
{
    printf("\r command format: app_showusedmemsize\r\n");
    printf("\r funcation: query used memory size(unit:byte)\r\n");
    
    fflush(stdout);
    
    return;
}

/*****************************************************************************
 函 数 名  : app_debugtaskmngauto
 功能描述  : 自动化测试taskmng
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 
 
 修改历史      :
  1.日    期   : 2014年9月3日
    作    者   : 唐衡
    修改内容   : 新生成函数

*****************************************************************************/
vos_u32 app_tasktest(char* tasknum)
{
    vos_u32 ultasknum = 0;            //测试的任务个数，界面输入
    vos_u32 ulcnt = 0;                //打印计数

    vos_u32 ulrunrslt = app_strnumToIntnum(tasknum, &ultasknum);
    if (VOS_OK != ulrunrslt)
    {
        return ulrunrslt;
    }
    while (ulcnt < ultasknum)
    {
        ulcnt++;
        STReqAddTask  stPara;
        stPara.set_uitaskid(ulcnt);
        stPara.set_uismptype(0);
        stPara.set_uimaintantype(AutoTeskTask);

        STKeyValue * pWave = stPara.add_stwavechs();
        pWave->set_uikey(1);
        pWave->set_uival(app_rscsch_wave_340nm_id);

        //样本
        STLiquidInfo * pSmpInfo = stPara.mutable_stsmpinfo();
        STCupPos * pPos = pSmpInfo->mutable_stcuppos();
        pPos->set_uidskid(1);
        if (ulcnt%2==1)
        {
            pPos->set_uicupid(1);
        }
        else{
            pPos->set_uicupid(2);
        }
        pSmpInfo->set_uiaddvol(150);

        //R1
        STLiquidInfo * pReagInfo = stPara.add_streagents();
        pReagInfo->set_uiaddvol(3000);
        pPos = pReagInfo->mutable_stcuppos();
        pPos->set_uidskid(0);
        if (ulcnt % 2 == 1)
        {
            pPos->set_uicupid(1);
        }
        else {
            pPos->set_uicupid(17);
        }

       

        //R2
        pReagInfo = stPara.add_streagents();
        pReagInfo->set_uiaddvol(300);
        pPos = pReagInfo->mutable_stcuppos();
        pPos->set_uidskid(0);
        if (ulcnt%2==1)
        {
            pPos->set_uicupid(1);
        }
        else {
            pPos->set_uicupid(15);
        }
        
        ulrunrslt = sendInnerMsg(SPid_Task, Pid_DebugTty, MSG_ReqAddTask, &stPara);
        printf("vos_sendmsg ulrunrslt=0x%lx,delaytime=4500\n", ulrunrslt);
        vos_threadsleep(100);
    }
    return ulrunrslt;
}

/*****************************************************************************
 函 数 名  : app_showtaskmngautohelpinfo
 功能描述  : 测试任务自动测试帮助信息
 输入参数  : void  
 输出参数  : 无
 返 回 值  : 
 
 修改历史      :
  1.日    期   : 2014年9月3日
    作    者   : 唐衡
    修改内容   : 新生成函数

*****************************************************************************/
void app_showautotaskmnghelpinfo(void)
{
    printf("\r command format: app_tasktest tasknum\r\n");
    printf("\r tasknum: task num\r\n");
    printf("\r sample: app_tasktest 3\r\n");
    
    fflush(stdout);
    return;
}

void app_showautotask(void)
{
	printf("\r command format: app_autotask\r\n");
	printf("\r tasknum: task num = 1\r\n");
	printf("\r sample: app_autotask\r\n");

	fflush(stdout);
	return;
}
/*****************************************************************************
 函 数 名  : app_debugdarkcurrent
 功能描述  : 暗电流测试
 输入参数  : char* testnum  
 输出参数  : 无
 返 回 值  : 
 
 修改历史      :
  1.日    期   : 2014年9月3日
    作    者   : 唐衡
    修改内容   : 新生成函数

*****************************************************************************/
vos_u32 app_debugdarkcurrent(char* testnum)
{
	WRITE_INFO_LOG("Enter app_debugdarkcurrent.\r\n");
    vos_u32 testcnt;
    vos_u32 collectAdKey[WAVE_NUM] = {app_rscsch_wave_340nm_id,app_rscsch_wave_405nm_id,app_rscsch_wave_450nm_id,app_rscsch_wave_478nm_id,
		app_rscsch_wave_505nm_id,app_rscsch_wave_542nm_id,app_rscsch_wave_570nm_id,app_rscsch_wave_605nm_id,app_rscsch_wave_630nm_id,
		app_rscsch_wave_660nm_id,app_rscsch_wave_700nm_id,app_rscsch_wave_750nm_id,app_rscsch_wave_805nm_id,app_rscsch_wave_850nm_id};
	
    app_strnumToIntnum(testnum, &testcnt);

    STReqDarkCurrentTest  stPara;
	
    stPara.set_testtype(0);
	stPara.set_testcount(testcnt);

	STKeyValue * pVal = NULL;
	for(vos_u32 i = 0 ; i < WAVE_NUM ; i++)
	{
		pVal = stPara.add_dpvalueinfo();
    	pVal->set_uikey(collectAdKey[i]);
		pVal->set_uival(666);   
	}
  
    return sendInnerMsg(SPid_DarkCurrent, Pid_DebugTty, MSG_ReqDarkCurrent, &stPara);
}

void app_showdarkcurrenthelpinfo(void)
{
    printf("\r command format: app_debugdarkcurrent \r\n");

    fflush(stdout);
    return;
}

/*****************************************************************************
 函 数 名  : app_debugsetioctlval
 功能描述  : 任意设置ioctrl值
 输入参数  : char* ctrlkey  
             char* ctrlval  
 输出参数  : 无
 返 回 值  : 
 
 修改历史      :
  1.日    期   : 2014年8月20日
    作    者   : 唐衡
    修改内容   : 新生成函数

*****************************************************************************/
vos_u32 app_debugsetioctlval(char* ctrlkey, char* ctrlval)
{
    vos_u32 ulrunrslt = 0;
    vos_u32 ulkey = 0;
    vos_u32 ulval = 0;

    printf("app_debugsetioctlval\n");
    ulrunrslt = app_strnumToIntnum(ctrlkey, &ulkey);
    if (VOS_OK != ulrunrslt)
    {
        printf("ERRORR");
        return ulrunrslt;
    }

    ulrunrslt = app_strnumToIntnum(ctrlval, &ulval);
    if (VOS_OK != ulrunrslt)
    {
        printf("ERRORR");
        return ulrunrslt;
    }

    
    app_setioctlvalue(ulkey, ulval);
    
    return VOS_OK; 
}

/*****************************************************************************
 函 数 名  : app_showioctrlhelpinfo
 功能描述  : ioctrl帮助信息
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 
 
 修改历史      :
  1.日    期   : 2014年8月20日
    作    者   : 唐衡
    修改内容   : 新生成函数

*****************************************************************************/
void app_showioctrlhelpinfo()
{
    printf("\r command format: app_debugsetioctlval ctrlkey ctrlval \r\n");
    printf("\r ctrlkey: fpga register key; \r\n");
    printf("\r ctrlval: fpga register value; \r\n");
    fflush(stdout);
    return;
}

extern vos_u32 ADtimer;
extern vos_u32 ADtime;
extern vos_u32 ADmode;
/*****************************************************************************
 函 数 名  : app_debugabsvalue
 功能描述  : 测试吸光度值
 输入参数  : app_char *mode ; 1:
单杯测光(1个杯的AD)，0:单周期测光(41个杯的AD)
 输出参数  : 无
 返 回 值  : 
 
 修改历史      :
  1.日    期   : 2014年9月15日
    作    者   : zxc
    修改内容   : 新生成函数

*****************************************************************************/
vos_u32 app_debugabsvalue(char *mode, char *time)
{
    vos_u32 ulrunrslt = VOS_OK;
    vos_u32 ulmode;         //AD_MODE 寄存器上默认是正常态 0-正常  1-测试
    //vos_u32 uladstart = 1;  //AD_START 寄存器上默认是0，0-停止 1-启动
    //vos_u32 ctrlkey   = AD_START;

    ulrunrslt = app_strnumToIntnum(mode, &ulmode);
    if (VOS_OK != ulrunrslt)
    {
        return ulrunrslt;
    }
    ulrunrslt = app_strnumToIntnum(time, &ADtime);
    if (VOS_OK != ulrunrslt)
    {
        return ulrunrslt;
    }
    switch (ulmode)
    {
    case app_ad_fpga_work_mode_tst://测试模式，单杯测光
    {
        staticTestAD();
        break;
    } 
    case app_ad_fpga_work_mode_nml://正常模式，单周期测光
    {
        dynamicTsetAD();
        break;
    }
    default:
        return VOS_OK;
    }
    ADmode = ulmode;
    if (0 == ADtime)
    {
        vos_stoptimer(ADtimer);
    }
    else
    {
        startTimer(ADtimer, ADtime);
    }
    return VOS_OK;

}

/*****************************************************************************
 函 数 名  : app_showabsvaluehelpinfo
 功能描述  : 动态测光帮助信息
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 
 
 修改历史      :
  1.日    期   : 2014年9月15日
    作    者   : zxc
    修改内容   : 新生成函数

*****************************************************************************/
void app_showabsvaluehelpinfo()
{
    printf("\r command format: app_debugabsvalue ctrlkey ctrlval \r\n");
    printf(" ulpara0  : 1、test mode (1 AD);0、noml mode (41 AD)");
    fflush(stdout);
    return;
}

vos_u32 app_debugctrllight(char *pmode)
{
    vos_u32 ulrunrslt = VOS_OK;
	vos_u32 mode;

	ulrunrslt = app_strnumToIntnum(pmode, &mode);
    if (VOS_OK != ulrunrslt)
    {
        printf("ERRORR");
        return ulrunrslt;
    }

    msg_stLightControl stPara;
    stPara.lightIntensity = mode;
    return app_reqDwmCmd(Pid_DebugTty, SPid_ReactDsk, Act_Light, sizeof(stPara), (char*)&stPara, APP_NO);
}

void app_debugctrllighthelpinfo()
{
    printf("\r command format: app_debugctrllight {para0} \r\n");
    printf(" para0 --- 0:close; 1:rest; 100:open.\r\n");
    fflush(stdout);
    return;
}

/*****************************************************************************
 函 数 名  : app_openprintswitch
 功能描述  : 打开模块级的串口打印开关
 输入参数  : char* pucmdname
 输出参数  : 无
 返 回 值  : vos_u32
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2014年8月27日
    作    者   : Albort,Feng
    修改内容   : 新生成函数

*****************************************************************************/
vos_u32 app_openprintswitch(char* pucmdname)
{
    vos_u32 ulrunrslt = VOS_OK;
    vos_u32 ulmdname  = 0;

    ulrunrslt = app_strnumToIntnum(pucmdname, &ulmdname);
    if (VOS_OK != ulrunrslt)
    {
        printf("ERRORR");
        return ulrunrslt;
    }

    if (ulmdname < ps_ebiDrv)
    {
        g_ulmdprintswitch |= ((vos_u32)(1 << ulmdname));
    }  
    return VOS_OK;
}

//设置日志级别
vos_u32 app_setloglevel(char* pucLogLevel)
{
    vos_u32 ulLogLevel = 0;
    if (VOS_OK != app_strnumToIntnum(pucLogLevel, &ulLogLevel))
    {
        printf("ERRORR");
    }
    else
    {
        SetLogLevel(ulLogLevel);
    }
    return VOS_OK;
}

/*****************************************************************************
 函 数 名  : app_showopenprintswitch
 功能描述  : 显示打开模块级的串口打印开关命令的帮助信息
 输入参数  : void
 输出参数  : 无
 返 回 值  : void
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2014年8月27日
    作    者   : Albort,Feng
    修改内容   : 新生成函数

*****************************************************************************/
void app_showopenprintswitch(void)
{
    printf("\r command format: app_openprintswitch mdname\r\n");
    printf("\r funcation: open special module print switch.\r\n");
    printf("\r mdname: the special module name.\r\n");
    printf("\r         0: command module.\r\n");
    printf("\r         1: common system module.\r\n");
    printf("\r         2: middwnmach cmd center module.\r\n");
    printf("\r         3: common excute state machine module.\r\n");
    printf("\r         4: uart management module.\r\n");
    printf("\r         5: oam module.\r\n");
    printf("\r         6: react ctrl system module.\r\n");
    printf("\r         7: reagent ctrl system module.\r\n");
    printf("\r         8: sample ctrl system module.\r\n");
    printf("\r         9: resource schedule module.\r\n");
    printf("\r         10: task management module.\r\n");
    printf("\r         11: oam dark current module.\r\n");
    printf("\r         12: oam management module.\r\n");
    printf("\r         13: oam reagent add sample module.\r\n");
    printf("\r         14: oam add sample module.\r\n");
    printf("\r         15: uart link detect module.\r\n");
    printf("\r         16: app and drv interface module.\r\n");
    printf("\r         17: scheduled react cup module.\r\n");
	printf("\r         18: board scan module.\r\n");
    printf("\r         19: sampling AD value module.\r\n");
    printf("\r         24: compensation module.\r\n");
    printf("\r         25: dwnmach fault management module .\r\n");
    printf("\r         26: midmach fault management module .\r\n");
    
    printf("\r         32: EBI drv module.\r\n");
    printf("\r         33: open all module print switch.\r\n");
    
    fflush(stdout);
    return;
}

void app_showsetloglevel(void)
{
    printf("Set Log Level: ERROR-0,WARN-1,INFO-2\n");
    fflush(stdout);
}

/*****************************************************************************
 函 数 名  : app_closeprintswitch
 功能描述  : 关闭模块级的串口打印开关
 输入参数  : char* pucmdname
 输出参数  : 无
 返 回 值  : vos_u32
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2014年8月27日
    作    者   : Albort,Feng
    修改内容   : 新生成函数

*****************************************************************************/
vos_u32 app_closeprintswitch(char* pucmdname)
{
    vos_u32 ulrunrslt = VOS_OK;
    vos_u32 ulmdname = 0;

    ulrunrslt = app_strnumToIntnum(pucmdname, &ulmdname);
    if (VOS_OK != ulrunrslt)
    {
        printf("ERRORR");
        return ulrunrslt;
    }

    if(33 == ulmdname)
    {
        g_ulmdprintswitch = 0;
    }
    else
    {
        g_ulmdprintswitch &= ((vos_u32)(~(1 << ulmdname)));
    }
    return VOS_OK;
}

/*****************************************************************************
 函 数 名  : app_showcloseprintswitch
 功能描述  : 显示关闭模块级的串口打印开关命令的帮助信息
 输入参数  : void
 输出参数  : 无
 返 回 值  : void
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2014年8月27日
    作    者   : Albort,Feng
    修改内容   : 新生成函数

*****************************************************************************/
void app_showcloseprintswitch(void)
{
    printf("\r command format: app_closeprintswitch mdname\r\n");
    printf("\r funcation: close special module print switch.\r\n");
    printf("\r mdname: the special module name.\r\n");
    printf("\r         0: command module.\r\n");
    printf("\r         1: common system module.\r\n");
    printf("\r         2: middwnmach cmd center module.\r\n");
    printf("\r         3: common excute state machine module.\r\n");
    printf("\r         4: uart management module.\r\n");
    printf("\r         5: oam module.\r\n");
    printf("\r         6: react ctrl system module.\r\n");
    printf("\r         7: reagent ctrl system module.\r\n");
    printf("\r         8: sample ctrl system module.\r\n");
    printf("\r         9: resource schedule module.\r\n");
    printf("\r         10: task management module.\r\n");
    printf("\r         11: oam dark current module.\r\n");
    printf("\r         12: oam management module.\r\n");
    printf("\r         13: oam reagent add sample module.\r\n");
    printf("\r         14: oam add sample module.\r\n");
    printf("\r         15: uart link detect module.\r\n");
    printf("\r         16: app and drv interface module.\r\n");
    printf("\r         17: scheduled react cup module.\r\n");
    printf("\r         18: board scan module.\r\n");
    printf("\r         19: sampling AD value module.\r\n");
    printf("\r         24: compensation module.\r\n");
    printf("\r         25: dwnmach fault management module .\r\n");
    printf("\r         26: midmach fault management module .\r\n");
    
    printf("\r         32: EBI drv module.\r\n");
    printf("\r         33: close all module print switch.\r\n");
    
    fflush(stdout);
    return;
}

/*****************************************************************************
 函 数 名  : app_printtimerstatus
 功能描述  : 打印定时器信息
 输入参数  : void
 输出参数  : 无
 返 回 值  : vos_u32
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2014年9月4日
    作    者   : Albort,Feng
    修改内容   : 新生成函数

*****************************************************************************/
vos_u32 app_printtimerstatus(void)
{
    vos_printalltimerinfo();
    
    return VOS_OK;
}

/*****************************************************************************
 函 数 名  : app_showprinttimerstatushelpinfo
 功能描述  : 显示查看定时器信息命令的帮助信息
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
void app_showprinttimerstatushelpinfo(void)
{
    printf("\r command format: app_printtimerstatus \r\n");
    printf("\r function: display all timer current info.\r\n");

    fflush(stdout);
    return;
}

/*****************************************************************************
 函 数 名  : app_startuartloopbacktest
 功能描述  : 开始下位机串口环回测试命令
 输入参数  : char* pucdwnmachname
             char* puclooplayer
             char* puctimeoutlen
             char* pucloopnum
 输出参数  : 无
 返 回 值  : vos_u32
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2014年9月17日
    作    者   : Albort,Feng
    修改内容   : 新生成函数

*****************************************************************************/
vos_u32 app_startuartloopbacktest(char* pucdwnmachname, char* puclooplayer,
                                  char* puctimeoutlen, char* pucloopnum)
{
    vos_u32 ulrunrslt     = VOS_OK;
    vos_u32 endwnmachname = xc8002_dwnmach_name_butt;
    vos_u32 ullooplayer   = xc8002_com_loopback_layer_butt;
    vos_u32 ultimeoutlen  = 2; //默认超时为200ms
    vos_u32 ulloopnum     = 5; //默认为5次

    app_start_lpbck_tst_req_msg_stru* pstvosmsg = VOS_NULL;

    ulrunrslt = app_strnumToIntnum(pucdwnmachname, &endwnmachname);
    if (VOS_OK != ulrunrslt)
    {
        return ulrunrslt;
    }

    ulrunrslt = app_strnumToIntnum(puclooplayer, &ullooplayer);
    if (VOS_OK != ulrunrslt)
    {
        return ulrunrslt;
    }

    ulrunrslt = app_strnumToIntnum(puctimeoutlen, &ultimeoutlen);
    if (VOS_OK != ulrunrslt)
    {
        ultimeoutlen = 2; //默认超时为200ms
    }

    ulrunrslt = app_strnumToIntnum(pucloopnum, &ulloopnum);
    if (VOS_OK != ulrunrslt)
    {
        ulloopnum = 5; //默认为5次
    }

    ulrunrslt = vos_mallocmsgbuffer(sizeof(app_start_lpbck_tst_req_msg_stru),
                                    (void**)(&pstvosmsg));
    if (VOS_OK != ulrunrslt)
    {
        printf("\r Call vos_mallocmsgbuffer() failed(0x%x)\r\n", (int)ulrunrslt);
        fflush(stdout);
        return ulrunrslt;
    }

    pstvosmsg->uldstpid     = Pid_LoopTest;
    pstvosmsg->ulsrcpid     = Pid_DebugTty;
    pstvosmsg->usmsgtype    = app_req_spsuart_lpbck_tst_msg_type;
    pstvosmsg->usmsgloadlen = vos_calcvosmsgldlen(app_start_lpbck_tst_req_msg_stru);
    
    pstvosmsg->endwnmachname  = (app_u16)endwnmachname;
    pstvosmsg->enloopendlayer = (app_u16)ullooplayer;
    pstvosmsg->uslpbcknum     = (app_u16)ulloopnum;
    pstvosmsg->ustimeoutlen   = (app_u16)ultimeoutlen;

    ulrunrslt = vos_sendmsg((vos_msg_header_stru*)pstvosmsg);
    if (VOS_OK != ulrunrslt)
    {
        printf("\r Call vos_sendmsg() failed(0x%x)\r\n", (int)ulrunrslt);
        fflush(stdout);
        return ulrunrslt;
    }
    
    return VOS_OK;
}

/*****************************************************************************
 函 数 名  : app_showstartuartloopbacktestcmdhelpinfo
 功能描述  : 显示启动串口环回测试命令的帮助信息
 输入参数  : void
 输出参数  : 无
 返 回 值  : void
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2014年9月17日
    作    者   : Albort,Feng
    修改内容   : 新生成函数

*****************************************************************************/
void app_showstartuartloopbacktestcmdhelpinfo(void)
{
    printf("\r command format: app_startuartloopbacktest dwnmachname looplayer timeoutlen testnum \r\n");
    printf("\r function: start the special down machine's uart loopback test.\r\n");
    printf("\r dwnmachname: the name for waiting test down machine.\r\n");
    printf("\r           0: sample disk.\r\n");
    printf("\r           1: sample needle.\r\n");
    printf("\r           2: inner reagent disk.\r\n");
    printf("\r           3: R1 reagent needle.\r\n");
    printf("\r           4: outer reagent disk.\r\n");
    printf("\r           5: R2 reagent needle.\r\n");
    printf("\r           6: react disk.\r\n");
    printf("\r           7: auto washing module.\r\n");
    printf("\r           8: sample mix rod.\r\n");
    printf("\r           9: reagent mix rod.\r\n");
    printf("\r          10: auto temperature control.\r\n");
    printf("\r          11: cooling reagent.\r\n");

    printf("\r looplayer: the loopback packet reach to module name.\r\n");
    printf("\r           0: the mid machine's app.\r\n");
    printf("\r           1: the mid machine's drv.\r\n");
    printf("\r           2: the mid machine's fpga.\r\n");
    printf("\r           3: the down machine's drv.\r\n");
    printf("\r           4: the down machine's app.\r\n");

    printf("\r timeoutlen: time out lengthe(unit: 100ms), default 2.\r\n");
    printf("\r testnum: the loopback test number, default 5.\r\n");
    
    fflush(stdout);
    return;
}

/*****************************************************************************
 函 数 名  : app_debugbarcodescan
 功能描述  : 执行条码扫描
 输入参数  : char* puctraytype  
             char* puctrayno    
             char* puccupno     
 输出参数  : 无
 返 回 值  : 
 
 修改历史      :
  1.日    期   : 2014年11月5日
    作    者   : 唐衡
    修改内容   : 新生成函数

*****************************************************************************/
vos_u32 app_debugbarcodescan(char* puctraytype, char* puctrayno, char* puccupno)
{
    vos_u32 traytype;   //盘类型  1: 样本盘 2: 试剂盘
    vos_u32 trayno;     //盘编号
    vos_u32 cupno;      //杯位号 0: 表示所有杯
    STCupPos * pCupPos = NULL;
    
    app_strnumToIntnum(puctraytype, &traytype);
    app_strnumToIntnum(puctrayno, &trayno);
    app_strnumToIntnum(puccupno, &cupno);

    STReqBarcodeScan  stPara;
	
	if(traytype == app_xc8002_tray_type_sample)
	{	
		pCupPos = stPara.add_stsmpcup();
	}
	if(traytype == app_xc8002_tray_type_reagent)
	{
    	pCupPos = stPara.add_streagcup();
	}
    pCupPos->set_uidskid(trayno);
	pCupPos->set_uicupid(cupno); 
  
    return sendInnerMsg(SPid_BcScan, Pid_DebugTty, MSG_ReqBarcodeScan, &stPara);
}

/*****************************************************************************
 函 数 名  : app_showbarcodescanhelpinfo
 功能描述  : 条码扫描帮助信息
 输入参数  : void  
 输出参数  : 无
 返 回 值  : 
 
 修改历史      :
  1.日    期   : 2014年11月5日
    作    者   : 唐衡
    修改内容   : 新生成函数

*****************************************************************************/
void app_showbarcodescanhelpinfo(void)
{
    printf("\r command format: app_debugbarcodescan traytype trayno cupno \r\n");
    printf("\r function: scan barcode \r\n");
    printf("\r traytype: \r\n");
    printf("\r        1: sample disk.\r\n");
    printf("\r        2: reagent disk.\r\n");
    printf("\r trayno:\r\n");
    printf("\r        [0~2]: if sample disk.\r\n");
    printf("\r        [1~2]: if reagent disk. 1: outer disk 2: inner disk \r\n");
    printf("\r cupno:\r\n");
    printf("\r        [1~35]: if sample disk and no. 0 circle .\r\n");
	printf("\r        [36~70]: if sample disk and no. 1 circle .\r\n");
	printf("\r        [71~105]: if sample disk and no. 2 circle .\r\n");
    printf("\r        [1~65]: if reagent disk and no. 1 circle(outer).\r\n");
    printf("\r        [1~45]: if reagent disk and no. 2 circle(inner).\r\n");

    fflush(stdout);
    return;
}

vos_u32 app_debugmarginscan(char* puctrayno, char* puccupno)
{
    vos_u32 trayno;     //盘编号
    vos_u32 cupno;      //杯位号 0: 表示所有杯
    
    app_strnumToIntnum(puctrayno, &trayno);
    app_strnumToIntnum(puccupno, &cupno);

    STReqMarginScan  stPara;

	STCupPos * pCupPos = stPara.add_stscancup();
    pCupPos->set_uidskid(trayno);
	pCupPos->set_uicupid(cupno);      
	
    return sendInnerMsg(SPid_MarginScan, Pid_DebugTty, MSG_ReqMarginScan, &stPara);
}

void app_showmarginscanhelpinfo(void)
{
    printf("\r command format: app_debugmarginscan trayno cupno \r\n");
    printf("\r function: scan reag margin \r\n");
    printf("\r trayno:\r\n");
    printf("\r        [1~2]: if reagent disk. 1: outer disk 2: inner disk \r\n");
    printf("\r cupno:\r\n");
    printf("\r        [1~65]: if reagent disk and no. 1 circle(outer).\r\n");
    printf("\r        [1~45]: if reagent disk and no. 2 circle(inner).\r\n");

    fflush(stdout);
    return;
}

vos_u32 app_debugsmooth(char* circleNum)
{
    vos_u32 circlenum;  //反应盘转动周期数
    
    app_strnumToIntnum(circleNum, &circlenum);

    STReqSmoothAreaTest  stPara;
	stPara.set_uicircle(circlenum);
  
    return sendInnerMsg(SPid_Smooth, Pid_DebugTty, MSG_ReqSmoothAreaTest, &stPara);
}

void app_showsmoothhelpinfo(void)
{
    printf("\r command format : app_debugsmooth para \r\n");
    printf("\r para : circle of reacdsk rtt\r\n");

    fflush(stdout);
    return;
}

vos_u32 app_debugshowave(char* para_Switch, char* para_Channel, char* para_Signal)
{
    vos_u32 Switch;
    vos_u32 Channel;
    vos_u32 Signal;
    
    app_strnumToIntnum(para_Switch, &Switch);
	app_strnumToIntnum(para_Channel, &Channel);
	app_strnumToIntnum(para_Signal, &Signal);

    STReqShowWave  stPara;
	stPara.set_uiswitch(Switch);
	stPara.set_uichannel(Channel);
	stPara.set_uisignal(Signal);
  
    return sendInnerMsg(SPid_Showave, Pid_DebugTty, MSG_ReqShowWave, &stPara);
}

void app_showavehelpinfo(void)
{
    printf("\r command format : app_debugshowave para0 para1 para2\r\n\n");
    printf("\r para0 : Switch state , on/off\r\n");
    printf("\r para1 : Channel , upMach window surport 2 channel\r\n");
	printf("\r para2 : Signal , dwnMach to be tested \r\n");

    fflush(stdout);
    return;
}


vos_u32 app_debuginjectionairout(char* pucairouttimes)
{
    vos_u32 airouttimes = 0;
    vos_u32 ulrunrslt = app_strnumToIntnum(pucairouttimes, &airouttimes);
    if (VOS_OK != ulrunrslt)
    {
        return ulrunrslt;
    }

    STReqAirOut stPara;
    stPara.set_uiouttimes(airouttimes);

    return sendInnerMsg(SPid_AirOut, Pid_DebugTty, MSG_ReqAirOut, &stPara);
}

void app_showinjectionairouthelpinfo(void)
{
	printf("\r command format: app_debuginjectionairout num \r\n");
    printf("\r function: threendl injection airout \r\n");
    printf("\r num: ndl injection airout times.\r\n");

    fflush(stdout);
    return;
}

/*****************************************************************************
 函 数 名  : app_debugpvctl
 功能描述  : 泵阀调试
 输入参数  : char* pucdsktype
             char* puccupid
 输出参数  : 无
 返 回 值  : vos_u32
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2014年11月21日
    作    者   : zxc
    修改内容   : 新生成函数

*****************************************************************************/
vos_u32 app_debugpvctl(char *num, char *ctrl)
{
    vos_u32 ulrunrslt = VOS_OK;
    vos_u32 ulPvunm;
    vos_u32 ulCtrl;     //0:关，1:关

    ulrunrslt = app_strnumToIntnum(num, &ulPvunm);
    if (VOS_OK != ulrunrslt)
    {
        return ulrunrslt;
    }

    ulrunrslt = app_strnumToIntnum(ctrl, &ulCtrl);
    if (VOS_OK != ulrunrslt)
    {
        return ulrunrslt;
    }
 
    map<vos_u32, vos_u32> pvNum;
    pvNum[2] = v02;
    pvNum[3] = v03;
    pvNum[4] = v04;
    pvNum[5] = v05;
    pvNum[7] = v07;
    pvNum[8] = v08;
    pvNum[9] = v09;
    pvNum[10] = v10;
    pvNum[11] = v11;
    pvNum[14] = v14;
    pvNum[15] = v15;
    pvNum[16] = v16;
    pvNum[17] = v17;
    pvNum[18] = v18;
    pvNum[19] = v19;
    pvNum[23] = v23;
    pvNum[25] = v25;
    pvNum[27] = v27;
    pvNum[103] = p03;
    pvNum[104] = p04;
    pvNum[106] = p06;
    pvNum[108] = p08;
    map<vos_u32, vos_u32>::iterator iter = pvNum.find(ulPvunm);
    
    if (iter != pvNum.end())
    {  
        msg_PVCtrl pvCtrl;
        pvCtrl.operateType = ulCtrl;
        pvCtrl.PVNum = iter->second;
        switch (ulPvunm)
        {
            case 2:
            case 7:
            {
                return app_reqDwmCmd(SPid_Maintain, SPid_SmpNdl, Act_PVCtrl, sizeof(pvCtrl), (char *)&pvCtrl, APP_YES);
            }
            case 3:
            case 8:
            {
                return app_reqDwmCmd(SPid_Maintain, SPid_R1, Act_PVCtrl, sizeof(pvCtrl), (char *)&pvCtrl, APP_YES);
            }
            case 4:
            case 9:
            {
                return app_reqDwmCmd(SPid_Maintain, SPid_R2, Act_PVCtrl, sizeof(pvCtrl), (char *)&pvCtrl, APP_YES);
            }
            case 10:
            {
                return app_reqDwmCmd(SPid_Maintain, SPid_SMix, Act_PVCtrl, sizeof(pvCtrl), (char *)&pvCtrl, APP_YES);
            }
            case 11:
            {
                return app_reqDwmCmd(SPid_Maintain, SPid_R2Mix, Act_PVCtrl, sizeof(pvCtrl), (char *)&pvCtrl, APP_YES);
            }
            default:
                return app_reqDwmCmd(SPid_Maintain, SPid_AutoWash, Act_PVCtrl, sizeof(pvCtrl), (char *)&pvCtrl, APP_YES);
        }
    }
    return VOS_OK;
}

/*****************************************************************************
 函 数 名  : app_showdebugwashtestcmdhelpinfo
 功能描述  : 试剂余量扫描命令帮助信息
 输入参数  : void
 输出参数  : 无
 返 回 值  : void
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2014年11月21日
    作    者   : zxc
    修改内容   : 新生成函数

*****************************************************************************/
void app_showdebugpvctlcmdhelpinfo(void)
{ 
    printf("\r command format: app_debugpvctl pvnum  pvctl \r\n");    
    printf("\r para0:pvnum\n");
    printf("\r para1:pvctl\n");
    fflush(stdout);
    return;
}




/*****************************************************************************
 函 数 名  : app_debugwashtest
 功能描述  : 液路调试命令
 输入参数  : char* pucdsktype
             char* puccupid
 输出参数  : 无
 返 回 值  : vos_u32
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2014年11月21日
    作    者   : zxc
    修改内容   : 新生成函数
 2. 日    期   : 2015年4月1日
    作    者   : zxc
    修改内容   : 根据需求重写
2. 日    期   : 2015年11月30日
   作    者   : PengChuan
   修改内容   : 添加判断，将组合命令处理放到maintain模块
*****************************************************************************/
vos_u32 app_debugwashtest(char *cmd, char *pvnum, char *ctl)
{
    vos_u32 ulrunrslt = VOS_OK;
    vos_u32 ulcmd;
    vos_u32 aucpvunm;
    vos_u32 aucctl;     //0:关，1:关
    vos_u32 ulmsgsize;

    ulrunrslt = app_strnumToIntnum(cmd, &ulcmd);
    if (VOS_OK != ulrunrslt)
    {
        printf("Call app_strnumToIntnum() failed(0x%x)\r\n", (int)ulrunrslt);
        return ulrunrslt;
    }
    
    if ((0 == ulcmd) || (1 == ulcmd) || (5 == ulcmd) || (8 == ulcmd) || (9 == ulcmd))
    {
        msg_stDwnCmdReq* pstmsg = VOS_NULL;
        ulmsgsize = sizeof(msg_stDwnCmdReq);
        
        ulrunrslt = vos_mallocmsgbuffer(ulmsgsize, (void**)(&pstmsg));
        if (VOS_OK != ulrunrslt)
        {
            printf("Call vos_mallocmsgbuffer() failed(0x%x)\r\n", (int)ulrunrslt);
            return ulrunrslt;
        } 
        
        ulrunrslt = app_strnumToIntnum(pvnum, &aucpvunm);
        if (VOS_OK != ulrunrslt)
        {
            vos_freemsgbuffer((vos_msg_header_stru *)pstmsg);
            return ulrunrslt;
        }

        ulrunrslt = app_strnumToIntnum(ctl, &aucctl);
        if (VOS_OK != ulrunrslt)
        {
            vos_freemsgbuffer((vos_msg_header_stru *)pstmsg);
            return ulrunrslt;
        }

        switch (ulcmd)
        {
            case 0:
                pstmsg->encmdtype = cmd_auto_wash_pnctl;
                pstmsg->aucrsv[0] = aucpvunm;
                pstmsg->aucrsv[1] = aucctl;
                break;    
            case 1:
                pstmsg->encmdtype = cmd_auto_wash_test_1;
                break;  
            case 5:
                pstmsg->encmdtype = cmd_auto_wash_test_5;            
                break;
            case 8:
                pstmsg->encmdtype = cmd_auto_wash_test_8;            
                break;
            case 9:
                pstmsg->encmdtype = cmd_auto_wash_test_9;            
                break;   
            default:
                printf(" ulcmdtype(0x%x) is error.\r\n", (int)aucpvunm);
                ulrunrslt = APP_INVALID_U32;
                break;
        }

        if (APP_INVALID_U32 == ulrunrslt)
        {
            vos_freemsgbuffer((vos_msg_header_stru *)pstmsg);
            pstmsg = VOS_NULL;
            RETURNERRNO(app_debugtty_errorcode_base, app_input_para_invalid_err);
        }
        
        pstmsg->uldstpid     = Pid_DwnMachUart;
        pstmsg->ulsrcpid     = Pid_DebugTty;
        pstmsg->usmsgtype    = app_req_dwnmach_act_cmd_msg_type;
        pstmsg->endwnmachsubsys = xc8002_dwnmach_name_autowash;
        pstmsg->usmsgloadlen = ulmsgsize - sizeof(vos_msg_header_stru);

        ulrunrslt = vos_sendmsg((vos_msg_header_stru*)pstmsg);
        if (VOS_OK != ulrunrslt)
        {
            printf("Call vos_sendmsg() failed(0x%x)\r\n", (int)ulrunrslt);
        }
        
        return ulrunrslt;
    }
    else
    {
        vos_app_maintain_debug_cmd_stru* pPara = VOS_NULL;
        
        ulmsgsize = sizeof(vos_app_maintain_debug_cmd_stru);
        ulrunrslt = app_mallocBuffer(ulmsgsize, (void**)(&pPara));   
        if (VOS_OK != ulrunrslt)
        {
            printf("Call app_mallocBuffer() failed(0x%x)\r\n", (int)ulrunrslt);
            return ulrunrslt;
        }
        pPara->ulcmdlen  = sizeof(vos_command_info_stru);
        switch (ulcmd)
        {           
            case 2:
                pPara->pcmdinfo.ulcommandid = app_auto_wash_test_2_req;
                break;
            case 3:
                pPara->pcmdinfo.ulcommandid = app_auto_wash_test_3_req;
                break;  
            case 4:
                pPara->pcmdinfo.ulcommandid = app_auto_wash_test_4_req;
                break;  
            case 6:
                pPara->pcmdinfo.ulcommandid = app_auto_wash_test_6_req;            
                break;
            case 71:
                pPara->pcmdinfo.ulcommandid = app_auto_wash_test_71_req;            
                break; 
            case 72:
                pPara->pcmdinfo.ulcommandid = app_auto_wash_test_72_req;            
                break;
            case 1000:
                pPara->pcmdinfo.ulcommandid = app_auto_wash_close_allPV_req;            
                break;    
            default:
                pPara->pcmdinfo.ulcommandid = app_auto_wash_close_allPV_req;  
                break;
        }
        msg_stHead rHead;
        app_constructMsgHead(&rHead, Pid_Maintain, Pid_DebugTty, app_maintain_msg_type);
        return app_sendMsg(&rHead, pPara, ulmsgsize);
    }
}

/*****************************************************************************
 函 数 名  : app_showdebugwashtestcmdhelpinfo
 功能描述  : 液路调试帮助信息
 输入参数  : void
 输出参数  : 无
 返 回 值  : void
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2014年11月21日
    作    者   : zxc
    修改内容   : 新生成函数

*****************************************************************************/
void app_showdebugwashtestcmdhelpinfo(void)
{ 
    printf("\r command format: app_debugwashtest cmd, pvnum, ctl \r\n");    
    printf("\r para0:cmd\n");
    printf("\r cmd = 0: for pvctl\n");
    printf("\r cmd = 1: for test_1 detergent wash\n");
    printf("\r cmd = 2: for test_2 outer wash\n");
    printf("\r cmd = 3: for test_3 stir wash\n");
    printf("\r cmd = 4: for test_4 inner wash\n");
    printf("\r cmd = 5: for test_5 water wash\n");
    printf("\r cmd = 6: for test_6 water flood\n");
    printf("\r cmd = 71:for test 7_1\n");
    printf("\r cmd = 72:for test 7_2\n");
    printf("\r cmd = 8: for test_8 open vacuum drain\n");
    printf("\r cmd = 9: for test_9 close vacuum drain\n");
    printf("\r cmd = 1000:close all PV\n");
    
    printf("\r para1:pvnum(input only cmd = 0)\n");
    printf("\r para2:ctl(input only cmd = 0)\n");
    fflush(stdout);
    return;
}

/*****************************************************************************
 函 数 名  : app_enabletempraturereport
 功能描述  : 设置温度上报
 输入参数  : void
 输出参数  : 无
 返 回 值  : vos_u32
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2014年12月17日
    作    者   : Albort,Feng
    修改内容   : 新生成函数

*****************************************************************************/
vos_u32 app_settempraturereport(char* pucpara)
{
    vos_u32 ulpara    = 0;
    vos_u32 ulrunrslt = VOS_OK;
    
    ulrunrslt = app_strnumToIntnum(pucpara, &ulpara);
    if (VOS_OK != ulrunrslt)
    {
        printf("ERRORR");
        return ulrunrslt;
    }

    #ifdef COMPILER_IS_ARM_LINUX_GCC
    if (0 == ulpara)
    {
        vos_stoptimer(COamMng::m_ulTimer);
    }
    else
    {
        vos_starttimer(COamMng::m_ulTimer);
    }
    
    #else
    app_settempraturereportbydisp(ulpara);
    #endif
    
    return VOS_OK;
}

/*****************************************************************************
 函 数 名  : app_showsettempraturereportcmdhelpinfo
 功能描述  : 显示设置温度上报命令帮助信息
 输入参数  : void
 输出参数  : 无
 返 回 值  : void
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2014年12月17日
    作    者   : Albort,Feng
    修改内容   : 新生成函数

*****************************************************************************/
void app_showsettempraturereportcmdhelpinfo(void)
{
    printf("\r command format: app_settempraturereport para0\r\n");
    printf("\r function: start or stop auto report temprature.\r\n");
    printf("\r para0:\r\n");
    printf("\r     0:disable auto report temprature\r\n");
    printf("\r     1:enable auto report temprature\r\n");

    fflush(stdout);
    return;
}

/*****************************************************************************
 函 数 名  : app_setschedulecircle
 功能描述  : 设置调度周期长度
 输入参数  : char* pucexecircle
             char* pucprecircle
 输出参数  : 无
 返 回 值  : vos_u32
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2014年12月18日
    作    者   : Albort,Feng
    修改内容   : 新生成函数

*****************************************************************************/
vos_u32 app_setschedulecircle(char* t, char* t1, char* t2, char* t3)
 {
    vos_u32 time = 36;
    vos_u32 time1 = 0;
    vos_u32 time2 = 0;
    vos_u32 time3 = 0;

    app_strnumToIntnum(t, &time);
    app_strnumToIntnum(t1, &time1);
    app_strnumToIntnum(t2, &time2);
    app_strnumToIntnum(t3, &time3);
    time1 = (time1 == 0) ? (time / 4) : time1;
    time2 = (time2 == 0) ? (time * 7 / 10) : time2;
    time3 = (time3 == 0) ? (time * 7 / 10) : time3;
    g_pActHandler->SetPri(time, time1, time2, time3);
    return VOS_OK;
}

/*****************************************************************************
 函 数 名  : app_showsetschedulecirclecmdhelpinfo
 功能描述  : 设置调度周期长度命令帮助信息
 输入参数  : void
 输出参数  : 无
 返 回 值  : void
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2014年12月18日
    作    者   : Albort,Feng
    修改内容   : 新生成函数

*****************************************************************************/
void app_showsetschedulecirclecmdhelpinfo(void)
{
    printf("\r command format: app_setschedulecircle para0 pare1\r\n");
    printf("\r function: set the schedule period length.\r\n");
    printf("\r para0: excute period length,default 3.2s(unit:0.1s)\r\n");
    printf("\r para1: prepare period length,default 1.3s(unit:0.1s)\r\n");

    fflush(stdout);
    return;
}

/*****************************************************************************
 函 数 名  : app_debugcooling
 功能描述  : 试剂制冷调试命令
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 
 
 修改历史      :
  1.日    期   : 2015年4月8日
    作    者   : zxc
    修改内容   : 新生成函数

*****************************************************************************/
vos_u32 app_debugcooling(char *cmd,char *para0,char *para1,char *para2)
{
    vos_u32 ulrunrslt = VOS_OK;
    vos_u32 ulmsgsize = 0;
	vos_u32 ulinvalidcmd = APP_FALSE;
    vos_u32 ulcmdtype;
    vos_u32 para[3]= {0};
    
	msg_stDwnCmdReq* pstmsg = VOS_NULL;
    cmd_cooling_debug_stru *pstcoolingctl = VOS_NULL;
    cmd_cooling_paraset_stru* pstparaset= VOS_NULL;
    
    ulrunrslt = app_strnumToIntnum(cmd, &ulcmdtype);
    if (VOS_OK != ulrunrslt)
    {
        return ulrunrslt;
    }
    
    GET_DEBUG_PARA_PARSE(0);
    GET_DEBUG_PARA_PARSE(1);
    GET_DEBUG_PARA_PARSE(2);
    //GET_DEBUG_PARA_PARSE(3);
    
    switch (ulcmdtype)
    {
        case cmd_cooling_debug:
            ulmsgsize = sizeof(msg_stDwnCmdReq)
                      - sizeof(xc8002_com_dwnmach_cmd_stru)
                      + sizeof(cmd_cooling_debug_stru);
            break;
        case cmd_cooling_control:
            ulmsgsize = sizeof(msg_stDwnCmdReq)
                      - sizeof(xc8002_com_dwnmach_cmd_stru)
                      + sizeof(cmd_cooling_debug_stru);
            break;
        case cmd_cooling_settemp:
            ulmsgsize = sizeof(msg_stDwnCmdReq)
                      - sizeof(xc8002_com_dwnmach_cmd_stru)
                      + sizeof(cmd_cooling_paraset_stru);
            break;
        default:
            printf(" ulcmdtype(%d) is error.\r\n", (int)ulcmdtype);
            ulinvalidcmd = APP_TRUE;
            break;
    }

    if (APP_TRUE == ulinvalidcmd)
    {
        RETURNERRNO(app_debugtty_errorcode_base,app_input_para_invalid_err);
    }
    
    ulrunrslt = vos_mallocmsgbuffer(ulmsgsize, (void**)(&pstmsg));
    if (VOS_OK != ulrunrslt)
    {
        printf("Call vos_mallocmsgbuffer() failed(0x%x)\r\n", (int)ulrunrslt);
        return ulrunrslt;
    }

    pstmsg->uldstpid     = Pid_DwnMachUart;
    pstmsg->ulsrcpid     = Pid_DebugTty;
    pstmsg->usmsgtype    = app_req_dwnmach_act_cmd_msg_type;
    pstmsg->usmsgloadlen = ulmsgsize - sizeof(vos_msg_header_stru);

    pstmsg->endwnmachsubsys = xc8002_dwnmach_name_cooling_reag;
    pstmsg->encmdtype = (app_u16)ulcmdtype;

    if(ulcmdtype == cmd_cooling_control)
    {
        pstcoolingctl = (cmd_cooling_debug_stru *)&(pstmsg->encmdtype);
        pstcoolingctl->switchctrl = para[0];
    }
    if(ulcmdtype == cmd_cooling_settemp)
    {
        pstparaset = (cmd_cooling_paraset_stru*)&(pstmsg->encmdtype);
        pstparaset->close_temp = para[0];
        pstparaset->open_temp  = para[1];
        pstparaset->LoopPumpOpen_temp = para[2];
    }
    ulrunrslt = vos_sendmsg((vos_msg_header_stru*)pstmsg);
    if (VOS_OK != ulrunrslt)
    {
        printf("Call vos_sendmsg() failed(0x%x)\r\n", (int)ulrunrslt);
    }
    
    return VOS_OK;
}

/*****************************************************************************
 函 数 名  : app_showdebugcoolinghelpinfo
 功能描述  : 试剂制冷调试命令帮助
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 
 
 修改历史      :
  1.日    期   : 2015年4月8日
    作    者   : zxc
    修改内容   : 新生成函数

*****************************************************************************/
void app_showdebugcoolinghelpinfo(void)
{
    printf("\r command format: app_debugcooling \r\n");
    printf("\r function: cooling debug.\r\n");
    printf("\r para0:cmd-0x4002:cooling ctl\n");
    printf("\r para1:ctl-0:close,1:open\n");

    printf("\r para0:cmd-0x4003:set temp\n");
    printf("\r para1:close_temp;      \n");
    printf("\r para2:open_temp;       \n");
    printf("\r para3:LoopPumpOpen_temp\n");

    fflush(stdout);
    return;
}

/*****************************************************************************
 函 数 名  : app_debugtempctl
 功能描述  : 温控调试命令 
 输入参数  : char *cmd
 输出参数  : 无
 返 回 值  : void
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2015年4月21日
    作    者   : zxc
    修改内容   : 新生成函数

*****************************************************************************/
vos_u32 app_debugtempctl( char *cmd, char *para0, char *para1,char *para2)
{
    return sendInnerMsg(SPid_DwnCmd, Pid_DebugTty, MSG_TempeQuery);

    vos_u32 ulrunrslt = VOS_OK;
    vos_u32 ulmsgsize = 0;
    vos_u32 ulinvalidcmd = APP_FALSE;
    vos_u32 ulcmdtype;
    vos_u32 para[3] = {0};
    msg_stDwnCmdReq* pstmsg = VOS_NULL;

    cmd_temp_ctrl_stru*         psttempctl = VOS_NULL;
    cmd_heater_ctrl_stru*       pstheaterctrl = VOS_NULL;
    cmd_reactdisk_tempset_stru* pstsetreacttemp = VOS_NULL;
    cmd_heater_targettemp_stru* psttargettemp = VOS_NULL;

    GET_DEBUG_PARA_PARSE(0);
    GET_DEBUG_PARA_PARSE(1);
    GET_DEBUG_PARA_PARSE(2);
    
    ulrunrslt = app_strnumToIntnum(cmd, &ulcmdtype);
    if (VOS_OK != ulrunrslt)
    {
        return ulrunrslt;
    }
    
    switch (ulcmdtype)
    {
        case cmd_temp_control_switch:
            ulmsgsize = sizeof(msg_stDwnCmdReq)
                      - sizeof(xc8002_com_dwnmach_cmd_stru)
                      + sizeof(cmd_temp_ctrl_stru);
            break;
        case cmd_temp_control_set_temp:
            ulmsgsize = sizeof(msg_stDwnCmdReq)
                      - sizeof(xc8002_com_dwnmach_cmd_stru)
                      + sizeof(cmd_heater_targettemp_stru);
            break;
        case cmd_temp_three_routes_ctl:
            ulmsgsize = sizeof(msg_stDwnCmdReq)
                      - sizeof(xc8002_com_dwnmach_cmd_stru)
                      + sizeof(cmd_heater_ctrl_stru);
            break;
        case cmd_temp_control_set_reacttemp:
            ulmsgsize = sizeof(msg_stDwnCmdReq)
                      - sizeof(xc8002_com_dwnmach_cmd_stru)
                      + sizeof(cmd_reactdisk_tempset_stru);
            break;
        default:
            printf(" ulcmdtype(%d) is error.\r\n", (int)ulcmdtype);
            ulinvalidcmd = APP_TRUE;
            break;
    }

    if (APP_TRUE == ulinvalidcmd)
    {
        RETURNERRNO(app_debugtty_errorcode_base,app_input_para_invalid_err);
    }
    
    ulrunrslt = vos_mallocmsgbuffer(ulmsgsize, (void**)(&pstmsg));
    if (VOS_OK != ulrunrslt)
    {
        printf("Call vos_mallocmsgbuffer() failed(0x%x)\r\n", (int)ulrunrslt);
        return ulrunrslt;
    }

    pstmsg->uldstpid     = Pid_DwnMachUart;
    pstmsg->ulsrcpid     = Pid_DebugTty;
    pstmsg->usmsgtype    = app_req_dwnmach_act_cmd_msg_type;
    pstmsg->usmsgloadlen = ulmsgsize - sizeof(vos_msg_header_stru);

    pstmsg->endwnmachsubsys = xc8002_dwnmach_name_auto_Rtmpctrl;
    pstmsg->encmdtype = (app_u16)ulcmdtype;

    if(cmd_temp_control_switch == ulcmdtype)
    {
        psttempctl = (cmd_temp_ctrl_stru*)&(pstmsg->encmdtype);
        psttempctl->switchctrl = para[0];
    }
    if(cmd_temp_three_routes_ctl == ulcmdtype)
    {
        pstheaterctrl = (cmd_heater_ctrl_stru*)&(pstmsg->encmdtype);
        pstheaterctrl->heaternum = para[0];
        pstheaterctrl->switchctrl= para[1];
    }
    if(cmd_temp_control_set_reacttemp == ulcmdtype)
    {
        pstsetreacttemp = (cmd_reactdisk_tempset_stru*)&(pstmsg->encmdtype);
        pstsetreacttemp->delta = para[0];
        pstsetreacttemp->tempreact = para[1];
    }
    if(cmd_temp_control_set_temp == ulcmdtype)
    {
        psttargettemp = (cmd_heater_targettemp_stru*)&(pstmsg->encmdtype);
        psttargettemp->temp1 = para[0];
        psttargettemp->temp2 = para[1];
        psttargettemp->temp3 = para[2];
    }
    ulrunrslt = vos_sendmsg((vos_msg_header_stru*)pstmsg);
    if (VOS_OK != ulrunrslt)
    {
        printf("Call vos_sendmsg() failed(0x%x)\r\n", (int)ulrunrslt);
    }

    return VOS_OK;
}

/*****************************************************************************
 函 数 名  : app_showdebugtempctlhelpinfo
 功能描述  : 温控调试命令帮助 
 输入参数  : char *cmd
 输出参数  : 无
 返 回 值  : void
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2015年4月21日
    作    者   : zxc
    修改内容   : 新生成函数

*****************************************************************************/

void app_showdebugtempctlhelpinfo(void)
{
    printf("\r command format: app_debugcooling cmd\r\n");
    printf("\r para0:cmd - 0x3801: temp ctl,close or open All\n");
    printf("\r para1:ctl,0:close all,1:open all\n");
    printf("\r -----\n");
    printf("\r para0: cmd-0x3804: set temp\n");
    printf("\r para1:temp1\n");
    printf("\r para2:temp2\n");
    printf("\r para3:temp3\n");
    printf("\r -----\n");
    printf("\r para0:cmd-0x3805: set reactdisk temp.\n");
    printf("\r para1:delta\n");
    printf("\r para2:react temp\n");
    printf("\r -----\n");
    printf("\r para0:cmd-0x3806: three routes temp ctl\n");
    printf("\r para1:heater num(1-4):5-all\n");
    printf("\r para2:ctl 0:close, 1:open\n");
    
    fflush(stdout);
    return;
}

//模拟报警
vos_u32 app_simulateAlarm(char* pucfaultid, char* pucstatus, char* para1, char* para2)
{
	vos_u32 ulfaultid        = 0x0;
	vos_u32 ulstatus         = 0;
    vos_u32 ulpara1			 = 0;
	vos_u32 ulpara2			 = 0;

	app_strnumToIntnum(pucfaultid, &ulfaultid);
	app_strnumToIntnum(pucstatus, &ulstatus);
	app_strnumToIntnum(para1, &ulpara1);
	app_strnumToIntnum(para2, &ulpara2);

	if (xc8002_fault_status_generate == ulstatus)
    {
		g_pAlarm->GenerateAlarm(ulfaultid, ulpara1, ulpara2);
    }
	else if (xc8002_fault_status_restore == ulstatus)
    {
		g_pAlarm->RecoverAlarm(ulfaultid, ulpara1);
    }
    return VOS_OK;
}

/*****************************************************************************
 函 数 名  : app_handrestorefaultinfo
 功能描述  : 模拟下位机故障上报命令帮助信息
 输入参数  : void
 输出参数  : 无
 返 回 值  : void
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2015年6月18日
    作    者   : wushuangxi
    修改内容   : 新生成函数

*****************************************************************************/
void app_simulateAlarmInfo(void)
{
    printf("\r command format: app_simulateAlarm Alarmid State Para \r\n");
    printf("\r function: simulate alarm report.\r\n");

    printf("\r [para1] Alarmid: the number of the alarm.\r\n");

    printf("\r [para2] State: .0: recover  1: trigger\r\n");
  
	printf("\r [para3] id: default downid.\r\n");

	printf("\r [para4] Para.\r\n");
    fflush(stdout);
    return;
}

/*****************************************************************************
 函 数 名  : app_debugintensifywash 
 功能描述  : 强化清洗命令
 输入参数  : char*
 输出参数  : 无
 返 回 值  : vos_u32
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2015年5月19日
    作    者   : zxc
    修改内容   : 新生成函数

*****************************************************************************/
vos_u32 app_debugintensifywash ()
{
    msg_stHead stHead;
    app_constructMsgHead(&stHead, SPid_IntensifyWash, Pid_DebugTty, MSG_ReqIntensifyWash);
    return app_sendMsg(&stHead, NULL, 0);
}
/*****************************************************************************
 函 数 名  : app_showdebugintensifywashhelpinfo
 功能描述  : 强化清洗帮助信息
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2015年5月19日
    作    者   : zxc
    修改内容   : 新生成函数

*****************************************************************************/
void  app_showdebugintensifywashhelpinfo( void)
{
    printf("\r command format: app_debugintensifywash\r\n");
    printf("\r function: req intensify wash for needls.\r\n");
}
/*****************************************************************************
 函 数 名  : app_subsysliquidpipe
 功能描述  : 液路子系统灌注
 输入参数  : 无
 输出参数  : 无
 返 回 值  : vos_u32
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2015年6月2日
    作    者   : zxc
    修改内容   : 新生成函数

*****************************************************************************/
vos_u32 app_subsysliquidpipe(char* para0)
{
    vos_u32 ulrunrslt = VOS_OK;
    vos_u32 para[1] = {0};

    GET_DEBUG_PARA(0);

    vos_u32 msglen    = 0;
    vos_u32 ulcommandid = 0;
    if(para[0] == 1)
    {
        ulcommandid = app_deion_pipe_flood_req;
    }
    else if(para[0] == 2)
    {
        ulcommandid = app_smp_pipe_flood_req;
    }
    else if(para[0] == 3)
    {
        ulcommandid = app_r1_pipe_flood_req;
    }
    else if(para[0] == 4)
    {
        ulcommandid = app_r2_pipe_flood_req;
    }
    else if(para[0] == 5)
    {
        ulcommandid = app_complete_drain_req;
    }
    else
    {
        return VOS_OK;
    }
    vos_app_maintain_debug_cmd_stru* pPara = VOS_NULL;  
    msglen = sizeof(vos_app_maintain_debug_cmd_stru);
    ulrunrslt = app_mallocBuffer(msglen, (void**)(&pPara));   
    if (VOS_OK != ulrunrslt)
    {
        printf("Call app_mallocBuffer() failed(0x%x)\r\n", (int)ulrunrslt);
        return ulrunrslt;
    }
    pPara->ulcmdlen  = sizeof(vos_command_info_stru);
    pPara->pcmdinfo.ulcommandid = ulcommandid;
    msg_stHead rHead;
    app_constructMsgHead(&rHead, Pid_Maintain, Pid_DebugTty, app_maintain_msg_type);
    return app_sendMsg(&rHead, pPara, msglen);
}

/*****************************************************************************
 函 数 名  : app_showsubsysliquidpipehelpinfo
 功能描述  : 液路子系统灌注帮助信息
 输入参数  : void
 输出参数  : 无
 返 回 值  : void
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2015年6月2日
    作    者   : zxc
    修改内容   : 新生成函数

*****************************************************************************/
void app_showsubsysliquidpipehelpinfo( void )
{
    printf("\r command format: app_subsysliquidpipe \r\n");
    printf("\r para0:\n");
    printf("          1 for deion_pipe_flood_req.\r\n");
    printf("          2 for app_smp_pipe_flood_req.\r\n");
    printf("          3 for app_r1_pipe_flood_req.\r\n");
    printf("          4 for app_r2_pipe_flood_req.\r\n");
    printf("          5 for app_complete_drain_req.\r\n");
    return;
}

vos_u32 app_setdwnmask( char *para0, char *para1 )
{

    vos_u32 dwnMachName;
	vos_u32 maskState;
	
    app_strnumToIntnum(para0, &dwnMachName);
	app_strnumToIntnum(para1, &maskState);

    if (dwnMachName > xc8002_dwnmach_name_smplEmer)
    {
        for (int i = 0; i < xc8002_dwnmach_name_smplEmer; i++)
        {
            app_setdwnmachmask(i, maskState);
        }
        return VOS_OK;
    }
    return app_setdwnmachmask(dwnMachName,maskState);
}

void app_showsetdwnmaskhelpinfo( void )
{
	printf("\r\n Command format: app_setdwnmask {para0} {para1} \r\n\n");

	printf(" para0  : \r\n");
    printf(" <0>-xc8002_dwnmach_name_smpldisk \r\n");
	printf(" <1>-xc8002_dwnmach_name_smplndl \r\n");
	printf(" <2>-xc8002_dwnmach_name_innerreagdisk \r\n");
	printf(" <3>-xc8002_dwnmach_name_r1reagneddle \r\n");
	printf(" <4>-xc8002_dwnmach_name_outerreagdisk \r\n");
	printf(" <5>-xc8002_dwnmach_name_r2reagneddle \r\n");
	printf(" <6>-xc8002_dwnmach_name_reactdisk \r\n");
	printf(" <7>-xc8002_dwnmach_name_autowash \r\n");
	printf(" <8>-xc8002_dwnmach_name_smpl_mixrod \r\n");
	printf(" <9>-xc8002_dwnmach_name_reag_mixrod \r\n");
	printf(" <10>-xc8002_dwnmach_name_auto_tmpctrl \r\n");
	printf(" <11>-xc8002_dwnmach_name_cooling_reag \r\n");
	printf(" \r\n\n");
	
    printf(" para1  : \r\n");
	printf(" <0>-unlock mask \r\n");
	printf(" <1>-mask \r\n");
    
    fflush(stdout);
	
    return;
}

/*****************************************************************************
 函 数 名  : app_accuracyaddtest
 功能描述  : 精密加样测试
 输入参数  : para0 部件:0-样本,1-R1, 2-R2
                        para1 位置
                        para2 体积
                        para3 次数
 输出参数  : 无
 返 回 值  : vos_u32
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2015年6月11日
    作    者   : zxc
    修改内容   : 新生成函数

*****************************************************************************/
vos_u32 app_accuracyaddtest(char *para0, char *para1, char *para2, char *para3)
{
    vos_u32 para[4] = { 0 };
    vos_u32 ulrunrslt = VOS_OK;
    ulrunrslt = app_strnumToIntnum(para0, &para[0]);
    if (VOS_OK != ulrunrslt)
    {
        return ulrunrslt;
    }
    ulrunrslt = app_strnumToIntnum(para1, &para[1]);
    if (VOS_OK != ulrunrslt)
    {
        return ulrunrslt;
    }
    ulrunrslt = app_strnumToIntnum(para2, &para[2]);
    if (VOS_OK != ulrunrslt)
    {
        return ulrunrslt;
    }
    ulrunrslt = app_strnumToIntnum(para3, &para[3]);
    if (VOS_OK != ulrunrslt)
    {
        return ulrunrslt;
    }
    vos_u32 subsys = 0;
    if (para[0] == 1)
    {
        subsys = SPid_R1;
    }else if (para[0] == 2)
    {
        subsys = SPid_R2;
    }else
    {
        subsys = SPid_SmpNdl;
        para[1] -= 1;//0~4->1~5
    }

    STReqAddLiquid stLiquid;
    stLiquid.set_endevtype((ServerPid)(subsys));
    stLiquid.set_uispace(para[1]);
    stLiquid.set_uivolume(para[2]);
    stLiquid.set_uicount(para[3]);

    ulrunrslt = sendInnerMsg(Pid_Maintain, Pid_DebugTty, MSG_STAddLiquid, &stLiquid);
    if (VOS_OK != ulrunrslt)
    {
        printf("ulrunrslt = %lu\n", ulrunrslt);
    }
    return ulrunrslt;
}

/*****************************************************************************
 函 数 名  : app_showaccuracyaddtesthelpinfo
 功能描述  : 精密加样命令说明
 输入参数  : void
 输出参数  : 无
 返 回 值  : vos_u32
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2015年6月11日
    作    者   : zxc
    修改内容   : 新生成函数

*****************************************************************************/
void app_showaccuracyaddtesthelpinfo(void)
{
    printf("\r command format: app_accuracyaddtest para0 para1 para2 para3 \r\n");
    printf("\r para0: ndltype .\r\n");
    printf("          smplndl  = 0, .\r\n");
    printf("          R1       = 1, .\r\n");
    printf("          R2       = 2, .\r\n");
    printf("\r para1: addspace.\r\n");
    printf("          1~5 for smp (outer)1~5(inner).\r\n");
    printf("          1 for inner \r\n");
    printf("          2 for outer \r\n");
    printf("\r para2: addvolume.(0.1ul)\r\n");
    printf("\r para3: addcount.\r\n");
    printf("\r example: app_accuracyaddtest 0 1 300 2.\r\n");
    return;
}

/*****************************************************************************
 函 数 名  : app_debugonlyaddordrain
 功能描述  : 液路只吸排测试
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 液路只吸排测试
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2015年7月8日
    作    者   : zxc
    修改内容   : 新生成函数

*****************************************************************************/
vos_u32 app_debugonlyaddordrain(char *para0, char *para1, char *para2, char *para3)
{
    vos_u32 ulrunrslt = VOS_OK;
    vos_u32 para[4] = {0};
    ulrunrslt = app_strnumToIntnum(para0, &para[0]);
    if (VOS_OK != ulrunrslt) 
    { 
        return ulrunrslt; 
    }
    ulrunrslt = app_strnumToIntnum(para1, &para[1]);
    if (VOS_OK != ulrunrslt) 
    { 
        return ulrunrslt; 
    }
    ulrunrslt = app_strnumToIntnum(para2, &para[2]);
    if (VOS_OK != ulrunrslt) 
    { 
        return ulrunrslt; 
    }
    ulrunrslt = app_strnumToIntnum(para3, &para[3]);
    if (VOS_OK != ulrunrslt) 
    { 
        return ulrunrslt; 
    }

    vos_u32 ulcommandid = 0;
    if(para[0] == 0){
        ulcommandid = app_add_smponly_req;}
    if(para[0] == 3){
        ulcommandid = app_drain_smponly_req;}   
    if(para[0] == 1){
            ulcommandid = app_add_r1only_req;}
    if(para[0] == 4){
            ulcommandid = app_drain_r1only_req;}      
    if(para[0] == 2){
            ulcommandid = app_add_r2only_req;}
    if(para[0] == 5){
            ulcommandid = app_drain_r2only_req;} 
    if(ulcommandid == 0){
        return APP_INVALID_U32;}
        
    vos_u32 msglen = sizeof(vos_app_maintain_debug_cmd_stru)+sizeof(app_drainoradd_only_test_stru);
    vos_app_maintain_debug_cmd_stru* pPara = VOS_NULL;
    ulrunrslt = app_mallocBuffer(msglen, (void**)&pPara);
    if (VOS_OK != ulrunrslt)
    {
        WRITE_ERR_LOG("call app_mallocBuffer failed(0x%x)\r\n",ulrunrslt);
        return ulrunrslt;
    }
    pPara->ulcmdlen  = msglen;
    pPara->pcmdinfo.ulcommandid = ulcommandid;
    app_drainoradd_only_test_stru* pstaddinfo = (app_drainoradd_only_test_stru*)(pPara->pcmdinfo.auccmdload);
    pstaddinfo->volumeair = para[1];
    pstaddinfo->volumesmp = para[2];
    pstaddinfo->addcount  = para[3];
    msg_stHead rHead;
    app_constructMsgHead(&rHead, Pid_Maintain, Pid_DebugTty, app_maintain_msg_type);
    return app_sendMsg(&rHead, pPara, msglen);
}


/*****************************************************************************
 函 数 名  : app_sdasdasdsadas
 功能描述  : 的帮助信息
 输入参数  : void
 输出参数  : 无
 返 回 值  : void
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2015年7月8日
    作    者   : zxc
    修改内容   : 新生成函数

*****************************************************************************/
void app_showonlyaddordrainhelpinfo( void )
{
    printf("\r command format: app_debugonlyaddordrain para0 para1 para2 para3 \r\n");
    printf("\r para0: req type .\r\n");
    printf("          0-app_add_smponly_req.\r\n");
    printf("          1-app_add_r1only_req.\r\n");
    printf("          2-app_add_r2only_req.\r\n");
    printf("          3-app_drain_smponly_req .\r\n");
    printf("          4-app_drain_r1only_req .\r\n");
    printf("          5-app_drain_r2only_req.\r\n");                            
    printf("\r para1: volume air.(0.1ul)\r\n");
    printf("\r para2: volume smp.(0.1ul)\r\n");
    printf("\r para3: addcount.\r\n");
    printf("\r example: app_accuracyaddtest 0 1 300 2.\r\n");
    return;
}

vos_u32 getdstPidByDwnMachName(vos_u32 endwnmachname)
{
    vos_u32 dstPid = (endwnmachname == xc8002_dwnmach_name_smpldisk) ? SPid_SmpDsk
        : (endwnmachname == xc8002_dwnmach_name_smpndl) ? SPid_SmpNdl
        : (endwnmachname == xc8002_dwnmach_name_smpndlb) ? SPid_SmpNdlb
        : (endwnmachname == xc8002_dwnmach_name_R2disk) ? SPid_R2Dsk
        : (endwnmachname == xc8002_dwnmach_name_r1Ndl) ? SPid_R1
        : (endwnmachname == xc8002_dwnmach_name_r1bNdl) ? SPid_R1b      
        : (endwnmachname == xc8002_dwnmach_name_R1disk) ? SPid_R1Dsk
        : (endwnmachname == xc8002_dwnmach_name_r2Ndl) ? SPid_R2
        : (endwnmachname == xc8002_dwnmach_name_r2bNdl) ? SPid_R2
        : (endwnmachname == xc8002_dwnmach_name_reactdisk) ? SPid_ReactDsk
        : (endwnmachname == xc8002_dwnmach_name_autowash) ? SPid_AutoWash
        : (endwnmachname == xc8002_dwnmach_name_smpl_mixrod) ? SPid_SMix
        : (endwnmachname == xc8002_dwnmach_name_reag_mixrod) ? SPid_R2Mix
        : (endwnmachname == xc8002_dwnmach_name_auto_Rtmpctrl) ? SPid_RTmpCtrl
        : (endwnmachname == xc8002_dwnmach_name_auto_wtmpctrl) ? SPid_WTmpCtrl        
        : (endwnmachname == xc8002_dwnmach_name_cooling_reag) ? SPid_CoolReag
        : (endwnmachname == xc8002_dwnmach_name_ADOuter) ? SPid_ADOuter
        : (endwnmachname == xc8002_dwnmach_name_ADInner) ? SPid_ADInner
        : (endwnmachname == xc8002_dwnmach_name_smplEmer) ? SPid_CoolReag
        : 0;
    if (dstPid == 0)
    {
        WRITE_ERR_LOG("Unknown endwnmachname(%d) \n", endwnmachname);
    }
    return dstPid;
}

void app_sendMidRunStateToDwnmach(vos_u32 endwnmachname, cmd_simplecmd_stru* cmdinfo)
{
    vos_u32 ulrunrslt = VOS_OK;
	vos_u32 ulmsglength = 0;
	vos_u32 dstPid = 0;
    msg_stHead rHead;

	msg_stSingleActUni* pstdwnmachreqmsg = VOS_NULL;
    msg_stMidState *pdstrst = VOS_NULL;

	ulmsglength = sizeof(msg_stSingleActUni) + sizeof(msg_stMidState);   

	ulrunrslt = app_mallocBuffer(ulmsglength,(void * *)&pstdwnmachreqmsg);
	if (VOS_OK != ulrunrslt)
    {
        WRITE_ERR_LOG("Call app_mallocBuffer() failed(%d) \n", ulrunrslt);
        return;
    }
	dstPid = getdstPidByDwnMachName(endwnmachname);
	app_constructMsgHead(&rHead, dstPid, Pid_DebugTty, app_com_device_req_exe_act_msg_type);
	//填数据内容
    pstdwnmachreqmsg->usActNum = 1;
    pstdwnmachreqmsg->stActHead.usActType = Act_MidState;
	pstdwnmachreqmsg->stActHead.usSingleRes = APP_NO;
    pstdwnmachreqmsg->ulDataLen = sizeof(msg_stMidState) + sizeof(msg_stActHead) ;

	pdstrst = (msg_stMidState*)(&(pstdwnmachreqmsg->astActInfo[0]));
	pdstrst->midState = cmdinfo->para;

	app_sendMsg(&rHead, pstdwnmachreqmsg, ulmsglength);
    return;
}

/*****************************************************************************
 函 数 名  : app_setfaultreport
 功能描述  : 设置下位机故障上报开关
 输入参数  : char*
 输出参数  : 无
 返 回 值  : vos_u32
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2015年6月25日
    作    者   : zxc
    修改内容   : 新生成函数

*****************************************************************************/
vos_u32 app_setfaultreport(char *para0, char *para1 )
{
    vos_u32 ulrunrslt = VOS_OK;
    vos_u32 para[2] = {0};
    cmd_simplecmd_stru debugcmdinfo = {0};
    ulrunrslt = app_strnumToIntnum(para0, &para[0]);
    if (VOS_OK != ulrunrslt) 
    { 
        return ulrunrslt; 
    }
    ulrunrslt = app_strnumToIntnum(para1, &para[1]);
    if (VOS_OK != ulrunrslt) 
    { 
        return ulrunrslt; 
    }
    debugcmdinfo.cmd = XC8002_COM_MIDRUNSTATE_CMD;
    debugcmdinfo.para = para[1];
    app_sendMidRunStateToDwnmach(para[0], &debugcmdinfo);
    if(para[0] > 11)
    {
        SENDFAULTCTL(para[1]);        
    }
	return ulrunrslt;
}

/*****************************************************************************
 函 数 名  : app_showsetfaultreporthelpinfo
 功能描述  : app_setfaultreport的帮助函数
 输入参数  : void  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2015年6月25日
    作    者   : zxc
    修改内容   : 新生成函数

*****************************************************************************/
void app_showsetfaultreporthelpinfo(void)
{
    printf("\r command format: app_setfaultreport para0 para1 \r\n");
    printf("\r para0: downmathin  .\r\n");
    printf("          xc8002_dwnmach_name_smpldisk      = 0, .\r\n");
    printf("          xc8002_dwnmach_name_smplndl       = 1, .\r\n");
    printf("          xc8002_dwnmach_name_innerreagdisk = 2, .\r\n");
    printf("          xc8002_dwnmach_name_r1reagneddle  = 3, .\r\n");
    printf("          xc8002_dwnmach_name_outerreagdisk = 4, .\r\n");
    printf("          xc8002_dwnmach_name_r2reagneddle  = 5, .\r\n");
    printf("          xc8002_dwnmach_name_reactdisk     = 6, .\r\n");
    printf("          xc8002_dwnmach_name_autowash      = 7, .\r\n");
    printf("          xc8002_dwnmach_name_smpl_mixrod   = 8, .\r\n");
    printf("          xc8002_dwnmach_name_reag_mixrod   = 9, .\r\n");
    printf("          xc8002_dwnmach_name_auto_tmpctrl  = 10,.\r\n");
    printf("          xc8002_dwnmach_name_cooling_reag  = 11,.\r\n");
    printf("          all  = 12,.\r\n");

    printf("\r para1: ctl - 0:close  1:open\r\n");
    printf("\r example: app_setfaultreport 0 1.\r\n");
    return;
}

/*****************************************************************************
 函 数 名  : app_debugmotorstepmove
 功能描述  : 电机微步调整
 输入参数  : 无
 输出参数  : 无
 返 回 值  : vos_u32
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2015年7月14日
    作    者   : zxc
    修改内容   : 新生成函数

*****************************************************************************/
vos_u32 app_debugmotorstepmove(char *para0, char *para1, char *para2, char *para3)
{
    vos_u32 ulrunrslt = VOS_OK;
    vos_u32 para[4] = {0};
    
    GET_DEBUG_PARA_PARSE(0);
    GET_DEBUG_PARA_PARSE(1);
    GET_DEBUG_PARA_PARSE(2);
    GET_DEBUG_PARA_PARSE(3);

    STReqActCmd  stPara;
    stPara.set_uisubsysid(para[0] + 900);
    stPara.set_uiacttype(Act_TrimOffset);
    stPara.add_uiparams(para[1]);
    stPara.add_uiparams(para[2]);
    stPara.add_uiparams(para[3]);

   return sendInnerMsg(SPid_DwnCmd, Pid_DebugTty, MSG_ReqActCmd, &stPara);
}

/*****************************************************************************
 函 数 名  : app_showmotorstepmovehelpinfo
 功能描述  : app_debugmotorstepmove的帮助信息
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2015年11月13日
    作    者   : zxc
    修改内容   : 新生成函数

*****************************************************************************/
void  app_showmotorstepmovehelpinfo()
{
    printf("\r command format: app_debugmotorstepmove para0 para1 para2 para3\r\n");
    printf("\r para0: subsys.\r\n");
    printf("    smpldisk      = 0, .\r\n");
    printf("    smplndl       = 1, .\r\n");
    printf("    innerreagdisk = 2, .\r\n");
    printf("    r1reagneddle  = 3, .\r\n");
    printf("    outerreagdisk = 4, .\r\n");
    printf("    r2reagneddle  = 5, .\r\n");
    printf("    reactdisk     = 6, .\r\n");
    printf("    autowash      = 7, .\r\n");
    printf("    smpl_mixrod   = 8, .\r\n");
    printf("    reag_mixrod   = 9, .\r\n");
    printf("    auto_tmpctrl  = 10,.\r\n");
    printf("    cooling_reag  = 11,.\r\n");
    printf("\r para1: motortype.\r\n");
    printf("          1-cmd_motortype_vrt.\r\n");
    printf("          2-cmd_motortype_rtt.\r\n");
    printf("          3-cmd_motortype_inj.\r\n");
    printf("\r para2: dir. \n ");
    printf("          0-cmd_motor_dir_up             \r\n");
    printf("          1-cmd_motor_dir_down           \r\n");
    printf("          1-cmd_motor_dir_ANTICLOCKWISE  \r\n");
    printf("          0-cmd_motor_dir_CLOCKWISE      \r\n");
    printf("          1-cmd_motor_dir_IN		     \r\n");
    printf("          0-cmd_motor_dir_OUT		     \r\n");
    printf("\r para3: step \r\n");
    printf("\r example: app_debugmotorstepmove 0x2451 1 0 50.\r\n");
    return;
}

/*****************************************************************************
 函 数 名  : app_machinoperationswitch
 功能描述  : 仪器运行状态切换
 输入参数  : char*
 输出参数  : 无
 返 回 值  : vos_u32
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2015年7月23日
    作    者   : zxc
    修改内容   : 新生成函数

*****************************************************************************/
vos_u32 app_machinoperationswitch(char* para0)
{   
    vos_u32 ulrunrslt = VOS_OK;
    vos_u32 para[1] = {0};
    
    GET_DEBUG_PARA_PARSE(0);

    STReqActCmd  stPara;
    stPara.set_uisubsysid(SPid_AutoWash);
    stPara.set_uiacttype(Act_Debug);
    stPara.add_uiparams(para[0]);
    
   return sendInnerMsg(SPid_DwnCmd, Pid_DebugTty, MSG_ReqActCmd, &stPara);
}

/*****************************************************************************
 函 数 名  : app_showmachinoperationswitchhelpinfo
 功能描述  : app_machinoperationswitch的帮助信息
 输入参数  : 无
 输出参数  : 无
 返 回 值  : void
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2015年11月13日
    作    者   : zxc
    修改内容   : 新生成函数

*****************************************************************************/
void app_showmachinoperationswitchhelpinfo(   )
{
    printf("\r command format: app_machinoperationswitch para0\r\n");
    printf("\r para0: switch  0:debug model,1:normal model.\r\n");
    return ;
}

/*****************************************************************************
 函 数 名  : app_debugonlyaddordrain
 功能描述  : 液路吸排测试
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 液路吸排测试
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2015年9月7日
    作    者   : zxc
    修改内容   : 新生成函数

*****************************************************************************/
vos_u32 app_debugaddordrain(char *para0, char *para1, char *para2, char *para3, char *para4)
{
    vos_u32 ulrunrslt = VOS_OK;
    vos_u32 para[5] = {0};
    vos_u32 ulcommandid = 0;
    
    GET_DEBUG_PARA_PARSE(0);
    GET_DEBUG_PARA_PARSE(1);
    GET_DEBUG_PARA_PARSE(2);
    GET_DEBUG_PARA_PARSE(3);
    GET_DEBUG_PARA_PARSE(4);  
    if (para[0] == 1)
    {
        ulcommandid = app_drain_add_smp_req;
    }
    else if (para[0] == 2)
    {
        ulcommandid = app_drain_add_r1_req;
    }
    else
    {
        ulcommandid = app_drain_add_r2_req;
    }

    vos_u32 msglen = sizeof(vos_app_maintain_debug_cmd_stru)+sizeof(app_drainoradd_test_stru);
    vos_app_maintain_debug_cmd_stru* pPara = VOS_NULL;
    ulrunrslt = app_mallocBuffer(msglen, (void**)&pPara);
    if (VOS_OK != ulrunrslt)
    {
        WRITE_ERR_LOG("call app_mallocBuffer failed(0x%x)\r\n",ulrunrslt);
        return ulrunrslt;
    }
    pPara->ulcmdlen  = msglen;
    pPara->pcmdinfo.ulcommandid = ulcommandid;
    app_drainoradd_test_stru* pstaddinfo = (app_drainoradd_test_stru*)(pPara->pcmdinfo.auccmdload);
    pstaddinfo->volumesmp = para[1];
    pstaddinfo->volumemore= para[2];
    pstaddinfo->volumeless= para[3];
    pstaddinfo->addcount  = para[4];
    msg_stHead rHead;
    app_constructMsgHead(&rHead, Pid_Maintain, Pid_DebugTty, app_maintain_msg_type);
    return app_sendMsg(&rHead, pPara, msglen);
}


/*****************************************************************************
 函 数 名  : app_sdasdasdsadas
 功能描述  : 的帮助信息
 输入参数  : void
 输出参数  : 无
 返 回 值  : void
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2015年7月8日
    作    者   : zxc
    修改内容   : 新生成函数

*****************************************************************************/
void app_showaddordrainhelpinfo( void )
{
    printf("\r command format: app_debugaddordrain para0 para1 para2 para3 para4 \r\n");
    printf("\r para0: req type .\r\n");
    printf("          1-app_drain_add_smp_req.\r\n");
    printf("          2-app_drain_add_r1_req. \r\n");
    printf("          3-app_drain_add_r2_req. \r\n");
    
    printf("\r para1: volumesmp.(0.1ul)\r\n");
    printf("\r para2: drain volu more.\r\n");
    printf("\r para3: drain volu less.\r\n");
    printf("\r para4: addcount.\r\n");
    printf("\r example: app_debugaddordrain 1 300 0 0 5.\r\n");
    return;
}

vos_u32 app_dwmcmd(char *para0, char *para1, char *para2, char *para3,
                                    char *para4, char *para5, char *para6,
                                    char *para7, char *para8, char *para9)
{
    vos_u32 ulrunrslt = VOS_OK;
    vos_u32 para[10] = {0};
    
    GET_DEBUG_PARA_PARSE(0);//subsys
    GET_DEBUG_PARA_PARSE(1);//acttype
    GET_DEBUG_PARA_PARSE(2);//[0]
    GET_DEBUG_PARA_PARSE(3);//[1]
    GET_DEBUG_PARA_PARSE(4);//[2]
    GET_DEBUG_PARA_PARSE(5);//[3]
    GET_DEBUG_PARA_PARSE(6);//[4]
    GET_DEBUG_PARA_PARSE(7);//[5]
    GET_DEBUG_PARA_PARSE(8);//[6]
    GET_DEBUG_PARA_PARSE(9);//[7]

    STReqActCmd  stPara;
    stPara.set_uisubsysid(900+para[0]);
    stPara.set_uiacttype(para[1]);
    stPara.add_uiparams(para[2]);
    stPara.add_uiparams(para[3]);
    stPara.add_uiparams(para[4]);
    stPara.add_uiparams(para[5]);
    stPara.add_uiparams(para[6]);
    stPara.add_uiparams(para[7]);
    stPara.add_uiparams(para[8]);
    stPara.add_uiparams(para[9]);

   return sendInnerMsg(SPid_DwnCmd, Pid_DebugTty, MSG_ReqActCmd, &stPara);
}

void app_showdwmcmd(void)
{
    printf("app_dwmcmd subsys acttype para...\n");
    printf("Act_Reset = 0,\n");
    printf("Act_Move = 1,\n");
    printf("Act_Absorb = 2,\n");
    printf("Act_Discharge = 3,\n");
    printf("Act_Wash = 4,\n");
    printf("Act_AirOut = 5,\n");
    printf("Act_Mix = 6,\n");
    printf("Act_IntensifyWash = 7,\n");
    printf("Act_BcScan = 8,\n");
    printf("Act_DwnQuery = 9,\n");
    printf("Act_TrimOffset = 10,\n");
    printf("Act_SaveOffset = 11,\n");
    printf("Act_NdlVrt = 12,\n");
    printf("Act_Light = 13,\n");
    printf("Act_Ad = 14,\n");
    printf("Act_DskHrotate = 15,\n");
    printf("Act_MarginScan = 16,\n");
    printf("Act_TempSwitch = 17,\n");
    printf("Act_Fault = 18,\n");
    printf("Act_Debug = 19,\n");
    printf("Act_TempQuery = 20,\n");
    printf("Act_CtrlP04 = 21,\n");
    printf("Act_MidState = 22\n");
    return;
}


vos_u32 app_debugequipmentquery(char * para0,char * para1)
{
    vos_u32 ulrunrslt = VOS_OK;
    vos_u32 para[2] = {0};

    GET_DEBUG_PARA_PARSE(0);
    GET_DEBUG_PARA_PARSE(1);

    STReqActCmd  stPara;
    stPara.set_uisubsysid(900+para[1]);
    stPara.set_uiacttype(Act_DwnQuery);
    stPara.add_uiparams(para[0]);
    return sendInnerMsg(SPid_DwnCmd, Pid_DebugTty, MSG_ReqActCmd, &stPara);
}

void app_showequipmentqueryhelpinfo(void)
{
    printf("\r command format: app_debugequipmentquery para0 para1\r\n");
    printf("\r para0: cha xun lei xing .\r\n");
    printf("    7,   = ban ben                   \n");
    printf("    9,   = bu chang                  \n");
    printf("    8,   = dian ya - guang yuan deng(6) \n");
    printf("    10,  = feng shan zhuang tai(11)      \n");
    printf("    11,  = dian liu - zhi leng pian(11)  \n");
    printf("    12,  = ye lu  - fu zi、ya li(7)     \n");
    printf("    13,  = wen du - zhi leng(11)     \n");
    printf("    14,  = wen du - wen kong(10)         \n");
    printf("    16,  = ri zhi                    \n");
    printf("\r para1: xia wei ji hao .\r\n");
    printf("    smpldisk      = 0, .\r\n");
    printf("    smplndl       = 1, .\r\n");
    printf("    innerreagdisk = 2, .\r\n");
    printf("    r1reagneddle  = 3, .\r\n");
    printf("    outerreagdisk = 4, .\r\n");
    printf("    r2reagneddle  = 5, .\r\n");
    printf("    reactdisk     = 6, .\r\n");
    printf("    autowash      = 7, .\r\n");
    printf("    smpl_mixrod   = 8, .\r\n");
    printf("    reag_mixrod   = 9, .\r\n");
    printf("    auto_tmpctrl  = 10,.\r\n");
    printf("    cooling_reag  = 11,.\r\n");
    printf("\r example: app_debugequipmentquery 7 1.\r\n");
    return;

}


vos_u32 app_linkdebug(char* para0, char* para1, char* para2)
{
    vos_u32 ulrunrslt = VOS_OK;
    app_linkcheck_stru pstdatebuf;
    vos_u32 para[3] = {0};
    
    GET_DEBUG_PARA_PARSE(0);
    GET_DEBUG_PARA_PARSE(1);
    GET_DEBUG_PARA_PARSE(2);

    pstdatebuf.sci_num      = para[0];
    pstdatebuf.device_type  = para[1];
    pstdatebuf.reg_addr     = para[2];
    
    app_linkcheck(&pstdatebuf);
    return ulrunrslt;
}
void app_showlinkdebughelpinfo()
{
     printf("\r command format: app_linkdebug para0 para1 para2\r\n");
     printf("\r para0: sci_num :UART number\r\n");
     printf("\r para1: device_type(0:drv reg; 1:drv buffer; 2:FPGA reg; 3:FPGA buffer;\r\n");
     printf("\r para2: reg_addr(reg addr \r\n");
     return;
}

vos_u32 app_getmidlog(char* para0)
{
    STGetMidLog stPara;
    stPara.set_strip((const char*)para0);
    
    return sendInnerMsg(SPid_Oammng, Pid_DebugTty, MSG_GetMidLog, &stPara);
}

void app_showgetmidloghelpinfo()
{
     printf("\r command format: app_getmidlog para0\r\n");
     printf("\r para0: upmachine ip\r\n");
     return;
}

vos_u32 app_cleanDwnLog(char* para0)
{
    vos_u32 ulrunrslt = VOS_OK;
    vos_u32 para[1] = { 0 };
    GET_DEBUG_PARA_PARSE(0);
    return app_reqDwmCmd(Pid_DebugTty, para[0] + 900, Act_BBClear, 0, NULL, VOS_NO);
}

void app_showcleanDwnLoghelpinfo()
{
    printf("\r command format: app_cleanDwnLog para0\r\n");
    printf("\r para0: dwnMachine name(0~11)\r\n");
    return;
}

vos_u32 app_updatedwn(char* para0, char* para1)
{
    vos_u32 ulrunrslt = VOS_OK;
    vos_u32 para[2] = { 0 };
    GET_DEBUG_PARA_PARSE(0);
    GET_DEBUG_PARA_PARSE(1);

    if (para[1] == 0x31 || para[1] == 0x21 || para[1] == 0x43 || para[1] == 0x93)
    {
        msg_updateDwn updateDwn;
        updateDwn.exeCmd = para[1];
        return app_reqDwmCmd(Pid_DebugTty, para[0], Act_UpdateDwn, sizeof(msg_updateDwn), (char*)&updateDwn, VOS_NO);
    }
    else if (para[1] != 1)
    {
        CDwnUpdate dwnUpdate;
        dwnUpdate.add_uisubsysid(para[0]);
        return sendInnerMsg(Spid_UpdateDwn, Pid_DebugTty, MSG_UpdateDwn, &dwnUpdate);
    }

    return sendInnerMsg(Spid_UpdateDwn, Pid_DebugTty, MSG_GetVres, NULL);
}

void  app_showapp_updatedwnhelpinfo()
{
    printf("\r command format: app_updatedwn para0 para1\r\n");
    printf("\r para0: dwnMachine name(900~911)\r\n");
    printf("\r para1: execmd：\r\n");
    printf("\r        (0x31)  WRITE_MEMORY 	\r\n");
    printf("\r        (0x21)  GO 			\r\n");
    printf("\r        (0x43)  ERASE  		\r\n");
    printf("\r        (0x93)  UPDATE_START  \r\n");
    printf("\r        (0)     UPDATE        \r\n");
    printf("\r        (1)     Get bootloader version \r\n");

    return;
}

//与上位机断连
vos_u32 app_disconnect(char* para0)
{
    vos_u32 para[1] = { 0 };
    app_strnumToIntnum(para0, &para[0]);
    if (0 == para[0])
    {
        return g_pMonitor->Disconnect();
    }
    return g_pMonitor->Disconnect(para[0]);
}

void  app_showapp_disconnectinfo(void)
{
}

vos_u32 app_stop()
{
    vos_stoptimer(ADtimer);
    return sendInnerMsg(SPid_Oammng, Pid_DebugTty, MSG_ReqStopMach);
}

vos_u32 app_setCCtrlModle(char* para0, char* para1)
{
	vos_u32 ulrunrslt = VOS_OK;
	vos_u32 para[2] = { 0 };
	GET_DEBUG_PARA_PARSE(0);
	GET_DEBUG_PARA_PARSE(1);

	msg_stMachineDebug debug;
	debug.operation = para[1];
	return app_reqDwmCmd(SPid_DwnCmd, para[0], Act_Debug, sizeof(debug), (char *)&debug, APP_YES);
}
void  app_showstopinfo(void)
{
	printf("command format: app_setsleep  para0\n");
	printf("\r para0: 1 can sleep,0 can not sleep\r\n");
}
extern bool sleepFlag;
vos_u32 app_setsleep(char* para0)
{
	vos_u32 para[1] = { 0 };
	app_strnumToIntnum(para0, &para[0]);
	sleepFlag = (0 == para[0]) ? false : true;
	return VOS_OK;
}

//蜂鸣器引脚控制
void app_setBeepIo(char *para0)
{
	vos_u32 para = 0;
	app_strnumToIntnum(para0, &para);
	string io = "96";
#ifdef COMPILER_IS_ARM_LINUX_GCC
	a5_ioCtrl(para, io);
#endif
}
void app_setBeepIoHelp(void)
{
	printf("para0: 0  1\n");
}

void app_setGain(char* para0, char* para1)
{
    vos_u32 para[2] = { 0 };
    app_strnumToIntnum(para0, &para[0]);
    app_strnumToIntnum(para1, &para[1]);

    act_stGain stPara;
    stPara.channel = para[0];
    stPara.gain = para[1];
    app_reqDwmCmd(SPid_DwnCmd, SPid_ADOuter, Act_SetGain, sizeof(stPara), (char*)&stPara, APP_NO);
}

void app_setGainHelp()
{
    printf("para0: channel\n");
    printf("para0: gain\n");
}


void app_getad(char* para0)
{
    vos_u32 para = 0;
    app_strnumToIntnum(para0, &para);

    g_ultestmode = TESTMODE_DEBUG;
    act_stReadAD stPara;
    stPara.AdCount = para;
    app_reqDwmCmd(Pid_DebugTty, SPid_ADOuter, Act_Ad, sizeof(stPara), (char*)&stPara, APP_NO);
    app_reqDwmCmd(Pid_DebugTty, SPid_ADInner, Act_Ad, sizeof(stPara), (char*)&stPara, APP_NO);

    act_stReactsysDskRtt stPara2;
    stPara2.usRttedCupNum = para;
    stPara2.dir = cmd_motor_dir_CLOCKWISE;
    app_reqDwmCmd(Pid_DebugTty, SPid_ReactDsk, Act_Move, sizeof(stPara2), (char*)&stPara2, APP_NO);
}

void app_getadHelp()
{
    printf("para0: count\n");
}

void app_getonead(char* para0)
{
    vos_u32 para = 0;
    app_strnumToIntnum(para0, &para);

    g_ultestmode = TESTMODE_DEBUG;
    act_stReadOneAD stPara;
    stPara.channel = para;
    app_reqDwmCmd(Pid_DebugTty, SPid_ADOuter, Act_OneAD, sizeof(stPara), (char*)&stPara, APP_NO);
    app_reqDwmCmd(Pid_DebugTty, SPid_ADInner, Act_OneAD, sizeof(stPara), (char*)&stPara, APP_NO);
}

void app_getoneadHelp()
{
    printf("para0: channel\n");
}

void app_saveOffset(char* para0, char* para1, char* para2)
{
    vos_u32 para[3] = { 0 };
    app_strnumToIntnum(para0, &para[0]);
    app_strnumToIntnum(para1, &para[1]);
    app_strnumToIntnum(para2, &para[2]);

    msg_stSaveOffset stPara;
    stPara.paranum = 0;
    stPara.type = para[1];
    app_reqDwmCmd(Pid_DebugTty, para[0], Act_SaveOffset, sizeof(stPara), (char*)&stPara, APP_NO);
}

void app_saveOffsetHelp()
{
    printf("para0: subsys\n");
    printf("para1: type\n");
}

void app_showwave(char* para0, char* para1, char* para2)
{
    vos_u32 para[3] = { 0 };
    app_strnumToIntnum(para0, &para[0]);
    app_strnumToIntnum(para1, &para[1]);
    app_strnumToIntnum(para2, &para[2]);

    g_ultestmode = TESTMODE_DEBUG;
    if (para[1] == 0 || para[1] == 2)
    {
        app_reqDwmCmd(Pid_DebugTty, SPid_ADInner, Act_Showave, 0, NULL, APP_NO);
    }
    else if (para[1] == 1 || para[1] == 2)
    {
        app_reqDwmCmd(Pid_DebugTty, SPid_ADOuter, Act_Showave, 0, NULL, APP_NO);
    }

    act_stReactsysDskRtt stPara2;
    stPara2.usRttedCupNum = para[0];
    stPara2.dir = cmd_motor_dir_CLOCKWISE;
    app_reqDwmCmd(Pid_DebugTty, SPid_ReactDsk, Act_Move, sizeof(stPara2), (char*)&stPara2, APP_NO);
}

void app_showwaveHelp()
{
    printf("para0: count\n");
    printf("para1: 0-inner,1-outer,2-all\n");
}

void app_preciseadd(char* para0)
{
    vos_u32 subsys;
    app_strnumToIntnum(para0, &subsys);

    STReqPreciseAdd stPara;
    stPara.set_uisubsysid(subsys);
    stPara.set_uiabsorbamount(100);
    stPara.set_uiabsorbpos(1);
    stPara.set_uidischargeamount(200);
    stPara.set_uidischargepos(2);
    stPara.set_uilooptimes(1);

    sendInnerMsg(SPid_PreciseAdd, Pid_DebugTty, MSG_ReqPreciseAdd, &stPara);
}

void app_preciseaddHelp()
{
    printf("para0: subsys\n");
    printf("SPid_SmpNdl = 901,   \n");
    printf("SPid_SmpNdlb = 902,  \n");
    printf("SPid_ReactDsk = 903, \n");
    printf("SPid_AutoWash = 904, \n");
    printf("SPid_SMix = 905,     \n");
    printf("SPid_R2Mix = 906,    \n");
    printf("SPid_R1 = 907,       \n");
    printf("SPid_R1Dsk = 908,    \n");
    printf("SPid_R1b = 909,      \n");
    printf("SPid_R2 = 910,       \n");
    printf("SPid_R2Dsk = 911,    \n");
    printf("SPid_R2b = 912,      \n");
    printf("SPid_RTmpCtrl = 913, \n");
    printf("SPid_WTmpCtrl = 914, \n");
    printf("SPid_CoolReag = 915, \n");
    printf("SPid_ADOuter = 916,  \n");
    printf("SPid_ADInner = 917,  \n");
    printf("SPid_SmpDsk = 919,   \n");
    printf("SPid_SmpEmer = 920,  \n");
}

void app_getin(char* para0)
{
    act_stGetin stPara;
    vos_u32 channel;
    app_strnumToIntnum(para0, &channel);
    stPara.channel = channel;
    app_reqDwmCmd(SPid_Task, SPid_SmpEmer, Act_GetIn, sizeof(stPara), (char*)&stPara, APP_YES);
}

void app_getinHelp()
{
    printf("para0: channel,0 - Emer;1 - Nolmal\n");
}

void app_autoload(char* para0, char* para1, char* para2,char* para3)
{
    vos_u32 pid;
    app_strnumToIntnum(para0, &pid);
    vos_u32 act;
    app_strnumToIntnum(para1, &act);
    act_autoload_move alm;
    vos_u32 dstpid;
    vos_u32 act_cmd;
    switch (pid)
    {
    case 0://转运
        dstpid = 951;
        break;
    case 1://垂直抓手
        dstpid = 952;
        break;
    case 2://L1盘
        dstpid = 953;
        break;
    case 3://L2盘
        dstpid = 954;
        break;
    default:
        break;
    }
    switch (act)
    {
    case 0:
        act_cmd = Act_Reset;
        break;
    case 1:
        act_cmd = Act_Move;
        break;
    default:
        break;
    }
    app_strnumToIntnum(para2, (vos_u32*)(&alm.type));
    app_strnumToIntnum(para3, (vos_u32*)(&alm.place));
    app_reqDwmCmd(0, dstpid, act_cmd, sizeof(alm), (char*)&alm, APP_YES);
}

void app_autoloadHelp()
{
    printf("para0: spid,0 - 951;1 - 952;2 - 953;3 - 954;\n para1:act,0 - Act_Reset;1 - Act_Move;\n \
                                 para2: type : 0-取; 1-放; \n para3: place\n");
}

void app_autoloadDebug(char* para0, char* para1,char* para2,char* para3)
{
    STOffsetPara alm;
    vos_u32 type;
    app_strnumToIntnum(para0, &type);
    alm.set_uitype(type);
    app_strnumToIntnum(para1, &type);
    alm.set_uipos(type);
    app_strnumToIntnum(para2, &type);
    alm.set_uidir(type);
    app_strnumToIntnum(para3, &type);
    alm.set_uistep(type);
    sendInnerMsg(SPid_Task, 0, MSG_AutoloadDebug, &alm);
    return;
}

void app_autoloadDebugHelp()
{
    printf("para0: 1-L1,2-L2;\n para1: 0-9;\n para2:1-R1,2-R2;\n para3: 0-59\n");
}
vos_u32 app_debugsyscmd(char* sys,char* pactType, char* ppara0, char* ppara1)
{
    vos_u32 actType;
    vos_u32 para0;
    vos_u32 para1;
    vos_u32 subsys;
    app_strnumToIntnum(sys, &subsys);
    app_strnumToIntnum(pactType, &actType);
    app_strnumToIntnum(ppara0, &para0);
    app_strnumToIntnum(ppara1, &para1);

    CChooseModeStrategy * pCho = new CChooseModeStrategy;
    pCho->_chooseModeByActtype(subsys, actType, para0, para1);

    delete pCho;
    pCho = NULL;
    return VOS_OK;
}
void app_debugsyscmdHelp()
{
    printf("para0: sys number;\n para1:act number;\n para2:act para;\n para3:act para\n");
}
/*****************************************************************************
 函 数 名  : app_initdebugcmdmd
 功能描述  : 调试命令模块初始化函数
 输入参数  : void
 输出参数  : 无
 返 回 值  : vos_u32
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2014年7月9日
    作    者   : Albort,Feng
    修改内容   : 新生成函数

*****************************************************************************/
vos_u32 app_initdebugcmdmd(void)
{
    //查询定时器信息
    app_registerDebugCmdfunc((char*)"app_printtimerstatus",
        (pdebugcmdfunc)app_printtimerstatus,
        app_showprinttimerstatushelpinfo, 0);
    //查询版本
    app_registerDebugCmdfunc((char*)"app_version",
        (pdebugcmdfunc)app_version,
        app_showversionhelpinfo, 0);
    //日志级别开关
    app_registerDebugCmdfunc((char*)"app_setloglevel",
        (pdebugcmdfunc)app_setloglevel,
        app_showsetloglevel, 1);
    //串口调试开关打开命令
    app_registerDebugCmdfunc((char*)"app_openprintswitch",
        (pdebugcmdfunc)app_openprintswitch,
        app_showopenprintswitch, 0);
    //串口调试开关关闭命令
    app_registerDebugCmdfunc((char*)"app_closeprintswitch",
        (pdebugcmdfunc)app_closeprintswitch,
        app_showcloseprintswitch, 0);
    //样本针
    app_registerDebugCmdfunc((char*)"app_debugsampleneedle",
        (pdebugcmdfunc)app_debugsampleneedle,
        app_showdebugsampleneedlehelpinfo, 3);
    //样本盘                       
    app_registerDebugCmdfunc((char*)"app_debugsampledisk",
        (pdebugcmdfunc)app_debugsampledisk,
        app_showdebugsamplediskhelpinfo, 3);
    //样本搅拌杆
    app_registerDebugCmdfunc((char*)"app_debugsamplemix",
        (pdebugcmdfunc)app_debugsamplemix,
        app_showdebugsamplemixhelpinfo, 3);
    //试剂搅拌杆
    app_registerDebugCmdfunc((char*)"app_debugreagmix",
        (pdebugcmdfunc)app_debugreagmix,
        app_showdebugreagmixhelpinfo, 3);
    //试剂针r1                      
    app_registerDebugCmdfunc((char*)"app_debugreag1needle",
        (pdebugcmdfunc)app_debugreag1needle,
        app_showdebugreag1needlehelpinfo, 3);
    //试剂针r2                      
    app_registerDebugCmdfunc((char*)"app_debugreag2needle",
        (pdebugcmdfunc)app_debugreag2needle,
        app_showdebugreag2needlehelpinfo, 3);
    //试剂盘外盘
    app_registerDebugCmdfunc((char*)"app_debugreagoutdisk",
        (pdebugcmdfunc)app_debugreagoutdisk,
        app_showdebugreagoutdiskhelpinfo, 3);
    //试剂盘内盘
    app_registerDebugCmdfunc((char*)"app_debugreagindisk",
        (pdebugcmdfunc)app_debugreagindisk,
        app_showdebugreagindiskhelpinfo, 3);
    //自动清洗
    app_registerDebugCmdfunc((char*)"app_debugautowashneedle",
        (pdebugcmdfunc)app_debugautowashneedle,
        app_showdebugautowashneedlehelpinfo, 3);
    //反应盘
    app_registerDebugCmdfunc((char*)"app_debugreactiondisk",
        (pdebugcmdfunc)app_debugreactiondisk,
        app_showdebugreactiondiskhelpinfo, 3);
    //公共子系统
    app_registerDebugCmdfunc((char*)"app_commonsyscmd",
        (pdebugcmdfunc)app_commonsyscmd,
        app_showcommonsyscmdhelpinfo, 0);

    //模拟环境下设置调度周期命令
    app_registerDebugCmdfunc((char*)"app_setschedulecircle",
        (pdebugcmdfunc)app_setschedulecircle,
        app_showsetschedulecirclecmdhelpinfo, 2);

    //更新中位机版本                      
    app_registerDebugCmdfunc((char*)"app_updateapp",
        (pdebugcmdfunc)app_updateapp,
        app_showupdateappcmdhelpinfo, 1);
    //显示已创建线程数
    app_registerDebugCmdfunc((char*)"app_showthreadsum",
        (pdebugcmdfunc)app_showthreadsum,
        app_showshowthreadsum, 0);

    app_registerDebugCmdfunc((char*)"app_showusedmemsize",
        (pdebugcmdfunc)app_showusedmemsize,
        app_showshowusedmemsizehelpinfo, 0);

    //自动测试任务
    app_registerDebugCmdfunc((char*)"app_tasktest",
        (pdebugcmdfunc)app_tasktest,
        app_showautotaskmnghelpinfo, 1);

    //暗电流注册
    app_registerDebugCmdfunc((char*)"app_debugdarkcurrent",
        (pdebugcmdfunc)app_debugdarkcurrent,
        app_showdarkcurrenthelpinfo, 1);

    //设置ioctrl值注册
    app_registerDebugCmdfunc((char*)"app_debugsetioctlval",
        (pdebugcmdfunc)app_debugsetioctlval,
        app_showioctrlhelpinfo, 2);
    //动态测光命令
    app_registerDebugCmdfunc((char*)"app_debugabsvalue",
        (pdebugcmdfunc)app_debugabsvalue,
        app_showabsvaluehelpinfo, 2);
    //控制光源灯
    app_registerDebugCmdfunc((char*)"app_debugctrllight",
        (pdebugcmdfunc)app_debugctrllight,
        app_debugctrllighthelpinfo, 1);
    //串口环回测试命令注册
    app_registerDebugCmdfunc((char*)"app_startuartloopbacktest",
        (pdebugcmdfunc)app_startuartloopbacktest,
        app_showstartuartloopbacktestcmdhelpinfo, 4);

    //条码扫描调试命令
    app_registerDebugCmdfunc((char*)"app_debugbarcodescan",
        (pdebugcmdfunc)app_debugbarcodescan,
        app_showbarcodescanhelpinfo, 3);

    //余量扫描调试命令
    app_registerDebugCmdfunc((char*)"app_debugmarginscan",
        (pdebugcmdfunc)app_debugmarginscan,
        app_showmarginscanhelpinfo, 2);

    //平坦区测试命令
    app_registerDebugCmdfunc((char*)"app_debugsmooth",
        (pdebugcmdfunc)app_debugsmooth,
        app_showsmoothhelpinfo, 1);

    //虚拟示波器测试命令
    app_registerDebugCmdfunc((char*)"app_debugshowave",
        (pdebugcmdfunc)app_debugshowave,
        app_showavehelpinfo, 3);

    //三针注射泵排空气
    app_registerDebugCmdfunc((char*)"app_debuginjectionairout",
        (pdebugcmdfunc)app_debuginjectionairout,
        app_showinjectionairouthelpinfo, 1);

    //液路调试命令
    app_registerDebugCmdfunc((char*)"app_debugwashtest",
        (pdebugcmdfunc)app_debugwashtest,
        app_showdebugwashtestcmdhelpinfo, 3);

	app_registerDebugCmdfunc((char*)"app_setsleep",
		(pdebugcmdfunc)app_setsleep,
		app_showstopinfo, 0);

    //单个泵阀开关
    app_registerDebugCmdfunc((char*)"app_debugpvctl",
        (pdebugcmdfunc)app_debugpvctl,
        app_showdebugpvctlcmdhelpinfo, 2);

    //设置温度自动上报命令
    app_registerDebugCmdfunc((char*)"app_settempraturereport",
        (pdebugcmdfunc)app_settempraturereport,
        app_showsettempraturereportcmdhelpinfo, 1);

    //制冷调试
    app_registerDebugCmdfunc((char*)"app_debugcooling",
        (pdebugcmdfunc)app_debugcooling,
        app_showdebugcoolinghelpinfo, 4);
    //温控调试
    app_registerDebugCmdfunc((char*)"app_debugtempctl",
        (pdebugcmdfunc)app_debugtempctl,
        app_showdebugtempctlhelpinfo, 4);

    //强化清洗命令
    app_registerDebugCmdfunc((char*)"app_debugintensifywash",
        (pdebugcmdfunc)app_debugintensifywash,
        app_showdebugintensifywashhelpinfo, 0);
    //液路子系统灌注命令
    app_registerDebugCmdfunc((char*)"app_subsysliquidpipe",
        (pdebugcmdfunc)app_subsysliquidpipe,
        app_showsubsysliquidpipehelpinfo, 1);
    //模拟报警
    app_registerDebugCmdfunc((char*)"app_simulateAlarm",
        (pdebugcmdfunc)app_simulateAlarm,
        app_simulateAlarmInfo, 4);

    //设置下位机掩码
    app_registerDebugCmdfunc((char*)"app_setdwnmask",
        (pdebugcmdfunc)app_setdwnmask,
        app_showsetdwnmaskhelpinfo, 2);
    //精密加样命令
    app_registerDebugCmdfunc((char*)"app_accuracyaddtest",
        (pdebugcmdfunc)app_accuracyaddtest,
        app_showaccuracyaddtesthelpinfo, 4);
    //下位机故障上报开关
    app_registerDebugCmdfunc((char*)"app_setfaultreport",
        (pdebugcmdfunc)app_setfaultreport,
        app_showsetfaultreporthelpinfo, 2);

    //只吸排命令
    app_registerDebugCmdfunc((char*)"app_debugonlyaddordrain",
        (pdebugcmdfunc)app_debugonlyaddordrain,
        app_showonlyaddordrainhelpinfo, 4);

    //电机微步调整
    app_registerDebugCmdfunc((char*)"app_debugmotorstepmove",
        (pdebugcmdfunc)app_debugmotorstepmove,
        app_showmotorstepmovehelpinfo, 4);

    //仪器状态切换命令
    app_registerDebugCmdfunc((char*)"app_machinoperationswitch",
        (pdebugcmdfunc)app_machinoperationswitch,
        app_showmachinoperationswitchhelpinfo, 1);
    //查询命令
    app_registerDebugCmdfunc((char*)"app_debugequipmentquery",
        (pdebugcmdfunc)app_debugequipmentquery,
        app_showequipmentqueryhelpinfo, 2);

    //液路吸排测试
    app_registerDebugCmdfunc((char*)"app_debugaddordrain",
        (pdebugcmdfunc)app_debugaddordrain,
        app_showaddordrainhelpinfo, 5);

    app_registerDebugCmdfunc((char*)"app_dwmcmd",
        (pdebugcmdfunc)app_dwmcmd,
        app_showdwmcmd, 10);

    //链路调试
    app_registerDebugCmdfunc((char*)"app_linkdebug",
        (pdebugcmdfunc)app_linkdebug,
        app_showlinkdebughelpinfo, 3);
    //获取中位机日志
    app_registerDebugCmdfunc((char*)"app_getmidlog",
        (pdebugcmdfunc)app_getmidlog,
        app_showgetmidloghelpinfo, 1);

    //清除下位机日志
    app_registerDebugCmdfunc((char*)"app_cleanDwnLog",
        (pdebugcmdfunc)app_cleanDwnLog,
        app_showcleanDwnLoghelpinfo, 1);

    //下位机升级
    app_registerDebugCmdfunc((char*)"app_updatedwn",
        (pdebugcmdfunc)app_updatedwn,
        app_showapp_updatedwnhelpinfo, 1);

    //断连
    app_registerDebugCmdfunc((char*)"app_disconnect",
        (pdebugcmdfunc)app_disconnect,
        app_showapp_disconnectinfo, 1);

    //停止
    app_registerDebugCmdfunc((char*)"app_stop",
        (pdebugcmdfunc)app_stop,
        app_showstopinfo, 0);
	//闭环控制
	app_registerDebugCmdfunc((char*)"app_setcctrlModle",
		(pdebugcmdfunc)app_setCCtrlModle,
		app_showequipmentqueryhelpinfo, 2);

	app_registerDebugCmdfunc((char*)"app_setbeepgpio",
		(pdebugcmdfunc)app_setBeepIo,
		app_setBeepIoHelp, 1);

    app_registerDebugCmdfunc((char*)"app_setGain",
        (pdebugcmdfunc)app_setGain,
        app_setGainHelp, 1);

    app_registerDebugCmdfunc((char*)"app_getad",
        (pdebugcmdfunc)app_getad,
        app_getadHelp, 1);

    app_registerDebugCmdfunc((char*)"app_getonead",
        (pdebugcmdfunc)app_getonead,
        app_getoneadHelp, 1);

    app_registerDebugCmdfunc((char*)"app_saveOffset",
        (pdebugcmdfunc)app_saveOffset,
        app_saveOffsetHelp, 3);

    app_registerDebugCmdfunc((char*)"app_showwave",
        (pdebugcmdfunc)app_showwave,
        app_showwaveHelp, 3);
    
    app_registerDebugCmdfunc((char*)"app_preciseadd",
        (pdebugcmdfunc)app_preciseadd,
        app_preciseaddHelp, 1);
    
    app_registerDebugCmdfunc((char*)"app_getin",
        (pdebugcmdfunc)app_getin,
        app_getinHelp, 1);
    
    app_registerDebugCmdfunc((char*)"app_autoload",
        (pdebugcmdfunc)app_autoload,
        app_autoloadHelp, 4);

    app_registerDebugCmdfunc((char*)"app_autoloaddebug",
        (pdebugcmdfunc)app_autoloadDebug, app_autoloadDebugHelp, 4);

    app_registerDebugCmdfunc((char*)"app_debugsyscmd",
        (pdebugcmdfunc)app_debugsyscmd, app_debugsyscmdHelp, 4);
    return VOS_OK;
}

#ifdef __cplusplus
}
#endif
