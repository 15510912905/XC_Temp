#ifndef _vos_errorcode_h_
#define _vos_errorcode_h_    
/******************************************************************************

                  ��Ȩ���� (C), 2013-2023, �Ĵ��³�����Ƽ����޹�˾

 ******************************************************************************
  �� �� ��   : vos_errorcode.h
  �� �� ��   : ����
  ��    ��   : Albort,Feng
  ��������   : 2013��8��30��
  ����޸�   :
  ��������   : ƽ̨�����붨��
  �����б�   :
  �޸���ʷ   :
  1.��    ��   : 2013��8��30��
    ��    ��   : Albort,Feng
    �޸�����   : �����ļ�

******************************************************************************/

#define RETURNERRNO(mderrbase,errno) {return ((mderrbase)|(errno));}
#define GENERRNO(mderrbase,errno) (((mderrbase)|(errno)))

//ƽ̨����ģ��������ֵ����
typedef enum
{
    
    vos_mnglist_errorcode_base      = 0x01000000, //�������ģ��������ֵ
    vos_mngmem_errorcode_base       = 0x02000000, //�ڴ����ģ��������ֵ
    vos_adaptsys_errorcode_base     = 0x03000000, //����ϵͳ����ģ��������ֵ
    vos_msgcenter_errorcode_base    = 0x04000000, //��Ϣ����ģ��������ֵ
    vos_fid_errorcode_base          = 0x05000000, //FIDģ��������ֵ
    vos_pid_errorcode_base          = 0x06000000, //PIDģ��������ֵ
    vos_faulmanage_errorcode_base   = 0x07000000, //���Ϲ���ģ��������ֵ
    vos_mnglog_errorcode_base       = 0x08000000, //��־����ģ��������ֵ
    vos_alarmcenter_errorcode_base  = 0x09000000, //�澯����ģ��������ֵ
    vos_messagetrace_errorcode_base = 0x0a000000, //��Ϣ׷��ģ��������ֵ
    vos_timer_errorcode_base        = 0x0b000000, //��ʱ��ģ��������ֵ
    vos_secrt_cent_errorcode_base   = 0x0c000000, //��ȫģ��������ֵ
    vos_mng_midmach_errorcode_base  = 0x0d000000, //��λ������ģ��������ֵ
    vos_mng_socket_errorcode_base   = 0x0e000000, //socket����ģ��������ֵ
    vos_ftp_errorcode_base          = 0x0f000000, //ftp������
    vos_networkmonitor_errcode_base = 0x10000000,   //�������ģ��
    vos_db_errorcode_base           = 0x11000000,   //ϵͳ����
    vos_db_errorcode_mysqlbase      = 0x12000000,   //mysql db2 ģ��
    vos_algo_errorcode_base         = 0x20030000,   //�㷨ģ��
    vos_matrix_errorcode_base       = 0x20020000,   //����ģ��
    vos_global_errorcode_base       = 0x20010000, //ȫ��
}vos_errorcode_base_enum;

