#ifndef _app_inner_msg_h_
#define _app_inner_msg_h_

#include "vos_basictypedefine.h"
#include <stdio.h>
#include <drv_ebi.h>
#include "cmd_downmachine.h"
#include "typedefine.pb.h"


typedef struct
{
    app_u16 dwnName;
    app_u16 AdCount;
    app_u32 value[0];
}msg_ADValue;

//===========����ͷ�ṹ==========
typedef struct
{
	app_u16 usActType;
	app_u16 usRctCupId;
	app_u16 usDevType;
	app_u16 usSingleRes;
    app_u32 ulActIndex;
}msg_stActHead;

//===========��Դ�ƿ������ݽṹ==========
typedef struct {
    app_u16 lightIntensity;  //��ǿ�� 0~255���ȵ���
}msg_stLightControl;

//===========�¿ؿ������ݽṹ==========
typedef struct {
    app_u16 switchState;  //���ؿ��� 0-�ر� 1-��
}msg_stTempSwitch;

//��·��ϴ������Ŀ���¶�
typedef struct {
    vos_u32 temp1;     //������1Ŀ���¶�
    app_u16 temp2;     //������2Ŀ���¶�
    app_u16 temp3;     //������3Ŀ���¶�
}msg_stWTemp;

//��Ӧ��Ŀ���¶�
typedef struct {
    vos_u32 delta;
    vos_u32 tempreact;  //��Ӧ��Ŀ���¶�
}msg_stRTemp;

//===========����AD�������ݽṹ==========
typedef struct {
    vos_u32 dpValue[WAVE_NUM];  //ÿ��ͨ����Ӧ��AD����ϵ��
}msg_stAdConfig;

//===========���ϻָ�����ṹ==========
typedef struct {
    vos_u32 faultId;
    app_u16 boardId;
    app_u16 deviceId;
}msg_stRsmFault;

//===========��λ��״̬�·�����ṹ==========
typedef struct {
    app_u16 midState;
}msg_stMidState;

//===========��������ʾ�����������ݽṹ==========
typedef struct {
    char  channel;
	char  switch_state;
}msg_stShowave;

//===========�Լ�����ɨ�����ݽṹ==========
typedef struct
{
    app_u16 usCupId;    //�Լ�����
    char  aucrsv[2];
}msg_stReagBcScanPara;

//===========��������ɨ�����ݽṹ==========
typedef struct
{
    app_u16  uccircleid;  //������Ȧ�� (0xFF: 3Ȧͬʱɨ��)
    app_u16  uccupid;     //�����̱���:(0:��Ȧɨ�����ٷ������ݣ�1-��󱭺�:����ָ����������)
}msg_stSmpBcScanPara;

#ifdef Version_A
typedef struct
{
    app_u16 usCircleId;  //�����̱��
    app_u16 usSmpCupId; //���������
}msg_paraSampleDskRtt;
#else
//===========������ˮƽ��ת�������ݽṹ==========
typedef struct
{
    app_u16 usCircleId;  //�����̱��
    app_u16 usSmpCupId; //���������
    uint32_t channel;	//ͨ���� ,0 ���1 ����
}msg_paraSampleDskRtt;
#endif

//===========ע����ſ���������Ϣ==========
typedef struct
{
    app_u16 usAirOutTimes;
    char  aucrsv[2];
}msg_stAirout;

//===========�����롢�Լ���ת���������ݽṹ==========
typedef struct
{
    app_u16 usDstPos;   //Ŀ��λ��
    app_u16  aucRsv;
}msg_stNdlRtt;

//===========�������������ݽṹ==========
typedef struct
{
    char  enabsrbtype;    //��ȡ���ͣ�8��ȫѪ
    char  reportmarginflag; //�ϱ�������־��1:�ϱ���0:���ϱ�
	app_u16 usTotalVol;
    vos_u32 ulsamplevolume; //�������(��������ϡ�͹����뷴Ӧ����һ��,���ﲻ��
    app_u16 diskid;   //�̺ţ����������ϱ�
    app_u16 cupid;    //��λ�ţ����������ϱ�
    vos_u32 reactcup;   //����ţ�������졢©�촦��
}msg_paraSampleAbsrb;

//===========�������������ݽṹ==========
typedef struct
{
    app_u16 usdischpos;     //��������λ��(����������Ҫת��������λ�Ϸ�)
    app_u16 ustotalvol;     //�������
    vos_u32 ulsamplevolume; //�������
    vos_u32 uldlntvolume;   //ϡ��Һ���
}msg_paraSampleDischarge;

