#include "vos_adaptsys.h"
#include "vos_mngmemory.h"
#include "app_errorcode.h"
#include "app_msgtypedefine.h"
#include "app_msg.h"
#include "xc8002_middwnmachshare.h"
#include "uart.h"
#include "inituart.h"
#include "app_downmsg.h"
#include "drv_ebi.h"
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <errno.h>
#include <sys/mman.h>
#include <fcntl.h> 
#include <signal.h>
#include <sys/ioctl.h>
#include <semaphore.h>
#include "app_xc8002shareinfo.h"
#include "app_comstructdefine.h"
//#include "zconf.h"
#include "apputils.h"
#include "vos_log.h"
#include "adsimulate.h"
vos_u32 ulThrdId;
volatile vos_u32* g_pSharemem=VOS_NULL;
map<app_u16, app_u16> g_uartHdl;
map<app_u16, app_u16> uartMapDev;
map<app_u16, app_u16> m_appWaveMap;
map<app_u16, app_u16> m_dpWaveMap;
map<app_u16, app_u16> m_fpgaWaveMap;
vos_u32 g_ultestmode = TESTMODE_NORMAL;
sem_t g_uartsem;//信号量 

#define CLEAR_VALUE_L 0XFFFF
#define CLEAR_VALUE_H 0X00FF

#ifdef COMPILER_IS_ARM_LINUX_GCC 
XC8002_MIDMACH_DRV_ABS_VALUE *drv_ebi_abs_value_pstr;  //共享内存吸光度地址
XC8002_MIDMACH_DRV_AD_VALUE  *drv_ebi_ad_value_pstr;   //共享内存AD地址
#endif

vos_u32 app_linkcheck(app_linkcheck_stru* pstdatebuf)
{
    app_i32 uarthdl = g_uartHdl[xc8002_link_debug];
    write(uarthdl, (char*)pstdatebuf, sizeof(app_linkcheck_stru));    
    return VOS_OK;
}


vos_u32 app_readlinkdate()
{
    vos_u32 ulrdedchrnum = 0;
    vos_u32 ulrunrslt = VOS_OK;
    vos_u32 ulrdtotallen = 0;
    char* pucnewbuf = NULL;
    char* pucrdsvbuf = NULL;
    char  aucrdbuf[APP_MAX_UART_RDBUF_LEN] = { 0 };
    char* printbuf = NULL;
    vos_u32 printloop = 0;

    ulrdtotallen = 0;
    while (1)
    {
        memset((char*)aucrdbuf, 0x00, APP_MAX_UART_RDBUF_LEN);

        ulrdedchrnum = 0;
        ulrdedchrnum = read(g_uartHdl[xc8002_link_debug], (char*)aucrdbuf, APP_MAX_UART_RDBUF_LEN);

        if (((0 == ulrdedchrnum) || (APP_INVALID_U32 == ulrdedchrnum)) && (0 == ulrdtotallen))
        {//本次读取没有数据
            printf("\n data is over !\n");
            DELETE_MEMEORY(pucrdsvbuf)
                break;
        }

        if ((ulrdedchrnum < APP_MAX_UART_RDBUF_LEN) && (0 == ulrdtotallen))
        {//一次将串口buffer中的数据读取完毕,向上层提交数据
            printf("UART:Readed out1 (%lu) bytes \n", ulrdedchrnum);
            printbuf = aucrdbuf;
            for (printloop = 0; printloop < ulrdedchrnum; printloop++)
            {
                printf("%02x ", *printbuf);
                printbuf++;
            }
            printf("\n");
            printbuf = NULL;

            DELETE_MEMEORY(pucrdsvbuf)
                break;
        }

        if ((0 == ulrdedchrnum) || (APP_INVALID_U32 == ulrdedchrnum))
        {//串口数据读取完毕,向上层提交数据
            printf("UART:Readed out2 (%lu) bytes \n", ulrdtotallen);
            printbuf = pucrdsvbuf;
            for (printloop = 0; printloop < ulrdtotallen; printloop++)
            {
                printf("%2x ", *printbuf);
                printbuf++;
            }
            printf("\n");
            printbuf = NULL;

            DELETE_MEMEORY(pucrdsvbuf)
                break;
        }

        ulrdtotallen += ulrdedchrnum;

        ulrunrslt = vos_mallocmemory(ulrdtotallen, vos_mem_type_static,
            (void**)(&pucnewbuf));
        if ((VOS_OK != ulrunrslt) || (NULL == pucnewbuf))
        {
            WRITE_ERR_LOG("Call vos_mallocmemory() failed(0x%x) \n", ulrunrslt);
            DELETE_MEMEORY(pucrdsvbuf)
                break;
        }

        if (NULL != pucrdsvbuf)
        {
            memcpy(pucnewbuf, pucrdsvbuf, (ulrdtotallen - ulrdedchrnum));
            DELETE_MEMEORY(pucrdsvbuf)
        }

        memcpy((pucnewbuf + (ulrdtotallen - ulrdedchrnum)), (char*)aucrdbuf, ulrdedchrnum);

        pucrdsvbuf = pucnewbuf;
        pucnewbuf = NULL;
    }

    return VOS_OK;
}

