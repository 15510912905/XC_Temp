#include "uart.h"
#include "msg.h"
#include "app_innermsg.h"
#include "app_comstructdefine.h"
#include "app_xc8002shareinfo.h"
#include "alarm.h"
#include "app_msgtypedefine.h"
#include "app_msg.h"
#include <stdio.h>
#include <string.h>
#include "app_errorcode.h"
#include "vos_timer.h"
#include "apputils.h"
#include"cmd_sampledisk.h"
#include"cmd_samplemixing.h"
#include"cmd_sampleneedle.h"
#include"cmd_reactiondisk.h"
#include"cmd_reagentmixing.h"
#include"cmd_autowashneedle.h"
#include"cmd_innerreagentdisk.h"
#include"cmd_outerreagentdisk.h"
#include"cmd_r1reagentneedle.h"
#include"cmd_r2reagentneedle.h"
#include "cmd_tempcontrol.h"
#include "cmd_coolingreagent.h"
#include"vos_adaptsys.h"
#include"drv_ebi.h"
#include"cmd_downmachine.h"
#include <stdlib.h>
#include <memory.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <termios.h>
#include <errno.h>
#include <sys/mman.h>
#include <fcntl.h> 
#include <signal.h>
#include "xc8002_middwnmachshare.h"
#include "app_debugtty.h"
#include <sys/ioctl.h>
#include "app_downmsg.h"
#include "app_simulation.h"
#include "vos_mngmemory.h"
#include "vos_errorcode.h"
#include "vos_pid.h"
#include "vos_init.h"
#include "typedefine.pb.h"
#include "inituart.h"
#include "vos_log.h"
#include "showave.h"
#include <iostream>

extern map<app_u16, app_u16> g_uartHdl;
extern map<app_u16, app_u16> m_appWaveMap;
extern map<app_u16, app_u16> m_dpWaveMap;
extern map<app_u16, app_u16> m_fpgaWaveMap;

extern vos_u32 g_ultestmode;

map<vos_u32,vos_u32> map_dwnmachmask;

app_u16 g_liquidFlag[6] = { 0 };

CUart::CUart(vos_u32 ulPid) : CCallBack(ulPid)
{
	_initMap();
#ifdef COMPILER_IS_ARM_LINUX_GCC
    _initCan();
#endif // DEBUG
}

CUart::~CUart()
{
    map<app_u16, char*>::iterator iter = m_dwnMsgBuf.begin();
    for (; iter != m_dwnMsgBuf.end();++iter)
    {
        if (NULL != iter->second)
        {
            delete(iter->second);
            iter->second = NULL;
        }
    }
    m_dwnMsgBuf.clear();
}

void CUart::_initMap()
{	
	//CallBack函数中MsgType与函数映射
	m_msgFucs[app_req_dwnmach_act_cmd_msg_type] = &CUart::_procDwnActReq;
    m_msgFucs[app_res_dwnmach_act_cmd_msg_type] = &CUart::_procDwnActRes;
	m_msgFucs[vos_pidmsgtype_timer_timeout] = &CUart::_procTimeOut;

	//下位机设备ID与消息缓存的映射
    for (int i = 0; i <= xc8002_dwnmach_name_fpga_ad; ++i)
    {
        app_single_dwnmach_cmdbuf_stru * pUnit = new app_single_dwnmach_cmdbuf_stru;
        string str = "xc8002_dwnmach_name" + i;
        pUnit->usTimerId = createTimer(Pid_DwnMachUart, str.c_str(), UART_RES_TIME_LEN);
		pUnit->runState = UART_IDLE;
        m_timerDwnNameMap[pUnit->usTimerId] = i;
        m_dwnMsgBuf[i] = (char *)pUnit;
		
		map_dwnmachmask[i] = VOS_NO;
    }
	//初始化AD采集模式为正常模式 
    setAdMode(app_ad_fpga_work_mode_nml);
}

vos_u32 CUart::CallBack(vos_msg_header_stru* pMsg)
{
	WRITE_INFO_LOG("Enter uart CallBack.\r\n");
	if (VOS_NULL == pMsg)
    {        
		WRITE_ERR_LOG("Input parameter is null \n");        
		return app_para_null_err;    
	}	
	
	map<app_u16, MsgFuc>::iterator iter = m_msgFucs.find(pMsg->usmsgtype);
    if (iter != m_msgFucs.end())
    {
        return (this->*(iter->second))(pMsg);
    }

    WRITE_ERR_LOG("Unknown msg type(%d) \n", pMsg->usmsgtype);
    return app_unknown_msg_type_err;
}

vos_u32 CUart::_getFrameId(vos_u32 endwnmachname)
{
    app_single_dwnmach_cmdbuf_stru * pMidDwnMsg = VOS_NULL;
    map<app_u16, char*>::iterator iter = m_dwnMsgBuf.find(endwnmachname);
    if (iter != m_dwnMsgBuf.end())
    {
        pMidDwnMsg = (app_single_dwnmach_cmdbuf_stru *)m_dwnMsgBuf[endwnmachname];
        pMidDwnMsg->ulcurcmdframeid = (0xffffffff == pMidDwnMsg->ulcurcmdframeid) ? 0 : (pMidDwnMsg->ulcurcmdframeid + 1);
        return pMidDwnMsg->ulcurcmdframeid;
    }
    return VOS_OK;
}

vos_u32 CUart::_calcChecksumForPacket(xc8002_uart_prot_data_format_stru* pstpack)
{
    if (VOS_NULL == pstpack)
    {
        WRITE_ERR_LOG("The input parameter is null \n");
        return 0;
    }
    
    vos_u32 ulupperthd = pstpack->usdatalen + sizeof(pstpack->ushead) + sizeof(pstpack->usdatalen);
    char* pucmv = (char*)pstpack;

    vos_u32 ulchksum = 0;
    for (vos_u32 ulloopvar = 0; ulupperthd > ulloopvar; ulloopvar++)
    {
        ulchksum += (char)(*(pucmv));
        pucmv += 1;
    }
    /*将数据缓存中checksum部分的值减去*/
    pucmv = (char*)&(pstpack->uschksum);
    ulchksum -= (char)(*(pucmv++));
    ulchksum -= (char)(*(pucmv));
    pucmv = VOS_NULL;  

    return ulchksum;
}

vos_u32 app_setdwnmachmask( vos_u32 dwnMachName, vos_u32 maskState )
{
    if (0 <= dwnMachName && dwnMachName <= xc8002_dwnmach_name_smplEmer)
	{
        map_dwnmachmask[dwnMachName] = maskState;
    }
    
    return VOS_OK;
}

