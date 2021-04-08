#include "singledwncmd.h"
#include "msg.h"
#include "app_innermsg.h"
#include "app_comstructdefine.h"
#include "app_xc8002shareinfo.h"
#include "app_msgtypedefine.h"
#include "app_msg.h"
#include <stdio.h>
#include "app_errorcode.h"
#include "apputils.h"
#include "vos_adaptsys.h"
#include "vos_log.h"

using namespace std;

vos_u32 CChooseModeStrategy::_chooseModeByActtype(vos_u32 dstPid, app_u16 actType, app_u16 para0, app_u16 para1)
{
	switch(actType)
	{
		case Act_NdlVrt:
		{
			CConstructCmdMode0 * pMode0 = new CConstructCmdMode0;
			pMode0->_constructCmdPara(dstPid, actType, para0, para1);

			delete pMode0;
			pMode0 = NULL;
			break;
		}
		default :
		{
			CConstructCmdMode1 * pMode1 = new CConstructCmdMode1;
			pMode1->_constructCmdPara(dstPid, actType, para0, para1);

			delete pMode1;
			pMode1 = NULL;
			break;
		}	
	}
	return VOS_OK;
}

vos_u32 CConstructCmdMode0::_constructCmdPara(vos_u32 dstPid, app_u16 actType, app_u16 para0, app_u16 para1)
{
	vos_u32 ulrunrslt = VOS_OK;
	vos_u32 ulmsglength = 0;
    msg_stHead rHead;

	msg_stSingleActUni* pstdwnmachreqmsg = VOS_NULL;
    paraMode0 *pdstrst = VOS_NULL;

	ulmsglength = sizeof(msg_stSingleActUni) + sizeof(paraMode0);   

	ulrunrslt = app_mallocBuffer(ulmsglength,(void * *)&pstdwnmachreqmsg);
	if (VOS_OK != ulrunrslt)
    {
        WRITE_ERR_LOG("Call app_mallocBuffer() failed(%d) \n", ulrunrslt);
        return ulrunrslt;
    }

	app_constructMsgHead(&rHead, dstPid, Pid_DebugTty, app_com_device_req_exe_act_msg_type);
	//填数据内容
    pstdwnmachreqmsg->usActNum = 1;
    pstdwnmachreqmsg->stActHead.usActType = actType;
	pstdwnmachreqmsg->stActHead.usSingleRes = APP_YES;
    pstdwnmachreqmsg->ulDataLen = sizeof(paraMode0) + sizeof(msg_stActHead) ;

	pdstrst = (paraMode0*)(&(pstdwnmachreqmsg->astActInfo[0]));
	pdstrst->para0 = para0;
	pdstrst->para1 = para1;
    
    return app_sendMsg(&rHead, pstdwnmachreqmsg, ulmsglength);;
}

vos_u32 CConstructCmdMode1::_constructCmdPara(vos_u32 dstPid, app_u16 actType, app_u16 para0, app_u16 para1)
{
	vos_u32 ulrunrslt = VOS_OK;
	vos_u32 ulmsglength = 0;
    msg_stHead rHead;

	msg_stSingleActUni* pstdwnmachreqmsg = VOS_NULL;
    paraMode1 *pdstrst = VOS_NULL;

	ulmsglength = sizeof(msg_stSingleActUni) + sizeof(paraMode1);   

	ulrunrslt = app_mallocBuffer(ulmsglength,(void * *)&pstdwnmachreqmsg);
	if (VOS_OK != ulrunrslt)
    {
        WRITE_ERR_LOG("Call app_mallocBuffer() failed(%d) \n", ulrunrslt);
        return ulrunrslt;
    }

	app_constructMsgHead(&rHead, dstPid, Pid_DebugTty, app_com_device_req_exe_act_msg_type);
	//填数据内容
    pstdwnmachreqmsg->usActNum = 1;
    pstdwnmachreqmsg->stActHead.usActType = actType;
	pstdwnmachreqmsg->stActHead.usSingleRes = APP_YES;
    pstdwnmachreqmsg->ulDataLen = sizeof(paraMode1) + sizeof(msg_stActHead) ;

	pdstrst = (paraMode1*)(&(pstdwnmachreqmsg->astActInfo[0]));
	pdstrst->para0 = para0;
	pdstrst->para1 = para1;
    
    return app_sendMsg(&rHead, pstdwnmachreqmsg, ulmsglength);;
}