vos_u32 GetFpgaAdValuePos(vos_u32 apppos)
{
    vos_u32 ulresult = 0;
    vos_u32 ulindex =  0;

    if (apppos >= WAVE_NUM)
    {
        WRITE_INFO_LOG("[Error]apppos = %d\n", (int)apppos);
        return VOS_INVALID_U32;
    }

    for (ulindex = 0; ulindex < WAVE_NUM; ulindex++)
    {
        if (m_appWaveMap[apppos] == m_fpgaWaveMap[ulindex])
        {
            break;
        }
    }
    if (ulindex >= WAVE_NUM)
    {
        ulresult = VOS_INVALID_U32;
    }
    else
    {
        ulresult = ulindex;
    }

    return ulresult;
}

vos_u32 GetAbsValueFromShareMem(vos_u32 *pabsnum, vos_u32 *pbuf, vos_u32 maxlen)
{
    vos_u32 ullen = 0;  //实际使用长度

    vos_u32 ulindex = 0; //索引
    vos_u32 ularrindex = 0;
    vos_u32 ulfpgapos = 0;

    XC8002_MIDMACH_DRV_AD_VALUE stadvalue;
    memset(&stadvalue, 0, sizeof(XC8002_MIDMACH_DRV_AD_VALUE));

#ifdef COMPILER_IS_LINUX_GCC
    vos_u32 arrdebugaddata1[WAVE_NUM] = 
    {   
        0x7500,0x7501,0x7502,0x7503,0x7504,0x7505,0x7506,
		0x7507,0x7508,0x7509,0x750a,0x750b,0x750c,0x750d
    };

    vos_u32 arrdebugaddata2[WAVE_NUM] = 
    {   
        1201710,1091503,2101483,901465,911466,921470,601465,801463,
        991466,651467,891469,968564,2228542,518556
    }; 
#endif    
    
    XC8002_MIDMACH_DRV_AD_VALUE *pfpgaadvalue = VOS_NULL;
    XC8002_MIDMACH_DRV_AD_VALUE *pappadvalue = VOS_NULL;
    vos_u32 uloffset = 0;

    //参数判断
    if ((NULL == pabsnum) || (NULL == pbuf))
    {
        WRITE_ERR_LOG("[Error] pabsnum = NULL or ppbuf = NULL. \n");
        return APP_FALSE;
    }
    
#ifdef COMPILER_IS_LINUX_GCC
    vos_u32 *pdebug = VOS_NULL;
    //吸光度个数
	if(g_ultestmode == TESTMODE_DARKCURRENT)
	{
		*pabsnum = 1;  //测试模式
	}
	else
    {
    	*pabsnum = 42;  //正常模式
    }

	if(g_ultestmode == TESTMODE_SMOOTH)
	{
		pdebug = arrdebugaddata1;
	}
	else
	{
    	pdebug = arrdebugaddata2;
	}

    //构造吸光度buffer
    for (ulindex = 0; ulindex < 14; ulindex++)
    {
        stadvalue.advalue[ulindex] = pdebug[ulindex];
    }

    //吸光度长度
    ullen = (*pabsnum)*sizeof(XC8002_MIDMACH_DRV_AD_VALUE);
#else
    //获取共享内存对应地址
    drv_ebi_abs_value_pstr = (XC8002_MIDMACH_DRV_ABS_VALUE*)((char *)g_pSharemem + SHM_OFFSET);
    drv_ebi_ad_value_pstr  = (XC8002_MIDMACH_DRV_AD_VALUE *)((char *)drv_ebi_abs_value_pstr 
        + sizeof(vos_u32));

    //吸光度个数
    *pabsnum = *((vos_u32*)drv_ebi_abs_value_pstr);    

    //吸光度长度
    ullen = (*pabsnum) * sizeof(XC8002_MIDMACH_DRV_AD_VALUE);
#endif 
    
    if (ullen > maxlen)  //这里加这个判断是为了防止调用分配内存过少造成内存越界的情况
    {
        WRITE_ERR_LOG("[Error] ullen[%d] > maxlen[%d]", (int)ullen, (int)maxlen);
        return APP_FALSE;
    }

    //因为逻辑上报的AD值并不是按照从小到大的顺序上报的，因此这里需要转化为从小到大的顺序
    app_print(ps_ADval, "AD ARRAY: num = %d\r\n", (int)(*pabsnum));
    app_print(ps_ADval, "AD ARRAY: ullen = %d\r\n", (int)ullen);
    for (ulindex = 0; ulindex < *pabsnum; ulindex++)
    {
        uloffset = ulindex * sizeof(XC8002_MIDMACH_DRV_AD_VALUE);
#ifdef COMPILER_IS_LINUX_GCC    
        pfpgaadvalue = (XC8002_MIDMACH_DRV_AD_VALUE *)((char*)&stadvalue);
#else
        pfpgaadvalue = (XC8002_MIDMACH_DRV_AD_VALUE *)((char*)drv_ebi_ad_value_pstr + uloffset);
#endif
        pappadvalue = (XC8002_MIDMACH_DRV_AD_VALUE *)((char*)pbuf + uloffset);

        app_print(ps_ADval, "FPGA AD: \r\n");
        for (ularrindex = 0; ularrindex < WAVE_NUM; ularrindex++)
        {
            ulfpgapos = GetFpgaAdValuePos(ularrindex);
            if (VOS_INVALID_U32 == ulfpgapos)
            {
                WRITE_ERR_LOG("[Error] get ulsrcpos fail, dstpos = %d, srcpos = %d. \n",(int)ularrindex, (int)ulfpgapos);
                return APP_FALSE;
            }
            if(g_ultestmode == TESTMODE_SMOOTH)
			{
#ifdef COMPILER_IS_LINUX_GCC  
				if(ulindex == 0 && ularrindex == 0)
				{
					pappadvalue->advalue[ularrindex] = (int)(pfpgaadvalue->advalue[ularrindex] + ((1<<31) & 0x80000000));
				}
				else if(ulindex >= 20)
				{
					pappadvalue->advalue[ularrindex] = (int)((pfpgaadvalue->advalue[ularrindex] >> 8) & 0xff);
				}
				else
				{
					pappadvalue->advalue[ularrindex] = (int)(pfpgaadvalue->advalue[ularrindex]);
				}
#else
				pappadvalue->advalue[ularrindex] = (int)(pfpgaadvalue->advalue[ularrindex]);
#endif
			}
            else
            {
#ifdef COMPILER_IS_LINUX_GCC  
                static vos_u32 loop = 0;
                pappadvalue->advalue[ularrindex] = g_pADSimulate->GetAdValue(loop, 0, ularrindex + 1);
                loop++;
#else
				pappadvalue->advalue[ularrindex] = (int)(pfpgaadvalue->advalue[ulfpgapos]/10000);
#endif
            }
        }
        
        for (ularrindex = 0; ularrindex < WAVE_NUM; ularrindex++)
        {
            app_print(ps_ADval, "  advalue[%d] = %u\r\n", ularrindex, ((pappadvalue->advalue[ularrindex])));
        }
    }
    return VOS_OK;
}

