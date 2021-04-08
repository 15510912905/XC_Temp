//modifyed
#ifndef _CMD_SAMPLEMIXING_H_
#define _CMD_SAMPLEMIXING_H_

#include"vos_basictypedefine.h"

//样本搅拌单元命令枚举定义
typedef enum
{
    cmd_sample_mix_ndl_min             = 0x2000, //样本搅拌单元最小命令
    cmd_sample_mix_ndl_reset           = 0x2008, //样本搅拌单元复位命令
    cmd_sample_mix_ndl_vrise           = 0x2009, //样本搅拌针垂直向上运动命令
    cmd_sample_mix_ndl_vdecline        = 0x200a, //样本搅拌针垂直向下运动命令
    cmd_sample_mix_ndl_hrotate_washpos = 0x200d, //样本搅拌针水平转动到清洗位命令
    cmd_sample_mix_ndl_hrotate_mixpos  = 0x200e, //样本搅拌针水平转动到搅拌位命令
    cmd_sample_mix_ndl_exe_washing     = 0x2015, //样本搅拌针执行清洗命令
    cmd_sample_mix_ndl_exe_mixxing     = 0x2016, //样本搅拌针执行搅拌命令
    cmd_sample_mix_ndl_v10_ctrl        = 0x2017, //样本搅拌外壁清洗电磁阀V10控制
    cmd_sample_mix_ndl_slowdown        = 0x2018, //样本搅拌杆指定步数、速度下降
    cmd_sample_pvctl_v10               = 0x2019, //样本搅拌杆泵阀开关命令
    
    cmd_sample_mix_motor_hold_ctl      = 0x2050, //样本搅拌电机保持力矩控制  
    cmd_sample_mix_step_move           = 0x2051, //样本搅拌微步移动调整

	cmd_sample_mix_chip_reset          = 0x2070, //芯片复位
	cmd_sample_mix_update              = 0x2080, //样本搅拌升级
    
    cmd_sample_mix_ndl_compensation    = 0x20c0, //样本搅拌杆补偿命令
    cmd_sample_mix_ndl_max             = 0x20ff, //样本搅拌单元最大命令
}cmd_sample_mix_ndl_type_enum;

#endif