//===========������ѹ�ÿ����������ݽṹ==========
typedef struct
{
    char operateType;
	char rsv;
}msg_stCtrlP04;

//===========�������������ݽṹ==========
typedef struct
{
	app_u16 usDischPos;     //��������λ��(����������Ҫת��������λ�Ϸ�)
	app_u16 usTotalVol;     //�������
	vos_u32 ulSampleVolume; //�������
	vos_u32 ulDlntVolume;   //ϡ��Һ���
}msg_stSampleSysDischarge;

//===========�������������ݽṹ==========
typedef struct
{
	char  enAbsrbType;    //��ȡ���ͣ�8��ȫѪ
	char  reportMarginFlag; //�ϱ�������־��1:�ϱ���0:���ϱ�
	app_u16 usTotalVol;  //�������
	vos_u32 ulSampleVolume; //�������(��������ϡ�͹����뷴Ӧ����һ��,���ﲻ�� 	//ϡ��Һ�����)
	app_u16 diskId;   //�̺ţ����������ϱ�
	app_u16 cupId;    //��λ�ţ����������ϱ�
	vos_u32 reactCup;   //����ţ�������졢©�촦��
}msg_stSampleSysAbsorb;

//===========���Լ��������ݽṹ==========
typedef struct
{
	app_u16 usDischPos;  //��ȡ����Լ�����Ҫֹͣ��Ŀ��λ
	app_u16 usTotalVol;  //�������
	vos_u32 ulReagVolume; //���Լ����
	vos_u32 ulDlntVolume; //��ϡ��Һ���
}msg_stReagSysDischarge;

//===========���Լ��������ݽṹ==========
typedef struct
{
	app_u16 usAbsrbPos;   //���Լ�λ��(��Ȧ����Ȧ)
	char  enAbsrbType;  //��ȡ����(����ˮor�Լ�)
	char  reportMarginFlag; //�ϱ�������־��1:�ϱ���0:���ϱ�
	vos_u32 ulVolume;     //��ȡ���
	app_u16 diskId;   //�̺ţ����������ϱ�
	app_u16 cupId;    //��λ�ţ����������ϱ�
	vos_u32 reactCup;   //����ţ�������졢©�촦��
}msg_stReagSysAbsorb;

//===========����������Ϣ==========
typedef struct
{
	vos_u32 ulDataLen;  //�����������ݳ���
	msg_stActHead stActHead;
	char  aucPara[0]; //��������2�ֽ�ʱ����Ĳ���
}msg_stComDevActInfo;

//===========����������Ϣ  app_com_device_req_exe_act_msg_type==========
typedef struct
{
	app_u16 usActNum;
	app_u16 usReserve;
	char astActInfo[0];      //������Ϣ
}msg_stComDevActReq;

//===========��һ����==========
typedef struct
{
	app_u16 usActNum;
	app_u16 usReserve;
	vos_u32 ulDataLen;  //�����������ݳ���
	msg_stActHead stActHead;  //����ͷ
	char astActInfo[0];
}msg_stSingleActUni;

//===========���΢�����������������ݽṹ==========
typedef struct
{
    app_u16 motorType;  //�������
    app_u16 dir;        //����
    vos_u32 step;       //΢����
}msg_stStepMove;

//===========��λ�������������ݽṹ==========
//��������
typedef struct {
    app_u16 location;   //����λ��
    app_u16 dir;        //�������� 1: ˳ʱ�� 2:��ʱ��
    vos_u32 step;       //�������� 
}msg_stCmpPara;

//��������
typedef struct {
    vos_u16 type;     //��������  1: ��λ����  2: ��ת���� 3: �������л� 4: ��� 5:��ֱ����
    vos_u16 paranum;  //������������
    msg_stCmpPara pPara[0];  //�������� �䳤
}msg_stCompensation;

//��������
typedef struct {
    vos_u16 type;     //��������  ҳ��
    vos_u16 paranum;  //������������
    vos_u32 pPara[0]; //�������� �䳤
}msg_stSaveOffset;

//��������
typedef struct {
    unsigned char  drvId;
    unsigned char  lineId;
    app_u16 startFre;			//����Ƶ��
    app_u16 endFre;			//���Ƶ��
    app_u16  echLaderStep;		//ÿ̨��΢��
    app_u16  upMaxLader;		//������̨��
    app_u16  sPar;				//S�����ߵ���״����}motor_para;
}msg_stMotorPara;

//��������
typedef struct {
    vos_u32 paranum;  //������������
    msg_stMotorPara pPara[0];  //�������� �䳤
}msg_stMotor;