vos_u32 CalcChecksumForPacket(xc8002_uart_prot_data_format_stru* pstpack)
{
    vos_u32 ulchksum   = 0;
    char* pucmv      = VOS_NULL;
    vos_u32 ulloopvar  = 0;
    vos_u32 ulupperthd = 0;

    if (VOS_NULL == pstpack)
    {
        WRITE_ERR_LOG("The input parameter is null \n");
        return 0;
    }
    
    ulupperthd = pstpack->usdatalen + sizeof(pstpack->ushead)
               + sizeof(pstpack->usdatalen);
    pucmv = (char*)pstpack;

    for (ulloopvar = 0; ulupperthd > ulloopvar; ulloopvar++)
    {
        ulchksum += (char)(*(pucmv));
        pucmv    += 1;
    }

    /*将数据缓存中checksum部分的值减去*/
    pucmv = (char*)&(pstpack->uschksum);

    ulchksum -= (char)(*(pucmv++));
    ulchksum -= (char)(*(pucmv));

    pucmv = VOS_NULL;
    
    return ulchksum;
}

vos_u32 SendToDwn(xc8002_middwnmach_com_msgtran_stru * pMsg)
{
	vos_u32 ulrunrslt = VOS_OK;
	vos_u32 ulprotpcklen = 0;
	vos_u32 uldatabuflen = 0;
	app_i32 uarthdl      = APP_NULL_HANDLE;
	
	xc8002_uart_prot_data_format_stru*  pstfmtdtbuf = VOS_NULL;
    xc8002_middwnmach_com_msgtran_stru* pstcmdinfo  = VOS_NULL;

	ulprotpcklen = sizeof(xc8002_uart_prot_data_format_stru) + sizeof(app_msg_payload_stru) + pMsg->uscmdinfolen;
	uldatabuflen = sizeof(app_msg_payload_stru) + pMsg->uscmdinfolen;
	uarthdl = g_uartHdl[pMsg->endwnmachname];
	
	ulrunrslt = vos_mallocmemory(ulprotpcklen, vos_mem_type_static,
                                 (void **)(&pstfmtdtbuf));
    if (VOS_OK != ulrunrslt)
    {
        WRITE_ERR_LOG("Call vos_mallocmemory() failed(%d) \n", ulrunrslt);
        return ulrunrslt;
    }
    
    pstcmdinfo = (xc8002_middwnmach_com_msgtran_stru*)&(pMsg->endwnmachname);
    
    pstfmtdtbuf->ushead    = XC8002_UART_FMT_HEAD;
    pstfmtdtbuf->usdatalen = xc8002_calcprotldlenbydtlen(uldatabuflen);
    pstfmtdtbuf->uscpuid   = xc8002_cmdkey_mapto_cpuid(pstcmdinfo->uscmdtype);

    memcpy(((char*)pstfmtdtbuf)+sizeof(xc8002_uart_prot_data_format_stru),
           (char*)&(pMsg->endwnmachname), uldatabuflen);

    pstfmtdtbuf->uschksum = CalcChecksumForPacket(pstfmtdtbuf);

	ulrunrslt = write(uarthdl, (char*)pstfmtdtbuf, ulprotpcklen);

	DELETE_MEMEORY(pstfmtdtbuf);
	return VOS_OK;
}

