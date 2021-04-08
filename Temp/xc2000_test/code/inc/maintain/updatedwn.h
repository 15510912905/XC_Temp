#ifndef __UPDATEDWN_H__
#define __UPDATEDWN_H__

#include "vos_pid.h"

#include "msg.h"
#include "app_comstructdefine.h"

#define BOOTLOADER_CMD_INIT 					(0x7F)
#define BOOTLOADER_CMD_GET_COMMAND  			(0x00)
#define BOOTLOADER_CMD_GETVERSION               (0x01)
#define BOOTLOADER_CMD_GET_ID 					(0x02)
#define BOOTLOADER_CMD_READ_MEMORY 				(0x11)
#define BOOTLOADER_CMD_WRITE_MEMORY 			(0x31)
#define BOOTLOADER_CMD_GO 						(0x21)
#define BOOTLOADER_CMD_ERASE  					(0x43)
#define BOOTLOADER_CMD_EXTENDED_ERASE 			(0x44)
#define BOOTLOADER_CMD_WRITE_PROTECT 			(0x63)
#define BOOTLOADER_CMD_WRITE_UNPROTECT 			(0x73)
#define BOOTLOADER_CMD_READOUT_PROTECT			(0x82)
#define BOOTLOADER_CMD_READOUT_UNPROTECT 		(0x92)
#define BOOTLOADER_CMD_UPDATE_START             (0x93)

#define MidFPGA (0xf1)
#define MidAPP  (0xf2)

typedef enum
{
    fpga_update_flag_fst = 0u,//开始 
    fpga_update_flag_dat = 1u,//数据
    fpga_update_flag_end = 2u,//完成
} fpga_update_flag_enum;

class CExeUpdate
{
public:
    CExeUpdate(string fileName, vos_u32 dispid);
    virtual ~CExeUpdate();

    virtual vos_u32 SendData(app_u16 rslt) { return VOS_OK; }
    virtual vos_u32 SendExeCmd(vos_u16 exeCmd){ return VOS_OK; }

public:
    int dwnfile;
    vos_u32 dataPage;
    vos_u32 subPid;
    string binfile;
};

class CUpdateDwn:public CExeUpdate
{
public:
    CUpdateDwn(string fileName, vos_u32 dispid) :CExeUpdate(fileName, dispid){}

    virtual vos_u32 SendData(app_u16 rslt);
    virtual vos_u32 SendExeCmd(vos_u16 exeCmd);
};

class CUpdateDwnFpga :public CExeUpdate
{
public:
    CUpdateDwnFpga(string fileName, vos_u32 dispid) :CExeUpdate(fileName, dispid){}

    virtual vos_u32 SendData(app_u16 rslt);

    virtual vos_u32 SendExeCmd(vos_u16 exeCmd);

};

class CUpdateMidFpga:public CExeUpdate
{
public:
    CUpdateMidFpga(string fileName, vos_u32 dispid) :CExeUpdate(fileName, dispid){}

    virtual vos_u32 SendData(app_u16 rslt);

    virtual vos_u32 SendExeCmd(vos_u16 exeCmd);
    vos_u32 UpdateMidFpga();
private:

};

class CUpdateMachine : public CCallBack
{
    typedef void(CUpdateMachine::*ReqAct)(STReqActCmd *pReq);

public:
    CUpdateMachine(vos_u32 ulPid);

public:
    virtual vos_u32 CallBack(vos_msg_header_stru* pMsg);
    void _sendState(vos_u32 subsysId, vos_u32 state);

private:
    vos_u32 _updateMachine(vos_msg_header_stru* pMsg);
    vos_u32 _procDwnAct(vos_u32 srcpid, app_com_dev_act_result_stru *pMsg);

    vos_u32 _procDwnUpdate(app_com_dev_act_result_stru * pMsg, vos_u32 srcpid);
    vos_u32 _procFpgaUpdate(app_com_dev_act_result_stru * pMsg, vos_u32 srcpid);
    vos_u32 _sendStart(vos_u32 subsysId);

	vos_u32 _updateMidApp(vos_u32 subsysId);
    vos_u32 _updateMidFpga(vos_u32 subsysId);

    vos_u32 _getBootloaderVres();
    vos_u32 _stopUpdate();
    void    _stopUpdate(vos_u32 sysId);
    //vos_u32 _start(CExeUpdate &);
    
private:
    STVersions m_versions;
    map< vos_u32, CExeUpdate * > exeUpdate;
    map<vos_u32, string> BinFileName;
};


#endif