vos_u32 app_getdwnmachmask(vos_u32 dwnMachName)
{
    if (0 <= dwnMachName && dwnMachName <= xc8002_dwnmach_name_smplEmer)
	{
        return map_dwnmachmask[dwnMachName];
    }
	
	return VOS_NO;
}

vos_u32 CUart::_sendToDwn(xc8002_middwnmach_com_msgtran_stru * pMsg)
{
	WRITE_INFO_LOG("Enter uart _sendToDwn.\r\n");
	vos_u32 ulrunrslt = VOS_OK;
	vos_u32 ulprotpcklen = 0;
	vos_u32 uldatabuflen = 0;
	
    st_CanFormat*  pstfmtdtbuf = VOS_NULL;

    ulprotpcklen = sizeof(st_CanFormat) + pMsg->uscmdinfolen - 4;
	uldatabuflen = sizeof(app_msg_payload_stru) + pMsg->uscmdinfolen;
	
	ulrunrslt = vos_mallocmemory(ulprotpcklen, vos_mem_type_static,
                                 (void **)(&pstfmtdtbuf));
    if (VOS_OK != ulrunrslt)
    {
        WRITE_ERR_LOG("Call vos_mallocmemory() failed(%d) \n", ulrunrslt);
        return ulrunrslt;
    }

    pstfmtdtbuf->cmdType = pMsg->uscmdtype;
    memcpy(((char*)&(pstfmtdtbuf->endwnmachname)),(char*)&(pMsg->endwnmachname), uldatabuflen);

    app_print(ps_cmdSch,"TIME %ld sendToDwn  subsys =%d cmd = %#x\n",getTime_ms(),pMsg->endwnmachname,pMsg->uscmdtype);
#ifdef COMPILER_IS_LINUX_GCC
    app_simuDwnmachResponse(pMsg);//模拟下位机
    CanWrite((char*)pstfmtdtbuf, ulprotpcklen, pMsg->endwnmachname);
#else

	if(VOS_YES == app_getdwnmachmask(pMsg->endwnmachname))
	{
        app_simuDwnmachResponse(pMsg);//模拟下位机
		DELETE_MEMEORY(pstfmtdtbuf);
		return ulrunrslt;
	}
    CanWrite((char*)pstfmtdtbuf, ulprotpcklen, pMsg->endwnmachname);
    //app_simuDwnmachResponse(pMsg);//模拟下位机
    //CanWrite((char*)pstfmtdtbuf, ulprotpcklen, pMsg->endwnmachname);
#endif
	
	DELETE_MEMEORY(pstfmtdtbuf);
	return VOS_OK;
}

vos_u32 CUart::_procDwnActReq(vos_msg_header_stru* pReqMsg)
{
	WRITE_INFO_LOG("Enter uart _procDwnActReq.\r\n");
	vos_u32 ulrunrslt = VOS_OK;
	vos_u32 ulcmdlen = 0;
	msg_stDwnCmdReq * pReq = (msg_stDwnCmdReq*)pReqMsg;
	app_single_dwnmach_cmdbuf_stru * pDwnReqMsg = VOS_NULL;
    vos_u32 EnDwnName = pReq->endwnmachsubsys;

	if ((xc8002_dwnmach_name_butt*2) <= EnDwnName)
    {
        WRITE_ERR_LOG("UART: unknown down machine cmd(0x%x)DwnName(%d) \n", (vos_u32)(pReq->encmdtype), EnDwnName);
        return ulrunrslt;
    }
	
	pDwnReqMsg = (app_single_dwnmach_cmdbuf_stru *)m_dwnMsgBuf[EnDwnName];
	if(pDwnReqMsg->runState == UART_IS_BUSY)
	{
		WRITE_ERR_LOG("Don't send command until the previous result back!!! \n");
		return ulrunrslt;
	}
	ulcmdlen  = pReq->usmsgloadlen - sizeof(pReq->endwnmachsubsys) - sizeof(pReq->usdevtype);

	if (VOS_OK != ulrunrslt)
    {
        WRITE_ERR_LOG("Call vos_mallocmemory() failed(%d) \n", ulrunrslt);
        return ulrunrslt;
    }

	pDwnReqMsg->ulsrcpid = pReq->ulsrcpid;
	pDwnReqMsg->endwnmachname = EnDwnName;
	pDwnReqMsg->enmsgtrantype = xc8002_req_msgtran_type;
	pDwnReqMsg->uscmdinfolen = ulcmdlen;
	pDwnReqMsg->ulcurcmdframeid = _getFrameId(pDwnReqMsg->endwnmachname);

	memcpy((char*)(&(pDwnReqMsg->uscmdtype)),(char*)(&(pReq->encmdtype)), ulcmdlen);
	
	if(pReq->encmdtype == XC8002_COM_DWNMACH_LOOP_CMD)
	{
		_sendToDwn((xc8002_middwnmach_com_msgtran_stru *)&(pDwnReqMsg->endwnmachname));
	}
	else
	{
		if(pReq->encmdtype == XC8002_COM_DWNMACH_COMB_CMD)
		{
			msg_stComDwnmachCombCmd * pCombMsg = (msg_stComDwnmachCombCmd *)&(pReq->encmdtype);
			pDwnReqMsg->usCmdNum = pCombMsg->cmdNum;
		}
		else
		{
			pDwnReqMsg->usCmdNum = 1;
		}
		pDwnReqMsg->runState = UART_IS_BUSY;
		
		_sendToDwn((xc8002_middwnmach_com_msgtran_stru *)&(pDwnReqMsg->endwnmachname));
        if ((pReq->encmdtype == CMD_DSK_RESET) && (pReq->endwnmachsubsys == xc8002_dwnmach_name_reactdisk))
        {
            startTimer(pDwnReqMsg->usTimerId, 12000);
        }
        else
        {
            startTimer(pDwnReqMsg->usTimerId, UART_RES_TIME_LEN);
        }
	}

	return VOS_OK;
}

