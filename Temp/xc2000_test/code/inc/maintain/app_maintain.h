#ifndef __MAINTAIN_H__
#define __MAINTAIN_H__

#include "vos_pid.h"
#include "msg.h"
#include "app_innermsg.h"

//液路维护状态
typedef enum{
    app_state_idll  = 0,
    //q去离子水管路灌注
    app_deion_pipeflood_open_p03,    //开启p03   1min
    app_deion_pipeflood_open_v10v11, //后开启v10 v11     20s
    app_deion_pipeflood_close_v10v11,//后关闭v10 v11同时打开v05  2s
    app_deion_pipeflood_open_p04,    //后打开齿轮增压泵P04   1s
    app_deion_pipeflood_close_v05,   //后关闭v05
    app_deion_pipeflood_success ,
    
    //样本单元管路灌注
    app_smp_pipeflood_reset,        //样本单元复位
    app_smp_pipeflood_open_v02,     //打开S内壁清洗阀V02  5s
    app_smp_pipeflood_open_v07,     //打开S外壁清洗阀V07；同时，S注射泵在5s内各进行一次排空、吸满和复位动作
    app_smp_pipeflood_close_v02v07, //关闭V02、V07
    app_smp_pipeflood_success,

    //样本单元S2管路灌注
    app_smpb_pipeflood_reset,        //样本单元S2复位
    app_smpb_pipeflood_open_v02,     //打开S2内壁清洗阀V02  5s
    app_smpb_pipeflood_open_v07,     //打开S2外壁清洗阀V07；同时，S注射泵在5s内各进行一次排空、吸满和复位动作
    app_smpb_pipeflood_close_v02v07, //关闭V02、V07
    app_smpb_pipeflood_success,

    //R1单元管路灌注
    app_r1_pipeflood_reset,         
    app_r1_pipeflood_open_v03,    
    app_r1_pipeflood_open_v08,    
    app_r1_pipeflood_close_v03v08,
    app_r1_pipeflood_success,

    //R12单元管路灌注
    app_r12_pipeflood_reset,
    app_r12_pipeflood_open_v03,
    app_r12_pipeflood_open_v08,
    app_r12_pipeflood_close_v03v08,
    app_r12_pipeflood_success,

    //R2单元管路灌注
    app_r2_pipeflood_reset,         
    app_r2_pipeflood_open_v04,    
    app_r2_pipeflood_open_v09,    
    app_r2_pipeflood_close_v04v09,
    app_r2_pipeflood_success,

    //R22单元管路灌注
    app_r22_pipeflood_reset,
    app_r22_pipeflood_open_v04,
    app_r22_pipeflood_open_v09,
    app_r22_pipeflood_close_v04v09,
    app_r22_pipeflood_success,

    //整机排空
    app_drain_closetempctl, //关闭温度控制关闭压力检测控制和浮子液位检测、关闭P02
    app_drain_releaseprss,  //释放系统残余真空
    app_drain_emptydrain,   //主真空容器排液
    app_drain_emptydrainover, //排液完成
    app_drain_closepx,      //关闭P03 P05 P06 P07
    //app_drain_closefloatctl,//关闭浮子液位检测\水箱\稀释(浓缩)清洗剂容器 
    app_drain_openvx,       //打开阀V01、V02、V03、V04、V05、V06、V07、V08、V09、V10、V11、V14、V15、V16、V17、V18、V19 
    app_drain_success,

    //加样调试-吸
    //样本针
    app_addsmp_smpndl_abair, //样本针吸空气
    app_addsmp_smpndl_down,  //样本针下降
    app_addsmp_smpndl_absmp, //样本针吸样
    app_addsmp_smpndl_up,    //样本针上升
    app_addsmp_smpndl_success,

    //试剂针1
    app_addsmp_r1ndl_abair, //R1针吸空气
    app_addsmp_r1ndl_down,  //R1针下降
    app_addsmp_r1ndl_absmp, //R1针吸样
    app_addsmp_r1ndl_up,    //R1针上升
    app_addsmp_r1ndl_success,

    //试剂针2
    app_addsmp_r2ndl_abair, //R2针吸空气
    app_addsmp_r2ndl_down,  //R2针下降
    app_addsmp_r2ndl_absmp, //R2针吸样
    app_addsmp_r2ndl_up,    //R2针上升
    app_addsmp_r2ndl_success,

    //加样调试-排
    //样本针
    app_drainsmp_smpndl_down,  //样本针下降
    app_drainsmp_smpndl_psmp,  //样本针排样
    app_drainsmp_smpndl_up,    //样本针上升
    app_drainsmp_smpndl_success,

    //试剂针1
    app_drainsmp_r1ndl_down,  //R1针下降
    app_drainsmp_r1ndl_psmp,  //R1针排样
    app_drainsmp_r1ndl_up,    //R1针上升
    app_drainsmp_r1ndl_success,

    //试剂针2
    app_drainsmp_r2ndl_down,  //R2针下降
    app_drainsmp_r2ndl_psmp,  //R2针排样
    app_drainsmp_r2ndl_up,    //R2针上升
    app_drainsmp_r2ndl_success,

    //吸排测试
    //样本针
    app_addordrain_smp_add,
    app_addordrain_smp_drain,
    app_addordrain_smp_success,
    
    //r1
    app_addordrain_r1_add,
    app_addordrain_r1_drain,
    app_addordrain_r1_success,
    
    //r2
    app_addordrain_r2_add,
    app_addordrain_r2_drain,
    app_addordrain_r2_success,
    
    
    /*外壁清洗*/
    app_outer_wash_open_p03_p07_v07_v08_v09,
    app_outer_wash_open_success,

    /*搅拌杆清洗*/
    app_stir_open_p03_p07_v10_v11,
    app_stir_open_success,

    /*内壁清洗*/
    app_inner_wash_open_p03_p07_v05,
    app_inner_wash_open_p04,
    app_inner_wash_close_v05_open_v02_v03_v04,
    app_inner_wash_success,
    
    /*液路灌注*/
    app_autowash_flood_open_p03_p06_p07_p08_v05,
    app_autowash_flood_open_p04,
    app_autowash_flood_close_v05_open_v02_to_v19,
    app_autowash_flood_success,

    /*液路时序测试*/
    app_autowash_timing1_open_p03_p06_p07_p08,//0.3
    app_autowash_timing1_success,

    app_autowash_timing_open_v23,// 1 0.3
    app_autowash_timing_open_v14_v15_v16_v17_v18_v19,//0.9 1.3
    app_autowash_timing_open_v10_v11,//0.1 2.2
    app_autowash_timing_close_v14_v15_v16_v17_v18_v19,//0.1 2.3
    app_autowash_timing_close_v23_open_v05,//0.2 2.4
    app_autowash_timing_open_p04,//0.2 2.6
    app_autowash_timing_close_v05_open_v21_v22_v25,//0.1 2.8  
    app_autowash_timing_open_v04_v09,//0.1 2.9
    app_autowash_timing_open_v03_v08,//0.1 3.0 
    app_autowash_timing_open_v02_v07,//0.4 3.1
    app_autowash_timing_close_v10_v11,//0.1 3.5
    app_autowash_timing_close_v04,//0.1 3.6
    app_autowash_timing_close_v03,//0.1 3.7
    app_autowash_timing_close_v02_v09,//0.1 3.8
    app_autowash_timing_close_v07_v08,//0.4 3.9
    app_autowash_timing_close_p04_v21_v22_v25,//0 4.3
    app_autowash_timing_success,

    /*关闭所有泵阀*/
    app_autowash_close_allPV,
    app_autowash_close_allPV_success,
    
    //微量测试S
    app_accuracyAddS_rtt,
    app_accuracyAddS_add,
    app_accuracyAddS_wash,
    app_accuracyAddS_success,

    //微量测试R1
    app_accuracyAddR1_rtt,
    app_accuracyAddR1_add,
    app_accuracyAddR1_wash,
    app_accuracyAddR1_success,

    //微量测试R2
    app_accuracyAddR2_rtt,
    app_accuracyAddR2_add,
    app_accuracyAddR2_wash,
    app_accuracyAddR2_success,

    app_accuracyRMix_rtt,
    app_accuracyRMix_mix,
    app_accuracyRMix_wash,
    app_accuracyRMix_success,

    app_accuracySMix_rtt,
    app_accuracySMix_mix,
    app_accuracySMix_wash,
    app_accuracySMix_success,

    app_state_butt
}app_pipe_flood_state_enum;
//命令信息
typedef struct
{
    vos_u32 ulcommandid;
    char  auccmdload[0];     //命令负载
}vos_command_info_stru;

