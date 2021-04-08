#include "typedefine.pb.h"
#include "upmsgstruct.pb.h"
#include "showave.h"
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
#include "vos_socketcommu.h"
#include "cmd_downmachine.h"


extern CSocketMonitor * g_pMonitor;

CShowave::CShowave(vos_u32 ulPid) : CCallBack(ulPid)
{
	#ifdef COMPILER_IS_LINUX_GCC
	m_chan0Open = 0;
	m_chan1Open = 0;
	#endif
}

vos_u32 CShowave::_procChangePort(vos_msg_header_stru * pReqMsg)
{
	WRITE_INFO_LOG("Enter _procChangePort.\r\n");
	CMsg stMsg;
    stMsg.ParseMsg(pReqMsg);
	STReqConnectPort*  pReq = dynamic_cast<STReqConnectPort*>(stMsg.pPara);

    g_pMonitor->Disconnect(ConnTimeLen, pReq->uiport());

	return VOS_OK;
}

vos_u32 CShowave::_procShowaveReq(vos_msg_header_stru * pReqMsg)
{
	WRITE_INFO_LOG("Enter _procShowaveReq.\r\n");
	m_srcPid = pReqMsg->ulsrcpid;

	CMsg stMsg;
    stMsg.ParseMsg(pReqMsg);
	STReqShowWave*  pReq = dynamic_cast<STReqShowWave*>(stMsg.pPara);

	_ctrlShowave(pReq);

	#ifdef COMPILER_IS_LINUX_GCC

	if(pReq->uichannel() == 0 && pReq->uiswitch())
	{
		m_chan0Open = 1;
		m_chanToSig[0] = pReq->uisignal();
	}
	else if(pReq->uichannel() == 0 && !pReq->uiswitch())
	{
		m_chan0Open = 0;
	}

	if(pReq->uichannel() == 1 && pReq->uiswitch())
	{
		m_chan1Open = 1;
		m_chanToSig[1] = pReq->uisignal();
	}
	else if(pReq->uichannel() == 1 && !pReq->uiswitch())
	{
		m_chan1Open = 0;
	}
		
	
	for(int i = 0; i < 10; i++)
	{
		if(m_chan0Open)
		{
			vos_u32 ulrunrslt = VOS_OK;
			vos_u32 ulmsglength = 0;
			vos_msg_header_stru * pResMsg = VOS_NULL;

			ulmsglength = sizeof(vos_msg_header_stru) + sizeof(xc8002_wave_smp_report_stru);
			ulrunrslt = app_mallocBuffer(ulmsglength,(void * *)&pResMsg);
			if (VOS_OK != ulrunrslt)
			{
	    		WRITE_ERR_LOG("Call app_mallocBuffer() failed(%d) \n", ulrunrslt);
	    		return ulrunrslt;
			}
			xc8002_wave_smp_report_stru * pWave = (xc8002_wave_smp_report_stru *)&(pResMsg->aucmsgload[0]);
			pWave->usstatustype = m_chanToSig[0] + 20;
			for(int i=0; i<(WAVE_SMP_SIZE*32); i++)
			{
				if(pWave->usstatustype == xc8002_dwnmach_report_innerdisk_wave0_type)
				{
					pWave->wave_smp[i/32] = (pWave->wave_smp[i/32] << 1) + (((i/32)<=1) ? (i/32):((i/32)%2));
				}
				else if(pWave->usstatustype == xc8002_dwnmach_report_outerdisk_wave0_type)
				{
					pWave->wave_smp[i/32] = (pWave->wave_smp[i/32] << 1) + 0;
				}
				else if(pWave->usstatustype == xc8002_dwnmach_report_smpdisk_wave0_type)
				{
					pWave->wave_smp[i/32] = (pWave->wave_smp[i/32] << 1) + 1;
				}
			}
		
			_procSigValReport(pResMsg);
			if (VOS_NULL != pResMsg)
			{
	    		app_freeBuffer(pResMsg);
			}
		}
		if(m_chan1Open)
		{
			vos_u32 ulrunrslt = VOS_OK;
			vos_u32 ulmsglength = 0;
			vos_msg_header_stru * pResMsg = VOS_NULL;

			ulmsglength = sizeof(vos_msg_header_stru) + sizeof(xc8002_wave_smp_report_stru);
			ulrunrslt = app_mallocBuffer(ulmsglength,(void * *)&pResMsg);
			if (VOS_OK != ulrunrslt)
			{
	    		WRITE_ERR_LOG("Call app_mallocBuffer() failed(%d) \n", ulrunrslt);
	    		return ulrunrslt;
			}
			xc8002_wave_smp_report_stru * pWave = (xc8002_wave_smp_report_stru *)&(pResMsg->aucmsgload[0]);
			pWave->usstatustype = m_chanToSig[1] + 20;
			for(int i=0; i<(WAVE_SMP_SIZE*32); i++)
			{
				if(pWave->usstatustype == xc8002_dwnmach_report_innerdisk_wave0_type)
				{
					pWave->wave_smp[i/32] = (pWave->wave_smp[i/32] << 1) + (((i/32)<=1) ? (i/32):((i/32)%2));
				}
				else if(pWave->usstatustype == xc8002_dwnmach_report_outerdisk_wave0_type)
				{
					pWave->wave_smp[i/32] = (pWave->wave_smp[i/32] << 1) + 0;
				}
				else if(pWave->usstatustype == xc8002_dwnmach_report_smpdisk_wave0_type)
				{
					pWave->wave_smp[i/32] = (pWave->wave_smp[i/32] << 1) + 1;
				}
			}
		
			_procSigValReport(pResMsg);
			if (VOS_NULL != pResMsg)
			{
	    		app_freeBuffer(pResMsg);
			}
		}
	}
	
	#endif

	return VOS_OK;
}

