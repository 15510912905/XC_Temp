#ifndef _app_down_msg_h_
#define _app_down_msg_h_


#include "cmd_downmachine.h"
#include <stdio.h>
#include <drv_ebi.h>
#include <stdint.h>
//===========��λ����ѯ�������ݽṹ==========
typedef struct
{
    app_u16 cmd;           //��ѯ����ؼ���:0xff09
    app_u16 queryType;     //��ѯ��״̬����
}cmd_stDwnQuery;
typedef struct
{
    app_u16 cmd;           //��ѯ����ؼ���:0x0009
    char  num;           //ҳ��
    char queryType;     //��д��־
}cmd_st2000DwnQuery;
//===========���ϻָ�����ṹ==========
typedef struct {
	app_u16 cmd;
    app_u16 rsv;  
    vos_u32 faultId;
    app_u16 boardId;
    app_u16 deviceId;
}cmd_stRsmFault;

//===========��λ��״̬�·�����ṹ==========
typedef struct {
	app_u16 cmd;
    app_u16 midState;
}cmd_stMidState;

//��������ʾ��������cmd���ݽṹ
typedef struct
{
    app_u16 cmd;
    char  channel;
	char  switch_state;
}cmd_stShowave;

//===========��λ�������������ݽṹ==========
//��������
typedef struct {
    app_u16 location;   //����λ��
    app_u16 dir;        //�������� 1: ˳ʱ�� 2:��ʱ��
    vos_u32 step;       //�������� 
}cmd_stCmpPara;

//��������
typedef struct {
    app_u16 cmd;      //����������
    app_u16 type;  //��������  1: ��λ����  2: ��ת���� 3: �������л� 4: ��� 5:��ֱ����
    vos_u32 paranum;  //������������
    cmd_stCmpPara *ppara;  //�������� �䳤
}cmd_stCompensation;

typedef struct
{
	app_u16 cmd;
	app_u16 flag;
}tempSleepStausStr;
//===========���΢�����������������ݽṹ==========
typedef struct
{
    app_u16 cmd;
    char    motorType;  //�������
    char    dir;        //����
    vos_u32 step;       //΢����
}cmd_stStepMove;

//===========��Ӧ��ˮƽת�������������ݽṹ==========
typedef struct
{
    app_u16 cmd;
    app_u16 rttCupNum;      //ת���ı�λ��
}msg_stReactDskHrotate;

//===========��Ӧ�̹�Դ�ƿ����������ݽṹ==========
typedef struct {
    app_u16 cmd;
    app_u16 lightIntensity;  //��ǿ�� 0~255���ȵ���
}cmd_stLightControl;

//===========�¿ؿ����������ݽṹ==========
typedef struct {
    app_u16 cmd;
    app_u16 switchState;  //���ؿ��� 0-�ر� 1-��
}cmd_stTempSwitch;

typedef struct {
    app_u16 cmd;
    app_u16 temp1;     //������1Ŀ���¶�
    app_u16 temp2;     //������2Ŀ���¶�
    app_u16 temp3;     //������3Ŀ���¶�
}cmd_stWTemp;

//��Ӧ��Ŀ���¶�
typedef struct {
    app_u16 cmd;
    app_u16 delta;
    vos_u32 tempreact;  //��Ӧ��Ŀ���¶�
}cmd_stRTem;

//===========����AD�������ݽṹ==========
typedef struct {
	app_u16 cmd;    //������
    app_u16 srv;    //�����ֽ�
    vos_u32 dpValue[WAVE_NUM];  //ÿ��ͨ����Ӧ��AD����ϵ��
}cmd_stAdConfig;

//===========������ˮƽת�������������ݽṹ==========
typedef struct
{
    app_u16 cmd;
    char  uccircleid;  //������Ȧ��
    char  uccupid;     //ָ���������Ż��ߴ�ת���ı�λ��
}msg_cmdSampleDskRtt;

//===========ע��������������ݽṹ==========
typedef struct
{
    app_u16 cmd;
    char  num;//ע��������������̵Ĵ���
    char  rsv;//Ԥ��
}msg_stNdlAirout;

//===========�Լ���ˮƽת�������������ݽṹ==========
typedef struct
{
    app_u16 cmd;
    char  ucPos;  //ת����λ��
    char  ucCupId;     //ָ�����Ż��ߴ�ת���ı�λ��
}cmd_stRDskMove;

