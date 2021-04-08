//modifyed
#ifndef _CMD_SAMPLEDISK_H_
#define _CMD_SAMPLEDISK_H_

#include "vos_basictypedefine.h"

//样本盘单元命令枚举定义
typedef enum
{
    cmd_sample_dsk_min                  = 0x1c00, //样本盘最小命令
    cmd_sample_dsk_reset                = 0x1c08, //样本盘复位命令
    cmd_sample_dsk_anticlockwise_rtt    = 0x1c09, //样本盘逆时针旋转x个杯位命令
    cmd_sample_dsk_clockwise_rtt        = 0x1c0a, //样本盘顺时针旋转x个杯位命令
    cmd_sample_dsk_spscup_rtt_to_spspos = 0x1c0b, //样本盘指定杯位转到指定位置
    cmd_sample_dsk_rtt_onecycle         = 0x1c0c, //样本盘旋转1圈
    cmd_sample_dsk_rtt_to_bcdscan       = 0x1c11, //样本盘指定杯位转到条码扫描位
	cmd_sample_dsk_fpga_update			= 0x1c20, //样本盘FPAGA 程序在线更新功能

    cmd_sample_dsk_motor_hold_ctl       = 0x1c50, //样本盘电机保持力矩控制  
    cmd_sample_dsk_step_move            = 0x1c51, //样本盘微步移动调整

	cmd_sample_dsk_chip_reset         	= 0x1c70, //芯片复位
	cmd_sample_dsk_show_wave_switch     = 0x1c71, //虚拟示波器开关
	cmd_sample_dsk_update           	= 0x1c80, //样本盘升级
    
    cmd_sample_dsk_compensation         = 0x1cc0, //样本盘补偿命令
    cmd_sample_dsk_lightCtl             = 0x1cc1, //样本指示灯控制
    cmd_sample_dsk_max                  = 0x1cff, //样本盘最大命令
}cmd_sample_dsk_type_enum;

//样本盘圈id定义
typedef enum
{
    cmd_sample_dsk_circle_1_id = 0,  //最外圈
    cmd_sample_dsk_circle_2_id = 1,
    cmd_sample_dsk_circle_3_id = 2,
    cmd_sample_dsk_circle_4_id = 3,
    //cmd_sample_dsk_circle_5_id = 4,  //最内圈

    cmd_sample_dsk_circle_butt
}cmd_sample_dsk_circle_enum;


typedef enum
{
	cmd_sample_dsk_circle_1_num = 54,  
	cmd_sample_dsk_circle_2_num = 108,
	cmd_sample_dsk_circle_3_num = 135,
	cmd_sample_dsk_circle_4_num = 162,
}cmd_sample_dsk_num_enum;
//样本盘 每圈最大杯数量





/*样本盘水平转动到条码扫描位命令数据结构*/
typedef struct
{
    app_u16 cmd;
    char  uccircleid;  //样本盘圈号 (0xFF: 3圈同时扫描)
    char  uccupid;     //样本盘杯号:(0:整圈扫描完再返回数据，1-最大杯号:返回指定杯号数据)
}cmd_sample_dsk_bcdscan_stru;

#endif