//ƽ̨�����붨��
typedef enum
{
    vos_para_null_err           = 0x00000001, //����ָ�����Ϊ�մ���
    vos_para_invalid_err        = 0x00000002, //����������ݷǷ�����
    vos_list_num_max_err        = 0x00000003, //ϵͳ���������Ѵﵽ���
    vos_malloc_mem_err          = 0x00000004, //�����ڴ�ʧ�ܴ���
    vos_list_id_invalid_err     = 0x00000005, //����id�Ƿ�����
    vos_list_no_create_err      = 0x00000006, //����Ϊ��������
    vos_list_no_sps_node_err    = 0x00000007, //����û��ָ���ڵ�
    vos_no_support_mem_type_err = 0x00000008, //��֧���ڴ����ʹ���
    vos_create_thread_err       = 0x00000009, //�����߳�ʧ��
    vos_set_threadpri_error     = 0x0000000a, //�����߳����ȼ�ʧ��
    vos_resum_thread_error      = 0x0000000b, //�ָ��߳�����ʧ��
    vos_list_exist_sps_node_err = 0x0000000c, //�����д���ָ���ؼ��ֽ�����
    vos_msgbuf_invalid_error    = 0x0000000d, //��Ϣ���治������Ϣ��������
    vos_fid_invalid_error       = 0x0000000e, //FID�Ƿ�����
    vos_pid_invalid_error       = 0x0000000f, //PID�Ƿ�����
    vos_fid_unreg_error         = 0x00000010, //FIDδע�����
    vos_pid_unreg_error         = 0x00000011, //PIDδע�����
    vos_suspend_thread_error    = 0x00000012, //�����߳�ʧ��
    vos_fid_reach_max_error     = 0x00000013, //�Ѵﵽϵͳ֧�����FIDģ����
    vos_max_pid_num_error       = 0x00000014, //�ѴﵽPID�����
    vos_max_fid_num_error       = 0x00000015, //�ѴﵽFID�����
    vos_uncrt_mailbox_error     = 0x00000016, //δ������Ϣ�������
    vos_mailbox_full_error      = 0x00000017, //��Ϣ����������
    vos_max_crt_thd_error       = 0x00000018, //�̴߳����ﵽ���
    vos_createmutex_error       = 0x00000019, //����������ʧ��
    vos_createnode_error        = 0x0000001a, //��������ڵ�ʧ��
    vos_faultid_invalid_error   = 0x0000001b, //��������Ч����
    vos_alarmid_invalid_error   = 0x0000001c, //�澯ID��Ч����
    vos_alarmid_inexist_error   = 0x0000001d, //�����ڸ澯���
    vos_faultid_exist_error     = 0x0000001e, //�ظ�ע����������
    vos_load_sock_lib_error     = 0x0000001f, //����sock��ʧ��
    vos_obtain_hostname_error   = 0x00000020, //��ȡ������ʧ��
    vos_dst_pid_error           = 0x00000021, //Ŀ��PID�Ƿ�
    vos_fid_registered_error    = 0x00000022, //FID�Ѿ���ע��
    vos_pid_registered_error    = 0x00000023, //PID�Ѿ���ע��
    vos_mailbox_confusion_error = 0x00000024, //��Ϣ�����ͻ����
    vos_alarm_generate_error    = 0x00000025, //�澯�ظ���������
    vos_getlinuxlocalip_error   = 0x00000026, //linux��ȡ����IP����
    vos_no_netinterface_err     = 0x00000027, //������IP������
    vos_no_alarmtype_error      = 0x00000028, //û�еĸ澯����
    vos_create_file_error       = 0x00000029, //�����ļ�ʧ��
    vos_get_file_attr_error     = 0x0000002a, //��ȡ�ļ�����ʧ��
    vos_open_file_error         = 0x0000002b, //���ļ�ʧ��
    vos_not_have_alarmstru_err  = 0x0000002c, //û�п��õĽṹ��
    vos_fauitidhas_in_team_error= 0x0000002d, //������ĳ��澯��������
    vos_load_dynamic_lib_error  = 0x0000002e, //���ض�̬��ʧ��
    vos_create_socket_error     = 0x0000002f, //����socketʧ��
    vos_socket_receive_error    = 0x00000030, //socket�������ݰ�����
    vos_socket_bind_error       = 0x00000031, //socket�󶨵�ַ����
    vos_socket_listen_error     = 0x00000032, //socket������������
    vos_socket_connect_error    = 0x00000033, //socket���Ӵ���
    vos_malloc_mutex_error      = 0x00000034, //����mutexʧ��
    vos_subfauitmap_full_err    = 0x00000035, //�澯�����ռ���ʹ�������
    vos_faultinfomap_full_err   = 0x00000036, //�澯������Ϣ�ﵽ���ֵ
    vos_no_alarm_err            = 0x00000037, //��ǰ�޸澯
    vos_isexistalarmnode_err    = 0x00000038, //�澯�ڵ��Ѵ���
    vos_existfaultinfo_err      = 0x00000039, //������Ϣ�Ѵ���
    vos_socket_accept_error     = 0x0000003a, //socket�ȴ��ͻ����������
    vos_noneedtrace_err         = 0x0000003b, //������Ϣ׷��
    vos_tracetimeout_err        = 0x0000003c, //׷�ٳ�ʱ
    vos_msgtypeoutline_err      = 0x0000003d, //��Ϣ���ͳ�������
    vos_trace_not_found_err     = 0x0000003e, //׷����Ϣ������
    vos_crt_basetimer_error     = 0x0000003f, //������׼��ʱ������
    vos_setcrt_basetimer_error  = 0x00000040, //Linux������׼��ʱ������
    vos_timer_not_find_error    = 0x00000041, //�Ҳ���ָ��ID�Ķ�ʱ��
    vos_no_timer_timername_err  = 0x00000042, //û��ָ�����ֵĶ�ʱ��
    vos_timername_repeat_err    = 0x00000043, //ͬ����ʱ������
    vos_toendofdir_error        = 0x00000044, //Ŀ¼��¼�Ѿ���ȡ��
    vos_unknown_msg_type_error  = 0x00000045, //δ֪��Ϣ����
    vos_reg_network_fault_error = 0x00000046, //ע���������ʧ��
    vos_ping_result_err         = 0x00000047, //pingʧ��
    vos_socket_send_error       = 0x00000048, //socket�������ݰ�����
    vos_wait_chkauth_tmout_error= 0x00000049, //�ȴ���Ȩ��ʱ����ʱ����
    vos_check_auth_failed_error = 0x0000004a, //��Ȩ���ʧ�ܴ���
    vos_reg_midmach_failed_error= 0x0000004b, //ע����λ��ʧ�ܴ���
    vos_malloc_mutex_faied_error= 0x0000004c, //�����ź���ʧ�ܴ���
    vos_read_file_error         = 0x0000004d, //���ļ�ʧ��
    vos_write_file_error        = 0x0000004e, //д�ļ�ʧ��
    vos_array_index_error       = 0x0000004f, //������������
    vos_get_curworkpath_error   = 0x00000050, //��ȡ��ǰ����·��ʧ��
    vos_init_thread_attr_error  = 0x00000051, //��ʼ���߳�����ʧ��
    vos_set_thread_detach_error = 0x00000052, //�����̷߳���ʧ��
    vos_set_thread_stack_error  = 0x00000053, //�����̶߳�ջ��Сʧ��
    vos_destory_thread_attr_error = 0x00000054, //�ݻ��߳�����ʧ��
    vos_get_threadpri_error       = 0x000000055, //�����߳����ȼ�ʧ��
    vos_timer_id_used_over_error  = 0x000000056, //��ʱid���������
    vos_timer_id_invalid_error    = 0x000000057, //��ʱid�Ƿ�����
    vos_sps_midmachip_uncfg_error = 0x000000058, //ָ����λ��IP��ַδ����
    vos_ansi_to_utf8_error        = 0x000000059, //ANSI��UTF8��ʽת��ʧ��
    vos_query_db_data_error       = 0x00000005a, //��ѯ���ݿ�ʧ��
    vos_objected_isregistered_error = 0x00000005b,//�����Ѿ�ע��
    vos_set_thread_policy_error   = 0x00000005c, //�����̵߳��Ȳ���ʧ��
}vos_errorcode_enum;

#endif
