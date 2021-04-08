//modifyed
#ifndef _CMD_REAGENTMIXING_H_
#define _CMD_REAGENTMIXING_H_

#include"vos_basictypedefine.h"

//�Լ����赥Ԫ����ö�ٶ���
typedef enum
{
    cmd_reagent_mix_ndl_min             = 0x0800, //�Լ������뵥Ԫ��С����
    cmd_reagent_mix_ndl_reset           = 0x0808, //�Լ������뵥Ԫ��λ����
    cmd_reagent_mix_ndl_vrise           = 0x0809, //�Լ������������˶�����
    cmd_reagent_mix_ndl_vdecline        = 0x080a, //�Լ������������˶�����
    cmd_reagent_mix_ndl_hrotate_washpos = 0x080d, //�Լ�������ˮƽ��ת����ϴλ����
    cmd_reagent_mix_ndl_hrotate_mixpos  = 0x080e, //�Լ�������ˮƽ��ת������λ����
    cmd_reagent_mix_ndl_exe_washing     = 0x0815, //�Լ�������ִ����ϴ����
    cmd_reagent_mix_ndl_exe_mixxing     = 0x0816, //�Լ�������ִ�н�������
	cmd_reagent_mix_ndl_slowdown        = 0x0818, //�Լ�������ָ���������ٶ��½�
    cmd_reagent_mix_pvctl_v11           = 0x0819, //�Լ�������÷���������˴�v11��ȷ�����޸ģ�����
    cmd_reagent_mix_fpga_update         = 0x0820, //�Լ�������fpga����
    
    cmd_reagent_mix_motor_hold_ctl      = 0x0850, //�Լ��������������ؿ���  
    cmd_reagent_mix_step_move           = 0x0851, //�Լ�����΢���ƶ�����

	cmd_reagent_mix_chip_reset          = 0x0870, //оƬ��λ
	cmd_reagent_mix_update              = 0x0880, //�Լ���������
    
    cmd_reagent_mix_compensation        = 0x08c0, //�Լ������벹������
    cmd_reagent_mix_ndl_max             = 0x08ff, //�Լ������뵥Ԫ�������
}cmd_reagent_mix_ndl_type_enum;

#endif