vos_u32 CUart::_noticePidReadAd(vos_u32 dstpid, ADValue_st* pstrepcmd, char dwnName)
{
	WRITE_INFO_LOG("Enter _noticePidReadAd \n");
    vos_u32 ulrunrslt = VOS_OK;
    
    msg_ADValue *pstMsg;
    vos_u32 ulmsgsize = sizeof(msg_ADValue) + sizeof(app_u32)* pstrepcmd->AdCount;
    ulrunrslt = app_mallocBuffer(ulmsgsize, (void**)(&pstMsg));
    if (VOS_OK != ulrunrslt)
    {
        WRITE_ERR_LOG("Call app_mallocBuffer() failed(%d) \n", ulrunrslt);
        return ulrunrslt;
    }
    pstMsg->dwnName = dwnName;
    pstMsg->AdCount = pstrepcmd->AdCount;
    if (0 != pstrepcmd->AdCount)
    {
        memcpy(pstMsg->value, pstrepcmd->value, sizeof(app_u32)* pstrepcmd->AdCount);
    }

    msg_stHead rHead;
	app_constructMsgHead(&rHead, dstpid, Pid_DwnMachUart,
        app_com_ad_data_ready_notice_msg_type);
    
    return app_sendMsg(&rHead, pstMsg, ulmsgsize);
}

vos_u32 CUart::_procFpgaAdRes(ADValue_st* pstrepcmd, char dwnName)
{
	WRITE_INFO_LOG("Enter uart _procFpgaAdRes \n");
	switch(g_ultestmode)
	{
		case TESTMODE_LIGHTCHECK:
            _noticePidReadAd(SPid_CheckLightCup, pstrepcmd, dwnName);
			break;

		case TESTMODE_NORMAL:
            _noticePidReadAd(SPid_Task, pstrepcmd, dwnName);
            _printAD(pstrepcmd, dwnName);
			break;

		case TESTMODE_SMOOTH:
            _noticePidReadAd(SPid_Smooth, pstrepcmd, dwnName);
			break;

		case TESTMODE_DEBUG://只打印	
            _printAD(pstrepcmd, dwnName);
            _sendTestADVal(pstrepcmd->AdCount, pstrepcmd->value, dwnName);
            break;

		case TESTMODE_DARKCURRENT:
            _noticePidReadAd(SPid_DarkCurrent, pstrepcmd, dwnName);
			break;

		default:
			WRITE_ERR_LOG("Unknown mode = %d \r\n",g_ultestmode);
			break;
	}
    
    return VOS_OK;
}
extern vos_u32 g_ulmdprintswitch;
void CUart::_printAD(ADValue_st* pstrepcmd, char dwnName)
{
    if ((1 == ((g_ulmdprintswitch >> 19) & 0x00000001)))
    {
        char current_absolute_path[64];
        getcwd(current_absolute_path, 64);
#ifdef COMPILER_IS_LINUX_GCC
        string cwdpath= current_absolute_path;
        cwdpath += "/ad.txt";
#else
        string cwdpath;// = current_absolute_path;
        cwdpath += "/opt/ad.txt";
#endif
        FILE*   pf = fopen(cwdpath.c_str(), "a+");
        if (VOS_NULL == pf)
        {
            printf(" open ad.txt failed path: %s \r\n",cwdpath.c_str());
            return;
        }
        vos_u32 *pValue = pstrepcmd->value;
        for (app_u32 i = 0; i < pstrepcmd->AdCount / WAVE_NUM; i++)
        {
            fprintf(pf, "AD[%d]%d-", dwnName, i + 1);
            app_print(ps_singleCmd, "AD[%d]%-5d-", dwnName, i + 1);
            for (int j = 0; j < WAVE_NUM; j++)
            {
                fprintf(pf, "[%d]=%d, ", j + 1, *pValue);
                app_print(ps_singleCmd, "[%-2d]=%-5d, ", j + 1, *pValue);
                ++pValue;
            }
            fprintf(pf, "\n");
            app_print(ps_singleCmd, "\n");
        }
        fclose(pf);
    }
}

