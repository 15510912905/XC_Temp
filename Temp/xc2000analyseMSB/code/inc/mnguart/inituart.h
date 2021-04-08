#ifndef __INITUART_H__
#define __INITUART_H__

#include <map>

#include"xc8002_middwnmachshare.h"
#include "vos_pid.h"

using namespace std;

//定义串口管理模块与中位机下位机命令交互中心的VOS消息负载结构
typedef struct
{
    xc8002_middwnmach_msg_header;
    app_u16 uscmdtype;
    char  aucrsv[2];
    char aucinfo[0];
}app_mnguart_middwnmach_cmd_msgload_stru;

typedef struct
{
    app_u16 sci_num;//串口号
    app_u16 device_type;
    vos_u32 reg_addr;//查询寄存器地址
    
}app_linkcheck_stru;

#define APP_USED_UART_NUM (14)
#define APP_MAX_UART_RDBUF_LEN   (256)

vos_u32 app_linkcheck(app_linkcheck_stru* pstdatebuf);
vos_u32 InitUart();
vos_u32 GetAbsValueFromShareMem(vos_u32 *pabsnum, vos_u32 *pbuf, vos_u32 maxlen);
void setAdMode(vos_u32 admode);
vos_u32 setDpValue(vos_u32 * dpValue);
vos_u32 writeDpValueToFile(vos_u32 * dpValue);
vos_u32 readDpValueFromFile(vos_u32 * dpValue);

#endif
