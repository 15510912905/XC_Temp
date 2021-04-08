#include "msg.h"
#include "app_innermsg.h"
#include "app_comstructdefine.h"
#include "app_xc8002shareinfo.h"
#include "app_msg.h"
#include "app_errorcode.h"
#include "vos_timer.h"
#include "apputils.h"
#include"vos_adaptsys.h"
#include"drv_ebi.h"
#include"vos_mngmemory.h"
#include"cmd_downmachine.h"
#include"app_downmsg.h"
#include"looptest.h"
#include "xc8002_middwnmachshare.h"
#include "app_msgtypedefine.h"
#include "app_debugtty.h"
#include "cmd_reactiondisk.h"
#include "vos_pid.h"
#include "vos_init.h"
#include "typedefine.pb.h"
#include <sys/ioctl.h>
#include "vos_log.h"

CLoopTest::CLoopTest(vos_u32 ulPid) : CCallBack(ulPid)
{
    m_ulTimer = createTimer(Pid_LoopTest, "LOOPTEST", LOOPBACK_TIMEROUT_LEN);
    m_ulPressTimer = createTimer(Pid_LoopTest, "PressTest", LOOPBACK_TIMEROUT_LEN);
    for (uint32 i = SPid_SmpNdl; i <= SPid_40; i++)
    {
        m_mLoopState[i] = true;
    }
    
}

CLoopTest::~CLoopTest()
{

}

vos_u32 CLoopTest::CallBack(vos_msg_header_stru* pMsg)
{
	if (VOS_NULL == pMsg)
    {        
		WRITE_ERR_LOG("Input parameter is null \n");        
		return app_para_null_err;    
	}
	
	WRITE_INFO_LOG(" LoopTest: Recevied msg(0x%x) from pid(%u)\n", pMsg->usmsgtype, pMsg->ulsrcpid);
	
	switch(pMsg->usmsgtype)
	{
		case app_req_spsuart_lpbck_tst_msg_type:
			//_procLoopTestReq(pMsg);
            _procPressTest(pMsg);
			break;
		
		case app_res_dwnmach_act_cmd_msg_type:
			_procLoopTestRes(pMsg);
			break;

		case vos_pidmsgtype_timer_timeout:
			_procTimeOut(pMsg);
			break;
        case app_com_device_res_exe_act_msg_type:
            _procPressTestRes(pMsg);
            break;
        case app_req_stop_midmach_msg_type:
            for (uint32 i = SPid_SmpNdl; i <= SPid_40; i++)
            {
                m_mLoopState[i] = true;
            }
            vos_stoptimer(m_ulPressTimer);
            break;

		default:
			WRITE_ERR_LOG("Unknown msg type(%d) \n", pMsg->usmsgtype);
			break;
	}
	return VOS_OK;
}

app_u16 CLoopTest::_setTtl(app_u16 LoopBackLayer)
{
	app_u16 LoopTtl = 64;
	switch (LoopBackLayer)
        {
         	case xc8002_com_loopback_layer_linuxapp:
                LoopTtl -= 1;
                break;

            case xc8002_com_loopback_layer_linuxdrv:
                LoopTtl -= 2;
                break;

            case xc8002_com_loopback_layer_linuxlgc:
                LoopTtl -= 3;
                break;

            case xc8002_com_loopback_layer_ucosdrv:
                LoopTtl -= 4;
                break;

            case xc8002_com_loopback_layer_ucosapp:
                LoopTtl -= 5;
                break;

            default:
                break;
        }
	return LoopTtl;
}

vos_u32 CLoopTest::_sendToUart(LoopTestCmdBufStru * pMsg)
{
	vos_u32 ulrunrslt = VOS_OK;
	msg_stHead rHead;
	LoopTestCmdBufStru* pstmsgbuf = VOS_NULL;
	vos_u32 ulvosmsglen = sizeof(LoopTestCmdBufStru);

	app_constructMsgHead(&rHead, Pid_DwnMachUart, Pid_LoopTest, 
        app_req_dwnmach_act_cmd_msg_type);

	ulrunrslt = app_mallocBuffer(ulvosmsglen, (void**)(&pstmsgbuf));
    if (VOS_OK != ulrunrslt)
    {
        WRITE_ERR_LOG("Call app_mallocBuffer() failed(%d) \n", ulrunrslt);
        return ulrunrslt;
    }
	
    memcpy((char*)(&(pstmsgbuf->endwnmachsubsys)), (char*)&(pMsg->endwnmachsubsys), ulvosmsglen);
    
    return app_sendMsg(&rHead, pstmsgbuf, ulvosmsglen);
}

vos_u32 CLoopTest::_setDeviceLoopTestState(app_u16 DwnName,app_u16 State,app_u16 Layer)
{
	app_i32 uarthdl = g_uartHdl[DwnName];
	vos_u32 uluartmode = State;
	if(Layer == xc8002_com_loopback_layer_linuxlgc)
	{
		ioctl(uarthdl, EBI_FPGA_TRANS_MODE, &uluartmode);
	}
	if(Layer == xc8002_com_loopback_layer_linuxdrv)
	{
		ioctl(uarthdl, EBI_APP_MODE, &uluartmode);
	}
    return VOS_OK;
}