//===========��λ����ѯ�������ݽṹ==========
typedef struct
{
    vos_u32 queryType;     //��ѯ��״̬����
}msg_stDwnQuery;
//===========��λ����ѯ�������ݽṹ==========
typedef struct
{
    char num;
    char queryType;     //��ѯ��״̬����
}msg_st2000DwnQuery;


//===========��������ʱ��ת�����ݽṹ==========
typedef struct
{
    char dskId;     //������Ȧ��,(�Լ��ͷ�Ӧ�̲���)
    char dir;       //����
    vos_u16 num;    //ת����λ��     
}msg_stDskHrotate;

//===========�봹ֱ�˶��������ݽṹ==========
typedef struct{
    char dir;       //����,(���� ʱ��������ݲ���д)
    char marginFlag;//�ϱ�������־��1:�ϱ���0:���ϱ�
    app_u16 totalVol; //�������
    app_u16 diskId;     //�̺ţ����������ϱ�
    app_u16 cupId;      //��λ�ţ����������ϱ�
    vos_u32 reactCupId;   //����ţ�������졢©�촦��
}msg_stVrt;

//===========����-����̬�л�==========
typedef struct
{
    vos_u32 operation;    //��������̬-0:����̬��1:����̬
}msg_stMachineDebug;

/*typedef enum{
    Press_our = 2,  //��Ȧ��ѹ
    Press_in = 3,   //��Ȧ��ѹ
    p07 = 1,    //��Һ��
    p08 = 2,	//����ձ�
    p09 = 3,    //������
    v06 = 8,    //ϡ�ͷ�
    v07 = 9,    //S�����ϴ������ʹ��
    v08 = 10,   //R1�����ϴ������ʹ��
    v09 = 11,   //R2�����ϴ������ʹ��
    v10 = 13,   //�������������ϴ��������ʹ��
    v11 = 12,   //�Լ����������ϴ��������ʹ��
    v01 = 15,	//��ˮ��
    v02 = 18,   //S�ڱ���ϴ������ʹ��
    v03 = 16,   //R1�ڱ���ϴ������ʹ��
    v04 = 17,   //R2�ڱ���ϴ������ʹ��
    v05 = 19,   //������
    v14 = 20,   //һ��עҺ
    v15 = 21,   //����עҺ
    v16 = 22,   //����עҺ
    v17 = 23,   //�Ľ�עҺ
    v18 = 24,   //���עҺ
    v19 = 25,   //����עҺ
    v21 = 27,	//��Ũ����Һ��
    v22 = 28,	//��Ũ����Һ��
    v23 = 29,	//���������Һ��
    v25 = 31,   //�������������
    v27 = 33,   //�������Һ��
    v28 = 34,   //������
    p04 = 2,    //������ѹ�� 2000
    p011 = 56,  //ֱ����ˮ��
    p06 = 57,   //ϡ����ϴ����
    p05 = 58,   //Ũ����ϴ����
    p03 = 59,	//ȥ����ˮ��
}app_autowash_pvnum_enum;*/
typedef enum{
    Press_our = 2,  //��Ȧ��ѹ
    Press_in = 3,   //��Ȧ��ѹ
    p07 = 34,   //��Һ��
    p08 = 32,	//����ձ�
    p09 = 29,   //������

    v07 = 1,   //S1�����ϴ������ʹ��
    v32 = 1,   //S2�����ϴ��
    v08 = 1,   //R11�����ϴ������ʹ��
    v34 = 1,   //R12�����ϴ������ʹ��
    v09 = 1,   //R21�����ϴ������ʹ��
    v38 = 1,   //R22�����ϴ������ʹ��
    v10 = 1,   //�������������ϴ��������ʹ��
    v11 = 1,   //�Լ����������ϴ��������ʹ��
    v01 = 12,	//��ˮ��
    v02 = 2,   //S1�ڱ���ϴ������ʹ��
    v31 = 2,   //S2�ڱ���ϴ��
    v03 = 2,   //R11�ڱ���ϴ������ʹ��
    v35 = 2,   //R12�ڱ���ϴ������ʹ��
    v04 = 2,   //R21�ڱ���ϴ������ʹ��
    v37 = 2,   //R22�ڱ���ϴ������ʹ��
    v05 = 19,   //������
    v06 = 8,    //ϡ�ͷ�

    v14 = 13,   //��һ��עҺ
    v15 = 14,   //�ڶ���עҺ
    v16 = 15,   //������עҺ
    v17 = 16,   //���Ľ�עҺ
    v18 = 17,   //�����עҺ
    v19 = 18,   //������עҺ
    v40 = 19,   //��һ��עҺ
    v41 = 20,   //�����עҺ
    v42 = 21,   //������עҺ
    v43 = 22,   //���Ľ�עҺ
    v44 = 23,   //�����עҺ
    v45 = 24,   //������עҺ
    v21 = 9,	//��Ũ����Һ��
    v22 = 10,	//��Ũ����Һ��
    v23 = 26,	//���������Һ��
    v25 = 31,   //�������������
    v27 = 25,   //�������Һ��
    v46 = 2,   //ISE�����ڱڷ�
    v47 = 11,   //��Ũ����Һ��2
    v48 = 27,   //���������Һ��2
    v49 = 28,   //���������Һ��3
    v28 = 34,   //������

    p04 = 2,   //������ѹ��
    p12 = 3,   //������ѹ��2
    p13 = 35,   //������ձ�1
    p14 = 35,   //������ձ�2
    p15 = 83,   //Ũ����ϴ����ת��
    p16 = 33,   //����ձ�2
    p17 = 85,   //ISE�ڱ���ѹ��
    p18 = 86,   //ISE�����ϴ��
    p19 = 8,   //ϡ�ͱ�
    p011 = 6,  //����ֱ����ˮ��
    p06 = 5,   //ϡ����ϴ����
    p05 = 4,   //Ũ����ϴ����
    p03 = 30,	//ȥ����ˮ��
}app_autowash_pvnum_enum;