void CUart::_sendRepmsgToStatusrep(xc8002_com_report_cmd_stru* pstrepcmd)
{
    vos_u32 ulrunrslt   = VOS_OK;
    vos_u32 ulmsglen    = 0;
    vos_u32 ulvalidflag = APP_YES;

	msg_stHead rHead;
	vos_u32 ulParaLen;
	xc8002_com_report_cmd_stru*  pPara = VOS_NULL;
    
    switch(pstrepcmd->usstatustype)
    {            
        case xc8002_dwnmach_report_remain_type:
            ulmsglen = (sizeof(app_dwnmach_rep_msg_stru)
                     + sizeof(xc8002_report_remaindepth_stru)
                     - sizeof(xc8002_com_report_cmd_stru));
            break;

        case xc8002_dwnmach_report_rctdsk_temp_type:
        case xc8002_dwnmach_report_reagdsk_temp_type:
            ulmsglen = (sizeof(app_dwnmach_rep_msg_stru)
                     + sizeof(xc8002_report_temprature_status_stru)
                     - sizeof(xc8002_com_report_cmd_stru));
            break;

        case xc8002_dwnmach_report_fault_type:
        	ulmsglen = (sizeof(app_dwnmach_rep_msg_stru)
                     + sizeof(xc8002_fault_report_stru)
                     - sizeof(xc8002_com_report_cmd_stru));
            break;
 
        //打开关闭试剂舱通知上报
        case xc8002_dwnmach_report_close_reagcabin_type:
        case xc8002_dwnmach_report_open_reagcabin_type:
		case xc8002_dwnmach_report_keydown_innerdisk_type://触发内盘旋转按钮状态上报
		case xc8002_dwnmach_report_keydown_outerdisk_type://触发外盘旋转按钮状态上报
            ulmsglen = sizeof(app_dwnmach_rep_msg_stru);
            break;
		case xc8002_dwnmach_report_innerdisk_wave0_type:
		case xc8002_dwnmach_report_innerdisk_wave1_type:
        case xc8002_dwnmach_report_outerdisk_wave0_type:
		case xc8002_dwnmach_report_smpdisk_wave0_type:
            ulmsglen = sizeof(app_dwnmach_rep_msg_stru) + sizeof(xc8002_wave_smp_report_stru) - sizeof(xc8002_com_report_cmd_stru);
            break;
        case xc8002_dwnmach_report_downmachin_versions:
            ulmsglen = (sizeof(app_dwnmach_rep_msg_stru)
                     + sizeof(cmd_fpga_dwnmach_version_stru)
                     - sizeof(xc8002_com_report_cmd_stru));
            break;
        case xc8002_dwnmach_report_LightBulbAD_type:
            ulmsglen = (sizeof(app_dwnmach_rep_msg_stru)
                     + sizeof(cmd_react_disk_lightgetval_stru)
                     - sizeof(xc8002_com_report_cmd_stru));
            break;
        case xc8002_dwnmach_report_compensation_type:
        {
            Report_PAGE_st *compensation = (Report_PAGE_st *)pstrepcmd;
            ulmsglen = (sizeof(app_dwnmach_rep_msg_stru)
                + sizeof(Report_PAGE_st)
                + (sizeof(app_u32)*(compensation->count))
                - sizeof(xc8002_com_report_cmd_stru));
            break;
        }
        case xc8002_dwnmach_report_motorline_type:
        {
            xc8002_report_dwnmachine_motorspeed *pMotorSpeed = (xc8002_report_dwnmachine_motorspeed *)pstrepcmd;
            ulmsglen = (sizeof(app_dwnmach_rep_msg_stru)
                + sizeof(xc8002_report_dwnmachine_motorspeed)
                + (sizeof(xc8002_report_motor_para)*(pMotorSpeed->paranum))
                - sizeof(xc8002_com_report_cmd_stru));
            break;
        }
		case xc8002_dwnmach_report_clot_air_type:
			ulmsglen = sizeof(app_dwnmach_rep_msg_stru) + sizeof(ClotAirDataReport_st) - sizeof(xc8002_com_report_cmd_stru);
        case xc8002_dwnmach_report_fanstatus_type:  
            ulmsglen = (sizeof(app_dwnmach_rep_msg_stru)
                     + sizeof(cmd_cooling_fan_status_stru)
                     - sizeof(xc8002_com_report_cmd_stru));
            break;
        case xc8002_dwnmach_report_electricity_type: 
            ulmsglen = (sizeof(app_dwnmach_rep_msg_stru)
                     + sizeof(cmd_cooling_vol_stru)
                     - sizeof(xc8002_com_report_cmd_stru));
            break;
        case xc8002_dwnmach_report_fl_and_pre_type:  
            ulmsglen = (sizeof(app_dwnmach_rep_msg_stru)
                     + sizeof(cmd_autowash_status_stru)
                     - sizeof(xc8002_com_report_cmd_stru));
            break;
        case xc8002_dwnmach_report_reagdsk_temp_query_type:
            ulmsglen = (sizeof(app_dwnmach_rep_msg_stru)
                     + sizeof(cmd_cooling_temp_stru)
                     - sizeof(xc8002_com_report_cmd_stru));
            break;
        case xc8002_dwnmach_report_thermal_control_temp_type:
		case  xc8002_dwnmach_report_ad_temp_type:
            ulmsglen = (sizeof(app_dwnmach_rep_msg_stru)
                     + sizeof(cmd_temp_control_report_stru)
                     - sizeof(xc8002_com_report_cmd_stru));
            break;
        case xc8002_dwnmach_report_eeprom_info_type:
            ulmsglen = (sizeof(app_dwnmach_rep_msg_stru)
                     + sizeof(cmd_temp_control_eeprom_info_stru)
                     - sizeof(xc8002_com_report_cmd_stru));
            break;
		case xc8002_dwnmach_report_pidstade:
			ulmsglen = (sizeof(app_dwnmach_rep_msg_stru)
				+ sizeof(cmd_temp_pid_stru)
				- sizeof(xc8002_com_report_cmd_stru));
			break;
        case xc8002_dwnmach_report_bblog_info_type:
        {
            xc8002_report_dwnmachine_bblog *bblog  = (xc8002_report_dwnmachine_bblog *)pstrepcmd;
            ulmsglen = (sizeof(app_dwnmach_rep_msg_stru)
                     + sizeof(xc8002_report_dwnmachine_bblog)
                     + (sizeof(xc8002_bblog_record)*(bblog->recordnum))
                     - sizeof(xc8002_com_report_cmd_stru));
            break;
        }            
        case xc8001_dwnmach_report_ID:
        {
            ulmsglen = (sizeof(app_dwnmach_rep_msg_stru)
                + sizeof(cmd_CPUID)
                - sizeof(xc8002_com_report_cmd_stru));
            break;
        }
        case dwnmach_report_shelf:
        {
            ulmsglen = (sizeof(app_dwnmach_rep_msg_stru))
                + sizeof(Report_shelf_st)
                - sizeof(xc8002_com_report_cmd_stru);
            break;
        }
        case xc8002_dwnmach_report_GwayState:
        {
            ulmsglen = (sizeof(app_dwnmach_rep_msg_stru))
                + sizeof(GwayState_st)
                - sizeof(xc8002_com_report_cmd_stru);
            break;
        }
        case dwnmach_show_wave:
        {
            ulmsglen = (sizeof(app_dwnmach_rep_msg_stru))
                + sizeof(Report_ShowOP_st)
                - sizeof(xc8002_com_report_cmd_stru);
            break;
        }

        default:
        {
            ulvalidflag = APP_NO;
            WRITE_ERR_LOG("Unknown downmachine report's type(%u)\n", pstrepcmd->usstatustype);
            break;
        }
    }

    if (APP_YES != ulvalidflag)
    {
        return;
    }
             
	ulParaLen = ulmsglen - sizeof(vos_msg_header_stru);
	ulrunrslt = app_mallocBuffer(ulParaLen, (void**)(&pPara));
	if (VOS_OK != ulrunrslt)
    {
        WRITE_ERR_LOG("Call app_mallocBuffer() failed(%d) \n", ulrunrslt);
        return;
    }
	memcpy((void*)pPara, (void*)pstrepcmd,ulParaLen);
	app_constructMsgHead(&rHead,Pid_ReportStatus,Pid_DwnMachUart,app_dwnmachin_stautsrep_msg_type);
    
	ulrunrslt = app_sendMsg(&rHead,pPara,ulParaLen);
    if (VOS_OK != ulrunrslt)
    {
        WRITE_ERR_LOG("Call app_sendMsg() failed(%d) \n", ulrunrslt);
        return;
    }

    return;
}

