#ifndef _vos_errorcode_h_
#define _vos_errorcode_h_    
/******************************************************************************

                  版权所有 (C), 2013-2023, 四川新成生物科技有限公司

 ******************************************************************************
  文 件 名   : vos_errorcode.h
  版 本 号   : 初稿
  作    者   : Albort,Feng
  生成日期   : 2013年8月30日
  最近修改   :
  功能描述   : 平台错误码定义
  函数列表   :
  修改历史   :
  1.日    期   : 2013年8月30日
    作    者   : Albort,Feng
    修改内容   : 创建文件

******************************************************************************/

#define RETURNERRNO(mderrbase,errno) {return ((mderrbase)|(errno));}
#define GENERRNO(mderrbase,errno) (((mderrbase)|(errno)))

//平台各个模块错误码基值定义
typedef enum
{
    
    vos_mnglist_errorcode_base      = 0x01000000, //链表管理模块错误码基值
    vos_mngmem_errorcode_base       = 0x02000000, //内存管理模块错误码基值
    vos_adaptsys_errorcode_base     = 0x03000000, //操作系统适配模块错误码基值
    vos_msgcenter_errorcode_base    = 0x04000000, //消息中心模块错误码基值
    vos_fid_errorcode_base          = 0x05000000, //FID模块错误码基值
    vos_pid_errorcode_base          = 0x06000000, //PID模块错误码基值
    vos_faulmanage_errorcode_base   = 0x07000000, //故障管理模块错误码基值
    vos_mnglog_errorcode_base       = 0x08000000, //日志管理模块错误码基值
    vos_alarmcenter_errorcode_base  = 0x09000000, //告警中心模块错误码基值
    vos_messagetrace_errorcode_base = 0x0a000000, //消息追踪模块错误码基值
    vos_timer_errorcode_base        = 0x0b000000, //定时器模块错误码基值
    vos_secrt_cent_errorcode_base   = 0x0c000000, //安全模块错误码基值
    vos_mng_midmach_errorcode_base  = 0x0d000000, //中位机管理模块错误码基值
    vos_mng_socket_errorcode_base   = 0x0e000000, //socket管理模块错误码基值
    vos_ftp_errorcode_base          = 0x0f000000, //ftp服务器
    vos_networkmonitor_errcode_base = 0x10000000,   //网络监视模块
    vos_db_errorcode_base           = 0x11000000,   //系统错误
    vos_db_errorcode_mysqlbase      = 0x12000000,   //mysql db2 模块
    vos_algo_errorcode_base         = 0x20030000,   //算法模块
    vos_matrix_errorcode_base       = 0x20020000,   //矩阵模块
    vos_global_errorcode_base       = 0x20010000, //全局
}vos_errorcode_base_enum;

