#include "updatedwn.h"
#include "vos_log.h"
#include "vos_init.h"
#include "apputils.h"
#include "app_msg.h"
#include "socketmonitor.h"
#include "app_errorcode.h"
#include "oammng.h"
#include <fcntl.h>
#include "upmsg.pb.h"
#include "op_flash.h"
#include "vos_adaptsys.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "app_debugcmd.h"


extern string g_szUpMachIp;
typedef enum
{
    FPGA_RC = 0, //试剂板逻辑
    FPGA_SC = 1, //样本板逻辑
    FPGA_WB = 2, //温控板逻辑
    FPGA_MB = 3  //中位机逻辑
}FPGA_type;

typedef struct
{
    vos_u16 execmd;
    vos_u16 rsv1;
    vos_u32 dataPage;
}updateRes;

CUpdateMachine::CUpdateMachine(vos_u32 ulPid) : CCallBack(ulPid)
{
    BinFileName[SPid_SmpDsk] = "sampdisk.bin";
    BinFileName[SPid_SmpNdl] = "sampndl.bin";
    BinFileName[SPid_R2Dsk] = "innerreag.bin";
    BinFileName[SPid_R1] = "r1.bin";
    BinFileName[SPid_R1Dsk] = "outreag.bin";
    BinFileName[SPid_R2] = "r2.bin";
    BinFileName[SPid_ReactDsk] = "reatdisk.bin";
    BinFileName[SPid_AutoWash] = "autowashl.bin";
    BinFileName[SPid_SMix] = "sampmix.bin";
    BinFileName[SPid_R2Mix] = "reagmix.bin";
    BinFileName[SPid_RTmpCtrl] = "tempctl.bin";
    BinFileName[SPid_CoolReag] = "cooling.bin";
    BinFileName[SPid_SmpDsk - 900] = "fpga_s.bin";
    BinFileName[SPid_R1Dsk - 900] = "fpga_r.bin";
    BinFileName[SPid_R2Mix - 900] = "fpga_t.bin";
    BinFileName[MidFPGA] = "fpga_m.bin";
	BinFileName[MidAPP] = "xc_app";

    exeUpdate[SPid_SmpDsk] = NULL;
    exeUpdate[SPid_SmpNdl] = NULL;
    exeUpdate[SPid_R2Dsk] = NULL;
    exeUpdate[SPid_R1] = NULL;
    exeUpdate[SPid_R1Dsk] = NULL;
    exeUpdate[SPid_R2] = NULL;
    exeUpdate[SPid_ReactDsk] = NULL;
    exeUpdate[SPid_AutoWash] = NULL;
    exeUpdate[SPid_SMix] = NULL;
    exeUpdate[SPid_R2Mix] = NULL;
    exeUpdate[SPid_RTmpCtrl] = NULL;
    exeUpdate[SPid_CoolReag] = NULL;
	exeUpdate[SPid_SmpDsk - 900] = NULL;
    exeUpdate[SPid_R1Dsk - 900] = NULL;
    exeUpdate[SPid_R2Mix - 900] = NULL;
    exeUpdate[MidFPGA] = NULL;	
	exeUpdate[MidAPP] = NULL;
}

vos_u32 CUpdateMachine::CallBack(vos_msg_header_stru* pMsg)
{
    switch (pMsg->usmsgtype)
    {
    case MSG_UpdateDwn:
        return _updateMachine(pMsg);
    case app_com_device_res_exe_act_msg_type:
        return _procDwnAct(pMsg->ulsrcpid, (app_com_dev_act_result_stru *)pMsg->aucmsgload);
    case MSG_GetVres:
        return _getBootloaderVres();
    case app_req_stop_midmach_msg_type:
        return _stopUpdate();
    default:
        WRITE_ERR_LOG("undefine MsgType (%#x)\n", pMsg->usmsgtype);
        break;
    }
    return VOS_OK;
}

vos_u32 CUpdateMachine::_updateMachine(vos_msg_header_stru* pMsg)
{
    CMsg stMsg;
    stMsg.ParseMsg(pMsg);
    CDwnUpdate*  pReq = dynamic_cast<CDwnUpdate*>(stMsg.pPara);
    for (vos_i32 loop = 0; loop < pReq->uisubsysid_size(); loop++)
    {   
        _sendStart(pReq->uisubsysid(loop));
    }
    return VOS_OK;
}