vos_u32 CShowave::_ctrlShowave(STReqShowWave * pReq)
{
	WRITE_INFO_LOG("Enter _startShowave.\r\n");

	vos_u32 dstPid;
	vos_u32 dwnChannel;
	vos_u32 ulrunrslt = VOS_OK;

	switch(pReq->uisignal())
	{
		case 0:
		{
            dstPid = SPid_R2Dsk;
			dwnChannel = 0;
			m_SigTochan[xc8002_dwnmach_report_innerdisk_wave0_type] = pReq->uichannel();
            break;
		}
		case 1:
		{
            dstPid = SPid_R2Dsk;
			dwnChannel = 1;
			m_SigTochan[xc8002_dwnmach_report_innerdisk_wave1_type] = pReq->uichannel();
            break;
		}
		case 2:
		{
            dstPid = SPid_R1Dsk;
			dwnChannel = 0;
			m_SigTochan[xc8002_dwnmach_report_outerdisk_wave0_type] = pReq->uichannel();
            break;
		}
		case 4:
		{
            dstPid = SPid_SmpDsk;
			dwnChannel = 0;
			m_SigTochan[xc8002_dwnmach_report_smpdisk_wave0_type] = pReq->uichannel();
            break;
		}
        default:
		{
			ulrunrslt = APP_COM_ERR;
            WRITE_ERR_LOG("Unknown Sig (%d) \n", pReq->uichannel());
            break;
        }
	}

	if(ulrunrslt != VOS_OK)
	{
		return ulrunrslt;
	}
	else
	{
		return _sendToDwn(dstPid, dwnChannel, pReq->uiswitch());
	}
}

