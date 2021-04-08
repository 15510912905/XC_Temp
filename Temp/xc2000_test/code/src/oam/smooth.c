#include "typedefine.pb.h"
#include "upmsgstruct.pb.h"
#include "smooth.h"
#include "msg.h"
#include "app_innermsg.h"
#include "app_comstructdefine.h"
#include "app_xc8002shareinfo.h"
#include "app_msgtypedefine.h"
#include "app_msg.h"
#include <stdio.h>
#include "app_errorcode.h"
#include "vos_timer.h"
#include "apputils.h"
#include "drv_ebi.h"
#include "inituart.h"
#include "vos_adaptsys.h"
#include "socketmonitor.h"
#include <sys/ioctl.h>
#include "xc8002_middwnmachshare.h"
#include "vos_log.h"
#include "task.h"

extern vos_u32 g_ultestmode;
extern map<app_u16, app_u16> g_uartHdl;

CSmooth::CSmooth(vos_u32 ulPid) : CCallBack(ulPid)
{
	
}

vos_u32 CSmooth::_procSmoothReq(vos_msg_header_stru * pReqMsg)
{
	WRITE_INFO_LOG("Enter _procSmoothReq.\r\n");
	m_srcPid = pReqMsg->ulsrcpid;

	//CMsg stMsg;
    //stMsg.ParseMsg(pReqMsg);
	//STReqSmoothAreaTest*  pReq = dynamic_cast<STReqSmoothAreaTest*>(stMsg.pPara);

	m_circleNum = 5;
	WRITE_INFO_LOG("Enter _procSmoothReq,m_circleNum = %d.\r\n",m_circleNum);

	app_reqReset(SPid_Smooth, SPid_ReactDsk, APP_YES); 

	return VOS_OK;
}

vos_u32 CSmooth::_procResetRes()
{
	WRITE_INFO_LOG("Enter _procResetRes.\r\n");

	_timeDelay(DELAY_TIME_LEN_1000MS);

	m_cupId = 89;
	m_busy = 0;
	setAdMode(app_ad_fpga_work_mode_smooth);
	_openAdStart();
	_rttReacDsk();

	return VOS_OK;
}

vos_u32 CSmooth::_openAdStart()
{
	WRITE_INFO_LOG("Enter _openAdStart.\r\n");

	g_ultestmode = TESTMODE_SMOOTH;

	vos_u32 fpgahdl = g_uartHdl[xc8002_dwnmach_name_fpga_ad]; 
	vos_u32 uladstart = 3;
	ioctl(fpgahdl, AD_START, &uladstart);

	return VOS_OK;
}

vos_u32 CSmooth::_closeAdStart()
{
	WRITE_INFO_LOG("Enter _closeAdStart.\r\n");

	vos_u32 fpgahdl = g_uartHdl[xc8002_dwnmach_name_fpga_ad]; 
	vos_u32 uladstart = 2;
	ioctl(fpgahdl, AD_START, &uladstart);

	return VOS_OK;
}

vos_u32 CSmooth::_rttReacDsk()
{
	WRITE_INFO_LOG("Enter _rttReacDsk.\r\n");

	act_stReactsysDskRtt stPara;
    stPara.usRttedCupNum = APP_RTT_RCTCUP_PER_PERIOD;
    return app_reqDwmCmd(SPid_Smooth, SPid_ReactDsk, Act_Move, sizeof(act_stReactsysDskRtt), (char*)&stPara, APP_YES);
}

vos_u32 CSmooth::_procDwnActRes()
{
	WRITE_INFO_LOG("Enter _procDwnActRes.\r\n");

	_closeAdStart();
	
#ifdef COMPILER_IS_LINUX_GCC
	_procAdRes();
#else
	_timeDelay(DELAY_TIME_LEN_1000MS);
#endif

	if(m_circleNum > 0)
	{
		m_circleNum--;
	}
	
	if(m_circleNum == 0)
	{
		setAdMode(app_ad_fpga_work_mode_nml);
	}
	else
	{
		_openAdStart();
		_rttReacDsk();	
	}
	
	return VOS_OK;
}

vos_u32 CSmooth::_timeDelay(vos_u32 timeLen)
{
	vos_threadsleep(timeLen);
	return VOS_OK;
}

vos_u32 CSmooth::_procAdRes()
{
	WRITE_INFO_LOG("Enter _procAdRes.\r\n");
	
	//从共享内存中取数据
    vos_u32 collectCupNumOnce = 0;
    vos_u32 collectAdValueOnce[82*14] = {0};

	GetAbsValueFromShareMem(&collectCupNumOnce, (vos_u32*)collectAdValueOnce, sizeof(collectAdValueOnce));

    for (vos_u32 i = 0; i < (collectCupNumOnce * 14); i++)
	{
		if(m_circleNum != 5 && m_cupId == 89 && ((collectAdValueOnce[i] >> 31) & 0x1))//转满一圈后，后续数据不再处理
		{
			m_busy = 1;
		}
		if(m_busy == 1)
		{
			break;
		}
		
		m_stTestResResult.add_uiadval(i);
		if(((collectAdValueOnce[i] >> 31) & 0x1))//如果带触发信号，将杯位号填入
		{
			m_stTestResResult.set_uiadval(i, collectAdValueOnce[i] + ((m_cupId<<16) & 0xff0000));
			if(m_cupId == 1)
			{
				m_cupId = 165;
			}
			else
			{
				m_cupId--;
			}
		}
		else
		{
			m_stTestResResult.set_uiadval(i, collectAdValueOnce[i]);
		}
	}

	_sendToUpmach();
    
	return VOS_OK;
}

vos_u32 CSmooth::_sendToUpmach()
{
	WRITE_INFO_LOG("Enter _sendToUpmach.\r\n");
	sendUpMsg(m_srcPid, SPid_Showave, MSG_ResSmoothAreaTest, &m_stTestResResult);
    m_stTestResResult.clear_uiadval();

	return VOS_OK;
}

vos_u32 CSmooth::CallBack(vos_msg_header_stru* pMsg)
{
	if (VOS_NULL == pMsg)
    {
        WRITE_ERR_LOG("Input parameter is null \n");
        return app_para_null_err;
    }

    vos_u32 ulResult = VOS_OK;
    WRITE_INFO_LOG("curpid=%u Recevied msg(0x%x) from pid(%u)\n",SPid_Smooth,pMsg->usmsgtype, pMsg->ulsrcpid);
	
    switch (pMsg->usmsgtype)
    {
        case MSG_ReqSmoothAreaTest:
            ulResult = _procSmoothReq(pMsg);
            break;
			
		case MSG_ResReset:
			ulResult = _procResetRes();
			break;
			
		case app_com_device_res_exe_act_msg_type:
            ulResult = _procDwnActRes();
            break;
			
		case app_com_ad_data_ready_notice_msg_type:
            ulResult = _procAdRes();


            break;

        default:
            WRITE_ERR_LOG("Unknown msg type(%d) \n", pMsg->usmsgtype);
            break;
    }
    return ulResult;
}

