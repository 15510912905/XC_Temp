/******************************************************************************

                  版权所有 (C), 2001-2100, 四川省新健康成股份有限公司

 ******************************************************************************
  文 件 名   : app_simulation.h
  版 本 号   : 初稿
  作    者   : zxc
  生成日期   : 2016年1月23日
  最近修改   :
  功能描述   : app_simulation.c 的头文件
  函数列表   :
  修改历史   :
  1.日    期   : 2016年1月23日
    作    者   : zxc
    修改内容   : 创建文件

******************************************************************************/

#ifndef __APP_SIMULATION_H__
#define __APP_SIMULATION_H__
#include <pthread.h>
#include "xc8002_middwnmachshare.h"
#include "uart.h"


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */


#define INVALID_FAULTID (0) //无效故障id
#define INVALID_CMDTYPE (0) //无效的命令类型

#define SAMPLE_DISK_3_CIRCLE_TOGETHER_SCAN 0xFF //样本盘同时3圈扫描

typedef xc8002_middwnmach_com_msgtran_stru msg_stMidDwnCom;
typedef xc8002_middwnmach_res_msgtran_stru msg_stMidRes;

typedef void (*simuFun)(msg_stMidDwnCom* pstLoad, vos_u32 cmdType, xc8002_com_dwnmach_cmd_stru *pstCmd);

//命令类型与响应函数
typedef struct
{
    vos_u32 cmdType;
    simuFun resFc;
}app_simuResFc_mng_stru;

//定义串口管理模块响应信息vos消息结构
typedef struct
{
    vos_msg_header;
    xc8002_middwnmach_res_msgtran_stru stresinfo;
}app_mnguart_middwnmach_cmd_res_msg_stru;

//异常构造函数
typedef void (*faultFun)(msg_stMidRes* pPara, vos_u32 targetPeriod,vos_u32 targetFaultid);
typedef vos_u32 (*reporFun)(msg_stMidDwnCom* pstLoad, xc8002_com_dwnmach_cmd_stru *pstCmd);
//命令类型与异常构造函数
typedef struct
{
    vos_u32 cmdType;        //命令类型
    vos_u32 taskCount;      //触发异常的任务数(第几个任务)
    vos_u32 faultId;        //该异常所需要报的故障id(0:为无效id)
    faultFun faultFc;       //异常构造函数
    reporFun reporFc;       //上报
    vos_u32 cmdCount;       //已收到此命令的个数
}app_faultFc_mng_stru;

typedef struct
{
    pthread_mutex_t    mutex;
    pthread_cond_t       cond;
}sim_stCondMutex;

typedef struct
{    
    xc8002_report_compensation_para g_cpucps0[11];  //xc8002_dwnmach_name_smpldisk 11 para
    xc8002_report_compensation_para g_cpucps1[18];  //xc8002_dwnmach_name_smplndl 18 para
    xc8002_report_compensation_para g_cpucps2[6];   //xc8002_dwnmach_name_innerreagdisk 6 para
    xc8002_report_compensation_para g_cpucps3[14];  //xc8002_dwnmach_name_r1reagneddle 14 para
    xc8002_report_compensation_para g_cpucps4[5];   //xc8002_dwnmach_name_outerreagdisk 5 para
    xc8002_report_compensation_para g_cpucps5[14];  //xc8002_dwnmach_name_r2reagneddle 14 para
    xc8002_report_compensation_para g_cpucps6[5];   //xc8002_dwnmach_name_reactdisk 4 para
    xc8002_report_compensation_para g_cpucps8[6];   //xc8002_dwnmach_name_smpl_mixrod 6 para
    xc8002_report_compensation_para g_cpucps9[5];   //xc8002_dwnmach_name_reag_mixrod 5 para
	xc8002_report_compensation_para g_cpucps7[2];   //xc8001_dwnmach_name_auto_tmpctrl 1 para
}app_auto_compensation_req_stru;

extern void app_cleanFaultFc();
extern vos_u32 app_constructRemainFault(msg_stMidDwnCom* pstLoad, xc8002_com_dwnmach_cmd_stru *pstCmd);
extern void app_constructSampBcd(app_dwn_barcode_stru *pdwnbarcode, vos_u32 count,
                                                vos_u32 trayno, vos_u32 cupid);