//维护---- bebugtty的数据结构
typedef struct{
    vos_u32 ulcmdlen;  //这个长度为sizeof(vos_command_info_stru)+负载
    vos_command_info_stru pcmdinfo;                 
}vos_app_maintain_debug_cmd_stru;

//加样测试结构体
typedef struct{
    vos_i32 addcount; 
    vos_u32 addvolume;
    vos_u32 addspace;
}app_addsmp_test_stru;

//吸样测试结构体
typedef struct{
    vos_u32 addcount; 
    vos_u32 volumeair;  //空气体积
    vos_u32 volumesmp;  //液体体积
}app_drainoradd_only_test_stru;

//吸排动作结构体
typedef struct{
    vos_u32 addcount;   //次数
    vos_u32 volumesmp;  //液体体积
    vos_u32 volumemore;  //+体积
    vos_u32 volumeless;  //-体积
}app_drainoradd_test_stru;

typedef struct 
{
    vos_i32 mixCount;
    vos_u32 mixSpace;
}app_mix_stru;
//管路灌注管理结构体
typedef struct{
    char  curtstate;   //当前状态
    char  ucovercmd;   //当前已完成的命令数
    char  ucsumcmdnum; //当前状态发送下位机命令总数
    vos_u32 nextActTimer;   //管路灌注定时器
    vos_u32 actTimer;//动作定时器
    vos_u32 timeLen;
}app_pipe_flood_mng_stru;