vos_u32 SendMsgUp(char* pmsgbuf, vos_u32 ulbuflen)
{
    vos_u32 ulvosmsglen = ulbuflen;
    vos_u32 ulrunrslt = VOS_OK;

    if (0 != (ulvosmsglen % 4))//消息四字节对齐
    {
        ulvosmsglen += (4 - (ulvosmsglen % 4));
    }
    msg_stMnguart2Downmach* pstmsgbuf = VOS_NULL;
    ulrunrslt = app_mallocBuffer(ulvosmsglen, (void**)(&pstmsgbuf));
    if (VOS_OK != ulrunrslt)
    {
        WRITE_ERR_LOG("Call app_mallocBuffer() failed(%d) \n", ulrunrslt);
        return ulrunrslt;
    }
    memcpy((char*)(&(pstmsgbuf->endwnmachname)), pmsgbuf, ulbuflen);

    msg_stHead rHead;
    app_constructMsgHead(&rHead, Pid_DwnMachUart, Pid_DwnMachUart, app_res_dwnmach_act_cmd_msg_type);

    return app_sendMsg(&rHead, pstmsgbuf, ulvosmsglen);
}

vos_u32 UnpackProtocolData(char* pucdatabuf, vos_u32 uldatalen)
{
	WRITE_INFO_LOG("uart datalen = %d\n",uldatalen);
    vos_u32 ulprocedlen = 0;
    vos_u32 ulrunrslt = VOS_OK;
	vos_u32 ulchksum = 0;
    
    xc8002_uart_prot_data_format_stru* pstprotdt = VOS_NULL;
    xc8002_middwnmach_com_msgtran_stru* pstcmdinfo = VOS_NULL ;
    
    if ((VOS_NULL == pucdatabuf) || (0 == uldatalen))
    {
        WRITE_ERR_LOG("The input parameter is null(memaddr= 0x%x,length= %d) \n",
                   (vos_u32)pucdatabuf, uldatalen);
        return ulrunrslt;
    }

    pstprotdt = (xc8002_uart_prot_data_format_stru*)pucdatabuf;
    
    ulprocedlen = 0;
    ulrunrslt = vos_mallocmemory(sizeof(xc8002_middwnmach_com_msgtran_stru), vos_mem_type_static,
                                (void **)(&pstcmdinfo));
    if (VOS_OK != ulrunrslt)
    {
        WRITE_ERR_LOG("Call vos_mallocmemory failed(0x%x) \n", ulrunrslt);
        return ulrunrslt;
    }
    while (ulprocedlen < uldatalen)
    {
        pstprotdt = (xc8002_uart_prot_data_format_stru*)(pucdatabuf + ulprocedlen);
        memcpy(pstcmdinfo, (void*)&(pstprotdt->aucdata[0]), sizeof(xc8002_middwnmach_com_msgtran_stru));
        if ((uldatalen - ulprocedlen) < xc8002_calcprotpcklenbyldlen(pstprotdt->usdatalen))
        {
            fflush(stdout);
            WRITE_ERR_LOG("The uart received invalid data(cmd= 0x%x,datalen= %d,packlen= %d) \n",
                       pstprotdt->ushead, pstprotdt->usdatalen, (uldatalen - ulprocedlen));
            break;
        }
        ulchksum = CalcChecksumForPacket(pstprotdt);
        if (ulchksum != ((vos_u32)(pstprotdt->uschksum)))
        {   
			if(pstcmdinfo->enmsgtrantype == xc8002_req_msgtran_type)
            {
				pstcmdinfo->enmsgtrantype = xc8002_nak_msgtran_type;
    			SendToDwn((xc8002_middwnmach_com_msgtran_stru *)&(pstcmdinfo->endwnmachname));
			}
            WRITE_ERR_LOG("Check sum don't pass(old chksum= %d, new chksum= %d) \n",
                       pstprotdt->uschksum, ulchksum);
            break;
        }
		else 
		{
			if(pstcmdinfo->enmsgtrantype == xc8002_req_msgtran_type)
            {
				pstcmdinfo->enmsgtrantype = xc8002_ack_msgtran_type;
    			SendToDwn((xc8002_middwnmach_com_msgtran_stru *)&(pstcmdinfo->endwnmachname));
			}
        	SendMsgUp((char*)(&(pstprotdt->aucdata[0])),
                          xc8002_calcprotdtlenbyldlen(pstprotdt->usdatalen));
		}

        ulprocedlen += xc8002_calcprotpcklenbyldlen(pstprotdt->usdatalen);
    }
    DELETE_MEMEORY(pstcmdinfo);
    return ulrunrslt;
}

