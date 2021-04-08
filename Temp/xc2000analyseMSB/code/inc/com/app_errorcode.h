//checked
#ifndef _app_errorcode_h_
#define _app_errorcode_h_

#include "vos_errorcode.h"

//�������ֵ����
typedef enum
{
    app_resource_schedule_errorcode_base = 0x00010000, //��Դ����ģ��������ֵ
    app_reactsys_mng_errorcode_base      = 0x00020000, //��Ӧ�̹���ģ��������ֵ
    app_samplesys_mng_errorcode_base     = 0x00030000, //�����̹���ģ��������ֵ
    app_reagentsys_mng_errorcode_base    = 0x00040000, //�Լ��̹���ģ��������ֵ
    app_testtask_mng_errorcode_base      = 0x00050000, //�����������ģ��������ֵ
    app_command_errorcode_base           = 0x00060000, //����ģ��
    app_maintain_errorcode_base          = 0x00070000, //ά��ģ��
    app_middwnmach_mng_errorcode_base    = 0x00080000, //����λ���������ģ��������ֵ
    app_mnguart_errorcode_base           = 0x00090000, //���ڹ���ģ��������ֵ
    app_mng_exeact_statemach_errorcode_base = 0x000a0000, //ִ��״̬������ģ��������ֵ
    app_debugtty_errorcode_base          = 0x000b0000, //���Դ���ģ��������ֵ
    app_commonsys_mng_errorcode_base     = 0x000c0000, //����ģ��������ֵ
    app_oam_mng_errorcode_base           = 0x000d0000, //OAM����ģ��������ֵ
    app_barcodescan_errorcode_base       = 0x000e0000, //����ɨ��ģ��������ֵ
    app_report_dispose_errorcode_base    = 0x000f0000, //״̬�ϱ�����ģ����󷢻�ֵ
}app_errorcode_base_enum;

typedef enum
{
    app_para_null_err                  = 0x00000001, //��ָ��
    app_open_file_error                = 0x00000002, //���ļ�����
    app_unknown_msg_pid_error          = 0x00000003, //δ֪��Ϣpid����
    app_unknown_timer_error            = 0x00000003, //δ֪��ʱ������
    app_inexistence_action_err         = 0x00000004, //�����ڵĶ�������
    app_cmd_retrans_out_error          = 0x00000005, //�������Դ�������
    app_cmd_crcunequal_error           = 0x00000006, //����crcУ�鲻һ��
    app_cmd_unknowncmd_error           = 0x00000007, //δ֪����id
    app_unknown_subsystem_err          = 0x00000008, //δ֪��ϵͳ����
    app_open_uart_dev_err              = 0x00000009, //�򿪴����豸�ļ�ʧ��
    app_unknown_msg_type_err           = 0x0000000a, //δ֪��Ϣ���ʹ���
    app_unknown_dwnmach_err            = 0x0000000b, //δ֪��λ�����ʹ���
    app_malloc_mutex_err               = 0x0000000c, //�����ź���ʧ�ܴ���
    app_mem_size_zero_err              = 0x0000000d, //�ڴ泤��Ϊ�����
    app_uart_handle_null_err           = 0x0000000e, //���ھ߱�Ϊ�մ���
    app_write_uart_handle_err          = 0x0000000f, //д���ڴ���
    app_full_exeact_statemach_err      = 0x00000010, //ִ��״̬��������
    app_exeact_statemach_nucreated_err = 0x00000011, //ִ��״̬��δ��������
    app_input_para_invalid_err         = 0x00000012, //�����������
    app_original_act_too_many_err      = 0x00000013, //ԭʼ����̫�����
    app_ctrl_statemach_busy_err        = 0x00000014, //����״̬��æ����
    app_req_func_act_invalid_err       = 0x00000015, //����Ĺ��ܶ����Ƿ�
    app_statemach_stop_rcving_dwnmach_err = 0x00000016, //״̬��ֹͣ��������յ���λ����Ӧ��Ϣ����
    app_statemach_no_sonact_err        = 0x00000017, //״̬����û���Ӷ�������
    app_invalid_uart_id_err            = 0x00000018, //��Ч�Ĵ��ں�
    app_unknown_device_type            = 0x00000019, //δ֪�豸���ʹ���
    app_invalid_extdev_name_err        = 0x0000001a, //��Ч���ⲿ�豸���ƴ���
    app_invalid_extdev_operatemode_err = 0x0000001b, //��Ч���ⲿ�豸����ģʽ
    app_stack_full_err                 = 0x0000001c, //��ջ������
    app_invalid_command_info_len_err   = 0x0000001d, //command��Ϣ���ȴ���
    app_unknown_command_id_err         = 0x0000001e, //command�Ŵ���
    app_ipaddr_unmatch_err             = 0x0000001f, //��λ���·���λ��IP��ƥ�����
    app_unknown_rpt_status_type_err    = 0x00000020, //δ֪��λ���ϱ�״̬���ʹ���
}app_errorcode_enum;

#endif