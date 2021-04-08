#ifndef __R2SYS_H__
#define __R2SYS_H__

#include "subsys.h"
#include "app_downmsg.h"
#include <map>

using namespace std;
typedef enum
{
    PLACE_R21RTT_DISCHARGE = 0,	/*ˮƽ���Լ�λ��*/
    PLACE_R21RTT_RESET = 1,	/*ˮƽ��λλ��*/
    PLACE_R21RTT_WASH1 = 2,	/*ˮƽϴҺ1λ��*/
    PLACE_R21RTT_WASH2 = 3,	/*ˮƽϴҺ2λ��*/
    PLACE_R21RTT_INHALATION1 = 4,	/*ˮƽ���Լ�1λ��*/
    PLACE_R21RTT_INHALATION2 = 5,	/*ˮƽ���Լ�2λ��*/
}EN_PLACE_R21RTT;

typedef enum
{
    PLACE_R22RTT_DISCHARGE = 0,	/*ˮƽ���Լ�λ��*/
    PLACE_R22RTT_RESET = 1,	/*ˮƽ��λλ��*/
    PLACE_R22RTT_WASH1 = 2,	/*ˮƽϴҺ1λ��*/
    PLACE_R22RTT_WASH2 = 3,	/*ˮƽϴҺ2λ��*/
    PLACE_R22RTT_INHALATION1 = 4,	/*ˮƽ���Լ�1λ��*/
    PLACE_R22RTT_INHALATION2 = 5,	/*ˮƽ���Լ�2λ��*/
}EN_PLACE_R22RTT;


class CR2Sys : public CSubSys
{
public:
    CR2Sys(vos_u32 ulPid, vos_u32 ulDstSubsys);
    ~CR2Sys();

private:

    //��ʼ����������
    void _initParseFuc();

    //R2��ת
    vos_u32 _parseR2Rtt(msg_stComDevActInfo* pComAct);

    //R2���Լ�
    vos_u32 _parseR2Absrb(msg_stComDevActInfo* pComAct);

    //R2���Լ�
    vos_u32 _parseR2Discharge(msg_stComDevActInfo* pComAct);

    //R2��ϴ
    vos_u32 _parseR2Wash(msg_stComDevActInfo* pComAct);

    //R2����ɨ��
    vos_u32 _parseR2MarginScan(msg_stComDevActInfo* pComAct);

    //ǿ����ϴ
    vos_u32 _parseIntensifyWash(msg_stComDevActInfo* pComAct);

    //ע����ſ���
    vos_u32 _parseAirOut(msg_stComDevActInfo* pComAct);
    
    //����΢������
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