//===========�Լ���������ʱ��ת�������������ݽṹ==========
typedef struct
{
    app_u16 cmd;
    char  cupNum;  //ת���ı�λ��
    char  rsv[1];  //ָ�����Ż��ߴ�ת���ı�λ��
}cmd_stRDskHrotate;

//===========�Լ���ת�������Լ�λ�����������ݽṹ==========
typedef struct
{
    app_u16 cmd;
    char  cupId;  //ת�����Լ�λ�ı�λ��
    char  rsv[1];  
}cmd_stRDskSwappos;

typedef struct
{
    app_u16 cmd;
    app_u16 cupNum;  //ת���ı�λ��
}cmd_stReactDskHrotate;

//===========������/�Լ���������or����ˮ�����������ݽṹ==========
typedef struct{
    app_u16 cmd;
    app_u16 quantity;//��������
}msg_stNdlAbsrb;

//===========�봹ֱת���������ݽṹ==========
typedef struct{
    app_u16 cmd;
    char  enposition; //�˶�����Ŀ��λ��
    char  reportmarginflag;//�ϱ�������־��1:�ϱ���0:���ϱ�
    app_u16 ustotalvol; //�������������������
    char unCheckLiquid;//����Һ���⣬1������
    char  rsv;
    app_u16 diskid;   //�̺ţ����������ϱ�
    app_u16 cupid;    //��λ�ţ����������ϱ�
    vos_u32 reactcup;   //����ţ�������졢©�촦��
}cmd_stNdlVrt;

//===========�����������������������ݽṹ==========
typedef struct{
    app_u16 cmd;
    app_u16 quantity;  //���Լ���
    char  diluteflag;
    char  rsv[3];
}msg_cmdSampleNdlDischarge;

//===========�Զ���ϴ�������ݽṹ==========
typedef struct
{
    app_u16 cmd;
    app_u16 washBitMap;
}msg_stAutoWash;

//===========��\�������������ݽṹ==========
typedef struct
{
    app_u16 cmd;
    char PVNum;
    char operateType;   //����
}cmd_PVCtrl;

//===========��\�������������ݽṹ==========
typedef struct
{
    app_u16 cmd;
    char operateType;   //����
    char rsv[1];
}cmd_stPVCtrl;

//===========��һ�������ݽṹ==========
typedef struct{
    app_u16 cmd;
    char rsv[2];
}msg_stOneCmd;

//===========�Լ���X��λ��ת������ɨ��λ==========
typedef struct
{
    app_u16 cmd;
    char  uccupnum; //ĳ��λת�����Լ�λ
    char  rsv[1];
}msg_stReagdskRttToBcscan;

//===========������X��λ��ת������ɨ��λ==========
typedef struct
{
    app_u16 cmd;
    char  uccircleid;  //������Ȧ�� (0xFF: 3Ȧͬʱɨ��)
    char  uccupid;     //�����̱���:(0:��Ȧɨ�����ٷ������ݣ�1-��󱭺�:����ָ����������)
}msg_stSmpdskRttToBcscan;

//===========��ˮƽת���������ݽṹ==========
typedef struct{
	app_u16 cmd;
	char  enPosition;  //�˶�����Ŀ��λ��
	char  rsv[1];
}msg_stNdlHrotate;

//===========������λ��ͨ�õ�������������ݽṹ==========
typedef struct
{
	vos_u32 ulCmdSize;            //�����ڴ��С
	char  aucCmdLoad[0];     //����� 
}msg_stComCombSingleCmd;

//===========������λ��ͨ������������ݽṹ==========
typedef struct
{
	app_u16 cmd;
	app_u16 cmdNum;               //��������
	char  aucCmdLoad[0];         //�����
}msg_stComDwnmachCombCmd;

//===========��λ������vos��Ϣ�ṹ����==========
typedef struct
{
	app_u16 enDwnmachSubSys;  //��λ��ϵͳ��(��Ӧ�̡��Լ��̡������̡��Լ�����ϵͳ)
	app_u16 usDevType;
	char  aucCmdLoad[0];     //����� 
}msg_stDwnmachActionReq;

//===========���崮�ڹ���ģ������λ����λ����������ĵ�VOS��Ϣ�ṹ==========
typedef struct
{
    char  endwnmachname;
    char  enmsgtrantype;
    app_u16 uscmdinfolen;
    vos_u32 ulcurcmdframeid;
    
    app_u16 usCmdType;
    char  enExeRslt;      //����ִ�н��
    char  ucRsv[1];
    char aucinfo[0];
}msg_stMnguart2Downmach;