class CMaintain:public CCallBack
{
    typedef vos_u32(CMaintain::*msgFucs)(vos_msg_header_stru* pMsg);
    typedef void(CMaintain::*exeReq)(vos_u32, void*);

public:
    CMaintain(vos_u32 ulPid);

    virtual vos_u32 CallBack(vos_msg_header_stru* pMsg);
private:
    void _initMtReq();
    vos_u32 _procMtReq(vos_msg_header_stru*);
    vos_u32 _procDwnRes(vos_msg_header_stru*);
    vos_u32 _procTimeOut(vos_msg_header_stru*);
    vos_u32 _procAddLiquid(vos_msg_header_stru*);
    vos_u32 _procStopMtCMD(vos_msg_header_stru*);

    void _exeReq_1(vos_u32 state, void*);
    void _exeReq_2(vos_u32 state, void*);
    void _exeReq_3(vos_u32 state, void*);
    void _exeReq_4(vos_u32 state, void*);
    void _exeReq_5(vos_u32 state, void*);
    void _exeReq_6(vos_u32 state, void*);
    void _exeReq_7(vos_u32 state, void*);
    void _exeReq_8(vos_u32 state, void*);
    void _exeReq_9(vos_u32 state, void*);
    void _exeReq_10(vos_u32 state, void*);
    void _exeReq_11(vos_u32 state, void*);
    void _exeReq_12(vos_u32 state, void*);
    void _exeReq_13(vos_u32 state, void*);
    void _exeReq_14(vos_u32 state, void*);
    void _exeReq_15(vos_u32 state, void*);
    void _exeReq_16(vos_u32 state, void*);
    void _exeReq_17(vos_u32 state, void*);
    void _exeReq_18(vos_u32 state, void*);
    void _exeReq_19(vos_u32 state, void*);
    void _exeReq_20(vos_u32 state, void*);
    void _exeReq_21(vos_u32 state, void*);
    void _exeReq_22(vos_u32 state, void* );
    void _exeReq_23(vos_u32 state, void* );
    void _exeReq_24(vos_u32 state, void* );
    void _exeReq_25(vos_u32 state, void*);
    void _exeReq_26(vos_u32 state, void*);

    void _exeReq_27(vos_u32 state, void*);
    void _exeReq_28(vos_u32 state, void*);
    void _exeReq_29(vos_u32 state, void*);
    void _exeReq_30(vos_u32 state, void*);
    void _exeReq_31(vos_u32 state, void*);
    void _exeReq_32(vos_u32 state, void*);
    void _exeReq_33(vos_u32 state, void*);

    vos_u32 _configAddOnlyInfo(STReqActCmd *AddOnlyInfo);
    vos_u32 _configAddOrDrainInfo(STReqActCmd *AddOrDrainInfo);
    vos_u32 _sendPvCtl(vos_u32 ensubsys, vos_u32 pvNum, vos_u32 operate);
    vos_u32 _sendPvCtl(vos_u32 pvNum, vos_u32 operate);
    vos_u32 _sendNdlUpDown(vos_u32 dir, vos_u32 subsys);
    vos_u32 _sendPumpAct(vos_u32 dir, vos_u32 volume, vos_u32 ulDstPid);
    vos_u32 _sendPrssCtl(vos_u32 ctrl);
    vos_u32 _sendTempCtl(vos_u32 tempctl);
    vos_u32 _sendNdlPreact(vos_u32 subsys, vos_u32 acttype);
    vos_u32 _sendNdlExeact(vos_u32 subsys);
    void configSmpInfo(msg_stComDevActReq * pPara);
    void configReagInfo(msg_stComDevActReq * pPara);

	void _configMtState(app_pipe_flood_state_enum state, char sumcmdnum, char overcmd, vos_u32 timelen);
        
private:
    app_pipe_flood_mng_stru m_stateMng; //状态管理
    map<vos_u32, msgFucs> m_msgFucs;    //回调
    map<vos_u32, exeReq> m_procMtReq;    //处理请求
    exeReq m_exeFunc;

    app_addsmp_test_stru m_AddInfo;
    app_drainoradd_only_test_stru m_AddOnlyInfo;
    app_drainoradd_test_stru m_AddOrDrainInfo;
    int addsys;
    app_mix_stru m_MixInfo;
};


#endif /* __MAINTAIN_H__ */
