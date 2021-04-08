//modifyed
#ifndef __CMD_TEMPCONTROL_H__
#define __CMD_TEMPCONTROL_H__



//�¶ȿ��Ƶ�Ԫ����ö�ٶ���
typedef enum
{
    cmd_temp_control_min                    = 0x3800,   //�¶ȿ��Ƶ�Ԫ��С����
    cmd_temp_control_switch                 = 0x3801,   //�¶ȿ��ƿ��� 0-�ر� 1-��  
    cmd_temp_control_set_temp           	= 0x3804,   //����Ŀ���¶�
    cmd_temp_control_set_reacttemp          = 0x3805,   //���÷�Ӧ��Ŀ���¶�    
    cmd_temp_three_routes_ctl               = 0x3806,   //��·�¿ؿ��ؿ���
	cmd_temp_sleep							= 0x3807,	//��·�¿�����

	cmd_temp_control_chip_reset         	= 0x3870, //оƬ��λ
	cmd_temp_control_update             	= 0x3880, //�¿�����
	
    cmd_temp_control_max                    = 0x38ff,   //�¶ȿ��Ƶ�Ԫ�������
}cmd_tempcontrol_type_enum;

//�¶ȿ������ݽṹ
typedef struct {
    app_u16 cmd;
    app_u16 switchctrl;  //���ؿ��� 0-�ر� 1-��
}cmd_temp_ctrl_stru;

//����������
typedef struct {
    app_u16 cmd;
    char heaternum;     //���������(1-3��·��4��Ӧ�̣�5ȫ��)
    char switchctrl;    //���ؿ��� 0-�ر� 1-��
}cmd_heater_ctrl_stru;

//��Ӧ��Ŀ���¶�
typedef struct {
    app_u16 cmd;
    app_u16 delta;        
    vos_u32 tempreact;  //��Ӧ��Ŀ���¶�
}cmd_reactdisk_tempset_stru;

//��·��ϴ������Ŀ���¶�
typedef struct {
    app_u16 cmd;
    app_u16 temp1;     //������1Ŀ���¶�
    app_u16 temp2;     //������2Ŀ���¶�
    app_u16 temp3;     //������3Ŀ���¶�
}cmd_heater_targettemp_stru;

//-------------------�����ϱ�------------------------//

//�¿��ϱ���Ϣ�ṹ�壨ʵ���¶�ΪС������Ҫ����100����ȡ����
typedef struct {
    app_u16 cmd;
    app_u16 enstatustype; //�ϱ���������
    vos_u32 dwnmachname;  //��λ������
    app_i32 temp_reac1; //1-3�ֱ��Ӧ��Ӧ���¶�1����Ӧ���¶�2����Ӧ���¶�3
    app_i32 temp_reac2;
    app_i32 temp_reac3;
    app_i32 temp_env;          //�����¶�
    app_i32 temp_washmach;       //������ϴˮ�¶�
    app_i32 temp_washcup;        //��Ӧ����ϴˮ�¶�
    app_i32 temp_detergentcup;   //��Ӧ����ϴ��
    app_u16 duty_reaction;
    app_u16 duty_washmach;
    app_u16 duty_washcup;
    app_u16 duty_detergentcup;
}cmd_temp_control_report_stru;

typedef struct {
    app_u16 cmd;
    app_u16 enstatustype; //�ϱ���������
    vos_u32 dwnmachname;  //��λ������
    app_u16 tempreact;
    app_u16 tempwash1;
    app_u16 tempwash2;
    app_u16 tempwash3;
    app_u16 delta;
    app_u16 rsv;
}cmd_temp_control_eeprom_info_stru;

typedef struct {
    app_u16 cmd;
    app_u16 enstatustype; //�ϱ���������
    vos_u32 dwnmachname;  //��λ������
    app_u16 close_temp;
    app_u16 open_temp;
    app_u16 LoopPumpOpen_temp;
}cmd_temp_cooling_paraset_stru;
typedef struct {
	app_u16 cmd;
	app_u16 enstatustype; //�ϱ���������
	app_u16 dwnmachname;  //��λ������
	app_u16 state;
	float kp;
	float ki;
	float kd;
	float dsttmp;
}cmd_temp_pid_stru;

#endif