vos_u32 CUart::_procDwnmachCmdReport(xc8002_middwnmach_res_msgtran_stru* pstcmdres)
{
	WRITE_INFO_LOG("Enter uart _procDwnmachCmdReport.\r\n");
	WRITE_INFO_LOG("report cmdinfo_len = %d\n",pstcmdres->uscmdinfolen);
	xc8002_com_report_cmd_stru* pstrepcmd = VOS_NULL;
    
	if (pstcmdres->uscmdtype == XC8002_COM_REPORT_CMD)
	{
    	pstrepcmd = (xc8002_com_report_cmd_stru*)&(pstcmdres->uscmdtype);
        if (dwnmach_report_AD == pstrepcmd->usstatustype)
        {
            app_print(ps_cmdSch, "report cmdinfo_len = %d\n", pstcmdres->uscmdinfolen);
            app_print(ps_cmdSch, "AD count = %d\n", ((ADValue_st*)pstrepcmd)->AdCount);
            _procFpgaAdRes((ADValue_st*)pstrepcmd, pstcmdres->endwnmachname);
        }
        else
        {
            _sendRepmsgToStatusrep(pstrepcmd);
        }
    }
    return VOS_OK;
}

vos_u32 CUart::_reportExecmdResult(vos_u32 CmdExeRslt,vos_u32 CmdInfoLen,vos_u32 DwnMachName,char* pPayLoad)
{
    vos_u32 ulrunrslt    = VOS_OK;
    msg_stDwnmachActionMsg* pstresmsg = VOS_NULL;
	app_single_dwnmach_cmdbuf_stru* pReqCmdBuf = VOS_NULL;

	pReqCmdBuf = (app_single_dwnmach_cmdbuf_stru *)m_dwnMsgBuf[DwnMachName];
	pReqCmdBuf->runState = UART_IDLE;
    vos_stoptimer(pReqCmdBuf->usTimerId);

    msg_stHead rHead;

	vos_u32 PayLoadLen = CmdInfoLen - sizeof(xc8002_com_dwnmach_cmd_stru);
    vos_u32 ulmsgsize = sizeof(msg_stDwnmachActionMsg) + PayLoadLen;
	
    ulrunrslt = app_mallocBuffer(ulmsgsize, (void**)(&pstresmsg));
    if (VOS_OK != ulrunrslt)
    {
        WRITE_ERR_LOG("Call app_mallocBuffer() failed(%d) \n", ulrunrslt);
        return ulrunrslt;
    }

	app_constructMsgHead(&rHead, pReqCmdBuf->ulsrcpid, Pid_DwnMachUart, app_res_dwnmach_act_cmd_msg_type);
    
    pstresmsg->enDwnmachSubSys = pReqCmdBuf->endwnmachname;
    pstresmsg->enCmdExeRslt    = CmdExeRslt;
    pstresmsg->enCmdType       = pReqCmdBuf->uscmdtype;

    if ((VOS_NULL != pPayLoad) && (0 != PayLoadLen))
    {
        memcpy(pstresmsg->aucPayLoad, pPayLoad, PayLoadLen);
        
        pstresmsg->ulPayLoadLen = PayLoadLen;
    }

    return app_sendMsg(&rHead, pstresmsg, ulmsgsize);
}

vos_u32 CUart::_reportLoopTestResult(xc8002_middwnmach_com_msgtran_stru* pstresinfo)
{
    app_loopback_test_vos_msgload_stru* pPara = NULL;
    vos_u32 ulParaLen = sizeof(app_loopback_test_vos_msgload_stru);
    vos_u32 ulrunrslt = app_mallocBuffer(ulParaLen, (void**)(&pPara));
    if (VOS_OK != ulrunrslt)
    {
        WRITE_ERR_LOG("Call app_mallocBuffer() failed(0x%x) \n", ulrunrslt);
        return ulrunrslt;
    }
    memcpy(pPara, pstresinfo, ulParaLen);

    msg_stHead rHead;
    app_single_dwnmach_cmdbuf_stru* pReqCmdBuf = (app_single_dwnmach_cmdbuf_stru *)m_dwnMsgBuf[pstresinfo->endwnmachname];
    app_constructMsgHead(&rHead, pReqCmdBuf->ulsrcpid, Pid_DwnMachUart, app_res_dwnmach_act_cmd_msg_type);

    return app_sendMsg(&rHead, pPara, ulParaLen);
}

vos_u32 CUart::_procDwnmachCmdResponse(xc8002_middwnmach_res_msgtran_stru* pstcmdres)
{
	WRITE_INFO_LOG("Enter uart _procDwnmachCmdResponse.\r\n");
    vos_u32 ulrunrslt = VOS_OK;
    if (VOS_NULL == pstcmdres)
    {
        WRITE_ERR_LOG("The input parameter is null \n");
        return ulrunrslt;
    }
    app_single_dwnmach_cmdbuf_stru*  pReqCmdBuf = (app_single_dwnmach_cmdbuf_stru *)m_dwnMsgBuf[pstcmdres->endwnmachname];
    
    if (pReqCmdBuf->ulcurcmdframeid != pstcmdres->ulcurcmdframeid)
    {
        WRITE_ERR_LOG("Frame id error(rescmd=0x%x, srcfrmid=%d, resfrmid=%d,machname=%d,len=%d,alarmid=%d) \n",
            pstcmdres->uscmdtype, pReqCmdBuf->ulcurcmdframeid, pstcmdres->ulcurcmdframeid, pstcmdres->endwnmachname, pstcmdres->uscmdinfolen, pstcmdres->rsv);
        return ulrunrslt;
    }

	if (XC8002_COM_DWNMACH_LOOP_CMD == pstcmdres->uscmdtype)
    {
		return _reportLoopTestResult((xc8002_middwnmach_com_msgtran_stru*)pstcmdres);
    }

    if (xc8002_nak_msgtran_type == pstcmdres->enmsgtrantype)
    {  
        WRITE_ERR_LOG("DownMach Nak, dwnmach=%lu, cmd = %#x\n", pstcmdres->endwnmachname, pstcmdres->uscmdtype);
		g_pAlarm->GenerateAlarm(ALarm_Dwn_Act_Failed + pstcmdres->endwnmachname, pstcmdres->endwnmachname, pstcmdres->uscmdtype);
        return _reportExecmdResult(xc8002_exe_cmd_failed,UART_CMDINFOLEN_OF_NOPAYLOADMSG,pReqCmdBuf->endwnmachname,VOS_NULL);
    }
    else if (xc8002_ack_msgtran_type == pstcmdres->enmsgtrantype)
	{
        app_print(ps_cmdSch, "TIME %ld ResACKDwn  subsys =%d cmd = %#x,exerslt= %d\n",
            getTime_ms(), pstcmdres->endwnmachname, pstcmdres->uscmdtype, pstcmdres->enexerslt);
	}
    else if (xc8002_res_msgtran_type == pstcmdres->enmsgtrantype)
    {
        app_print(ps_cmdSch, "TIME %ld ResponDwn  subsys =%d cmd = %#x,exerslt= %d\n",
            getTime_ms(), pstcmdres->endwnmachname, pstcmdres->uscmdtype, pstcmdres->enexerslt);

        if (pstcmdres->enexerslt != xc8002_exe_cmd_sucess&&pstcmdres->enexerslt != 8 && pstcmdres->enexerslt != LiquidErr)
        {
            WRITE_ERR_LOG("ResponDwn  subsys =%d, cmd = %#x,exerslt= %d,alarmnum=%d\n", pstcmdres->endwnmachname, pstcmdres->uscmdtype, pstcmdres->enexerslt, pstcmdres->rsv);
			//上传下位机报警信息
            //g_pAlarm->GenerateAlarm(ALarm_Dwn_Act_Failed + pstcmdres->endwnmachname, pstcmdres->endwnmachname, pstcmdres->uscmdtype);
            g_pAlarm->GenerateAlarm(pstcmdres->rsv, pstcmdres->endwnmachname, pstcmdres->uscmdtype);
        }
		
		pReqCmdBuf->usCmdNum = pReqCmdBuf->usCmdNum - 1;
        if (pReqCmdBuf->usCmdNum > 0)
        {
			if ((pstcmdres->uscmdtype == cmd_r1_reagent_ndl_vdecline || pstcmdres->uscmdtype == cmd_r2_reagent_ndl_vdecline
                || pstcmdres->uscmdtype == cmd_sample_ndl_vdecline))
            {
                if (pReqCmdBuf->usCmdNum <= 2)
                {_procLiquidFlag(pstcmdres);}
            }
            pstcmdres->enexerslt = (pstcmdres->enexerslt == LiquidErr) ? xc8002_exe_cmd_sucess : pstcmdres->enexerslt;
			if(pstcmdres->enexerslt != xc8002_exe_cmd_sucess)
			{
				return _reportExecmdResult(pstcmdres->enexerslt,pstcmdres->uscmdinfolen,pstcmdres->endwnmachname,(char*)&(pstcmdres->aucinfo[0]));
			}
            return ulrunrslt;
        }
        pstcmdres->enexerslt = (pstcmdres->enexerslt == LiquidErr) ? xc8002_exe_cmd_sucess : pstcmdres->enexerslt;
        return _reportExecmdResult(pstcmdres->enexerslt,pstcmdres->uscmdinfolen,pstcmdres->endwnmachname,(char*)&(pstcmdres->aucinfo[0]));
    }
	return ulrunrslt;
}