//平台错误码定义
typedef enum
{
    vos_para_null_err           = 0x00000001, //输入指针参数为空错误
    vos_para_invalid_err        = 0x00000002, //输入参数内容非法错误
    vos_list_num_max_err        = 0x00000003, //系统中链表数已达到最大
    vos_malloc_mem_err          = 0x00000004, //申请内存失败错误
    vos_list_id_invalid_err     = 0x00000005, //链表id非法错误
    vos_list_no_create_err      = 0x00000006, //链表为创建错误
    vos_list_no_sps_node_err    = 0x00000007, //链表没有指定节点
    vos_no_support_mem_type_err = 0x00000008, //不支持内存类型错误
    vos_create_thread_err       = 0x00000009, //创建线程失败
    vos_set_threadpri_error     = 0x0000000a, //设置线程优先级失败
    vos_resum_thread_error      = 0x0000000b, //恢复线程运行失败
    vos_list_exist_sps_node_err = 0x0000000c, //链表中存在指定关键字结点错误
    vos_msgbuf_invalid_error    = 0x0000000d, //消息缓存不是有消息中心申请
    vos_fid_invalid_error       = 0x0000000e, //FID非法错误
    vos_pid_invalid_error       = 0x0000000f, //PID非法错误
    vos_fid_unreg_error         = 0x00000010, //FID未注册错误
    vos_pid_unreg_error         = 0x00000011, //PID未注册错误
    vos_suspend_thread_error    = 0x00000012, //挂起线程失败
    vos_fid_reach_max_error     = 0x00000013, //已达到系统支持最大FID模块数
    vos_max_pid_num_error       = 0x00000014, //已达到PID最大数
    vos_max_fid_num_error       = 0x00000015, //已达到FID最大数
    vos_uncrt_mailbox_error     = 0x00000016, //未创建消息邮箱错误
    vos_mailbox_full_error      = 0x00000017, //消息邮箱满错误
    vos_max_crt_thd_error       = 0x00000018, //线程创建达到最大
    vos_createmutex_error       = 0x00000019, //创建互斥锁失败
    vos_createnode_error        = 0x0000001a, //创建链表节点失败
    vos_faultid_invalid_error   = 0x0000001b, //故障码无效错误
    vos_alarmid_invalid_error   = 0x0000001c, //告警ID无效错误
    vos_alarmid_inexist_error   = 0x0000001d, //不存在告警类别
    vos_faultid_exist_error     = 0x0000001e, //重复注册故障码错误
    vos_load_sock_lib_error     = 0x0000001f, //加载sock库失败
    vos_obtain_hostname_error   = 0x00000020, //获取主机名失败
    vos_dst_pid_error           = 0x00000021, //目的PID非法
    vos_fid_registered_error    = 0x00000022, //FID已经被注册
    vos_pid_registered_error    = 0x00000023, //PID已经被注册
    vos_mailbox_confusion_error = 0x00000024, //消息邮箱冲突错误
    vos_alarm_generate_error    = 0x00000025, //告警重复产生错误
    vos_getlinuxlocalip_error   = 0x00000026, //linux获取本地IP出错
    vos_no_netinterface_err     = 0x00000027, //本机上IP不可用
    vos_no_alarmtype_error      = 0x00000028, //没有的告警类型
    vos_create_file_error       = 0x00000029, //创建文件失败
    vos_get_file_attr_error     = 0x0000002a, //获取文件属性失败
    vos_open_file_error         = 0x0000002b, //打开文件失败
    vos_not_have_alarmstru_err  = 0x0000002c, //没有可用的结构体
    vos_fauitidhas_in_team_error= 0x0000002d, //已属于某类告警类型下属
    vos_load_dynamic_lib_error  = 0x0000002e, //加载动态库失败
    vos_create_socket_error     = 0x0000002f, //创建socket失败
    vos_socket_receive_error    = 0x00000030, //socket接收数据包错误
    vos_socket_bind_error       = 0x00000031, //socket绑定地址错误
    vos_socket_listen_error     = 0x00000032, //socket启动监听错误
    vos_socket_connect_error    = 0x00000033, //socket链接错误
    vos_malloc_mutex_error      = 0x00000034, //申请mutex失败
    vos_subfauitmap_full_err    = 0x00000035, //告警下属空间已使用完错误
    vos_faultinfomap_full_err   = 0x00000036, //告警故障信息达到最大值
    vos_no_alarm_err            = 0x00000037, //当前无告警
    vos_isexistalarmnode_err    = 0x00000038, //告警节点已存在
    vos_existfaultinfo_err      = 0x00000039, //故障信息已存在
    vos_socket_accept_error     = 0x0000003a, //socket等待客户端请求错误
    vos_noneedtrace_err         = 0x0000003b, //无需消息追踪
    vos_tracetimeout_err        = 0x0000003c, //追踪超时
    vos_msgtypeoutline_err      = 0x0000003d, //消息类型超过定义
    vos_trace_not_found_err     = 0x0000003e, //追踪信息不存在
    vos_crt_basetimer_error     = 0x0000003f, //创建基准定时器错误
    vos_setcrt_basetimer_error  = 0x00000040, //Linux启动基准定时器错误
    vos_timer_not_find_error    = 0x00000041, //找不到指定ID的定时器
    vos_no_timer_timername_err  = 0x00000042, //没有指定名字的定时器
    vos_timername_repeat_err    = 0x00000043, //同名定时器错误
    vos_toendofdir_error        = 0x00000044, //目录记录已经提取完
    vos_unknown_msg_type_error  = 0x00000045, //未知消息类型
    vos_reg_network_fault_error = 0x00000046, //注册网络故障失败
    vos_ping_result_err         = 0x00000047, //ping失败
    vos_socket_send_error       = 0x00000048, //socket发送数据包错误
    vos_wait_chkauth_tmout_error= 0x00000049, //等待鉴权定时器超时错误
    vos_check_auth_failed_error = 0x0000004a, //鉴权结果失败错误
    vos_reg_midmach_failed_error= 0x0000004b, //注册中位机失败错误
    vos_malloc_mutex_faied_error= 0x0000004c, //申请信号量失败错误
    vos_read_file_error         = 0x0000004d, //读文件失败
    vos_write_file_error        = 0x0000004e, //写文件失败
    vos_array_index_error       = 0x0000004f, //数组索引错误
    vos_get_curworkpath_error   = 0x00000050, //获取当前工作路径失败
    vos_init_thread_attr_error  = 0x00000051, //初始化线程属性失败
    vos_set_thread_detach_error = 0x00000052, //设置线程分离失败
    vos_set_thread_stack_error  = 0x00000053, //设置线程堆栈大小失败
    vos_destory_thread_attr_error = 0x00000054, //摧毁线程属性失败
    vos_get_threadpri_error       = 0x000000055, //设置线程优先级失败
    vos_timer_id_used_over_error  = 0x000000056, //定时id消耗完错误
    vos_timer_id_invalid_error    = 0x000000057, //定时id非法错误
    vos_sps_midmachip_uncfg_error = 0x000000058, //指定中位机IP地址未配置
    vos_ansi_to_utf8_error        = 0x000000059, //ANSI到UTF8格式转换失败
    vos_query_db_data_error       = 0x00000005a, //查询数据库失败
    vos_objected_isregistered_error = 0x00000005b,//对像已经注册
    vos_set_thread_policy_error   = 0x00000005c, //设置线程调度策略失败
}vos_errorcode_enum;

#endif
