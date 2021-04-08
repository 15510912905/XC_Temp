//modifyed
#ifndef _cmd_coolingreagent_h_
#define _cmd_coolingreagent_h_



//�¶ȿ��Ƶ�Ԫ����ö�ٶ���
typedef enum
{
    cmd_cooling_min             = 0x4000,  //���䵥Ԫ��С����
    cmd_cooling_debug           = 0x4001,  //�����ѯ����
    cmd_cooling_control		    = 0x4002,  //���俪�ؿ���,����0�����䣬1������
    cmd_cooling_settemp         = 0x4003,  //�����¶Ȳ���

	cmd_cooling_chip_reset      = 0x4070, //оƬ��λ
	cmd_cooling_update          = 0x4080, //оƬ��λ
	
    cmd_cooling_max             = 0x40ff,  //���䵥Ԫ�������
}cmd_cooling_type_enum;

//����������ݽṹ
typedef struct{
    app_u16 cmd;
    app_u16 switchctrl;  //���ؿ��� 0-�ر� 1-��
}cmd_cooling_debug_stru;

//����������ýṹ��
typedef struct{
    app_u16 cmd;
    app_u16 close_temp;       //�ر����������¶�
    app_u16 open_temp;        //�������������¶�
    app_u16 LoopPumpOpen_temp;//ѭ���ÿ��������¶�
}cmd_cooling_paraset_stru;

//--------------��ѯ------------------
//�ϱ��¶���ʾ�ṹ�壨ʵ���¶�ΪС������Ҫ����100����ȡ����
typedef struct {
    app_u16 cmd;
    app_u16 enstatustype; //�ϱ���������
    vos_u32 dwnmachname;  //��λ������
    app_i32 temp_liqiud;
    app_i32 temp_board;
}cmd_cooling_temp_stru;

//�ϱ�����״̬��ʾ�ṹ
typedef struct {
    app_u16 cmd;
    app_u16 enstatustype; //�ϱ���������
    vos_u32 dwnmachname;  //��λ������
    char status[8];   //��Ƿ���״̬������or�쳣
}cmd_cooling_fan_status_stru;

//�ϱ���ѹ��ʾ�ṹ�壨ʵ�ʵ�ѹΪС������Ҫ����100����ȡ����
typedef struct {
    app_u16 cmd;
    app_u16 enstatustype; //�ϱ���������
    vos_u32 dwnmachname;  //��λ������
    vos_u32 vol[16];
}cmd_cooling_vol_stru;

#endif
