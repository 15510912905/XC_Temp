//modifyed
#ifndef _CMD_INNERREAGENTDISK_H_
#define _CMD_INNERREAGENTDISK_H_

#include"vos_basictypedefine.h"

//��Ȧ�Լ��̵�Ԫ����ö�ٶ���
typedef enum
{
    cmd_inner_reagentdisk_min               = 0x2800, //��Ȧ�Լ��̵�Ԫ��С����
    cmd_inner_reagentdisk_reset             = 0x2808, //��Ȧ�Լ��̸�λ����
    cmd_inner_reagentdisk_anticlockwise_rtt = 0x2809, //��Ȧ�Լ�����ʱ����תx����λ����
    cmd_inner_reagentdisk_clockwise_rtt     = 0x280a, //��Ȧ�Լ���˳ʱ����תx����λ����
    cmd_inner_reagentdisk_rtt_to_absbpos    = 0x280b, //��Ȧ�Լ���x��λ��ת��ָ�����Լ�λ
    cmd_inner_reagentdisk_rtt_to_swappos    = 0x280c, //��Ȧ�Լ���x��λ��ת�����Լ�λ
    cmd_inner_reagentdisk_rtt_to_bcdscan    = 0x2811, //��Ȧ�Լ���x��λ��ת������ɨ��λ
    cmd_inner_reagentdisk_rtt_onecycle      = 0x2812, //��Ȧ�Լ�����תһȦ

    cmd_inner_reagentdisk_motor_hold_ctl    = 0x2850, //��Ȧ�Լ��̵���������ؿ���  
    cmd_inner_reagentdisk_step_move         = 0x2851, //��Ȧ�Լ��̲��ƶ�����

	cmd_inner_reagentdisk_chip_reset        = 0x2870, //оƬ��λ
	cmd_inner_reagentdisk_show_wave_switch  = 0x2871, //����ʾ��������
	cmd_inner_reagentdisk_update            = 0x2880, //��Ȧ�Լ�������

    cmd_inner_reagentdisk_compensation      = 0x28c0, //��Ȧ�Լ��̲�������
    cmd_inner_reagentdisk_comb_cmd          = 0x28cc, //��Ȧ�Լ����������
    cmd_inner_reagentdisk_max               = 0x28ff, //��Ȧ�Լ��̵�Ԫ�������
}cmd_inner_reagentdisk_type_enum;

/*��Ȧ�Լ������Լ�λ�ö���*/
typedef enum
{
    cmd_inner_reagentdisk_r1_absbpos = 0,  //��Ȧ�Լ���R1�Լ������Լ�λ
    cmd_inner_reagentdisk_r2_absbpos = 1,  //��Ȧ�Լ���R2�Լ������Լ�λ

    cmd_inner_reagentdisk_absbpos_butt
}cmd_inner_reagentdisk_absbpos_enum;

#endif
