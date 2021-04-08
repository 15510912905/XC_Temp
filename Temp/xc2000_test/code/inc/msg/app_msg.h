#ifndef _app_msg_h_
#define _app_msg_h_

#include <string.h>
#include "vos_mngmemory.h"
#include "vos_messagecenter.h"
#include "app_downmsg.h"
#include "app_innermsg.h"
#include "google/protobuf/message.h"

typedef enum{
    mcu_smp_place_ise    = 0,
    mcu_smp_place_dilute = 1,
    mcu_smp_place_drain    = 2,
    mcu_smp_place_wash     = 3,
    mcu_smp_place_smp1    = 4,
    mcu_smp_place_smp2    = 5,
    mcu_smp_place_smp3    = 6,
    mcu_smp_place_smp4    = 7,    
    mcu_smp_place_smp5    = 8,    
    
    mcu_smp_place_butt
}mcu_smp_place_enum;

//反应系统设备类型定义
typedef enum
{
    app_reactsys_dev_type_disk = 0,
    app_reactsys_dev_type_auto_wash = 1,
    app_reactsys_dev_type_reag_mix_ndl = 2,
    app_reactsys_dev_type_smpl_mix_ndl = 3,
    app_reactsys_dev_type_lamp = 4,

    app_reactsys_dev_type_butt
}app_reactsys_dev_type_enum;

//试剂针停止位置类型定义
typedef enum
{
    app_reagent_ndl_stop_pos_wash       = 0,  //清洗位置
    app_reagent_ndl_stop_pos_absrbOut   = 1,  //吸试剂位置
    app_reagent_ndl_stop_pos_discharg   = 2,  //排试剂位置
    app_reagent_ndl_stop_pos_absrbIN    = 3,  //吸试剂位置

    app_reagent_ndl_stop_pos_butt
}app_reagent_ndl_stop_pos_enum;

//试剂盘停止位置类型定义
typedef enum
{
    app_reagent_dsk_stop_pos_r1_absrb = 0,  //R1试剂针吸试剂位置
    app_reagent_dsk_stop_pos_r2_absrb = 1,  //R2试剂针吸试剂位置
    app_reagent_dsk_stop_pos_swap     = 2,  //换试剂位置

    app_reagent_dsk_stop_pos_butt
}app_reagent_dsk_stop_pos_enum;

//搅拌杆水平目标位置
typedef enum
{
    app_reactsys_mix_ndl_wash_pos = 0,  //清洗位置
    app_reactsys_mix_ndl_mixing_33pos = 1,  //33号搅拌位
    app_reactsys_mix_ndl_mixing_38pos = 2,  //38号搅拌位
    app_reactsys_mix_ndl_mixing_67pos = 3,  //67号搅拌位

    app_reactsys_mix_ndl_hpos_butt
}app_reactsys_mix_ndl_hpos_enum;

//动作标志定义
typedef enum
{
    EN_AF_Success = 0,//动作执行成功
    EN_AF_Failed = 1, //动作执行失败
    EN_AF_Doing = 2,  //动作执行中
    EN_AF_Unstart = 3,  //动作位开始
}EN_ActFlag;

//===========//消息头==========
typedef struct
{
    vos_u32 ulDstPid;          //目标PID
    vos_u32 ulSrcPid;          //源PID
    vos_u32 ulMsgType;      //消息类型
    vos_u32 ulDstIp;            //目标IP
}msg_stHead;

//构建消息头
void app_constructMsgHead(msg_stHead * pHead,vos_u32 dstPid,vos_u32 srcPid,vos_u32 msgType);

//发送消息到上位机
vos_u32 sendUpMsg(vos_u32 dstPid, vos_u32 srcPid, vos_u32 msgType, google::protobuf::Message* pPara = NULL);

//发送内部消息
vos_u32 sendInnerMsg(vos_u32 dstPid, vos_u32 srcPid, vos_u32 msgType, google::protobuf::Message* pPara = NULL);

//发送消息
vos_u32 app_sendMsg(msg_stHead * pHead, void* pPara, vos_u32 ulParaLen, vos_u16 ulNameLen = 0, bool bOuter = false);

//分配内存
vos_u32 app_mallocBuffer(vos_u32 ulBufLen,  void** pBuffer);

//释放内存
void app_freeBuffer(void * pBuffer);


#endif


