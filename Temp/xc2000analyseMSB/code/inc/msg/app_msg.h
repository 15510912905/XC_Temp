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

//��Ӧϵͳ�豸���Ͷ���
typedef enum
{
    app_reactsys_dev_type_disk = 0,
    app_reactsys_dev_type_auto_wash = 1,
    app_reactsys_dev_type_reag_mix_ndl = 2,
    app_reactsys_dev_type_smpl_mix_ndl = 3,
    app_reactsys_dev_type_lamp = 4,

    app_reactsys_dev_type_butt
}app_reactsys_dev_type_enum;

//�Լ���ֹͣλ�����Ͷ���
typedef enum
{
    app_reagent_ndl_stop_pos_wash       = 0,  //��ϴλ��
    app_reagent_ndl_stop_pos_absrbOut   = 1,  //���Լ�λ��
    app_reagent_ndl_stop_pos_discharg   = 2,  //���Լ�λ��
    app_reagent_ndl_stop_pos_absrbIN    = 3,  //���Լ�λ��

    app_reagent_ndl_stop_pos_butt
}app_reagent_ndl_stop_pos_enum;

//�Լ���ֹͣλ�����Ͷ���
typedef enum
{
    app_reagent_dsk_stop_pos_r1_absrb = 0,  //R1�Լ������Լ�λ��
    app_reagent_dsk_stop_pos_r2_absrb = 1,  //R2�Լ������Լ�λ��
    app_reagent_dsk_stop_pos_swap     = 2,  //���Լ�λ��

    app_reagent_dsk_stop_pos_butt
}app_reagent_dsk_stop_pos_enum;

//�����ˮƽĿ��λ��
typedef enum
{
    app_reactsys_mix_ndl_wash_pos = 0,  //��ϴλ��
    app_reactsys_mix_ndl_mixing_33pos = 1,  //33�Ž���λ
    app_reactsys_mix_ndl_mixing_38pos = 2,  //38�Ž���λ
    app_reactsys_mix_ndl_mixing_67pos = 3,  //67�Ž���λ

    app_reactsys_mix_ndl_hpos_butt
}app_reactsys_mix_ndl_hpos_enum;

//������־����
typedef enum
{
    EN_AF_Success = 0,//����ִ�гɹ�
    EN_AF_Failed = 1, //����ִ��ʧ��
    EN_AF_Doing = 2,  //����ִ����
    EN_AF_Unstart = 3,  //����λ��ʼ
}EN_ActFlag;

//===========//��Ϣͷ==========
typedef struct
{
    vos_u32 ulDstPid;          //Ŀ��PID
    vos_u32 ulSrcPid;          //ԴPID
    vos_u32 ulMsgType;      //��Ϣ����
    vos_u32 ulDstIp;            //Ŀ��IP
}msg_stHead;

//������Ϣͷ
void app_constructMsgHead(msg_stHead * pHead,vos_u32 dstPid,vos_u32 srcPid,vos_u32 msgType);

//������Ϣ����λ��
vos_u32 sendUpMsg(vos_u32 dstPid, vos_u32 srcPid, vos_u32 msgType, google::protobuf::Message* pPara = NULL);

//�����ڲ���Ϣ
vos_u32 sendInnerMsg(vos_u32 dstPid, vos_u32 srcPid, vos_u32 msgType, google::protobuf::Message* pPara = NULL);

//������Ϣ
vos_u32 app_sendMsg(msg_stHead * pHead, void* pPara, vos_u32 ulParaLen, vos_u16 ulNameLen = 0, bool bOuter = false);

//�����ڴ�
vos_u32 app_mallocBuffer(vos_u32 ulBufLen,  void** pBuffer);

//�ͷ��ڴ�
void app_freeBuffer(void * pBuffer);


#endif


