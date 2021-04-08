//modified
#ifndef _xc8002_middwnmachshare_h_
#define _xc8002_middwnmachshare_h_

#include "cmd_downmachine.h"

#define xc8002_cmdkey_mapto_cpuid(cmd) ((app_u16)(((cmd)&0xff00)>>10))
#define LiquidErr  0xf
//命令执行结果定义
typedef enum
{
    xc8002_exe_cmd_sucess  = 0,
    xc8002_exe_cmd_failed  = 1,
    xc8002_exe_cmd_timeout = 2,
}xc8002_exe_cmd_result_enum;

//硬件单元类型定义
typedef enum
{
    xc8002_dwnmach_name_smpndl          = 1,   //样本针单元-外
    xc8002_dwnmach_name_smpndlb         = 2,   //样本针单元-内
    xc8002_dwnmach_name_reactdisk       = 3,   //反应盘单元
    xc8002_dwnmach_name_autowash        = 4,   //自动清洗单元
    xc8002_dwnmach_name_smpl_mixrod     = 5,   //样本搅拌杆单元
    xc8002_dwnmach_name_reag_mixrod     = 6,   //试剂搅拌杆单元
    xc8002_dwnmach_name_r1Ndl           = 7,   //R1试剂针单元-外
    xc8002_dwnmach_name_R1disk          = 8,   //R1试剂盘单元
    xc8002_dwnmach_name_r1bNdl          = 9,   //R1试剂针单元-内
    xc8002_dwnmach_name_r2Ndl           = 10,  //R2试剂针单元-外
    xc8002_dwnmach_name_R2disk          = 11,  //R2试剂盘单元
    xc8002_dwnmach_name_r2bNdl          = 12,  //R2试剂针单元-内   
    xc8002_dwnmach_name_auto_Rtmpctrl   = 13,  //反应盘温控控制
    xc8002_dwnmach_name_auto_wtmpctrl   = 14,  //清洗水温度控制
    xc8002_dwnmach_name_cooling_reag    = 15,  //试剂制冷单元
    xc8002_dwnmach_name_ADOuter         = 16,  // AD单元外圈
    xc8002_dwnmach_name_ADInner         = 17,  // AD单元内圈
    xc8002_dwnmach_name_smpldisk        = 19,  //常规定位
    xc8002_dwnmach_name_smplEmer        = 20,  //急诊定位
    xc8002_dwnmach_name_press           = 21,  //增压泵
    xc2000_dwnmach_name_Light           =22,   //光源灯
    xc8002_dwnach_name_ALTspt           =30,  //自动装载转运
    xc8002_dwnach_name_ALVG             =31,  //自动装载垂直抓手
    xc8002_dwnach_name_ALL1             =32,  //自动装载L1盘
    xc8002_dwnach_name_ALL2             =33,  //自动装载L2盘
    xc8002_dwnmach_name_fpga_ad		  = 41,  //FPGA AD采集单元
    xc8002_link_debug                 = 42,  //链路调试
    
    xc8002_dwnmach_name_butt
}xc8002_dwnmach_name_enum;

//消息传输类型定义
typedef enum
{
    xc8002_req_msgtran_type = 0,  //请求消息传输
    xc8002_ack_msgtran_type = 1,  //确认消息传输
    xc8002_nak_msgtran_type = 2,  //否定消息传输
    xc8002_res_msgtran_type = 3,  //响应消息传输

    xc8002_msg_tran_type_butt
}xc8002_msg_tran_type_enum;
typedef char xc8002_msg_tran_type_enum_u8;

//中下位机消息传输头
#define xc8002_middwnmach_msg_header \
    char  endwnmachname;   \
    char  enmsgtrantype;   \
    app_u16 uscmdinfolen;    \
    vos_u32 ulcurcmdframeid
    

#pragma warning(disable:4200)
//中位机下位机通用数据传输结构
typedef struct
{
    xc8002_middwnmach_msg_header;
    app_u16 uscmdtype;
    char  aucrsv[2];
    char aucdata[0];
}xc8002_middwnmach_com_msgtran_stru;

//中位机下位机消息结果响应传输数据结构
typedef struct
{
    xc8002_middwnmach_msg_header;
    app_u16 uscmdtype;
    app_u16  enexerslt;      //命令执行结果
    app_u32  rsv;
    char  aucinfo[0];
}xc8002_middwnmach_res_msgtran_stru;

//===================消息返回负载结构=================
//条码TLV结构体
typedef struct
{
    char trayno;      //盘编号
    char cupno;       //杯编号
    app_u16 len;        //长度
    char aucbarcode[0];   //条码编号 !!!下位机务必保证4字节对齐,并同时更新len值!!!
}app_dwn_barcode_tlv_stru;

//下位机条码结构体
typedef struct
{
    app_u16 traytype;   //盘类型
    app_u16 tlvnum;     //tlv个数    
    app_dwn_barcode_tlv_stru auctlvinfo[0];  //tlv信息
}app_dwn_barcode_stru;
/**********end of 条码扫描数据结构体**********/

//反应盘温度消息负载：
typedef struct
{
    app_i32 slavertemp;    //平均温度值   
    app_i32 slcurtemp1;    //通道1温度值 
    app_i32 slcurtemp2;    //通道2温度值 
    app_i32 slcurtemp3;    //通道3温度值 
    app_i32 slcurtemp4;    //通道4温度值(=实际温度*10倍,防止异系统间小数传输) 
}ReactTemp_st;

//清洗温度消息负载：
typedef struct
{
    app_i32 slmachine2;    //整机加热2温度值 
    app_i32 slmachine1;    //整机加热1温度值 
    app_i32 slcleaner;     //清洗剂温度值 
    app_i32 slwater;      //清洗水温度值(=实际温度*10倍,防止异系统间小数传输) 
}WarterTemp_st;
#pragma warning(default:)

#endif