vos_u32 CUart::_procDwnActRes(vos_msg_header_stru* pResMsg)
{
	WRITE_INFO_LOG("Enter uart _procDwnActRes.\r\n");
	xc8002_middwnmach_res_msgtran_stru* pDwnResMsg = (xc8002_middwnmach_res_msgtran_stru *)&(pResMsg->aucmsgload[0]);
	if(pDwnResMsg->endwnmachname == xc8002_dwnmach_name_fpga_ad)
	{
/*        _procFpgaAdRes();*/
        return VOS_OK;
	}
	else if (xc8002_req_msgtran_type == pDwnResMsg->enmsgtrantype)
    {
        _procDwnmachCmdReport(pDwnResMsg);
         return VOS_OK;
    }
	else 
	{
    	_procDwnmachCmdResponse(pDwnResMsg);
		return VOS_OK;
	}
}

vos_u32 CUart::_procTimeOut(vos_msg_header_stru* pResMsg)
{
	vos_timeroutmsg_stru* pTimeOutMsg = (vos_timeroutmsg_stru*)pResMsg;
	app_u16 endwnmachname = m_timerDwnNameMap[pTimeOutMsg->ultimerid];
    WRITE_ERR_LOG("DownMach Res Time Out, dwnmach=%lu. \n", endwnmachname);
	g_pAlarm->GenerateAlarm(ALarm_Dwn_Timeout + endwnmachname, endwnmachname);
	return _reportExecmdResult(xc8002_exe_cmd_timeout,UART_CMDINFOLEN_OF_NOPAYLOADMSG,endwnmachname,VOS_NULL);
}

vos_u32 CUart::_sendTestADVal(vos_u32 adNum, vos_u32*adVal, vos_u32 dwnName)
{
    STTestAD msg;
    vos_u32 cup = dwnName;//杯位号留给上位机记录
    vos_u32 * pAdVal = adVal;
    for (vos_u16 cupLoop = 0; cupLoop < adNum / WAVE_NUM; cupLoop++)
    {
        STTestVal* stVal = msg.add_stval();
        stVal->set_uicup(cup);
        for (vos_u16 AdLoop = 0; AdLoop < WAVE_NUM; AdLoop++)
        {
            stVal->add_uival(*pAdVal);
            pAdVal++;
        }
    }
	return sendUpMsg(CPid_Maintain, Pid_DwnMachUart, MSG_ADtest, &msg);
}

void CUart::_procLiquidFlag(xc8002_middwnmach_res_msgtran_stru *pstcmdres)
{
	vos_u16 dwnMachine = pstcmdres->endwnmachname;
	if (dwnMachine <= 5)
	{
		g_liquidFlag[dwnMachine] = pstcmdres->enexerslt;
	}
}

int g_canSocket = 0;
struct sockaddr_can g_stSockAddr = { 0 };

#ifndef AF_CAN
#define AF_CAN 29
#endif
#ifndef PF_CAN
#define PF_CAN AF_CAN
#endif

#define CAN1