extern void app_constructSingleCmdFault(msg_stMidRes* pPara, vos_u32 taskCount,vos_u32 faultId);
extern bool app_exeFaultFc(msg_stMidRes* pPara, vos_u32 cmdType, msg_stMidDwnCom* pstLoad, xc8002_com_dwnmach_cmd_stru *pstCmd);
extern vos_u32 app_exeResFc(msg_stMidDwnCom* pstLoad, vos_u32 cmdType, xc8002_com_dwnmach_cmd_stru *pstCmd);
extern vos_u32 app_initcompensationpara(app_auto_compensation_req_stru *data);
extern vos_u32 app_initSimuTest(void);
extern vos_u32 app_prostorecompensation(vos_u32 cmd, vos_u32 compType, cmd_compensation_para *compPara);
extern vos_u32 app_registerFaultFc(app_faultFc_mng_stru stFaultFc);
extern vos_u32 app_simuCompReport(msg_stMidDwnCom* pstload);
extern vos_u32 app_simuMotorReport(msg_stMidDwnCom* pstload);
extern void app_simuDwnmachComCmdRes(msg_stMidDwnCom* pstLoad, vos_u32 cmdType, xc8002_com_dwnmach_cmd_stru *pstCmd);
extern vos_u32 app_simuDwnmachQueryData(msg_stMidDwnCom* paurtpack);
extern void app_simuDwnmachResponse(msg_stMidDwnCom* paurtpack);
extern void app_simuDwnmachSingleCmdRes(msg_stMidDwnCom* pstLoad, vos_u32 cmdType, xc8002_com_dwnmach_cmd_stru *pstCmd);
extern vos_u32 app_simuFpgaAdResponse(xc8002_middwnmach_com_msgtran_stru *pfpgapack);
extern void app_simuNdlDown(msg_stMidDwnCom* pstLoad, vos_u32 cmdType, xc8002_com_dwnmach_cmd_stru *pstCmd);
void app_simuReadAD(msg_stMidDwnCom* pstLoad, vos_u32 cmdType, xc8002_com_dwnmach_cmd_stru *pstCmd);
extern void app_simuReagBcdScan(msg_stMidDwnCom* pstLoad, vos_u32 cmdType, xc8002_com_dwnmach_cmd_stru *pstCmd);
void app_simuTempQuery(msg_stMidDwnCom* pstLoad, vos_u32 cmdType, xc8002_com_dwnmach_cmd_stru *pstCmd);
extern void app_simuReagMargScan(msg_stMidDwnCom* pstLoad, vos_u32 cmdType, xc8002_com_dwnmach_cmd_stru *pstCmd);
extern vos_u32 app_simuRemainDepth(msg_stMidDwnCom* pstLoad, xc8002_com_dwnmach_cmd_stru *pstCmd);
//新的模拟试剂余量的函数接口
vos_u32 app_simuRemainDepth_regent(msg_stMidDwnCom* pstLoad, xc8002_com_dwnmach_cmd_stru *pstCmd);
vos_u32 app_simuADValue(msg_stMidDwnCom* pstLoad, xc8002_com_dwnmach_cmd_stru *pstCmd);
extern void app_simuSampBcdScan(msg_stMidDwnCom* pstLoad, vos_u32 cmdType, xc8002_com_dwnmach_cmd_stru *pstCmd);
extern void app_simuSetComp(msg_stMidDwnCom* pstLoad, vos_u32 cmdType, xc8002_com_dwnmach_cmd_stru *pstCmd);
extern vos_u32 app_simuTestMsgCallBack(vos_msg_header_stru* pstvosmsg);
extern void app_simuUartAckMsg(msg_stMidDwnCom* paurtpack);
extern void app_simuUartResMsg(msg_stMidDwnCom* paurtpack);
extern vos_u32 SendMsgToAppUart(char* pmsgbuf, vos_u32 ulbuflen);
extern void app_simuDwnUpdate(msg_stMidDwnCom* pstLoad, vos_u32 cmdType, xc8002_com_dwnmach_cmd_stru *pstCmd);
extern void app_simuFpgaUpdate(msg_stMidDwnCom* pstLoad, vos_u32 cmdType, xc8002_com_dwnmach_cmd_stru *pstCmd);
extern void app_simuRNdlDown(msg_stMidDwnCom* pstLoad, vos_u32 cmdType, xc8002_com_dwnmach_cmd_stru *pstCmd);
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __APP_SIMULATION_H__ */
