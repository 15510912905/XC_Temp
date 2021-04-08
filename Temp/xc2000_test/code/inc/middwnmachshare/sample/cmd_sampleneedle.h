//modifyed
#ifndef _CMD_SAMPLENEEDLE_H_
#define _CMD_SAMPLENEEDLE_H_

#include"vos_basictypedefine.h"

//样本针单元命令枚举定义
typedef enum
{
    cmd_sample_ndl_min                = 0x2400, //样本针单元最小命令
    cmd_sample_ndl_reset              = 0x2408, //样本针复位命令
    cmd_sample_ndl_vrise              = 0x2409, //样本针垂直向上运动
    cmd_sample_ndl_vdecline           = 0x240a, //样本针垂直向下运动
    cmd_sample_ndl_hrtt_nml_dischpos  = 0x240d, //样本针水平旋转到正常排样本位
    cmd_sample_ndl_hrtt_ise_dischpos  = 0x241a, //样本针水平旋转到ISE排样本位
    cmd_sample_ndl_hrtt_dsk_absrb1pos = 0x240e, //样本针水平旋转到样本盘吸样本位1(最外圈)
    cmd_sample_ndl_hrtt_dsk_absrb2pos = 0x240f, //样本针水平旋转到样本盘吸样本位2
    cmd_sample_ndl_hrtt_dsk_absrb3pos = 0x2410, //样本针水平旋转到样本盘吸样本位3
    cmd_sample_ndl_hrtt_dsk_absrb4pos = 0x2411, //样本针水平旋转到样本盘吸样本位4
    cmd_sample_ndl_hrtt_dsk_absrb5pos = 0x2412, //样本针水平旋转到样本盘吸样本位5
    cmd_sample_ndl_hrtt_dilute_pos    = 0x241c, //样本针水平旋转到稀释位
    cmd_sample_ndl_hrtt_rail_absrbpos = 0x241b, //样本针水平旋转到轨道吸样本位
    cmd_sample_ndl_hrtt_wash_pos      = 0x2413, //样本针水平旋转到清洗位
    cmd_sample_ndl_absorb_sample      = 0x241d, //样本针吸样本
    cmd_sample_ndl_discharge          = 0x241e, //样本针排样本
    cmd_sample_ndl_exe_wash           = 0x241f, //样本针清洗动作命令
    cmd_sample_ndl_absorb_water       = 0x2420, //样本针吸蒸馏水
    cmd_sample_ndl_slowdown           = 0x2421, //样本针指定步数、速度下降
    cmd_sample_pvctl_v07              = 0x2423, //样本针泵阀开关-外
    cmd_sample_pvctl_v02              = 0x2424, //样本针泵阀开关-内
    
    cmd_sample_ndl_vrise_setspeed     = 0x2439, //指定速度样本针垂直向上运动
    cmd_sample_ndl_vdecline_setspeed  = 0x243a, //指定速度样本针垂直向下运动
    cmd_sample_ndl_hrtt_setspeed      = 0x243b, //指定速度样本针水平旋转到指定位置
    cmd_sample_ndl_absorb_sample_setvol=0x243d, //指定回推体积吸样

    cmd_sample_ndl_set_blood_clot_value=0x2440, //设置血凝块灵敏度值
    
    cmd_sample_ndl_motor_hold_ctl     = 0x2450, //样本针电机保持力矩控制  
    cmd_sample_ndl_step_move          = 0x2451, //样本针微步移动调整

	cmd_sample_ndl_chip_reset         = 0x2470, //芯片复位
	cmd_sample_ndl_intensify_wash     = 0x2471, //强化清洗
	cmd_sample_ndl_air_out         	  = 0x2473, //注射泵排气
	cmd_sample_ndl_update             = 0x2480, //样本针升级
    
    cmd_sample_ndl_compensation       = 0x24c0, //样本针补偿命令
    cmd_sample_ndl_pressurecheck      = 0x24c1, //样本针压力检测
    cmd_sample_ndl_long_wash          = 0x24c2, //长周期清洗
    cmd_sample_ndl_wash_rtt           = 0x24c3, //带清洗的水平运动
    cmd_sample_ndl_max                = 0x24ff, //样本针单元最大命令
}cmd_sampleneddle_type_enum;

//样本针水平位置定义
typedef enum
{
    cmd_sample_ndl_1circle_absorb_spot  = 0,  //最外圈取样本位
    cmd_sample_ndl_2circle_absorb_spot  = 1,
    cmd_sample_ndl_3circle_absorb_spot  = 2,
    cmd_sample_ndl_4circle_absorb_spot  = 3,
    cmd_sample_ndl_5circle_absorb_spot  = 4,  //最内圈取样本位
	cmd_sample_ndl_wash_spot            = 5,  //清洗位
    cmd_sample_ndl_rail_absorb_spot     = 6,  //轨道上取样本位
	cmd_sample_ndl_dilute_spot          = 7,  //稀释位
    cmd_sample_ndl_nml_discharge_spot   = 8,  //反应盘排样本位
    cmd_sample_ndl_ise_discharge_spot   = 9,  //ISE排样本位
    
    
    cmd_sample_ndl_absorb_spot_butt
}cmd_sampleneddle_absorb_spot_enum;

/*样本针吸样本or蒸馏水动作命令数据结构*/
typedef struct{
    app_u16 cmd;
    app_u16 quantity;//吸样本量
}cmd_sample_ndl_absrb_stru;

/*样本针排样本动作命令数据结构*/
typedef struct{
    app_u16 cmd;
    app_u16 quantity;  //排试剂量
    char  diluteflag;
    char  rsv[3];
}cmd_sample_ndl_discharge_stru;

#endif
