//modifyed
#ifndef _CMD_REACTIONDISK_H_
#define _CMD_REACTIONDISK_H_



//反应盘单元命令枚举定义
typedef enum
{
    cmd_react_disk_min               = 0x0400,  //反应盘单元最小命令
    cmd_react_disk_reset             = 0x0408,  //反应盘复位命令
    cmd_react_disk_anticlockwise_rtt = 0x040c,  //反应盘逆时针旋转X个杯位
    cmd_react_disk_clockwise_rtt     = 0x040d,  //反应盘顺时针旋转X个杯位
    cmd_react_disk_light_control     = 0x040f,  //反应盘光源控制 0-关闭 100-打开
    cmd_react_disk_get_adval         = 0x0410,  //获取反应杯AD值
    cmd_react_disk_src_2_dst         = 0x0411,  //反应盘指定杯位旋转到目标位置
    cmd_react_disk_rtt_onecycle      = 0x0412,  //反应盘旋转一周
    cmd_react_disk_lightsize_ctl     = 0x0413,  //灯亮度调整
    
    cmd_react_disk_motor_hold_ctl    = 0x0450, //反应盘电机保持力矩控制  
    cmd_react_disk_step_move         = 0x0451, //反应盘微步移动调整
    cmd_react_disk_get_LightBulbAD   = 0x0452, //获取光源灯AD值 

	cmd_react_disk_chip_reset         = 0x0470, //芯片复位
	cmd_react_disk_update            = 0x0480, //反应盘升级
    
    cmd_react_disk_compensation      = 0x04c0,  //反应盘补偿命令
    cmd_react_disk_max               = 0x04ff,  //反应盘单元最大命令
}cmd_reactdisk_type_enum;

#endif