#if 1
void CUart::_initCan()
{
#ifdef CAN0
    system("ifconfig can0 down");
    system("/sbin/ip link set can0 up type can bitrate 500000 triple-sampling off loopback off");
    system("ifconfig can0 up");
#else
    system("ifconfig can1 down");
    system("/sbin/ip link set can1 up type can bitrate 500000 triple-sampling off loopback off");
    system("ifconfig can1 up");
#endif
    m_canSocket = socket(PF_CAN, SOCK_RAW, CAN_RAW);		/* ?????	*/
    if (m_canSocket < 0) {
        perror("socket PF_CAN failed");
        WRITE_ERR_LOG("socket PF_CAN failed\n");
        return;
    }

    struct ifreq ifr;
#ifdef CAN0
    strcpy(ifr.ifr_name, "can0");
#else
    strcpy(ifr.ifr_name, "can1");
#endif
    int ret = ioctl(m_canSocket, SIOCGIFINDEX, &ifr);
    if (ret < 0) {
        perror("ioctl failed");
        WRITE_ERR_LOG("ioctl failed\n");
        return;
    }
    struct sockaddr_can addr;
    addr.can_family = PF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;

    int recv_own_msgs = 0;//set loop back:  1 enable 0 disable
    setsockopt(m_canSocket, SOL_CAN_RAW, CAN_RAW_RECV_OWN_MSGS, &recv_own_msgs, sizeof(recv_own_msgs));
    ret = ::bind(m_canSocket, (struct sockaddr *)&addr, sizeof(addr));
    if (ret < 0) {
        perror("bind failed");
        WRITE_ERR_LOG("bind failed\n");
        return ;
    }
    g_canSocket = m_canSocket;
    pthread_t thread;
    int res = pthread_create(&thread, NULL, _canRecvThreadEnter, NULL);
    if (res != 0)
    {
        WRITE_ERR_LOG("pthread_create failed\n");
    }
}
#endif
#if 0
void CUart::_initCan() 
{
    system("ifconfig can0 down");
    system("ip link set can0 up type can bitrate 1000000 triple-sampling on");
    system("ifconfig can0 up");

    struct sockaddr_can addr;
    //struct ifreq ifr;
    m_canSocket = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    if (m_canSocket < 0) {
        printf("error\n");
        return;
    }

    addr.can_family = AF_CAN;
    int ret;

    struct ifreq ifr;
    strcpy((char *)(ifr.ifr_name), "can0");
    ret = ioctl(m_canSocket, SIOCGIFINDEX, &ifr);  //get index
    if (ret && ifr.ifr_ifindex == 0) {
        printf("Can't get interface index for can0, code= %d, can0 ifr_ifindex value: %d, name: %s\n", ret, ifr.ifr_ifindex, ifr.ifr_name);
        close(m_canSocket);
        return;
    }

    printf("%s can_ifindex = %x\n", ifr.ifr_name, ifr.ifr_ifindex);
    addr.can_ifindex = ifr.ifr_ifindex;
    //ioctl(sock,SIOCGIFNAME,&ifr);
    //printf("ret = %d can0 can_ifname = %s\n",ret,ifr.ifr_name);	

    int recv_own_msgs = 0;//set loop back:  1 enable 0 disable

    setsockopt(m_canSocket, SOL_CAN_RAW, CAN_RAW_RECV_OWN_MSGS, &recv_own_msgs, sizeof(recv_own_msgs));
    if (bind(m_canSocket, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        printf("bind error\n");
        close(m_canSocket);
        return;
    }
    pthread_t thread;
    int res = pthread_create(&thread, NULL, _canRecvThreadEnter, NULL);
    if (res != 0)
    {
        WRITE_ERR_LOG("pthread_create failed\n");
    }
    g_canSocket = m_canSocket;
}
#endif

#if 0
void CUart::_initCan()
{
    system("ifconfig can0 down");
    system("ip link set can0 up type can bitrate 1000000 triple-sampling on");
    system("ifconfig can0 up");

    m_canSocket = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    struct ifreq stIfr;
    strcpy((char *)(stIfr.ifr_name), "can0");
    ioctl(m_canSocket, SIOCGIFINDEX, &stIfr);
    m_stSockAddr.can_family = AF_CAN;
    m_stSockAddr.can_ifindex = stIfr.ifr_ifindex;
    int ret = bind(m_canSocket, (struct sockaddr*)&m_stSockAddr, sizeof(m_stSockAddr));
    if (ret < 0)
    {
        WRITE_ERR_LOG("bind CAN_SOCKET error %d", ret);
    }

    g_canSocket = m_canSocket;
    memcpy(&g_stSockAddr, &m_stSockAddr, sizeof(m_stSockAddr));

    pthread_t thread;
    int res = pthread_create(&thread, NULL, _canRecvThreadEnter, NULL);
    if (res != 0)
    {
        WRITE_ERR_LOG("pthread_create failed\n");
    }
}
#endif
void CUart::CanWrite(char* pData, app_u32 len, app_u32 id)
{
    char* pTemp = pData;
    app_print(ps_can, "\n CAN-SEND-BEGIN: %lu\n", len);

    while (len > 0)
    {
        struct can_frame frame = { 0 };
        frame.can_id = 0x40 | id;
        frame.can_dlc = ((len >= 7) ? 7 : len) + 1;
        frame.data[0] = _getSerial(len);
        memcpy(frame.data + 1, pTemp, frame.can_dlc - 1);
#ifdef COMPILER_IS_ARM_LINUX_GCC       
        //int nbytes = write(m_canSocket, &frame, sizeof(frame));       
        ssize_t sendSize = write(m_canSocket, &frame, sizeof(struct can_frame)); //sendto(m_canSocket, &frame, sizeof(struct can_frame), 0, (struct sockaddr*)&m_stSockAddr, sizeof(m_stSockAddr));
        app_print(ps_can, "SID = %d,Send size=%d,total=%d\n", id, sendSize, sizeof(struct can_frame));
        while(sendSize == -1)
        {
            usleep(1);
            sendSize = write(m_canSocket, &frame, sizeof(struct can_frame)); //sendto(m_canSocket, &frame, sizeof(struct can_frame), 0, (struct sockaddr*)&m_stSockAddr, sizeof(m_stSockAddr));
            app_print(ps_can, "SID = %d,Send size=%d,total=%d\n", id, sendSize, sizeof(struct can_frame));
        }
#endif
        app_print(ps_can, "%2x %2x %2x %2x %2x %2x %2x %2x\n", frame.data[0], frame.data[1], frame.data[2], frame.data[3], frame.data[4], frame.data[5], frame.data[6], frame.data[7]);
        fflush(stdout);
        pTemp = pTemp + frame.can_dlc - 1;
        len = (len >= 7) ? (len - 7) : 0;
    }
    app_print(ps_can, "CAN-SEND-END\n");
}

char CUart::_getSerial(app_u32 len)
{
    static char serial = 1;
    char rSerial = serial;
    if (len <= 7)
    {
        rSerial = 0x80 | serial;
        serial = (serial >= 127) ? 1 : (serial + 1);
    }
    return rSerial;
}

subCan::subCan()
{
    app_mallocBuffer(FORMAT_LEN, (void **)&m_rcvBuffer);
    _initialize(FORMAT_LEN);
}

subCan::subCan(app_u32 len)
{
    app_mallocBuffer(len, (void **)&m_rcvBuffer);
    _initialize(len);
}

subCan::~subCan()
{

}

void subCan::_initialize(app_u32 len)
{
    m_bufferIndex = 0;
    memset(m_rcvBuffer, 0, len);
}

app_u32 subCan::_upToCan()
{
    st_CanFormat *para = (st_CanFormat*)m_rcvBuffer;
    if (para->cmdType == 2)
    {
        WRITE_INFO_LOG("CAN Format ACK MSG \n");
        return 0;
    }
    if (para->uscmdinfolen != (m_bufferIndex - 12))
    {
        printf("id=%02d , Get", para->endwnmachname);
        for (size_t i = 0; i < m_bufferIndex; i++)
        {
            printf("%02x ", *(m_rcvBuffer + i));
        }
        printf("\n");
        WRITE_ERR_LOG("uscmdinfolen is (%d) != m_bufferIndex(%d - 12)\n", para->uscmdinfolen, m_bufferIndex);
        return 1;
    }

    vos_u32 ulvosmsglen = m_bufferIndex;
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
    memcpy((char*)(&(pstmsgbuf->endwnmachname)), (char*)(&(para->endwnmachname)), m_bufferIndex - 4);//去掉开始的4个字节

    msg_stHead rHead;
    app_constructMsgHead(&rHead, Pid_DwnMachUart, Pid_DwnMachUart, app_res_dwnmach_act_cmd_msg_type);

    return app_sendMsg(&rHead, pstmsgbuf, ulvosmsglen);
}


void subCan::_saveResInfo(can_frame *pFrame)
{
    memcpy(m_rcvBuffer + m_bufferIndex, pFrame->data + 1, pFrame->can_dlc - 1);
    m_bufferIndex += pFrame->can_dlc - 1;
}

void subCan::AnalysisData(can_frame *pFrame)
{
    //std::lock_guard<std::mutex> lock(CANMutex);
    char serial = pFrame->data[0];

    //_saveResInfo(pFrame);
    memcpy(m_rcvBuffer + m_bufferIndex, pFrame->data + 1, pFrame->can_dlc - 1);
    m_bufferIndex += pFrame->can_dlc - 1;
    
    if ((serial & 0x80) == 0x80)
    {
        //包接收完，向上传
        if (VOS_OK != _upToCan())
        {
            printf("fid = %x\n", pFrame->data[0]);            
        }
        
        //_initialize();
        m_bufferIndex = 0;
        memset(m_rcvBuffer, 0, FORMAT_LEN);
    }
}
void * _canRecvThreadEnter(void *para)
{
    map <app_u32, subCan*> mSubCan;  
    mSubCan[xc8002_dwnmach_name_smpldisk] = new subCan;
    mSubCan[xc8002_dwnmach_name_smplEmer] = new subCan;
    mSubCan[xc8002_dwnmach_name_smpndl] = new subCan;
    mSubCan[xc8002_dwnmach_name_smpndlb] = new subCan;
    mSubCan[xc8002_dwnmach_name_R2disk] = new subCan;
    mSubCan[xc8002_dwnmach_name_r1Ndl] = new subCan;
    mSubCan[xc8002_dwnmach_name_r1bNdl] = new subCan;
    mSubCan[xc8002_dwnmach_name_R1disk] = new subCan;
    mSubCan[xc8002_dwnmach_name_r2Ndl] = new subCan;
    mSubCan[xc8002_dwnmach_name_r2bNdl] = new subCan;
    mSubCan[xc8002_dwnmach_name_reactdisk] = new subCan;
    mSubCan[xc8002_dwnmach_name_autowash] = new subCan;
    mSubCan[xc8002_dwnmach_name_smpl_mixrod] = new subCan;
    mSubCan[xc8002_dwnmach_name_reag_mixrod] = new subCan;
    mSubCan[xc8002_dwnmach_name_auto_Rtmpctrl] = new subCan;
    mSubCan[xc8002_dwnmach_name_auto_wtmpctrl] = new subCan;
    mSubCan[xc8002_dwnmach_name_cooling_reag] = new subCan;
    mSubCan[xc8002_dwnmach_name_ADOuter] = new subCan(1024 * 10);
    mSubCan[xc8002_dwnmach_name_ADInner] = new subCan(1024 * 10);
    mSubCan[xc8002_dwnmach_name_press] = new subCan;

    mSubCan[xc8002_dwnach_name_ALTspt] = new subCan;
    mSubCan[xc8002_dwnach_name_ALVG] = new subCan;
    mSubCan[xc8002_dwnach_name_ALL1] = new subCan;
    mSubCan[xc8002_dwnach_name_ALL2] = new subCan;
    mSubCan[xc2000_dwnmach_name_Light] = new subCan;

    // CAN数据写
    ofstream out;
    out.open("/opt/CANData.txt", ios::out | ios::trunc);

    fd_set rdfds;
    struct can_frame frame = { 0 };
    while (1)
    {
        FD_ZERO(&rdfds);
        FD_SET(g_canSocket, &rdfds);
        int ret = select(g_canSocket + 1, &rdfds, NULL, NULL, NULL);
        if (ret <= 0) {
            WRITE_ERR_LOG("select time out");
        }

        int nbytes = read(g_canSocket, &frame, 128); //recvfrom(g_canSocket, &frame, sizeof(struct can_frame), 0, (struct sockaddr *)&g_stSockAddr, (socklen_t*)&len);
        //int nbytes = read(g_canSocket, &frame, sizeof(struct can_frame));
        //app_print(ps_can, "==CAN READ== %d,%d\n", nbytes, len);
        //app_print(ps_equipment, "Read id=%d,nbytes =%d,len =%d\n", frame.can_id, nbytes, frame.can_dlc);
        out << &frame;
        out << "\n";
        if (nbytes > 0 && frame.can_id <= 40 )
        {
            if (frame.data[1] != 02 && frame.data[2] != 00 && frame.can_dlc == 3)
            {
                continue;
            }
            //app_print(ps_can, "%2x %2x %2x %2x %2x %2x %2x %2x\n", frame.data[0], frame.data[1], frame.data[2], frame.data[3], frame.data[4], frame.data[5], frame.data[6], frame.data[7]);
            mSubCan[frame.can_id]->AnalysisData(&frame);
        }
        if (frame.can_id > 40)
        {
            WRITE_ERR_LOG("unfind can_id[%d]\n", frame.can_id);
        }
    }
    out.close();
    return NULL;
}