//===========��\�������������ݽṹ==========
typedef struct
{
    vos_u16 operateType;
	vos_u16 PVNum;
}msg_PVCtrl;

//===========ע��ÿ����������ݽṹ=========
typedef struct
{
    vos_u16 dir;
    vos_u16 volume;
}msg_PumpCtrl;

//===========����=========
typedef struct
{
    vos_u32 operation; //1:����0:��
}msg_switch;

//����ָʾ�ƿ���
typedef struct
{
    char    ctl;
    char    rsv[3];
}msg_sample_lightCtl;

//��λ���������ݴ���
typedef struct
{
    vos_u16 exeCmd;//ִ�ж���
    vos_u16 rsv;
    vos_u32 dataPage;//ҳ��
    char data[DWNPAGE_SIZE];//����
}msg_updateDwn;

//����FPGA
typedef struct
{
    vos_u16 flag;  //�Ƿ�Ϊ���һ֡���ݱ�־(�·�����λ��)
    vos_u16 dataPage;//��֡����ִ��״̬(����λ������)
    vos_u16 length;//data�ĳ���(ȡֵֻ��Ϊ0----256)
    vos_u16 rsv;
    char  data[DWNPAGE_SIZE]; //��������
}msg_updateFpga;
//Һ·����
typedef struct
{
    LiquidDBG_Type debugType;
}msg_liquidDBG;
typedef struct 
{
    vos_u16 state;
}msg_stateSwitch;


#define ACT

//===========��Ӧ��ˮƽ��ת�������ݽṹ==========
typedef struct
{
    app_u16 usRttedCupNum; //ת����λ��
    char dir;
    char aucRsv;
}act_stReactsysDskRtt;

//===========��Ӧ���Զ���ϴ�������ݽṹ==========
typedef struct
{
    app_u16 usWashOutBitMap; //��Ҫ��ϴ·λͼ(bit0:1��λ,bit9:10��λ),��ӦbitΪ1��������Ҫ��ϴ,Ϊ0������Ҫ��ϴ
    app_u16 usWashInBitMap2;
}act_stReactSysAutoWash;

//===========�Լ���ˮƽ��ת�������ݽṹ==========
typedef struct
{
    app_u16 usCupId;    //�Լ�����
    app_u16 usDstPos;   //Ŀ��λ��(���Լ�λ/���Լ�λ)
}act_stReagSysDskRtt;


typedef struct
{
    app_u32 channel;
}act_stReadOneAD;

typedef struct
{
    app_u32 AdCount;
}act_stReadAD;

typedef struct
{
    uint16_t channel;			/*ͨ����*/
    uint16_t gain;				/*����ֵ*/
}act_stGain;

typedef struct
{
    uint16_t channel;			/*ͨ����*//*ͨ����0 ���1 ����*/
    uint16_t place;             /*��ȡλ��*/
}act_stGetin;
/*�Զ�װ�ض�λ*/
typedef struct
{
    uint32_t  type;  /*ȡ/�ű�־*/
    uint32_t  place; /*Ŀ��λ��*/
}act_autoload_move;
typedef struct  
{
    vos_u16 cmd;
    uint8_t state;/*����״̬*/
}act_state_switch;
#endif
