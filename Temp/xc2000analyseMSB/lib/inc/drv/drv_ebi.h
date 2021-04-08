#ifndef __EBI_DRV_H
#define __EBI_DRV_H

#define EBI_FPGA_TRANS_MODE 8     //�����߼��������ݵķ�ʽ  0--����  1--�ػ�
#define EBI_APP_MODE        9			//Ӧ�ó���������ͨ�ŵ�ģʽ 0--����  1--�ػ�
#define EBI_APP_BAUD        10    //Ӧ�ó������������봮�ڵĲ����� 
#define AD_FILTER           20		//����AD�ɼ����˲�ϵ��
#define AD_START            30		//����AD�ɼ� 1--���� 0--ֹͣ
#define AD_SET_DP_VALUE     40		//�������� ��Χ0~255
#define AD_SEL_CHANNEL      50		//ѡ��ͨ�� ��Ӧ14������
#define AD_JUDGE_DP_DONE    60		//��ѯ�����������óɹ�
#define AD_MODE             70		//����AD�ɼ���ģʽ 0--���� 1---����
#define PRINT_SWITCH        80    //������ӡ��Ϣ����� 0-�ر� 1--��ӡ
#define FPGA_PROGRAM_SWITCH	90    //���¼����߼����� 0 -�ر� 1-����
#define FPGA_PROGRAM_DONE	100   //��ѯ�߼����¼������޳ɹ�
#define FPGA_UART_RESET_L	4   	//uart 0-15��ʼ��
#define FPGA_UART_RESET_H	5   	//uart 16-23��ʼ��

#define EBI_FPGA_BASE    		0x50000000  //FPGA�������ַ 0x50000000
#define A5_PIN_PORTA_BASE   0xFFFFF200  //ATMEL PIOA����ַ
#define A5_PIN_PORTE_BASE   0xFFFFFA00  //ATMEL PIOE����ַ

#define GPIO_FPGA_EXINT1  	AT91_PIN_PB16  //AD�ж�
#define GPIO_FPGA_EXINT2    AT91_PIN_PE30 //���ڷ����ж�
#define GPIO_FPGA_EXINT3    AT91_PIN_PE31 //���ڽ����ж�

#define HSMC2_SETUP    0xFFFFC628            //a5 hsmc�����Ĵ�����ַ
#define HSMC2_PULSE    0xFFFFC62C            //a5 hsmc����Ĵ�����ַ
#define HSMC2_CYCLE    0xFFFFC630            //a5 hsmc���ڼĴ�����ַ
#define HSMC2_MODE     0xFFFFC638            //a5 hsmcģʽ�Ĵ�����ַ
#define PIOB_ISR       0xFFFFF44C            //a5 PB���ж�״̬�Ĵ�����ַ
#define PIOE_ISR       0xFFFFFA4C            //a5 PE���ж�״̬�Ĵ�����ַ

#define BUFFSIZE 2048
#define READBUFFSIZE 2048
#define WRITEBUFFSIZE 2048
#define READADBUFF 4096
#define UART_NUM  22        //21·���� ���AD
#define WAVE_NUM 14        //��������
#define ABS_NUM_MAX 41     //����ȵ�������
#define SHM_OFFSET 0x20		//�����ڴ��ƫ�Ƶ�ַ

#define DPVALUE_FILENAME ("dpvalue.txt")

#define LIGHTINTENSITY_OFF  0   
#define LIGHTINTENSITY_REST 100   
#define LIGHTINTENSITY_ON   120

//���ڲ����ʶ���
typedef enum
{
	drv_ebi_aurt_baud_rate_2400   = 2400,
	drv_ebi_aurt_baud_rate_4800   = 4800,
	drv_ebi_aurt_baud_rate_9600   = 9600,
	drv_ebi_aurt_baud_rate_19200  = 19200,
	drv_ebi_aurt_baud_rate_38400  = 38400,
	drv_ebi_aurt_baud_rate_57600  = 57600,
	drv_ebi_aurt_baud_rate_76800  = 76800,
	drv_ebi_aurt_baud_rate_115200 = 115200
}drv_ebi_aurt_baud_rate_enum;
typedef unsigned long drv_ebi_aurt_baud_rate_enum_u32;


typedef struct
{
	int advalue[WAVE_NUM]; //ÿ��������Ӧ��ADֵ ����˳�� 570...340
} XC8002_MIDMACH_DRV_AD_VALUE;

typedef struct 
{
	int absorbancenum; //����ȸ���
	XC8002_MIDMACH_DRV_AD_VALUE absorbance[0];  //��������飬�䳤
} XC8002_MIDMACH_DRV_ABS_VALUE;

#endif
