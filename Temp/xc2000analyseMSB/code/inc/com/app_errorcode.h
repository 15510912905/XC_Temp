//checked
#ifndef _app_errorcode_h_
#define _app_errorcode_h_

#include "vos_errorcode.h"

//错误码基值定义
typedef enum
{
    app_resource_schedule_errorcode_base = 0x00010000, //资源调度模块错误码基值
    app_reactsys_mng_errorcode_base      = 0x00020000, //反应盘管理模块错误码基值
    app_samplesys_mng_errorcode_base     = 0x00030000, //样本盘管理模块错误码基值
    app_reagentsys_mng_errorcode_base    = 0x00040000, //试剂盘管理模块错误码基值
    app_testtask_mng_errorcode_base      = 0x00050000, //测试任务管理模块错误码基值
    app_command_errorcode_base           = 0x00060000, //命令模块
    app_maintain_errorcode_base          = 0x00070000, //维测模块
    app_middwnmach_mng_errorcode_base    = 0x00080000, //中下位机命令管理模块错误码基值
    app_mnguart_errorcode_base           = 0x00090000, //串口管理模块错误码基值
    app_mng_exeact_statemach_errorcode_base = 0x000a0000, //执行状态机管理模块错误码基值
    app_debugtty_errorcode_base          = 0x000b0000, //调试串口模块错误码基值
    app_commonsys_mng_errorcode_base     = 0x000c0000, //公共模块错误码基值
    app_oam_mng_errorcode_base           = 0x000d0000, //OAM管理模块错误码基值
    app_barcodescan_errorcode_base       = 0x000e0000, //条码扫描模块错误码基值
    app_report_dispose_errorcode_base    = 0x000f0000, //状态上报处理模块错误发基值
}app_errorcode_base_enum;

typedef enum
{
    app_para_null_err                  = 0x00000001, //空指针
    app_open_file_error                = 0x00000002, //打开文件错误
    app_unknown_msg_pid_error          = 0x00000003, //未知消息pid错误
    app_unknown_timer_error            = 0x00000003, //未知定时器错误
    app_inexistence_action_err         = 0x00000004, //不存在的动作类型
    app_cmd_retrans_out_error          = 0x00000005, //命令重试次数超过
    app_cmd_crcunequal_error           = 0x00000006, //命令crc校验不一致
    app_cmd_unknowncmd_error           = 0x00000007, //未知命令id
    app_unknown_subsystem_err          = 0x00000008, //未知子系统错误
    app_open_uart_dev_err              = 0x00000009, //打开串口设备文件失败
    app_unknown_msg_type_err           = 0x0000000a, //未知消息类型错误
    app_unknown_dwnmach_err            = 0x0000000b, //未知下位机类型错误
    app_malloc_mutex_err               = 0x0000000c, //申请信号量失败错误
    app_mem_size_zero_err              = 0x0000000d, //内存长度为零错误
    app_uart_handle_null_err           = 0x0000000e, //串口具备为空错误
    app_write_uart_handle_err          = 0x0000000f, //写串口错误
    app_full_exeact_statemach_err      = 0x00000010, //执行状态机满错误
    app_exeact_statemach_nucreated_err = 0x00000011, //执行状态机未创建错误
    app_input_para_invalid_err         = 0x00000012, //输入参数错误
    app_original_act_too_many_err      = 0x00000013, //原始动作太多错误
    app_ctrl_statemach_busy_err        = 0x00000014, //控制状态机忙错误
    app_req_func_act_invalid_err       = 0x00000015, //请求的功能动作非法
    app_statemach_stop_rcving_dwnmach_err = 0x00000016, //状态机停止的情况下收到下位机响应消息错误
    app_statemach_no_sonact_err        = 0x00000017, //状态机里没有子动作错误
    app_invalid_uart_id_err            = 0x00000018, //无效的串口号
    app_unknown_device_type            = 0x00000019, //未知设备类型错误
    app_invalid_extdev_name_err        = 0x0000001a, //无效的外部设备名称错误
    app_invalid_extdev_operatemode_err = 0x0000001b, //无效的外部设备操作模式
    app_stack_full_err                 = 0x0000001c, //堆栈满错误
    app_invalid_command_info_len_err   = 0x0000001d, //command信息长度错误
    app_unknown_command_id_err         = 0x0000001e, //command号错误
    app_ipaddr_unmatch_err             = 0x0000001f, //上位机下发中位机IP不匹配错误
    app_unknown_rpt_status_type_err    = 0x00000020, //未知下位机上报状态类型错误
}app_errorcode_enum;

#endif