vos_u32 CUpdateMachine::_sendStart(vos_u32 subsysId)
{
    subsysId = (subsysId == SPid_CoolReag + 1) ? (SPid_SmpDsk - 900)
        : (subsysId == SPid_CoolReag + 2) ? (SPid_R1Dsk - 900)
        : (subsysId == SPid_CoolReag + 3) ? (SPid_R2Mix - 900)
        : (subsysId == SPid_CoolReag + 4) ? (MidFPGA)
		: (subsysId == SPid_CoolReag + 5) ? (MidAPP)
        : subsysId;

    if (exeUpdate[subsysId] != NULL)
    {
        WRITE_ERR_LOG("(%lu)is in the update ,please stop first\n", subsysId);
        return VOS_OK;
    }

    _sendState(subsysId, UR_ing);

    if (subsysId >= SPid_SmpDsk && subsysId <= SPid_CoolReag)//下位机
    {
        exeUpdate[subsysId] = new CUpdateDwn(BinFileName[subsysId], subsysId);
        return exeUpdate[subsysId]->SendExeCmd(BOOTLOADER_CMD_UPDATE_START);
    }
    else if (subsysId == SPid_SmpDsk - 900 || subsysId == SPid_R1Dsk - 900 || subsysId == SPid_R2Mix - 900)//下位机逻辑
    {
        exeUpdate[subsysId] = new CUpdateDwnFpga(BinFileName[subsysId], subsysId);
        return exeUpdate[subsysId]->SendExeCmd(fpga_update_flag_fst);
    }
    else if (subsysId == MidFPGA)//中位机逻辑
    {
        return _updateMidFpga(subsysId);
    }
    else if (subsysId == MidAPP)//中位机app
	{
		return _updateMidApp(subsysId);
	}

    return VOS_OK;
}

vos_u32 CUpdateMachine::_updateMidApp(vos_u32 subsysId)
{
	char  aucipaddr[VOS_MAX_IP_BUF_LEN] = { 0 };

	g_szUpMachIp.copy(aucipaddr, g_szUpMachIp.size(), 0);

	_sendState(subsysId, UR_success);

	return app_updateapp(aucipaddr);
}

vos_u32 CUpdateMachine::_updateMidFpga(vos_u32 subsysId)
{
    exeUpdate[subsysId] = new CUpdateMidFpga(BinFileName[subsysId], subsysId);
    if (VOS_OK == exeUpdate[subsysId]->SendData(exeUpdate[subsysId]->SendExeCmd(MidFPGA_CMD_ERASE)))
    {
        _sendState(subsysId, UR_success);
    }
    else
    { 
        _sendState(subsysId, UR_failed);
    }
    delete(exeUpdate[MidFPGA]);
    exeUpdate[MidFPGA] = NULL;
    return VOS_OK;
}

vos_u32 CUpdateMachine::_procDwnAct(vos_u32 srcpid, app_com_dev_act_result_stru *pMsg)
{
    if (pMsg->enacttype == Act_UpdateDwn)
    {
    	return _procDwnUpdate(pMsg, srcpid);
    }

    if (pMsg->enacttype == Act_UpdateFpga)
    {
        return _procFpgaUpdate(pMsg, srcpid-900);
    }
    return VOS_OK;
}

vos_u32 CUpdateMachine::_procDwnUpdate(app_com_dev_act_result_stru * pMsg, vos_u32 srcpid)
{
    updateRes* pRes = (updateRes*)pMsg->aucrsltinfo;
    if (exeUpdate[srcpid] == NULL)
    {
        return VOS_OK;
    }

    switch (pRes->execmd)
    {
    case BOOTLOADER_CMD_UPDATE_START:
        if (app_action_res_success != pMsg->enactrslt)
        {
			_sendState(srcpid, UR_failed);
            delete(exeUpdate[srcpid]);
            exeUpdate[srcpid] = NULL;
            break;
        }
        vos_threadsleep(800);//等待下位机进入bootloader
        exeUpdate[srcpid]->SendExeCmd(BOOTLOADER_CMD_ERASE);
        break;
    case BOOTLOADER_CMD_ERASE:
        if (app_action_res_success != pMsg->enactrslt)
        {
			_sendState(srcpid, UR_failed);
            delete(exeUpdate[srcpid]);
            exeUpdate[srcpid] = NULL;
            break;
        }
    case BOOTLOADER_CMD_WRITE_MEMORY:
        if (VOS_INVALID_U32 == exeUpdate[srcpid]->SendData(pMsg->enactrslt))
        {
			_sendState(srcpid, UR_failed);
            delete(exeUpdate[srcpid]);
            exeUpdate[srcpid] = NULL;
        }
        break;
    case BOOTLOADER_CMD_GO:
    {
		vos_threadsleep(2000);//等待下位机重启
		SENDBOOTFINISH2DOWNM();
        printf("Run(%lu)!\n", srcpid);

		_sendState(srcpid, UR_success);

        delete(exeUpdate[srcpid]);
        exeUpdate[srcpid] = NULL;
    }
        break;
    default:
        break;
    }
    return VOS_OK;
}

