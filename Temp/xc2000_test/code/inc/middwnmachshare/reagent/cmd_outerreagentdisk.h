//modifyed
#ifndef _CMD_OUTERREAGENTDISK_H_
#define _CMD_OUTERREAGENTDISK_H_

#include"vos_basictypedefine.h"

//外圈试剂盘单元命令枚举定义
typedef enum
{
    cmd_outer_reagentdisk_min               = 0x3400, //外圈试剂盘单元最小命令
    cmd_outer_reagentdisk_reset             = 0x3408, //外圈试剂盘复位命令
    cmd_outer_reagentdisk_anticlockwise_rtt = 0x3409, //外圈试剂盘逆时针旋转x个杯位命令
    cmd_outer_reagentdisk_clockwise_rtt     = 0x340a, //外圈试剂盘顺时针旋转x个杯位命令
    cmd_outer_reagentdisk_rtt_to_absbpos    = 0x340b, //外圈试剂盘x杯位旋转到指定吸试剂位
    cmd_outer_reagentdisk_rtt_to_swappos    = 0x340c, //外圈试剂盘x杯位旋转到换试剂位
    cmd_outer_reagentdisk_rtt_to_bcdscan    = 0x3411, //外圈试剂盘x杯位旋转到条码扫描位
    cmd_outer_reagentdisk_rtt_onecycle      = 0x3412, //外圈试剂盘旋转一圈
    cmd_outer_reagentdisk_fpga_update    	= 0x3420, //外圈试剂盘FPGA 程序在线更新功能
    
    cmd_outer_reagentdisk_motor_hold_ctl    = 0x3450, //外圈试剂盘电机保持力矩控制  
    cmd_outer_reagentdisk_step_move         = 0x3451, //外圈试剂盘步移动调整

	cmd_outer_reagentdisk_chip_reset        = 0x3470, //芯片复位
	cmd_outer_reagentdisk_show_wave_switch  = 0x3471, //虚拟示波器开关
	cmd_outer_reagentdisk_update            = 0x3480, //外圈试剂盘升级
    
    cmd_outer_reagentdisk_compensation      = 0x34c0, //外圈试剂盘补偿命令
    cmd_outer_reagentdisk_comb_cmd          = 0x34cc, //外圈试剂盘组合命令
    cmd_outer_reagentdisk_max               = 0x34ff,  //外圈试剂盘单元最大命令
}cmd_outer_reagentdisk_type_enum;

/*外圈试剂盘吸试剂位置定义*/
typedef enum
{
    cmd_outer_reagentdisk_r1_absbpos = 0,  //外圈试剂盘R1试剂针吸试剂位
    cmd_outer_reagentdisk_r2_absbpos = 1,  //外圈试剂盘R2试剂针吸试剂位

    cmd_outer_reagentdisk_absbpos_butt
}cmd_outer_reagentdisk_absbpos_enum;
typedef char cmd_outer_reagentdisk_absbpos_enum_u8;

/*外圈试剂盘X杯位旋转到条码扫描位*/
typedef struct
{
    app_u16 cmd;
    char  uccupnum; //某杯位转到换试剂位
    char  rsv[1];
}cmd_outer_reagdsk_rtt_to_bcdscan_stru;

#endif
