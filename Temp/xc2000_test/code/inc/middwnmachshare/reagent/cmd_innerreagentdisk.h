//modifyed
#ifndef _CMD_INNERREAGENTDISK_H_
#define _CMD_INNERREAGENTDISK_H_

#include"vos_basictypedefine.h"

//内圈试剂盘单元命令枚举定义
typedef enum
{
    cmd_inner_reagentdisk_min               = 0x2800, //内圈试剂盘单元最小命令
    cmd_inner_reagentdisk_reset             = 0x2808, //内圈试剂盘复位命令
    cmd_inner_reagentdisk_anticlockwise_rtt = 0x2809, //内圈试剂盘逆时针旋转x个杯位命令
    cmd_inner_reagentdisk_clockwise_rtt     = 0x280a, //内圈试剂盘顺时针旋转x个杯位命令
    cmd_inner_reagentdisk_rtt_to_absbpos    = 0x280b, //内圈试剂盘x杯位旋转到指定吸试剂位
    cmd_inner_reagentdisk_rtt_to_swappos    = 0x280c, //内圈试剂盘x杯位旋转到换试剂位
    cmd_inner_reagentdisk_rtt_to_bcdscan    = 0x2811, //内圈试剂盘x杯位旋转到条码扫描位
    cmd_inner_reagentdisk_rtt_onecycle      = 0x2812, //内圈试剂盘旋转一圈

    cmd_inner_reagentdisk_motor_hold_ctl    = 0x2850, //内圈试剂盘电机保持力矩控制  
    cmd_inner_reagentdisk_step_move         = 0x2851, //内圈试剂盘步移动调整

	cmd_inner_reagentdisk_chip_reset        = 0x2870, //芯片复位
	cmd_inner_reagentdisk_show_wave_switch  = 0x2871, //虚拟示波器开关
	cmd_inner_reagentdisk_update            = 0x2880, //内圈试剂盘升级

    cmd_inner_reagentdisk_compensation      = 0x28c0, //内圈试剂盘补偿命令
    cmd_inner_reagentdisk_comb_cmd          = 0x28cc, //内圈试剂盘组合命令
    cmd_inner_reagentdisk_max               = 0x28ff, //内圈试剂盘单元最大命令
}cmd_inner_reagentdisk_type_enum;

/*内圈试剂盘吸试剂位置定义*/
typedef enum
{
    cmd_inner_reagentdisk_r1_absbpos = 0,  //内圈试剂盘R1试剂针吸试剂位
    cmd_inner_reagentdisk_r2_absbpos = 1,  //内圈试剂盘R2试剂针吸试剂位

    cmd_inner_reagentdisk_absbpos_butt
}cmd_inner_reagentdisk_absbpos_enum;

#endif