vos_u32 CLoopTest::_procLoopTestReq(vos_msg_header_stru* pReqMsg)
{
	app_start_lpbck_tst_req_msg_stru* pReq = (app_start_lpbck_tst_req_msg_stru*)pReqMsg;
		
	m_SrcPid = pReq->ulsrcpid;
	m_LoopBackNum = LOOPBACKNUM;

	pLoopBuf = new LoopTestCmdBufStru;
		
	pLoopBuf->endwnmachsubsys = pReq->endwnmachname;
	pLoopBuf->cmd = XC8002_COM_DWNMACH_LOOP_CMD;
	pLoopBuf->usttl = _setTtl(pReq->enloopendlayer);
	pLoopBuf->enloopbacklayer = pReq->enloopendlayer;
	
	WRITE_INFO_LOG("LoopTest: pLoopBuf->cmd(0x%x),pLoopBuf->endwnmachname(%d)",pLoopBuf->cmd,pLoopBuf->endwnmachsubsys);
	if(pLoopBuf->enloopbacklayer == xc8002_com_loopback_layer_linuxapp)
	{
		for(int i = 0; i < LOOPBACKNUM; i++)
		{
			printf("\r Uart%d: come from linux drv replay, time<%dms ttl=%d\r\n",pLoopBuf->endwnmachsubsys,LOOPBACK_TIMEROUT_LEN,pLoopBuf->usttl);
		}
		return VOS_OK;
	}
	if((pLoopBuf->enloopbacklayer == xc8002_com_loopback_layer_linuxdrv) || (pLoopBuf->enloopbacklayer == xc8002_com_loopback_layer_linuxlgc))
	{
		_setDeviceLoopTestState(pLoopBuf->endwnmachsubsys,app_uart_tran_mode_loopback,pLoopBuf->enloopbacklayer);
	}
	vos_starttimer(m_ulTimer);
	return _sendToUart((LoopTestCmdBufStru*)&(pLoopBuf->endwnmachsubsys));
}

vos_u32 CLoopTest::_procLoopTestRes(vos_msg_header_stru* pResMsg)
{
	vos_stoptimer(m_ulTimer);
	printf("\r Uart%d: come from linux drv replay, time<%dms ttl=%d\r\n",pLoopBuf->endwnmachsubsys,LOOPBACK_TIMEROUT_LEN,pLoopBuf->usttl);
	
	m_LoopBackNum = m_LoopBackNum - 1;
	if(m_LoopBackNum > 0)
	{
		vos_starttimer(m_ulTimer);
		_sendToUart((LoopTestCmdBufStru*)&(pLoopBuf->endwnmachsubsys));
	}
	else
	{
		if((pLoopBuf->enloopbacklayer == xc8002_com_loopback_layer_linuxdrv) || (pLoopBuf->enloopbacklayer == xc8002_com_loopback_layer_linuxlgc))
		{
			_setDeviceLoopTestState(pLoopBuf->endwnmachsubsys,app_uart_tran_mode_normal,pLoopBuf->enloopbacklayer);
		}
	}
	return VOS_OK;
}

vos_u32 CLoopTest::_procTimeOut(vos_msg_header_stru* pResMsg)
{
    msg_stMachineDebug para;
    para.operation = 0;
    for (uint32 i = SPid_SmpNdl; i <= SPid_40; i++)
    {
        if (m_mLoopState[i] == true)
        {
            m_mLoopState[i] = false;
            app_reqDwmCmd(Pid_LoopTest, i, Act_Debug, sizeof(para), (char*)&para, APP_YES);
        }
        else
        {
            printf("*****PressTest %d: loopback test time out.*****\r\n", i);
        }
    }
    startTimer(m_ulPressTimer,m_ulTimerLen);
// 
// 	vos_stoptimer(m_ulTimer);
// 	printf("\r Uart%d: loopback test time out(%d ms).\r\n",pLoopBuf->endwnmachsubsys,LOOPBACK_TIMEROUT_LEN * 10);
// 	m_LoopBackNum = m_LoopBackNum - 1;
// 	if(m_LoopBackNum > 0)
// 	{
// 		vos_starttimer(m_ulTimer);
// 		_sendToUart((LoopTestCmdBufStru*)&(pLoopBuf->endwnmachsubsys));
// 	}
// 	else
// 	{
// 		if((pLoopBuf->enloopbacklayer == xc8002_com_loopback_layer_linuxdrv) || (pLoopBuf->enloopbacklayer == xc8002_com_loopback_layer_linuxlgc))
// 		{
// 			_setDeviceLoopTestState(pLoopBuf->endwnmachsubsys,app_uart_tran_mode_normal,pLoopBuf->enloopbacklayer);
// 		}
// 	}
	return VOS_OK;
}

vos_u32 CLoopTest::_procPressTest(vos_msg_header_stru* pReqMsg)
{
    app_start_lpbck_tst_req_msg_stru* pReq = (app_start_lpbck_tst_req_msg_stru*)pReqMsg;
    msg_stMachineDebug para;
    para.operation = 0;

    if (pReq->endwnmachname <= (SPid_40 - 900))
    {
        m_mLoopState[pReq->endwnmachname + 900] = false;
        return app_reqDwmCmd(Pid_LoopTest, pReq->endwnmachname + 900, Act_Debug, sizeof(para), (char*)&para, APP_YES);
    }

    for (uint32 i = SPid_SmpNdl; i <= SPid_40; i++)
    {
        if (m_mLoopState[i] == true)
        {
            m_mLoopState[i] = false;
            app_reqDwmCmd(Pid_LoopTest, i, Act_Debug, sizeof(para), (char*)&para, APP_YES);
        }
    }
    m_ulTimerLen = pReq->ustimeoutlen;
    if (m_ulTimerLen == 0)
    {
        return 0;
    }
    return startTimer(m_ulPressTimer, m_ulTimerLen);
}

vos_u32 CLoopTest::_procPressTestRes(vos_msg_header_stru* pMsg)
{
    m_mLoopState[pMsg->ulsrcpid] = true;
    return 0;
}