vos_u32 CUpdateMachine::_procFpgaUpdate(app_com_dev_act_result_stru * pMsg, vos_u32 srcpid)
{
    updateRes* pRes = (updateRes*)pMsg->aucrsltinfo;
    if (exeUpdate[srcpid] == NULL)
    {
        return VOS_OK;
    }
    printf("execmd = %d\n", pRes->execmd);
    switch (pRes->execmd)
    {
    case fpga_update_flag_fst:
        if (app_action_res_success != pMsg->enactrslt)
        {
            printf("start!\n");
			_sendState(srcpid, UR_failed);
            delete(exeUpdate[srcpid]);
            exeUpdate[srcpid] = NULL;
            break;
        }
    case fpga_update_flag_dat:
        if (VOS_INVALID_U32 == exeUpdate[srcpid]->SendData(pMsg->enactrslt))
        {
			_sendState(srcpid, UR_failed);
            delete(exeUpdate[srcpid]);
            exeUpdate[srcpid] = NULL;
        }
        break;
    case fpga_update_flag_end:
    {
        printf("Run FPGA(%lu)!\n", srcpid);

        _sendState(srcpid, pMsg->enactrslt);

        delete(exeUpdate[srcpid]);
        exeUpdate[srcpid] = NULL;
    }
        break;
    }
    return VOS_OK;
}

vos_u32 CUpdateMachine::_getBootloaderVres()
{
    //进入bootloader
    msg_updateDwn updateDwn = { 0 };
    updateDwn.exeCmd = BOOTLOADER_CMD_UPDATE_START;
    updateDwn.dataPage = 0;
    for (vos_u32 subPid = SPid_SmpDsk; subPid <= SPid_CoolReag; subPid++)
    {
    	app_reqDwmCmd(Spid_UpdateDwn, subPid, Act_UpdateDwn, sizeof(msg_updateDwn), (char*)&updateDwn, VOS_NO);
    }
    vos_threadsleep(800);//等待下位机进入bootloader

    sendInnerMsg(SPid_DwnCmd, Spid_UpdateDwn, MSG_GetVres, NULL);
    vos_threadsleep(500);//等待版本查询

    updateDwn.exeCmd = BOOTLOADER_CMD_GO;
    for (vos_u32 subPid = SPid_SmpDsk; subPid <= SPid_CoolReag; subPid++)
    {
    	app_reqDwmCmd(Spid_UpdateDwn, subPid, Act_UpdateDwn, sizeof(msg_updateDwn), (char*)&updateDwn, VOS_NO);
    }
    vos_threadsleep(500);//等待启动

    SENDBOOTFINISH2DOWNM();
    return VOS_OK;
}

vos_u32 CUpdateMachine::_stopUpdate()
{
    for (vos_u32 sysId = SPid_SmpDsk; sysId <= SPid_CoolReag; sysId++)
    {
        //停止下位机逻辑升级
        if ((sysId == SPid_SmpDsk || sysId == SPid_R1Dsk || sysId == SPid_R2Mix) && (exeUpdate[sysId - 900] != NULL))
        {
            vos_u32 fpgaId = sysId - 900;
            WRITE_ERR_LOG("stop update FPGA(%lu)!\n", fpgaId);
            _stopUpdate(fpgaId);
        }
        //停止下位机升级
        if (exeUpdate[sysId] != NULL)
        {
            WRITE_ERR_LOG("stop update(%lu)!\n", sysId);
            _stopUpdate(sysId);
        }
    }
    //停止中位机逻辑升级
    if (exeUpdate[MidFPGA] != NULL)
    {
        WRITE_ERR_LOG("stop update MidFPGA!\n");
        _stopUpdate(MidFPGA);
    }
    return VOS_OK;
}