void ReadSpsUart(vos_u32 uluartid)
{
#ifdef COMPILER_IS_LINUX_GCC
    WRITE_INFO_LOG("Uart Simulation ,Return .\n");
    return;
#endif
    vos_u32 ulrdedchrnum = 0;
    vos_u32 ulrunrslt = VOS_OK;
    vos_u32 ulrdtotallen = 0;
    char* pucnewbuf = VOS_NULL;
    char* pucrdsvbuf = VOS_NULL;
    char  aucrdbuf[APP_MAX_UART_RDBUF_LEN] = { 0 };

    if (APP_USED_UART_NUM <= uluartid)
    {
        WRITE_ERR_LOG("Invalid uartid(%d) \n", uluartid);
    }

    ulrdtotallen = 0;
    while (1)
    {
        memset((char*)aucrdbuf, 0x00, APP_MAX_UART_RDBUF_LEN);

        ulrdedchrnum = 0;
        ulrdedchrnum = read(g_uartHdl[uluartid],
            (char *)aucrdbuf, APP_MAX_UART_RDBUF_LEN);

        if (((0 == ulrdedchrnum) || (APP_INVALID_U32 == ulrdedchrnum)) && (0 == ulrdtotallen))
        {//本次读取没有数据
            DELETE_MEMEORY(pucrdsvbuf)
                break;
        }

        if ((ulrdedchrnum < APP_MAX_UART_RDBUF_LEN) && (0 == ulrdtotallen))
        {
            //一次将串口buffer中的数据读取完毕,向上层提交数据
            UnpackProtocolData((char*)aucrdbuf, ulrdedchrnum);
            DELETE_MEMEORY(pucrdsvbuf)
                break;
        }

        if ((0 == ulrdedchrnum) || (APP_INVALID_U32 == ulrdedchrnum))
        {
            //串口数据读取完毕,向上层提交数据
            UnpackProtocolData((char*)pucrdsvbuf, ulrdtotallen);
            DELETE_MEMEORY(pucrdsvbuf)
                break;
        }

        ulrdtotallen += ulrdedchrnum;

        ulrunrslt = vos_mallocmemory(ulrdtotallen, vos_mem_type_static,
            (void **)(&pucnewbuf));
        if ((VOS_OK != ulrunrslt) || (VOS_NULL == pucnewbuf))
        {
            WRITE_ERR_LOG("Call vos_mallocmemory() failed(0x%x) \n", ulrunrslt);
            DELETE_MEMEORY(pucrdsvbuf)
                break;
        }

        if (VOS_NULL != pucrdsvbuf)
        {
            memcpy(pucnewbuf, pucrdsvbuf, (ulrdtotallen - ulrdedchrnum));
            DELETE_MEMEORY(pucrdsvbuf)
        }

        memcpy((pucnewbuf + (ulrdtotallen - ulrdedchrnum)),
            (char*)aucrdbuf, ulrdedchrnum);

        pucrdsvbuf = pucnewbuf;
        pucnewbuf = VOS_NULL;
    }

    return;
}

