//modifyed
#ifndef _CMD_OUTERREAGENTDISK_H_
#define _CMD_OUTERREAGENTDISK_H_

#include"vos_basictypedefine.h"

//��Ȧ�Լ��̵�Ԫ����ö�ٶ���
typedef enum
{
    cmd_outer_reagentdisk_min               = 0x3400, //��Ȧ�Լ��̵�Ԫ��С����
    cmd_outer_reagentdisk_reset             = 0x3408, //��Ȧ�Լ��̸�λ����
    cmd_outer_reagentdisk_anticlockwise_rtt = 0x3409, //��Ȧ�Լ�����ʱ����תx����λ����
    cmd_outer_reagentdisk_clockwise_rtt     = 0x340a, //��Ȧ�Լ���˳ʱ����תx����λ����
    cmd_outer_reagentdisk_rtt_to_absbpos    = 0x340b, //��Ȧ�Լ���x��λ��ת��ָ�����Լ�λ
    cmd_outer_reagentdisk_rtt_to_swappos    = 0x340c, //��Ȧ�Լ���x��λ��ת�����Լ�λ
    cmd_outer_reagentdisk_rtt_to_bcdscan    = 0x3411, //��Ȧ�Լ���x��λ��ת������ɨ��λ
    cmd_outer_reagentdisk_rtt_onecycle      = 0x3412, //��Ȧ�Լ�����תһȦ
    cmd_outer_reagentdisk_fpga_update    	= 0x3420, //��Ȧ�Լ���FPGA �������߸��¹���
    
    cmd_outer_reagentdisk_motor_hold_ctl    = 0x3450, //��Ȧ�Լ��̵���������ؿ���  
    cmd_outer_reagentdisk_step_move         = 0x3451, //��Ȧ�Լ��̲��ƶ�����

	cmd_outer_reagentdisk_chip_reset        = 0x3470, //оƬ��λ
	cmd_outer_reagentdisk_show_wave_switch  = 0x3471, //����ʾ��������
	cmd_outer_reagentdisk_update            = 0x3480, //��Ȧ�Լ�������
    
    cmd_outer_reagentdisk_compensation      = 0x34c0, //��Ȧ�Լ��̲�������
    cmd_outer_reagentdisk_comb_cmd          = 0x34cc, //��Ȧ�Լ����������
    cmd_outer_reagentdisk_max               = 0x34ff,  //��Ȧ�Լ��̵�Ԫ�������
}cmd_outer_reagentdisk_type_enum;

/*��Ȧ�Լ������Լ�λ�ö���*/
typedef enum
{
    cmd_outer_reagentdisk_r1_absbpos = 0,  //��Ȧ�Լ���R1�Լ������Լ�λ
    cmd_outer_reagentdisk_r2_absbpos = 1,  //��Ȧ�Լ���R2�Լ������Լ�λ

    cmd_outer_reagentdisk_absbpos_butt
}cmd_outer_reagentdisk_absbpos_enum;
typedef char cmd_outer_reagentdisk_absbpos_enum_u8;

/*��Ȧ�Լ���X��λ��ת������ɨ��λ*/
typedef struct
{
    app_u16 cmd;
    char  uccupnum; //ĳ��λת�����Լ�λ
    char  rsv[1];
}cmd_outer_reagdsk_rtt_to_bcdscan_stru;

#endif