//===========��λ��ִ�н��vos��Ϣ����==========
typedef struct
{
    app_u16 enDwnmachSubSys;  //��λ��ϵͳ��(��Ӧ�̡��Լ��̡������̡��Լ�����ϵͳ)
    app_u16 usDevType;
    app_u16 enCmdType;         //��������
    app_u16 enCmdExeRslt;      //����ִ�н��
    vos_u32 ulPayLoadLen;      //���س���
    char aucPayLoad[0];      //������Ϣ
}msg_stDwnmachActionMsg;

typedef struct
{
    char  endwnmachname;
    char  enmsgtrantype;
    app_u16 uscmdinfolen;
    vos_u32 ulcurcmdframeid;

    app_u16 usCmdType;
    app_u16 enStatusType; //�ϱ���������
    app_u32 shelfNum;
}msg_stDwnmachShelfReprot;

/*R1�Լ������Լ�����ṹ*/
typedef struct{
    app_u16 cmd;
    app_u16 quantity;  //���Լ���
    char  diluteflag;
    char  rsv[3];
}msg_stR1Discharge;


//---------------��λ������-����̬�л�--------------------
typedef struct
{
    app_u16 cmd;        //����ؼ���0xff0a
    app_u16 operation;  //�л�0:����̬��1:����̬
}cmd_stMachineDebug;


//===========����=========
typedef struct
{
    vos_u16 cmd;
    app_u16 operation; //1:����0:��
}cmd_switch;


typedef enum
{
    samp_key_led_off = 0,
    samp_key_led_on = 1,
    samp_key_led_flicker = 2,
    samp_key_led_butt,
}samp_key_led_value;

//����ָʾ�ƿ���
typedef struct
{
    vos_u16 cmd;
    char    ctl;
    char    rsv;
}cmd_sample_lightCtl;

//��λ���������ݴ���
typedef struct
{
    vos_u16 cmd;
    vos_u16 exeCmd;//ִ�ж���
    vos_u32 dataPage;//ҳ��
    char data[DWNPAGE_SIZE];//����
}cmd_updateDwn;

//����FPGA
typedef struct
{
    vos_u16 cmd;
    vos_u16 flag;  //�Ƿ�Ϊ���һ֡���ݱ�־(�·�����λ��)
    vos_u16 dataPage;//��֡����ִ��״̬(����λ������)
    vos_u16 length;//data�ĳ���(ȡֵֻ��Ϊ0----256)
    char  data[DWNPAGE_SIZE]; //��������
}cmd_updateFpga;


#define CMDTYPE
typedef enum {
    CMD_NDL_RESET = 0x0011,		        /*�븴λ*/
    CMD_NDL_UP = 0x0012,		        /*������*/
    CMD_NDL_DOWN = 0x0013,		        /*���½�*/
    CMD_NDL_TURN = 0x0014,		        /*����ת*/
    CMD_NDL_SAMPLE = 0x0015,		    /*�����*/
    CMD_NDL_BLOWOFF = 0x0016,		    /*������*/
    CMD_NDL_WASH = 0x0017,		        /*����ϴ*/
    CMD_NDL_WATER_UPTAKE = 0x0018,	    /*����ˮ*/
    CMD_NDL_WASH_VALVE_CONTROL = 0x0019,/*����ϴ������*/
    CMD_NDL_AIR_OUT = 0x001a,		    /*������*/
} CMD_NDL_ENUM;

typedef enum {
    CMD_DSK_RESET = 0x0053,		/*�̸�λ*/
    CMD_DSK_TURN = 0x0054,		/*����ת*/
} CMD_DSK_ENUM;

typedef enum {
    CMD_AUTOWASH_RESET = 0x0081,	 /*�Զ���ϴ��λ*/
    CMD_AUTOWASH_MOVE = 0x0082,		/*�Զ���ϴ����*/
    CMD_AUTOWASH = 0x0083,		    /*�Զ���ϴ*/
    CMD_AUTOWASH_PVCONTROL = 0x0084,/*�����÷�����*/
} CMD_AUTOWASH_ENUM;

typedef enum {
    CMD_STIR_RESET = 0x0021,		/*���踴λ*/
    CMD_STIR_UPDOWN = 0x0022,		/*��������*/
    CMD_STIR_TURN = 0x0023,		    /*����ת��*/
    CMD_STIR_WASH = 0x0024,		    /*������ϴ*/
    CMD_STIR = 0x0025,		        /*����*/
    CMD_STIR_PVCONTROL = 0x0026,    /*����÷�����*/
} CMD_STIR_ENUM;

