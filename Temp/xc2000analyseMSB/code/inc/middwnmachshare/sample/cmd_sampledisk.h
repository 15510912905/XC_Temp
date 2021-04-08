//modifyed
#ifndef _CMD_SAMPLEDISK_H_
#define _CMD_SAMPLEDISK_H_

#include "vos_basictypedefine.h"

//�����̵�Ԫ����ö�ٶ���
typedef enum
{
    cmd_sample_dsk_min                  = 0x1c00, //��������С����
    cmd_sample_dsk_reset                = 0x1c08, //�����̸�λ����
    cmd_sample_dsk_anticlockwise_rtt    = 0x1c09, //��������ʱ����תx����λ����
    cmd_sample_dsk_clockwise_rtt        = 0x1c0a, //������˳ʱ����תx����λ����
    cmd_sample_dsk_spscup_rtt_to_spspos = 0x1c0b, //������ָ����λת��ָ��λ��
    cmd_sample_dsk_rtt_onecycle         = 0x1c0c, //��������ת1Ȧ
    cmd_sample_dsk_rtt_to_bcdscan       = 0x1c11, //������ָ����λת������ɨ��λ
	cmd_sample_dsk_fpga_update			= 0x1c20, //������FPAGA �������߸��¹���

    cmd_sample_dsk_motor_hold_ctl       = 0x1c50, //�����̵���������ؿ���  
    cmd_sample_dsk_step_move            = 0x1c51, //������΢���ƶ�����

	cmd_sample_dsk_chip_reset         	= 0x1c70, //оƬ��λ
	cmd_sample_dsk_show_wave_switch     = 0x1c71, //����ʾ��������
	cmd_sample_dsk_update           	= 0x1c80, //����������
    
    cmd_sample_dsk_compensation         = 0x1cc0, //�����̲�������
    cmd_sample_dsk_lightCtl             = 0x1cc1, //����ָʾ�ƿ���
    cmd_sample_dsk_max                  = 0x1cff, //�������������
}cmd_sample_dsk_type_enum;

//������Ȧid����
typedef enum
{
    cmd_sample_dsk_circle_1_id = 0,  //����Ȧ
    cmd_sample_dsk_circle_2_id = 1,
    cmd_sample_dsk_circle_3_id = 2,
    cmd_sample_dsk_circle_4_id = 3,
    //cmd_sample_dsk_circle_5_id = 4,  //����Ȧ

    cmd_sample_dsk_circle_butt
}cmd_sample_dsk_circle_enum;


typedef enum
{
	cmd_sample_dsk_circle_1_num = 54,  
	cmd_sample_dsk_circle_2_num = 108,
	cmd_sample_dsk_circle_3_num = 135,
	cmd_sample_dsk_circle_4_num = 162,
}cmd_sample_dsk_num_enum;
//������ ÿȦ�������





/*������ˮƽת��������ɨ��λ�������ݽṹ*/
typedef struct
{
    app_u16 cmd;
    char  uccircleid;  //������Ȧ�� (0xFF: 3Ȧͬʱɨ��)
    char  uccupid;     //�����̱���:(0:��Ȧɨ�����ٷ������ݣ�1-��󱭺�:����ָ����������)
}cmd_sample_dsk_bcdscan_stru;

#endif
