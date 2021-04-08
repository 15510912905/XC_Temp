//modifyed
#ifndef _CMD_R2REAGENTNEEDLE_H_
#define _CMD_R2REAGENTNEEDLE_H_

#include"vos_basictypedefine.h"

//R2试剂针单元命令枚举定义
typedef enum{
    cmd_r2_reagent_ndl_min               = 0x3000, //R2试剂针单元最小命令
    cmd_r2_reagent_ndl_reset             = 0x3008, //R2试剂针复位命令
    cmd_r2_reagent_ndl_vrise             = 0x3009, //R2试剂针垂直上升
    cmd_r2_reagent_ndl_vdecline          = 0x300a, //R2试剂针垂直下降
    cmd_r2_reagent_ndl_hrotate_washpos   = 0x300d, //R2试剂针水平转到清洗位
    cmd_r2_reagent_ndl_hrotate_absbpos   = 0x300e, //R2试剂针水平转到吸试剂位
    cmd_r2_reagent_ndl_hrotate_dischpos  = 0x300f, //R2试剂针水平转到排试剂位
    cmd_r2_reagent_ndl_absorb_reagent    = 0x3010, //R2试剂针吸试剂
    cmd_r2_reagent_ndl_discharge_reagent = 0x3011, //R2试剂针排试剂
    cmd_r2_reagent_ndl_exe_washaction    = 0x3018, //R2试剂针执行清洗
    cmd_r2_reagent_ndl_absorb_water      = 0x3019, //R2试剂针吸蒸馏水
    cmd_r2_reagent_ndl_marginscan        = 0x301a, //R2试剂针余量扫描命令
    cmd_r2_reagent_ndl_compensation      = 0x30c0, //R2试剂针补偿命令
    cmd_r2_reagent_ndl_comb_cmd          = 0x30cc, //R2试剂针组合命令
    cmd_r2_reagent_ndl_slowdown          = 0x30cd, //R2指定步数、速度下降
    cmd_r2_reagent_pvctl_v09             = 0x3023, //R2泵阀开关
    cmd_r2_reagent_pvctl_v04             = 0x3024, //R2泵阀开关

    cmd_r2_ndl_vrise_setspeed            = 0x3039, //R2试剂针垂直上升
    cmd_r2_ndl_vdecline_setspeed         = 0x303a, //R2试剂针垂直下降
    cmd_r2_ndl_hrotate_setspeed          = 0x303b, //R2试剂针水平转
    cmd_r2_ndl_absorb_reagent_setvol     = 0x303d, //R2试剂针吸试剂
    
    cmd_r2_ndl_motor_hold_ctl            = 0x3050, //R2针电机保持力矩控制  
    cmd_r2_ndl_step_move                 = 0x3051, //R2针微步移动调整

	cmd_r2_ndl_chip_reset         		 = 0x3070, //芯片复位
	cmd_r2_ndl_intensify_wash         	 = 0x3071, //强化清洗
	cmd_r2_ndl_air_out         	  		 = 0x3073, //注射泵排气
	cmd_r2_ndl_update            		 = 0x3080, //R2 升级
    
    cmd_r2_reagent_ndl_max               = 0x30ff, //R2试剂针单元最大命令
}cmd_r2_reagentndl_type_enum;

//R2试剂针取试剂位定义
typedef enum{
    cmd_r2_reagentndl_inner_absrb_pos = 0, //R2试剂针内圈取试剂位
    cmd_r2_reagentndl_outer_absrb_pos = 1, //R2试剂针外圈取试剂位

    cmd_r2_reagentndl_absorb_position_butt
}cmd_r2_reagentndl_absorb_position_enum;
typedef char cmd_r2_reagentndl_absorb_position_enum_u8;

/*R2试剂针试剂余量检查结果反馈数据结果*/
typedef struct{
    app_u16 uscmdtype;
    char  enexerslt;      //命令执行结果
    char  aucrsv[1];
    app_u16 usdeclinehigh;  //针下降高度
    app_u16 usrsv;
}cmd_r2_reagent_marginscan_res_stru;

#endif