typedef enum {
    CMD_ORBITRESET = 0x0040,    /*ģ�鸴λ*/
    CMD_PUSHIN = 0x0041,        /*����*/
    CMD_TRANSFER = 0x0042,      /*����*/
    CMD_PUSHOUT = 0x0043,       /*�˼�*/
    CMD_TRANSFERFREE = 0x0044,  /*�����ͷ�*/
    CMD_TRANSFER_STEP = 0x0045, /*����Nλ*/
} CMD_TRANSFER_ORBIT_ENUM;

typedef enum {
    CMD_READ_ONE_AD = 0x00B5,	/*��һ��ADֵ*/
    CMD_READ_AD = 0x00B6,		/*�������*/
    CMD_SET_GAIN = 0x00B7,		/*��������*/
    CMD_READ_ALL_AD = 0x00B8,		/*��ƽ̹������*/
} CMD_AD_ENUM;

#ifdef Version_A
typedef enum {
    CMD_ResetShelf = 0x0090,
    CMD_GetShelf = 0x0091,	    /*��һ��ADֵ*/
    CMD_MoveShelf = 0x0092,		/*�������*/
    CMD_FreeShelf = 0x0093,		/*��������*/
} CMD_Gway_ENUM;
#else
typedef enum {
    CMD_ORBIT_RESET = 0x0090,		/*ģ�鸴λ*/
    CMD_GET = 0x0091,		        /*��ȡ*/
    CMD_LOCATING = 0x0092,		    /*��λ*/
    CMD_FREE = 0x0093,		        /*�ͷų�*/
    CMD_FREEIN = 0x0098,				/*�ͷ���*/
    CMD_GET_IN = 0x0099,			/*����*/
} CMD_ORBIT_ENUM;
#endif

typedef enum {
    CMD_READ_TEMPERATURE = 0x00BA,      /*��ȡ�¶�ֵ*/
    CMD_TEMPERATURE_SWITCH = 0x00BB,      /*�¿ؿ��أ�Ĭ�Ϲر�*/
} CMD_REACTION_TEMPERATURE_CONTROL_ENUM;

typedef enum {
    CMD_COM_RW_PARAMETER_PAGE = 0x0009,//��д��λ������
}CMD_COM_RW_PARAMETER_PAGE_ENUM;

typedef enum {
    CMD_SET_LIGHT = 0x00BC,	/*���ù�Դ�Ʋ���*/
} CMD_LIGHT_SOURCE_ENUM;
/*�Զ�װ����λ��ͨ��*/
typedef enum{
    CMD_AUTO_LOAD_VG_RE = 0x0040,
    CMD_AUTO_LOAD_TSPT_RE = 0x0041,
    CMD_AUTO_LOAD_TSPT_MOVE = 0x0042,
    CMD_AUTO_LOAD_VG_MOVE=0x0043,
}CMD_AUTO_LOAD_DOWN;

#define  CMDSTRUCT

/*������*/
typedef struct {
    uint16_t cmd;				/*������*/
    uint8_t rsv[2];
} CMD_Single_TYPE;

/*�븴λ*/
typedef struct {
    uint16_t cmd;				/*������*/
    uint16_t dis_wash;			/*����ϴ��־λ*/
    uint32_t time;				/*��ϴʱ��*/
} CMD_NDL_RESET_TYPE;

/*������*/
typedef struct {
    uint16_t cmd;				/*������*/
    uint8_t  rsv[2];
} CMD_NDL_UP_TYPE;

/*���½�*/
typedef struct {
    uint16_t cmd;				/*������*/
    uint8_t en_liquid_level_measuring;	/*ʹ��Һ�����־λ*/
    uint8_t en_reporting_margin;	/*ʹ�������ϱ���־λ*/
    uint16_t total_volume;	/*�����*/
    uint16_t ring_number;	/*Ȧ��*/
    uint16_t place_number;	/*λ��*/
    uint16_t cup_number;	/*��Ӧ����*/
} CMD_NDL_DOWN_TYPE;

/*��ת��*/
typedef struct {
    uint16_t cmd;				/*������*/
    uint8_t  place;				/*Ŀ��λ��*/
    uint8_t  rsv;
} CMD_NDL_TURN_TYPE;