vos_u32 ReadSpsFpga(vos_u32 ulfpgaid)
{
	#ifdef COMPILER_IS_LINUX_GCC
	WRITE_INFO_LOG("FPGA AD Simulation ,Return .\n");
	return VOS_OK;
	#endif
	
    if (APP_USED_UART_NUM < ulfpgaid)
    {
        WRITE_ERR_LOG("[Error] Invalid uartid(%d) \n", ulfpgaid);
    }
	//通知响应处理函数
    vos_u32 ulrunrslt = VOS_OK;
    
    msg_stHead rHead;
	app_mnguart_middwnmach_cmd_msgload_stru * pPara = VOS_NULL;
	vos_u32 ulParaLen = sizeof(app_mnguart_middwnmach_cmd_msgload_stru);
	ulrunrslt = app_mallocBuffer(ulParaLen, (void**)(&pPara));
	if (VOS_OK != ulrunrslt)
    {
        WRITE_ERR_LOG("Call app_mallocBuffer() failed(0x%x) \n", ulrunrslt);
        return ulrunrslt;
    }

	pPara->endwnmachname = xc8002_dwnmach_name_fpga_ad;
		
	app_constructMsgHead(&rHead, Pid_DwnMachUart, Pid_DwnMachUart,
        app_res_dwnmach_act_cmd_msg_type);
    
	return app_sendMsg(&rHead, pPara, ulParaLen);
}
void * UartReadThreadEnter(void* para)
{
    sem_wait(&g_uartsem);

    while (1)
    {
        volatile vos_u32 value = 0;
        value = *g_pSharemem;
        *g_pSharemem &= ~value;

        volatile vos_u32 advalue = 0;
        advalue = *(g_pSharemem + 2);
        if (advalue != 0)
        {
            *(g_pSharemem + 2) = 0;
        }

        while (0 != value || 0 != advalue)
        {
            if (value & 0x01) // 0001(0)
            {
                ReadSpsUart(8);//搅拌杆线程
            }

            if (value & 0x02) // 0010(1)
            {
                ReadSpsUart(1); //样本针
            }

            if (value & 0x08) // 1000(3)
            {
                ReadSpsUart(0); //样本盘
            }

            if (value & 0x20) // 10 0000(5)
            {
                ReadSpsUart(5); //R2
            }

            if (value & 0x40) // 100 0000(6)
            {
                ReadSpsUart(2); //试剂内盘
            }

            if (value & 0x80) // 1000 0000(7)
            {
                ReadSpsUart(3); //R1
            }

            if (value & 0x100) // 1 0000 0000(8)
            {
                ReadSpsUart(4); //试剂外盘
            }

            if (value & 0x400) // 100 0000 0000(10)
            {
                ReadSpsUart(10); //温控
            }

            if (value & 0x800) // 1000 0000 0000(11)
            {
                ReadSpsUart(9); //试剂搅拌杆
            }

            if (value & 0x1000) // 1 0000 0000 0000(12)
            {
                ReadSpsUart(6); //反应盘
            }

            if (value & 0x2000) // 10 0000 0000 0000(13)
            {
                ReadSpsUart(7); //自动清洗
            }

            if (value & 0x100000) //0001 0000 0000 0000 0000 0000
            {
                ReadSpsUart(11); //试剂制冷
            }

            if (advalue != 0) // 1 << 31
            {
                ReadSpsFpga(xc8002_dwnmach_name_fpga_ad); //获取AD值
            }

            if (value & 0x80000) // 1 << 19
            {
                app_readlinkdate(); //获取链路调试信息
            }

            //防止读串口过程中其它串口到达新的数据
            value = *g_pSharemem;
            *g_pSharemem &= ~value;

            advalue = *(g_pSharemem + 2);
            if (advalue != 0)
            {
                *(g_pSharemem + 2) = 0;
            }
        }

        for (int i = 0; i < xc8002_dwnmach_name_fpga_ad; ++i)
        {
            ReadSpsUart(i);
        }

        sem_wait(&g_uartsem);
    }
    WRITE_ERR_LOG("UartReadThreadEnter exit.\n");
    return NULL;
}

vos_u32 CreatUartReadThread(void)
{
    int  res = 0;
    res = sem_init(&g_uartsem, 0, 0);
    if (res == -1)
    {
        WRITE_ERR_LOG("sem_init failed\n");
    }

    pthread_t thread;
    res = pthread_create(&thread, NULL, UartReadThreadEnter, NULL);
    if (res != 0)
    {
        WRITE_ERR_LOG("pthread_create failed\n");
    }

    return VOS_OK;
}

vos_u32 GenUartHdlMap(void)
{
	vos_u32 ulloopvar = 0;
    app_i32 sltmphdl   = APP_NULL_HANDLE;
	char  aucaurtdevname[APP_MAX_UART_RDBUF_LEN] = {'\0'};

	for (ulloopvar = 0; APP_USED_UART_NUM > ulloopvar; ulloopvar++)
    {
        memset((char*)aucaurtdevname, 0x00, APP_MAX_UART_RDBUF_LEN);
        
        sprintf((char*)aucaurtdevname, "/dev/dev%u", 
                (unsigned int)uartMapDev[ulloopvar]);

		sltmphdl = open((char*)aucaurtdevname, O_RDWR | O_NONBLOCK);
        if (APP_NULL_HANDLE == sltmphdl)
        {
            WRITE_ERR_LOG("Open the %s uart failed \n", (char*)aucaurtdevname);
            break;
        }
        g_uartHdl[ulloopvar] = sltmphdl;
    }

	//公共内存与驱动地址绑定
    vos_u32 fd_map = open("/dev/mem", O_RDWR|O_SYNC);  
    g_pSharemem = (vos_u32* )mmap(NULL, 0x100000, PROT_READ|PROT_WRITE, MAP_SHARED, fd_map, 0x3fe00000);
	
	return VOS_OK;
}

void UartSignalHandler(int arg)
{
    sem_post(&g_uartsem);
}

void setAdMode(vos_u32 admode)
{
    app_i32 fpgahdl      = APP_NULL_HANDLE;
    
    fpgahdl = g_uartHdl[xc8002_dwnmach_name_fpga_ad]; //索引12表示fpga设备
    
    if (APP_NULL_HANDLE == fpgahdl)
    {
        WRITE_ERR_LOG("[Error] fpgahdl is NULL. \n");
    }
    ioctl(fpgahdl, AD_MODE, &admode);

    return;
}

