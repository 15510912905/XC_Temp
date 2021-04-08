#ifndef __SMPNDL_H__
#define __SMPNDL_H__

#include "subsys.h"
#include "app_downmsg.h"
#include <map>

typedef enum
{
    PLACE_S1RTT_DISCHARGE = 0,	/*ˮƽ����λ��*/
    PLACE_S1RTT_THINNING = 1,	/*ˮƽϡ��λ��*/
    PLACE_S1RTT_RESET = 2,	/*ˮƽ��λλ��*/
    PLACE_S1RTT_WASH1 = 3,	/*ˮƽϴҺ1λ��*/
    PLACE_S1RTT_WASH2 = 4,	/*ˮƽϴҺ2λ��*/
    PLACE_S1RTT_WASH3 = 5,	/*ˮƽϴҺ3λ��*/
    PLACE_S1RTT_INHALATION1 = 6,	/*��������*/
    PLACE_S1RTT_INHALATION2 = 7,	/*��������*/
}EN_PLACE_S1RTT;

typedef enum
{
    PLACE_S2RTT_THINNING = 0,	/*ˮƽϡ��λ��*/
    PLACE_S2RTT_DISCHARGE = 1,	/*ˮƽ����λ��*/
    PLACE_S2RTT_RESET = 2,	/*ˮƽ��λλ��*/
    PLACE_S2RTT_WASH1 = 3,	/*ˮƽϴҺ1λ��*/
    PLACE_S2RTT_WASH2 = 4,	/*ˮƽϴҺ2λ��*/
    PLACE_S2RTT_WASH3 = 5,	/*ˮƽϴҺ3λ��*/
    PLACE_S2RTT_INHALATION1 = 6,	/*��������*/
    PLACE_S2RTT_INHALATION2 = 7,	/*��������*/
}EN_PLACE_S2RTT;

class CSmpNdl : public CSubSys
{
public:
    CSmpNdl(vos_u32 ulPid, vos_u32 ulDstSubsys);
    ~CSmpNdl();

private:
    //��ʼ����������
    void _initParseFuc();

    //������λ����
    vos_u32 _parseReset(msg_stComDevActInfo* pComAct);

	//������ˮƽת������
    vos_u32 _parseNdlRtt(msg_stComDevActInfo* pComAct);

    //����������
    vos_u32 _parseAbsrb(msg_stComDevActInfo* pComAct);

    //�����Ŷ���
    vos_u32 _parseDischarge(msg_stComDevActInfo* pComAct);

    //������ϴ����
    vos_u32 _parseWashing(msg_stComDevActInfo* pComAct);

    //����ϴ����
    vos_u32 _parseWashLong(msg_stComDevActInfo* pComAct);
    //����ǿ����ϴ����
    vos_u32 _parseIntensifyWash(msg_stComDevActInfo* pComAct);

    //����������ˮƽת����Ϣ
    void _setSampleNdlRttPos(msg_stNdlHrotate* pHrotate, app_u16 dstPos);

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