/*������*/
typedef struct {
    uint16_t cmd;				/*������*/
    uint16_t size;				/*������*/
} CMD_NDL_SAMPLE_TYPE;

/*������*/
typedef struct {
    uint16_t cmd;				/*������*/
    uint16_t size;				/*������*/
} CMD_NDL_BLOWOFF_TYPE;

/*����ϴ*/
typedef struct {
    uint16_t cmd;				/*������*/
    uint16_t type;				/*��ϴ��ʽ*/
    uint32_t time;				/*��ϴʱ��*/
} CMD_NDL_WASH_TYPE;

/*����ˮ*/
typedef struct {
    uint16_t cmd;				/*������*/
    uint16_t size;				/*��ˮ��*/
} CMD_NDL_WATER_UPTAKE_TYPE;

/*����ϴ������*/
typedef struct {
    uint16_t cmd;				/*������*/
    uint8_t number;				/*�����*/
    uint8_t status;				/*״̬*/
    uint32_t appedn[2];
} CMD_NDL_WASH_VALVE_CONTROL_TYPE;

/*������*/
typedef struct {
    uint16_t cmd;				/*������*/
    uint16_t number;			/*����*/
} CMD_NDL_AIR_OUT_TYPE;


/*�̸�λ*/
typedef struct {
    uint16_t cmd;				/*������*/
    uint8_t rsv[2];
} CMD_DSK_RESET_TYPE;

/*��ת��*/
typedef struct {
    uint16_t cmd;				/*������*/
    uint16_t type;				/*�˶�����*/
    uint16_t cup_number;		/*λ�Ż���*/
    uint16_t place;				/*λ��*/
} CMD_DSK_TURN_TYPE;

//���Ӷ�λ
typedef struct {
    uint16_t cmd;			/*������*/
    uint8_t currloca;	    /*��ǰλ��*/
    uint8_t destloca;	    /*Ŀ��λ��*/
}CMD_SHELF_LOCA_TYPE;

/*���踴λ*/
typedef struct {
    uint16_t cmd;				/*������*/
    uint16_t dis_wash;			/*����ϴ��־λ*/
    uint32_t time;				/*��ϴʱ��*/
} CMD_STIR_RESET_TYPE;

/*��������*/
typedef struct {
    uint16_t cmd;				/*������*/
    uint8_t place;				/*λ�ñ��*/
    uint8_t rsv;
} CMD_STIR_UPDOWN_TYPE;

/*����ת��*/
typedef struct {
    uint16_t cmd;				/*������*/
    uint8_t  place;				/*λ�ñ��*/
    uint8_t  rsv;
} CMD_STIR_TURN_TYPE;

/*������ϴ*/
typedef struct {
    uint16_t cmd;				/*������*/
    uint16_t dis_wash;			/*��ֹ��ϴ��־*/
    uint32_t time;				/*����/��ϴʱ��*/
} CMD_STIR_WASH_TYPE;

/*����*/
typedef struct {
    uint16_t cmd;				/*������*/
    uint16_t  dis_wash;			/*��ֹ��ϴ��־*/
    uint32_t  time;				/*����/��ϴʱ��*/
} CMD_STIR_TYPE;

/*����÷�����*/
typedef struct {
    uint16_t cmd;				/*������*/
    uint8_t  number;			/*�÷����*/
    uint8_t  status;			/*����״̬*/
} CMD_STIR_PVCONTROL_TYPE;


/*�Զ���ϴ��λ*/
typedef struct {
    uint16_t cmd;				/*������*/
    uint8_t rsv[2];
} CMD_AUTOWASH_RESET_TYPE;

/*�Զ���ϴ����*/
typedef struct {
    uint16_t cmd;				/*������*/
    uint16_t place;				/*λ�ñ��*/
} CMD_AUTOWASH_MOVE_TYPE;

/*�Զ���ϴ*/
typedef struct {
    uint16_t cmd;				/*������*/
    uint8_t listOut;			/*��Ȧλ�ű�*/
    uint8_t listIn;				/*��Ȧλ�ű�*/
} CMD_AUTOWASH_TYPE;

/*�Զ���ϴ�����÷�����*/
typedef struct {
    uint16_t cmd;				/*������*/
    uint8_t number;				/*�÷����*/
    uint8_t status;				/*����״̬*/
} CMD_AUTOWASH_PVCONTROL_TYPE;

/*��һ��ADֵ*/
typedef struct {
    uint16_t cmd;				/*������*/
    uint8_t channel;			/*ͨ����*/
    uint8_t rsv;
} CMD_READ_ONE_AD_TYPE;

