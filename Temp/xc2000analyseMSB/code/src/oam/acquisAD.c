//#pragma  once
#include "typedefine.pb.h"
#include "upmsgstruct.pb.h"
#include "acquisAD.h"
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

CAcquisAD::CAcquisAD(vos_u32 ulPid)
	: CCallBack(ulPid)
{
	m_Timer = createTimer(SPid_ShowAD, "TEMP_AD", TEMPAD_TIME_LEN, vos_timer_type_loop);
}
vos_u32 CAcquisAD::CallBack(vos_msg_header_stru* pMsg)
{
	if (VOS_NULL == pMsg)
	{
		WRITE_ERR_LOG("Input parameter is null \n");
		return app_para_null_err;
	}
	vos_u32 ulResult = VOS_OK;

	switch (pMsg->usmsgtype)
	{
	case MSG_ReqShowAD:
		ulResult = _procAcquisAdReq(pMsg);
		break;

	case vos_pidmsgtype_timer_timeout:
		ulResult = _procAdRes();
		break;
	default:
		WRITE_ERR_LOG("Unknown msg type(%d) \n", pMsg->usmsgtype);
		break;
	}	
	return ulResult;
}

vos_u32 CAcquisAD::_procAcquisAdReq(vos_msg_header_stru * pMsg)
{
	vos_u32 ulResult = VOS_OK;
	CMsg stMsg;
	stMsg.ParseMsg(pMsg);
	STReqShowAD*  pReq = dynamic_cast<STReqShowAD*>(stMsg.pPara);

	if (0 == pReq->uiswitch())
	{
		vos_stoptimer(m_Timer);
	}
	else
	{
		vos_starttimer(m_Timer);
		
	}
	return ulResult;
}

vos_u32 CAcquisAD::_procAdRes()
{
	STReqActCmd  stPara;
	stPara.set_uisubsysid(900 );
	stPara.set_uiacttype(Act_DwnQuery);
	stPara.add_uiparams(30);
	return sendInnerMsg(SPid_DwnCmd, Pid_DebugTty, MSG_ReqShowAD, &stPara);
}

