#ifndef __R1SYS_H__
#define __R1SYS_H__

#include "subsys.h"
#include "app_downmsg.h"
#include <map>

using namespace std;

typedef enum
{
    PLACE_R11RTT_DISCHARGE = 0,	/*ˮƽ���Լ�λ��*/
    PLACE_R11RTT_RESET = 1,	/*ˮƽ��λλ��*/
    PLACE_R11RTT_WASH1 = 2,	/*ˮƽϴҺ1λ��*/
    PLACE_R11RTT_WASH2 = 3,	/*ˮƽϴҺ2λ��*/
    PLACE_R11RTT_WASH3 = 4,	/*ˮƽϴҺ3λ��*/
    PLACE_R11RTT_INHALATION1 = 5,	/*ˮƽ���Լ�1λ��*/
    PLACE_R11RTT_INHALATION2 = 6,	/*ˮƽ���Լ�2λ��*/
}EN_PLACE_R11RTT;

typedef enum
{
    PLACE_R12RTT_INHALATION2 = 0,	/*ˮƽ���Լ�2λ��*/
    PLACE_R12RTT_INHALATION1 = 1,	/*ˮƽ���Լ�1λ��*/
    PLACE_R12RTT_WASH3 = 2,	/*ˮƽϴҺ3λ��*/
    PLACE_R12RTT_WASH2 = 3,	/*ˮƽϴҺ2λ��*/
    PLACE_R12RTT_WASH1 = 4,	/*ˮƽϴҺ1λ��*/
    PLACE_R12RTT_RESET = 5,	/*ˮƽ��λλ��*/
    PLACE_R12RTT_DISCHARGE = 6,	/*ˮƽ����λ��*/
}EN_PLACE_R12RTT;

class CR1Sys : public CSubSys
{
public:
    CR1Sys(vos_u32 ulPid, vos_u32 ulDstSubsys);
    ~CR1Sys();

private:
    //��ʼ����������
    void _initParseFuc();

    //R1��ת
    vos_u32 _parseR1Rtt(msg_stComDevActInfo* pComAct);

    //R1���Լ�
    vos_u32 _parseR1Absrb(msg_stComDevActInfo* pComAct);

    //R1���Լ�
    vos_u32 _parseR1Discharge(msg_stComDevActInfo* pComAct);

    //R1��ϴ
    vos_u32 _parseR1Wash(msg_stComDevActInfo* pComAct);

    //R1����ɨ��
    vos_u32 _parseR1MarginScan(msg_stComDevActInfo* pComAct);

    //ǿ����ϴ
    vos_u32 _parseIntensifyWash(msg_stComDevActInfo* pComAct);

    //ע����ſ���
    vos_u32 _parseAirOut(msg_stComDevActInfo* pComAct);
   
    //΢������
    vos_u32 _parseTrimOffset(msg_stComDevActInfo* pComAct);

    //��ֱ�˶�
    vos_u32 _parseNdlVrt(msg_stComDevActInfo* pComAct);
    
    //�������ϴ��
    vos_u32 _parsePVCtrl(msg_stComDevActInfo* pComAct);

    //ע��ò���
    vos_u32 _parsePumpCtrl(msg_stComDevActInfo* pComAct);
    uint8_t _setPos(app_u16 usDstPos);
};

#endif