vos_u32 getFpgaDpValuePos(vos_u32 chanIndex)
{   
    vos_u32 ulresult = 0;
    vos_u32 ulindex =  0;

    if (chanIndex >= WAVE_NUM)
    {
        WRITE_INFO_LOG("[Error]fpgapos = %d\n", (int)chanIndex);
        return VOS_INVALID_U32;
    }

    for (ulindex = 0; ulindex < WAVE_NUM; ulindex++)
    {
        if (m_dpWaveMap[ulindex] == m_appWaveMap[chanIndex])
        {
            break;
        }
    }

    if (ulindex >= WAVE_NUM)
    {
        ulresult = VOS_INVALID_U32;
    }
    else
    {
        ulresult = ulindex;
    }

    return ulresult;
}

vos_u32 setDpValue(vos_u32 * dpValue)
{
    vos_u32 ultryidx = 0; 
    vos_u32 ulapppos = 0;
    vos_u32 uldpdone = 0;

	vos_u32 fpgahdl = g_uartHdl[xc8002_dwnmach_name_fpga_ad];

	for(int i=0 ; i < WAVE_NUM ; i++)
	{
		ulapppos = getFpgaDpValuePos(i);
    	ioctl(fpgahdl, AD_SEL_CHANNEL, &ulapppos);
    	ioctl(fpgahdl, AD_SET_DP_VALUE, &dpValue[i]);   
		WRITE_INFO_LOG("dpValue[%d] = %d\n",i,dpValue[i]);
    	uldpdone = 0;
    	for (ultryidx = 0; ultryidx < 3; ultryidx++)
    	{
        	ioctl(fpgahdl, AD_JUDGE_DP_DONE, &uldpdone);   
        	if (1 == uldpdone)
        	{
            break;
        	}
        	usleep(100);
    	}
#ifdef COMPILER_IS_ARM_LINUX_GCC 
    	if (1 != uldpdone)
    	{
        	WRITE_ERR_LOG("[Error] uldpdone = %d \n", uldpdone);
        	return APP_INVALID_U32;
    	}
#endif
	}
    return VOS_OK;
}

vos_u32 writeDpValueToFile(vos_u32 * dpValue)
{
	FILE *filefp = NULL;

	filefp = fopen((char*)DPVALUE_FILENAME,"w");

	if(filefp == NULL)
    {
        WRITE_ERR_LOG("Write dpvalue.txt failed \n");
        return APP_INVALID_U32;
    }

	fseek(filefp,0,SEEK_SET);
	for(int i=0 ; i < WAVE_NUM ; i++)
	{
		fprintf(filefp,"%d,",(int)dpValue[i]);
	}
	fclose(filefp);

	return VOS_OK;
}

vos_u32 readDpValueFromFile(vos_u32 * dpValue)
{
	FILE *filefp = NULL;

	filefp = fopen((char*)DPVALUE_FILENAME,"r");

	if(filefp == NULL)
    {
        WRITE_ERR_LOG("Read dpvalue.txt failed \n");
        return APP_INVALID_U32;
    }

	fseek(filefp,0,SEEK_SET);
	for(int i=0 ; i < WAVE_NUM ; i++)
	{
		if(fscanf(filefp,"%d,",(int*)&dpValue[i]) == EOF)
		{
			break;
		}
	}
	fclose(filefp);

	return VOS_OK;
}

vos_u32 InitUartSignal(void)
{
    vos_u32 oflags, loopval;
    app_i32 tmpuarthdl = 0;
    vos_u32 tmpfpgahdl = 0;
    app_i32 tmpfilter = 5000; 

    signal(SIGIO, &UartSignalHandler); 

    for(loopval = 0; loopval < APP_USED_UART_NUM; loopval++ )
    {    
        tmpuarthdl = g_uartHdl[loopval];
        fcntl(tmpuarthdl, F_SETOWN, getpid());
        oflags = fcntl(tmpuarthdl, F_GETFL);
        fcntl(tmpuarthdl, F_SETFL, oflags | FASYNC);
    }

    //初始化测试模式为正常模式
    setAdMode(app_ad_fpga_work_mode_nml);

    tmpfpgahdl = g_uartHdl[xc8002_dwnmach_name_fpga_ad];

	//复位FPGA
    vos_u32 uluartmode = CLEAR_VALUE_L;
    ioctl(tmpfpgahdl, FPGA_UART_RESET_L, &uluartmode);

	uluartmode = CLEAR_VALUE_H;
    ioctl(tmpfpgahdl, FPGA_UART_RESET_H, &uluartmode);
	
    //设置滤波系数
    ioctl(tmpfpgahdl, AD_FILTER, &tmpfilter);

    //设置增益系数
    vos_u32 dpValue[WAVE_NUM] = {0};
    readDpValueFromFile(dpValue);
	setDpValue(dpValue); 

    return VOS_OK;
}

