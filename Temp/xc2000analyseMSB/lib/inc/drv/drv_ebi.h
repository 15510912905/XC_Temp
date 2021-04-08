#ifndef __EBI_DRV_H
#define __EBI_DRV_H

#define EBI_FPGA_TRANS_MODE 8     //设置逻辑传输数据的方式  0--正常  1--回环
#define EBI_APP_MODE        9			//应用程序与驱动通信的模式 0--正常  1--回环
#define EBI_APP_BAUD        10    //应用程序向驱动传入串口的波特率 
#define AD_FILTER           20		//设置AD采集的滤波系数
#define AD_START            30		//启动AD采集 1--启动 0--停止
#define AD_SET_DP_VALUE     40		//设置增益 范围0~255
#define AD_SEL_CHANNEL      50		//选择通道 对应14个波长
#define AD_JUDGE_DP_DONE    60		//查询增益有无设置成功
#define AD_MODE             70		//设置AD采集的模式 0--正常 1---测试
#define PRINT_SWITCH        80    //驱动打印信息的输出 0-关闭 1--打印
#define FPGA_PROGRAM_SWITCH	90    //重新加载逻辑程序 0 -关闭 1-加载
#define FPGA_PROGRAM_DONE	100   //查询逻辑重新加载有无成功
#define FPGA_UART_RESET_L	4   	//uart 0-15初始化
#define FPGA_UART_RESET_H	5   	//uart 16-23初始化

#define EBI_FPGA_BASE    		0x50000000  //FPGA物理基地址 0x50000000
#define A5_PIN_PORTA_BASE   0xFFFFF200  //ATMEL PIOA基地址
#define A5_PIN_PORTE_BASE   0xFFFFFA00  //ATMEL PIOE基地址

#define GPIO_FPGA_EXINT1  	AT91_PIN_PB16  //AD中断
#define GPIO_FPGA_EXINT2    AT91_PIN_PE30 //串口发送中断
#define GPIO_FPGA_EXINT3    AT91_PIN_PE31 //串口接收中断

#define HSMC2_SETUP    0xFFFFC628            //a5 hsmc建立寄存器地址
#define HSMC2_PULSE    0xFFFFC62C            //a5 hsmc脉宽寄存器地址
#define HSMC2_CYCLE    0xFFFFC630            //a5 hsmc周期寄存器地址
#define HSMC2_MODE     0xFFFFC638            //a5 hsmc模式寄存器地址
#define PIOB_ISR       0xFFFFF44C            //a5 PB口中断状态寄存器地址
#define PIOE_ISR       0xFFFFFA4C            //a5 PE口中断状态寄存器地址

#define BUFFSIZE 2048
#define READBUFFSIZE 2048
#define WRITEBUFFSIZE 2048
#define READADBUFF 4096
#define UART_NUM  22        //21路串口 外加AD
#define WAVE_NUM 14        //波长个数
#define ABS_NUM_MAX 41     //吸光度的最大个数
#define SHM_OFFSET 0x20		//共享内存的偏移地址

#define DPVALUE_FILENAME ("dpvalue.txt")

#define LIGHTINTENSITY_OFF  0   
#define LIGHTINTENSITY_REST 100   
#define LIGHTINTENSITY_ON   120

//串口波特率定义
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
	int advalue[WAVE_NUM]; //每个波长对应的AD值 排列顺序 570...340
} XC8002_MIDMACH_DRV_AD_VALUE;

typedef struct 
{
	int absorbancenum; //吸光度个数
	XC8002_MIDMACH_DRV_AD_VALUE absorbance[0];  //吸光度数组，变长
} XC8002_MIDMACH_DRV_ABS_VALUE;

#endif
