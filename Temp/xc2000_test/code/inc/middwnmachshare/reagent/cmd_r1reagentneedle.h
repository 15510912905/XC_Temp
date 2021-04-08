//modifyed
#ifndef _CMD_R1REAGENTNEEDLE_H_
#define _CMD_R1REAGENTNEEDLE_H_

#include"vos_basictypedefine.h"

//R1试剂针单元命令枚举定义
typedef enum{
    cmd_r1_reagent_ndl_min               = 0x2c00, //R1试剂针单元最小命令
    cmd_r1_reagent_ndl_reset             = 0x2c08, //R1试剂针复位命令
    cmd_r1_reagent_ndl_vrise             = 0x2c09, //R1试剂针垂直上升
    cmd_r1_reagent_ndl_vdecline          = 0x2c0a, //R1试剂针垂直下降
    cmd_r1_reagent_ndl_hrotate_washpos   = 0x2c0d, //R1试剂针水平转到清洗位
    cmd_r1_reagent_ndl_hrotate_absbpos   = 0x2c0e, //R1试剂针水平转到吸试剂位
    cmd_r1_reagent_ndl_hrotate_dischpos  = 0x2c0f, //R1试剂针水平转到排试剂位
    cmd_r1_reagent_ndl_absorb_reagent    = 0x2c10, //R1试剂针吸试剂
    cmd_r1_reagent_ndl_discharge_reagent = 0x2c11, //R1试剂针排试剂
    cmd_r1_reagent_ndl_exe_washaction    = 0x2c18, //R1试剂针执行清洗
    cmd_r1_reagent_ndl_absorb_water      = 0x2c19, //R1试剂针吸蒸馏水
    cmd_r1_reagent_ndl_marginscan        = 0x2c1a, //R1试剂针余量扫描命令
    cmd_r1_reagent_ndl_compensation      = 0x2cc0, //R1试剂针补偿命令
    cmd_r1_reagent_ndl_comb_cmd          = 0x2ccc, //R1试剂针组合命令
    cmd_r1_reagent_ndl_slowdown          = 0x2ccd, //R1指定步数、速度下降
    cmd_r1_reagent_pvctl_v08             = 0x2c23, //R1v08开关命令-外
    cmd_r1_reagent_pvctl_v03             = 0x2c24, //R1v03开关命令-内
    
    cmd_r1_ndl_vrise_setspeed            = 0x2c39, //R1试剂针垂直上升
    cmd_r1_ndl_vdecline_setspeed         = 0x2c3a, //R1试剂针垂直下降
    cmd_r1_ndl_hrotate_setspeed          = 0x2c3b, //R1试剂针水平转
    cmd_r1_ndl_absorb_reagent_setvol     = 0x2c3d, //R1试剂针吸试剂
    
    cmd_r1_ndl_motor_hold_ctl            = 0x2c50, //R1针电机保持力矩控制  
    cmd_r1_ndl_step_move                 = 0x2c51, //R1针微步移动调整

	cmd_r1_ndl_chip_reset         		 = 0x2c70, //芯片复位
	cmd_r1_ndl_intensify_wash         	 = 0x2c71, //强化清洗
	cmd_r1_ndl_air_out         	  		 = 0x2c73, //注射泵排气
	cmd_r1_ndl_chip_update         		 = 0x2c80, //R1升级

    cmd_r1_ndl_update                    = 0x2cf0, //R1针升级命令
    cmd_r1_reagent_ndl_max               = 0x2cff, //R1试剂针单元最大命令
}cmd_r1_reagentndl_type_enum;

//R1试剂针取试剂位定义
typedef enum{
    cmd_r1_reagentndl_inner_absrb_pos = 0, //R1试剂针内圈取试剂位
    cmd_r1_reagentndl_outer_absrb_pos = 1, //R1试剂针外圈取试剂位

    cmd_r1_reagentndl_absorb_position_butt
}cmd_r1_reagentndl_absorb_position_enum;
typedef char cmd_r1_reagentndl_absorb_position_enum_u8;

/*R1试剂针垂直旋转命令结构*/
typedef struct{
    app_u16 cmd;
    char  enposition; //运动到的目标位置
    char  reportmarginflag;//上报余量标志，1:上报，0:不上报
    app_u16 ustotalvol; //杯总体积
    char  rsv[2];
    app_u16 diskid;   //盘号，用于余量上报
    app_u16 cupid;    //杯位号，用于余量上报
    vos_u32 reactcup;   //任务号，用于误检、漏检处理
}cmd_r1_reagent_ndl_vrotate_stru;

/*R1试剂针吸试剂或蒸馏水命令结构*/
typedef struct{
    app_u16 cmd;
    app_u16 quantity;  //取试剂量
}cmd_r1_reagent_ndl_absorb_stru;

/*R1试剂针排试剂命令结构*/
typedef struct{
    app_u16 cmd;
    app_u16 quantity;  //排试剂量
    char  diluteflag;
    char  rsv[3];
}cmd_r1_reagent_ndl_discharge_stru;

#endif