void CUpdateMachine::_stopUpdate(vos_u32 sysId)
{ 
    delete(exeUpdate[sysId]);
    exeUpdate[sysId] = NULL;
    _sendState(sysId, UR_stop);
}

void CUpdateMachine::_sendState(vos_u32 subPid, vos_u32 state)
{
    CDwnUpdateRes resUpdate;
    STKeyValue *res = resUpdate.mutable_updateres();

    subPid = (subPid == SPid_SmpDsk - 900) ? (SPid_CoolReag + 1)
        : (subPid == SPid_R1Dsk - 900) ? (SPid_CoolReag + 2)
        : (subPid == SPid_R2Mix - 900) ? (SPid_CoolReag + 3)
        : (subPid == MidFPGA) ? (SPid_CoolReag + 4)
		: (subPid == MidAPP) ? (SPid_CoolReag + 5)
        : subPid;

    res->set_uikey(subPid);
    res->set_uival(state);
    sendUpMsg(CPid_Maintain, Spid_UpdateDwn, MSG_UpdateDwn, &resUpdate);
}

CExeUpdate::CExeUpdate(string fileName, vos_u32 dispid)
{
    //获取bin文件
#ifdef COMPILER_IS_ARM_LINUX_GCC
    string syscmd = "tftp -g -r " + fileName + " " + g_szUpMachIp;
    system(syscmd.c_str());
#endif
	
    //打开bin文件
	binfile = fileName;
    dwnfile = open(binfile.c_str(), O_RDONLY);
    subPid = dispid;
    dataPage = 0;
}

CExeUpdate::~CExeUpdate()
{
    if (-1 != dwnfile && 0 != dwnfile)
    {
        close(dwnfile);
#ifdef COMPILER_IS_ARM_LINUX_GCC
        string syscmd = "rm " + binfile;
        system(syscmd.c_str());
#endif
    }
}

vos_u32 CUpdateDwn::SendExeCmd(vos_u16 exeCmd)
{
    if (dwnfile == -1 || dwnfile == 0)
    {
        WRITE_ERR_LOG("update(%d) failed! can not open<%s>\n", subPid, binfile.c_str());
        return VOS_INVALID_U32;
    }

    msg_updateDwn updateDwn = { 0 };
    updateDwn.exeCmd = exeCmd;
    updateDwn.dataPage = 0;
    return app_reqDwmCmd(Spid_UpdateDwn, subPid, Act_UpdateDwn, sizeof(msg_updateDwn), (char*)&updateDwn, VOS_YES);
}

vos_u32 CUpdateDwn::SendData(app_u16 rslt)
{
    static vos_u32 failedCount = 0;
    msg_updateDwn updateDwn = { 0 };

    if (app_action_res_success == rslt)
    {
        failedCount = 0;
        dataPage++;
    }
    else
    {
        if (++failedCount >= 3)
        {
            WRITE_ERR_LOG("update(%u) failed! write flash error!\n", subPid);
            return  VOS_INVALID_U32;
        }
        lseek(dwnfile, -DWNPAGE_SIZE, SEEK_CUR);//上一次没传成功，向前移动256，重新发送。
    }

    if (0 == read(dwnfile, updateDwn.data, DWNPAGE_SIZE))
    {
        return SendExeCmd(BOOTLOADER_CMD_GO);
    }

    updateDwn.exeCmd = BOOTLOADER_CMD_WRITE_MEMORY;
    updateDwn.dataPage = dataPage;
    return app_reqDwmCmd(Spid_UpdateDwn, subPid, Act_UpdateDwn, sizeof(msg_updateDwn), (char*)&updateDwn, VOS_YES);
}

vos_u32 CUpdateDwnFpga::SendData(app_u16 rslt)
{
    if (dwnfile == -1 || dwnfile == 0)
    {
        WRITE_ERR_LOG("update DwnFPGA failed! can not open<%s>\n", binfile.c_str());
        return VOS_INVALID_U32;
    }

    static vos_u32 failedCount = 0;
    msg_updateFpga updateFpga = { 0 };
    if (app_action_res_success == rslt)
    {
        failedCount = 0;
        dataPage++;
    }
    else
    {
        if (++failedCount >= 3)
        {
            WRITE_ERR_LOG("update FPGA(%u) failed! write flash error!\n", subPid+900);
            return  VOS_INVALID_U32;
        }
        lseek(dwnfile, -DWNPAGE_SIZE, SEEK_CUR);//上一次没传成功，向前移动256，重新发送。
    }

    

    updateFpga.length = read(dwnfile, updateFpga.data, DWNPAGE_SIZE);
    if (0 == updateFpga.length)
    {
        printf("send end!\n");
        return SendExeCmd(fpga_update_flag_end);
    }
    printf("send date[%lu]!\n", dataPage);
    updateFpga.flag = fpga_update_flag_dat;
    updateFpga.dataPage = dataPage;
    return app_reqDwmCmd(Spid_UpdateDwn, subPid+900, Act_UpdateFpga, sizeof(msg_updateFpga), (char*)&updateFpga, VOS_YES);

}