/*�������*/
typedef struct {
    uint16_t cmd;				/*������*/
    uint16_t AdCount;				/*������*/
} CMD_READ_AD_TYPE;

/*��������*/
typedef struct {
    uint16_t cmd;				/*������*/
    uint16_t channel;			/*ͨ����*/
    uint32_t gain;				/*����ֵ*/
} CMD_SET_GAIN_TYPE;

/*��λ*/
typedef struct {
    uint16_t cmd;            /*������*/
    uint8_t rsv[2];
} CMD_ORBITRESET_TYPE;

/*����*/
typedef struct {
    uint16_t cmd;            /*������*/
    uint8_t rsv[2];
} CMD_PUSHIN_TYPE;

/*����Nλ*/
typedef struct {
    uint16_t cmd;            /*������*/
    uint8_t number;            /*Ҫ�����Ĵ���*/
    uint8_t rsv;
} CMD_TRANSFER_STEP_TYPE;

/*����һ��*/
typedef struct {
    uint16_t cmd;            /*������*/
    uint8_t number;          /*��������*/
    uint8_t rsv;
} CMD_TRANSFER_TYPE;

/*�˼�*/
typedef struct {
    uint16_t cmd;            /*������*/
    uint8_t rsv[2];
} CMD_PUSHOUT_TYPE;

/*�ͷ�*/
typedef struct {
    uint16_t cmd;            /*������*/
    uint8_t rsv[2];
} CMD_TRANSFERFREE_TYPE;

/*��ȡ��ϴˮ�¶�ֵ*/
typedef struct {
    uint16_t cmd;            /*������*/
    uint16_t channel;         /*ͨ���ţ�1 ��������2�¶ȣ�2 ��������1�¶ȣ�3 ��ϴ�������¶ȣ� 4 ��ϴˮ�����¶�*/
} CMD_READ_TEMP_TYPE;

/*��ϴˮ�¿ؿ���*/
typedef struct {
    uint16_t cmd;            /*������*/
    uint16_t mode;            /*0 �رգ�1����*/
} CMD_TEMP_SWITCH_TYPE;

/*��д����ҳ*/
typedef struct {
    uint16_t cmd;            /*������*/
    uint8_t  number;         /*ҳ��*/
    uint8_t  rw;             /*��дѡ��*/
    uint8_t  datas[0];       /*����ֵ*/
}CMD_COMMON_DATA_RW_PAGE_DATA_TYPE;

/*���ù�Դ�Ʋ���*/
typedef struct {
    uint16_t cmd;	/*������*/
    uint16_t voltage;	/*��ѹֵ*/
} CMD_SET_LIGHT_TYPE;

/*ģ�鸴λ*/
typedef struct {
    uint16_t cmd;				/*������*/
    uint8_t rsv[2];
} CMD_ORBIT_RESET_TYPE;

/*��ȡ*/
typedef struct {
    uint16_t cmd;				/*������*/
    uint16_t channel;			/*ͨ����0 ���1 ����*/
    uint16_t  place;             /*��ȡλ��*/
} CMD_GET_TYPE;

/*��λ*/
typedef struct {
    uint16_t cmd;				/*������*/
    uint8_t currloca;			/*����*/
    uint8_t destloca;			/*Ŀ��λ��*/
    uint16_t channel;			/*ͨ����0 ���1 ����*/
}CMD_LOCATING_TYPE;

/*�ͷ�*/
typedef struct {
    uint16_t cmd;				/*������*/
    uint16_t channel;			/*ͨ����0 ���1 ����*/
}CMD_FREE_TYPE;

/*����*/
typedef struct {
    uint16_t cmd;				/*������*/
    uint16_t channel;			/*ͨ����0 ���1 ����*/
}CMD_GET_IN_TYPE;
/*�Լ����Զ�װ��ת�˻����ƶ�λ��*/
typedef struct
{
    uint16_t cmd;    /*������*/
    uint8_t  number; /*λ�ñ��*/
}CMD_MODULE_MOVE_POSITION_TYPE;
/*�Զ�װ�ش�ֱץ��ģ���˶�*/
typedef struct
{
    uint16_t cmd;   /*������*/
    uint8_t  type;  /*ȡ/�ű�־*/
    uint8_t  place; /*Ŀ��λ��*/
}CMD_AUTOLOAD_MOTZMOVEPOSITION_TYPE;
#endif