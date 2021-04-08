//modifyed
#ifndef _CMD_REAGENTMIXING_H_
#define _CMD_REAGENTMIXING_H_

#include"vos_basictypedefine.h"

//试剂搅拌单元命令枚举定义
typedef enum
{
    cmd_reagent_mix_ndl_min             = 0x0800, //试剂搅拌针单元最小命令
    cmd_reagent_mix_ndl_reset           = 0x0808, //试剂搅拌针单元复位命令
    cmd_reagent_mix_ndl_vrise           = 0x0809, //试剂搅拌针向上运动命令
    cmd_reagent_mix_ndl_vdecline        = 0x080a, //试剂搅拌针向下运动命令
    cmd_reagent_mix_ndl_hrotate_washpos = 0x080d, //试剂搅拌针水平旋转到清洗位命令
    cmd_reagent_mix_ndl_hrotate_mixpos  = 0x080e, //试剂搅拌针水平旋转到搅拌位命令
    cmd_reagent_mix_ndl_exe_washing     = 0x0815, //试剂搅拌针执行清洗命令
    cmd_reagent_mix_ndl_exe_mixxing     = 0x0816, //试剂搅拌针执行搅拌命令
	cmd_reagent_mix_ndl_slowdown        = 0x0818, //试剂搅拌针指定步数、速度下降
    cmd_reagent_mix_pvctl_v11           = 0x0819, //试剂搅拌针泵阀开关命令，此处v11正确，勿修改！！！
    cmd_reagent_mix_fpga_update         = 0x0820, //试剂搅拌针fpga升级
    
    cmd_reagent_mix_motor_hold_ctl      = 0x0850, //试剂搅拌电机保持力矩控制  
    cmd_reagent_mix_step_move           = 0x0851, //试剂搅拌微步移动调整

	cmd_reagent_mix_chip_reset          = 0x0870, //芯片复位
	cmd_reagent_mix_update              = 0x0880, //试剂搅拌升级
    
    cmd_reagent_mix_compensation        = 0x08c0, //试剂搅拌针补偿命令
    cmd_reagent_mix_ndl_max             = 0x08ff, //试剂搅拌针单元最大命令
}cmd_reagent_mix_ndl_type_enum;

#endif