vos_u32 CUpdateDwnFpga::SendExeCmd(vos_u16 exeCmd)
{
    if (dwnfile == -1 || dwnfile == 0)
    {
        WRITE_ERR_LOG("update FPGA(%d) failed! can not open<%s>\n", subPid+900, binfile.c_str());
        return VOS_INVALID_U32;
    }

    msg_updateFpga updateFpga = { 0 };
    updateFpga.flag = exeCmd;
    updateFpga.dataPage = 0;
    return app_reqDwmCmd(Spid_UpdateDwn, subPid+900, Act_UpdateFpga, sizeof(msg_updateDwn), (char*)&updateFpga, VOS_YES);
}


vos_u32 CUpdateMidFpga::SendData(app_u16 rslt)
{
    if (dwnfile == -1 || dwnfile == 0)
    {
        WRITE_ERR_LOG("update MidFPGA failed! can not open<%s>\n", binfile.c_str());
        return VOS_INVALID_U32;
    }

    vos_u32 len = 0;
    struct stat statbuff;
    if (stat((const char *)(binfile.c_str()), &statbuff) < 0)
    {
        return VOS_INVALID_U32;
    }
    len = statbuff.st_size;
    printf("file len %lu\n",len);

    char *buff = (char*)malloc(len);
    if (buff == NULL)
    {
        return VOS_INVALID_U32;
    }
    if ((int)len != read(dwnfile, buff, len))
    {
        free(buff);
        return VOS_INVALID_U32;
    }
    free(buff);
#ifdef COMPILER_IS_ARM_LINUX_GCC
    m25p16_op(MidFPGA_CMD_WRITE, buff, len);
    //复位逻辑
    Enable_PB();
    Disable_PB();
#endif
    return VOS_OK;
}

vos_u32 CUpdateMidFpga::SendExeCmd(vos_u16 exeCmd)
{
    if (exeCmd == MidFPGA_CMD_ERASE)
    {
#ifdef COMPILER_IS_ARM_LINUX_GCC
        return m25p16_op(MidFPGA_CMD_ERASE, NULL, 0);
#endif
    }
    return VOS_OK;
}

// CUpdateMidFpga::CUpdateMidFpga(string fileName, vos_u32 dispid)
// {
//     //创建升级线程
//     vos_multi_thread_para_stru stthdinfo;
//     stthdinfo.pthrdfunc = CUpdateMidFpga::UpdateMidFpga();
//     stthdinfo.pthrdinpara = VOS_NULL;
//     stthdinfo.ulstacksize = 512 * 512;
//     stthdinfo.ulthreadpri = vos_thread_pri_65;
//     stthdinfo.arglen = 0;
//     stthdinfo.eninitstatus = vos_thread_init_status_running;
//     vos_u32 ulrunrslt = vos_createthread(&stthdinfo);
//     if (VOS_OK != ulrunrslt)
//     {
//         CUpdateMachine::_sendState(subPid, UR_failed);
//     }
// }

// vos_u32 CUpdateMidFpga::UpdateMidFpga()
// {
//     if (VOS_OK != SendExeCmd(MidFPGA_CMD_ERASE))
//     {
//         WRITE_ERR_LOG("MidFPGA_CMD_ERASE failed!\n");
//         CUpdateMachine::_sendState(subPid, UR_failed);
//     }
//     if (VOS_OK != SendData(VOS_OK))
//     {
//         WRITE_ERR_LOG("MidFPGA_CMD_WRITE failed!\n");
//         CUpdateMachine::_sendState(subPid, UR_failed);
//     }
//     CUpdateMachine::_sendState(subPid, UR_success);
//     return VOS_EXIST_THREAD_CODE;
// }