vos_u32 CShowave::_sendToDwn(vos_u32 dstPid, vos_u32 dwnChannel, vos_u32 switchState)
{
	WRITE_INFO_LOG("Enter _sendToDwn.\r\n");

	vos_u32 ulrunrslt = VOS_OK;
	vos_u32 ulmsglength = 0;
    msg_stHead rHead;

	msg_stSingleActUni* pstdwnmachreqmsg = VOS_NULL;
    msg_stShowave *pdstrst = VOS_NULL;

	ulmsglength = sizeof(msg_stSingleActUni) + sizeof(msg_stShowave);   

	ulrunrslt = app_mallocBuffer(ulmsglength,(void * *)&pstdwnmachreqmsg);
	if (VOS_OK != ulrunrslt)
    {
        WRITE_ERR_LOG("Call app_mallocBuffer() failed(%d) \n", ulrunrslt);
        return ulrunrslt;
    }

	app_constructMsgHead(&rHead, dstPid, SPid_Showave, app_com_device_req_exe_act_msg_type);
	//ÌîÊý¾ÝÄÚÈÝ
    pstdwnmachreqmsg->usActNum = 1;
    pstdwnmachreqmsg->stActHead.usActType = Act_Showave;
	pstdwnmachreqmsg->stActHead.usSingleRes = APP_NO;
    pstdwnmachreqmsg->ulDataLen = sizeof(msg_stShowave) + sizeof(msg_stActHead) ;

	pdstrst = (msg_stShowave*)(&(pstdwnmachreqmsg->astActInfo[0]));
	pdstrst->channel = dwnChannel;
	pdstrst->switch_state = switchState;
    
    return app_sendMsg(&rHead, pstdwnmachreqmsg, ulmsglength);
}

vos_u32 CShowave::_procSigValReport(vos_msg_header_stru * pResMsg)
{
	WRITE_INFO_LOG("Enter _procSigValReport.\r\n");
	xc8002_wave_smp_report_stru * pWave = (xc8002_wave_smp_report_stru *)&(pResMsg->aucmsgload[0]);

	map<vos_u32,vos_u32>::iterator iter = m_SigTochan.find(pWave->usstatustype);
    if (iter != m_SigTochan.end())
    {
		m_stTestResResult.Clear();
        m_stTestResResult.set_uichannel(iter->second);
		WRITE_INFO_LOG("channel = %d\n\n",iter->second);
		for(int i=0; i<(WAVE_SMP_SIZE*32); i++)
		{
			vos_u32 sigval = ((pWave->wave_smp[i/32])>>(31-(i%32))) & 0x1;
			WRITE_INFO_LOG("sigval = %d\n",sigval);
			m_stTestResResult.add_uisigval(sigval);
		}
    }

	return _sendToUpmach();
}

vos_u32 CShowave::_sendToUpmach()
{
	WRITE_INFO_LOG("Enter _sendToUpmach.\r\n");

	sendUpMsg(m_srcPid, SPid_Showave, MSG_ResShowWave, &m_stTestResResult);
    m_stTestResResult.Clear();

	return VOS_OK;
}

vos_u32 CShowave::_procStopReq()
{
// 	_sendToDwn(SPid_InnerDsk, 0, 0);
// 	_sendToDwn(SPid_OuterDsk, 0, 0);
// 	_sendToDwn(SPid_SmpDsk, 0, 0);

	return VOS_OK;
}

vos_u32 CShowave::CallBack(vos_msg_header_stru* pMsg)
{
	if (VOS_NULL == pMsg)
    {
        WRITE_ERR_LOG("Input parameter is null \n");
        return app_para_null_err;
    }

    vos_u32 ulResult = VOS_OK;
    WRITE_INFO_LOG("curpid=%u Recevied msg(0x%x) from pid(%u)\n",SPid_Showave,pMsg->usmsgtype, pMsg->ulsrcpid);
	
    switch (pMsg->usmsgtype)
    {
	case MSG_ReqConnectPort:
		//ulResult = _procChangePort(pMsg);
		break;

	case MSG_ReqShowWave:
		ulResult = _procShowaveReq(pMsg);
		break;

	case app_report_msg_type:
		ulResult = _procSigValReport(pMsg);
		break;

	case app_req_stop_midmach_msg_type:
		_procStopReq();
		break;

        default:
            WRITE_ERR_LOG("Unknown msg type(%d) \n", pMsg->usmsgtype);
            break;
    }
    return ulResult;
}