vos_u32 InitUart()
{
	//通道与波长映射
	m_appWaveMap[0] = app_rscsch_wave_340nm_id;
    m_appWaveMap[1] = app_rscsch_wave_405nm_id;
    m_appWaveMap[2] = app_rscsch_wave_450nm_id;
    m_appWaveMap[3] = app_rscsch_wave_478nm_id;
    m_appWaveMap[4] = app_rscsch_wave_505nm_id;
    m_appWaveMap[5] = app_rscsch_wave_542nm_id;
    m_appWaveMap[6] = app_rscsch_wave_570nm_id;
    m_appWaveMap[7] = app_rscsch_wave_605nm_id;
    m_appWaveMap[8] = app_rscsch_wave_630nm_id;
    m_appWaveMap[9] = app_rscsch_wave_660nm_id;
    m_appWaveMap[10] = app_rscsch_wave_700nm_id;
    m_appWaveMap[11] = app_rscsch_wave_750nm_id;
    m_appWaveMap[12] = app_rscsch_wave_805nm_id;
    m_appWaveMap[13] = app_rscsch_wave_850nm_id;

	m_dpWaveMap[13] = app_rscsch_wave_630nm_id;
    m_dpWaveMap[12] = app_rscsch_wave_700nm_id;
    m_dpWaveMap[11] = app_rscsch_wave_850nm_id;
    m_dpWaveMap[10] = app_rscsch_wave_570nm_id;
    m_dpWaveMap[9] = app_rscsch_wave_660nm_id;
    m_dpWaveMap[8] = app_rscsch_wave_750nm_id;
    m_dpWaveMap[0] = app_rscsch_wave_605nm_id;
    m_dpWaveMap[1] = app_rscsch_wave_505nm_id;
    m_dpWaveMap[2] = app_rscsch_wave_450nm_id;
    m_dpWaveMap[3] = app_rscsch_wave_340nm_id;
    m_dpWaveMap[4] = app_rscsch_wave_542nm_id;
    m_dpWaveMap[5] = app_rscsch_wave_478nm_id;
    m_dpWaveMap[6] = app_rscsch_wave_405nm_id;
    m_dpWaveMap[7] = app_rscsch_wave_805nm_id; 
	
	m_fpgaWaveMap[0] = app_rscsch_wave_630nm_id;
    m_fpgaWaveMap[1] = app_rscsch_wave_700nm_id;
    m_fpgaWaveMap[2] = app_rscsch_wave_850nm_id;
    m_fpgaWaveMap[3] = app_rscsch_wave_570nm_id;
    m_fpgaWaveMap[4] = app_rscsch_wave_660nm_id;
    m_fpgaWaveMap[5] = app_rscsch_wave_750nm_id;
    m_fpgaWaveMap[6] = app_rscsch_wave_605nm_id;
    m_fpgaWaveMap[7] = app_rscsch_wave_505nm_id;
    m_fpgaWaveMap[8] = app_rscsch_wave_450nm_id;
    m_fpgaWaveMap[9] = app_rscsch_wave_340nm_id;
    m_fpgaWaveMap[10] = app_rscsch_wave_542nm_id;
    m_fpgaWaveMap[11] = app_rscsch_wave_478nm_id;
    m_fpgaWaveMap[12] = app_rscsch_wave_405nm_id;
    m_fpgaWaveMap[13] = app_rscsch_wave_805nm_id;
	//中位机串口号与驱动串口号的映射
	uartMapDev[xc8002_dwnmach_name_smpldisk] 		= 3;
	uartMapDev[xc8002_dwnmach_name_smpndl] 		= 1;
	uartMapDev[xc8002_dwnmach_name_R2disk] 	= 6;
	uartMapDev[xc8002_dwnmach_name_r1Ndl] 	= 7;
	uartMapDev[xc8002_dwnmach_name_R1disk] 	= 8;
	uartMapDev[xc8002_dwnmach_name_r2Ndl] 	= 5;
	uartMapDev[xc8002_dwnmach_name_reactdisk] 		= 12;
	uartMapDev[xc8002_dwnmach_name_autowash] 		= 13;
	uartMapDev[xc8002_dwnmach_name_smpl_mixrod] 	= 0;
	uartMapDev[xc8002_dwnmach_name_reag_mixrod] 	= 11;
	uartMapDev[xc8002_dwnmach_name_auto_Rtmpctrl] 	= 10;
	uartMapDev[xc8002_dwnmach_name_cooling_reag] 	= 20;
	uartMapDev[xc8002_dwnmach_name_fpga_ad] 		= 31;
    uartMapDev[xc8002_link_debug] 		            = 19;
    
	#ifdef COMPILER_IS_LINUX_GCC
	WRITE_INFO_LOG("Simulation ,Return .\n");
	return VOS_OK;
	#endif
	
	GenUartHdlMap();
	CreatUartReadThread();
	InitUartSignal();
	
	return VOS_OK;
}
