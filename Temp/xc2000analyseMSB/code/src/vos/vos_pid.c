/******************************************************************************

                  ��Ȩ���� (C), 2013-2023, �Ĵ��³�����Ƽ����޹�˾

 ******************************************************************************
  �� �� ��   : vos_pid.c
  �� �� ��   : ����
  ��    ��   : Albort,Feng
  ��������   : 2013��9��9��
  ����޸�   :
  ��������   : PIDģ�鹦�ܶ����ļ�
  �����б�   :
  �޸���ʷ   :
  1.��    ��   : 2013��9��9��
    ��    ��   : Albort,Feng
    �޸�����   : �����ļ�

******************************************************************************/
#include <memory.h>
#include "vos_basictypedefine.h"
#include "vos_configure.h"
#include "vos_errorcode.h"
#include "vos_mngmemory.h"
#include "vos_messagecenter.h"
#include "vos_selffid.h"
#include "vos_selfpid.h"
#include "vos_pid.h"
#include "vos_init.h"
#include "vos_log.h"
#include "upmsg.pb.h"

/*��ÿ��FID�׶�Ҫ��ȥFID�ű�������PID����ϵͳ���ü�һ*/
#define VOS_SELF_MAX_PID_NUM_PER_FID  (VOS_MAX_PID_NUM_PER_FID - 1)

//PID���ڱ�־ö�ٶ���
typedef enum
{
    vos_pid_register_yes = 0,  //PID�ѱ�ע��
    vos_pid_register_no  = 1,  //PIDδ��ע��

    vos_pid_register_butt
}vos_pid_register_enum;
typedef vos_u32 vos_pid_register_enum_u32;


//PIDע����Ϣ����ṹ
typedef struct
{
    vos_pid_register_info_stru* apidreginfo[VOS_MAX_SUPPORT_FID_NUM];
}vos_mng_pid_reginfo_stru;

#ifdef __cplusplus
extern "C" {
#endif

STATIC vos_mng_pid_reginfo_stru* g_pstmngpidreginfo = VOS_NULL;
STATIC void* g_pmutexforpidmng = VOS_NULL;

/*****************************************************************************
 �� �� ��  : vos_getsavepidinfobufheadforfid
 ��������  : ��ȡָ��FIDģ���PID��Ϣ�����׵�ַ,���ýӿ���ر�֤FID��ȷ
 �������  : vos_u32 ulfid
 �������  : ��
 �� �� ֵ  : vos_pid_register_info_stru*
 
 �޸���ʷ      :
  1.��    ��   : 2013��9��11��
    ��    ��   : Albort,Feng

    �޸�����   : �����ɺ���

*****************************************************************************/
vos_pid_register_info_stru* vos_getsavepidinfobufheadforfid(vos_u32 ulfid)
{
    vos_u32 ulrunrslt = VOS_OK;
    vos_u32 ulindex   = 0;

    ulrunrslt = vos_checkfidvalidity(ulfid);
    if (VOS_OK != ulrunrslt)
    {
        WRITE_ERR_LOG("call vos_checkfidvalidity failed(0x%x)\r\n", ulrunrslt);
        return VOS_NULL;
    }
    
    ulindex = vos_fidmaptoindex(ulfid);

    /*ȡ�õ�ǰFIDģ��洢PIDע����Ϣ���׵�ַ*/
    return (g_pstmngpidreginfo->apidreginfo[ulindex]);
}

/*****************************************************************************
 �� �� ��  : vos_mallocpidreginfomem
 ��������  : Ϊһ��FID���뱣��PIDע����Ϣ���ڴ�
 �������  : vos_u32 ulfid
 �������  : ��
 �� �� ֵ  : vos_u32
 
 �޸���ʷ      :
  1.��    ��   : 2013��9��10��
    ��    ��   : Albort,Feng

    �޸�����   : �����ɺ���

*****************************************************************************/
vos_u32 vos_mallocpidreginfomem(vos_u32 ulfid)
{
    vos_u32 ulindex   = 0;
    vos_u32 ulmemsize = 0;
    vos_u32 ulrunrslt = VOS_OK;
    vos_u32 ulloopvar = 0;
    
    vos_pid_register_info_stru* psttmpbuf = VOS_NULL;

    ulindex = vos_fidmaptoindex(ulfid);

    if (VOS_MAX_SUPPORT_FID_NUM <= ulindex)
    {
        RETURNERRNO(vos_pid_errorcode_base, vos_array_index_error);
    }
    
    vos_obtainmutex(g_pmutexforpidmng);
    
    if (VOS_NULL != g_pstmngpidreginfo->apidreginfo[ulindex])
    {//�Ѿ����벻���ظ�����
        vos_releasemutex(g_pmutexforpidmng);
        
        return VOS_OK;
    }

    vos_releasemutex(g_pmutexforpidmng);

    ulmemsize = VOS_SELF_MAX_PID_NUM_PER_FID * sizeof(vos_pid_register_info_stru);
    ulrunrslt = vos_mallocmemory(ulmemsize, vos_mem_type_static,
                                 (void**)&psttmpbuf);
    if (VOS_OK != ulrunrslt)
    {
        WRITE_ERR_LOG(
                    "call vos_mallocmemory failed(0x%x)\r\n",
                    ulrunrslt);
        return ulrunrslt;
    }

    for (ulloopvar = 0; VOS_SELF_MAX_PID_NUM_PER_FID > ulloopvar; ulloopvar++)
    {
        (psttmpbuf + ulloopvar)->ulpid         = VOS_INVALID_PID;
        (psttmpbuf + ulloopvar)->pidrcvmsgfunc = VOS_NULL;
        (psttmpbuf + ulloopvar)->pCallBack = VOS_NULL;
    }

    vos_obtainmutex(g_pmutexforpidmng);
    
    g_pstmngpidreginfo->apidreginfo[ulindex] = psttmpbuf;
    
    vos_releasemutex(g_pmutexforpidmng);
    
    return VOS_OK;
}

/*****************************************************************************
 �� �� ��  : vos_freepidreginfomem
 ��������  : �ͷű���PIDע����Ϣ���ڴ�
 �������  : vos_u32 ulfid
 �������  : ��
 �� �� ֵ  : void
 
 �޸���ʷ      :
  1.��    ��   : 2013��9��17��
    ��    ��   : Albort,Feng

    �޸�����   : �����ɺ���

*****************************************************************************/
void vos_freepidreginfomem(vos_u32 ulfid)
{
    vos_u32 ulrunrslt = VOS_OK;
    vos_u32 ulindex   = 0;

    ulrunrslt = vos_checkfidvalidity(ulfid);
    if (VOS_OK != ulrunrslt)
    {
        WRITE_ERR_LOG(
                    "call vos_checkfidvalidity failed(0x%x)\r\n",
                    ulrunrslt);
        return;
    }

    ulindex = vos_fidmaptoindex(ulfid);
    if (VOS_MAX_SUPPORT_FID_NUM <= ulindex)
    {
        return;
    }

    vos_obtainmutex(g_pmutexforpidmng);
    
    if (VOS_NULL == g_pstmngpidreginfo->apidreginfo[ulindex])
    {//�Ѿ��ͷ������ظ��ͷ�
        vos_releasemutex(g_pmutexforpidmng);
        
        return;
    }

    vos_freememory((void*)(g_pstmngpidreginfo->apidreginfo[ulindex]));

    g_pstmngpidreginfo->apidreginfo[ulindex] = VOS_NULL;

    vos_releasemutex(g_pmutexforpidmng);
    
    return;
}


/*****************************************************************************
 �� �� ��  : vos_initpidmd
 ��������  : PIDģ���ʼ���ӿ�
 �������  : void
 �������  : ��
 �� �� ֵ  : vos_u32
 
 �޸���ʷ      :
  1.��    ��   : 2013��9��9��
    ��    ��   : Albort,Feng

    �޸�����   : �����ɺ���

*****************************************************************************/
vos_u32 vos_initpidmd(void)
{
    vos_u32 ulrunrslt = VOS_OK;
    vos_u32 ulmemsize = 0;
    vos_u32 ulloopvar = 0;

    g_pmutexforpidmng = vos_mallocmutex();
    if (VOS_NULL == g_pmutexforpidmng)
    {
        RETURNERRNO(vos_pid_errorcode_base,vos_malloc_mutex_error);
    }

    ulrunrslt = vos_createmutex(g_pmutexforpidmng);
    if (VOS_OK != ulrunrslt)
    {
        vos_freemutex(g_pmutexforpidmng);
        g_pmutexforpidmng = VOS_NULL;
        
        return ulrunrslt;
    }
    
    ulmemsize = sizeof(vos_mng_pid_reginfo_stru);
    ulrunrslt = vos_mallocmemory(ulmemsize, vos_mem_type_static,
                                 (void**)&g_pstmngpidreginfo);
    if (VOS_OK != ulrunrslt)
    {   
        vos_deletemutex(g_pmutexforpidmng);
        vos_freemutex(g_pmutexforpidmng);
        g_pmutexforpidmng = VOS_NULL;
        
        return ulrunrslt;
    }

    for (ulloopvar = 0; VOS_MAX_SUPPORT_FID_NUM > ulloopvar; ulloopvar++)
    {
        g_pstmngpidreginfo->apidreginfo[ulloopvar] = VOS_NULL;
    }
    
    return VOS_OK;
}

/*****************************************************************************
 �� �� ��  : vos_checkpidregistered
 ��������  : ���PID�Ƿ��ѱ�ע��
 �������  : vos_u32 ulfid
             vos_u32 ulpid
 �������  : ��
 �� �� ֵ  : vos_u32
 
 �޸���ʷ      :
  1.��    ��   : 2013��10��24��
    ��    ��   : Albort,Feng

    �޸�����   : �����ɺ���

*****************************************************************************/
vos_u32 vos_checkpidregistered(vos_u32 ulfid, vos_u32 ulpid)
{
    vos_u32 ulloopvar = 0;
    
    vos_pid_register_info_stru* pstmove = VOS_NULL;

    /*��ȡ���ڵ�ǰFID�洢PIDע����Ϣ�Ĵ洢��*/
    pstmove = vos_getsavepidinfobufheadforfid(ulfid);
    for (ulloopvar = 0; VOS_SELF_MAX_PID_NUM_PER_FID > ulloopvar; ulloopvar++)
    {
        if (ulpid == pstmove->ulpid)
        {
            break;
        }

        pstmove += 1;
    }

    if (VOS_SELF_MAX_PID_NUM_PER_FID <= ulloopvar)
    {
        return vos_pid_register_no;
    }

    return vos_pid_register_yes;
}
vos_u32 vos_checkFidAndPid(vos_u32 ulfid, vos_u32 ulpid)
{
    vos_u32 ulrunrslt = VOS_OK;
    ulrunrslt = vos_checkfidvalidity(ulfid);
    if (VOS_OK != ulrunrslt)
    {
        WRITE_ERR_LOG("call vos_checkfidvalidity failed(0x%x)\r\n", ulrunrslt);
        return ulrunrslt;
    }

    /*���PID�Ƿ�Ϸ�*/
    if ((ulpid < ulfid) || (ulpid >= (ulfid + VOS_MAX_PID_NUM_PER_FID)))
    {
        RETURNERRNO(vos_pid_errorcode_base, vos_pid_invalid_error);
    }

    ulrunrslt = vos_checkpidregistered(ulfid, ulpid);
    if (vos_pid_register_yes == ulrunrslt)
    {
        RETURNERRNO(vos_pid_errorcode_base, vos_pid_invalid_error);
    }
    return VOS_OK;
}
/*****************************************************************************
�� �� ��  : vos_getpidreginfo
��������  : ��ȡPIDע����Ϣ
�������  : vos_u32 ulfid
vos_u32 ulpid
�������  : ��
�� �� ֵ  : vos_pid_register_info_stru*

�޸���ʷ      :
1.��    ��   : 2013��10��24��
��    ��   : Albort,Feng

�޸�����   : �����ɺ���

*****************************************************************************/
vos_pid_register_info_stru* vos_getpidreginfo(vos_u32 ulfid, vos_u32 ulpid)
{
    vos_u32 ulloopvar = 0;

    vos_pid_register_info_stru* pstmove = VOS_NULL;

    pstmove = vos_getsavepidinfobufheadforfid(ulfid);
    if (VOS_NULL == pstmove)
    {
        return VOS_NULL;
    }

    for (ulloopvar = 0; VOS_SELF_MAX_PID_NUM_PER_FID > ulloopvar; ulloopvar++)
    {
        if (ulpid == pstmove->ulpid)
        {
            break;
        }

        pstmove += 1;
    }

    if (VOS_SELF_MAX_PID_NUM_PER_FID <= ulloopvar)
    {
        return VOS_NULL;
    }

    return pstmove;
}

/*****************************************************************************
 �� �� ��  : vos_deliverymsgtopid
 ��������  : ��Ϣ�ַ�����������ϢĿ��PID����ϢͶ�ݸ���Ӧ��PIDģ��
 �������  : vos_msg_header_stru* pstmsg
 �������  : ��
 �� �� ֵ  : vos_u32
 
 �޸���ʷ      :
  1.��    ��   : 2013��9��6��
    ��    ��   : Albort,Feng

    �޸�����   : �����ɺ���

*****************************************************************************/
vos_u32 vos_deliverymsgtopid(vos_msg_header_stru* pstmsg)
{
    vos_u32 ulrunrslt = VOS_OK;
    vos_u32 ulfid     = VOS_INVALID_FID;

    vos_pid_register_info_stru* pstpidreginf = VOS_NULL;

    if (VOS_NULL == pstmsg)
    {
        RETURNERRNO(vos_pid_errorcode_base, vos_para_null_err);
    }

    ulfid     = vos_pidmaptofid(pstmsg->uldstpid);
    ulrunrslt = vos_checkfidvalidity(ulfid);
    if (VOS_OK != ulrunrslt)
    {
        WRITE_ERR_LOG(
                    "call vos_checkfidvalidity failed(0x%x)\r\n",
                    ulrunrslt);
        RETURNERRNO(vos_pid_errorcode_base, vos_fid_invalid_error);
    }

    /*��ȡĿ��PIDע����Ϣ*/
    pstpidreginf = vos_getpidreginfo(ulfid, pstmsg->uldstpid);

    /*�ж�PID�Ƿ�ע���Լ��Ƿ�ע����Ϣ���ջص�*/
    if ((VOS_NULL == pstpidreginf))
    {
        RETURNERRNO(vos_pid_errorcode_base, vos_pid_unreg_error);
    }
    if (VOS_NULL != pstpidreginf->pidrcvmsgfunc)
    {
        /*����Ϣ�ύ��Ŀ��PIDģ��*/
        ulrunrslt = pstpidreginf->pidrcvmsgfunc(pstmsg);
    }
    else if (VOS_NULL != pstpidreginf->pCallBack)
    {

        pstpidreginf->pCallBack->CallBack(pstmsg);
    }
    else
    {
        RETURNERRNO(vos_pid_errorcode_base, vos_pid_unreg_error);
    }

    return ulrunrslt;
}

/*****************************************************************************
 �� �� ��  : vos_checkpidvalidity
 ��������  : PID��Ч��У��
 �������  : vos_u32 ulpid
 �������  : ��
 �� �� ֵ  : vos_u32
 
 �޸���ʷ      :
  1.��    ��   : 2013��9��11��
    ��    ��   : Albort,Feng

    �޸�����   : �����ɺ���

*****************************************************************************/
vos_u32 vos_checkpidvalidity(vos_u32 ulpid)
{
    vos_u32 ulfid     = VOS_INVALID_FID;
    vos_u32 ulrunrslt = VOS_OK;

    if (VOS_INVALID_PID == ulpid)
    {
        RETURNERRNO(vos_pid_errorcode_base, vos_pid_invalid_error);
    }
    
    ulfid = vos_pidmaptofid(ulpid);
    ulrunrslt = vos_checkfidvalidity(ulfid);
    if (VOS_OK != ulrunrslt)
    {
        WRITE_ERR_LOG(
                    "call vos_checkfidvalidity failed(0x%x)\r\n",
                    ulrunrslt);
        
        return ulrunrslt;
    }

    ulrunrslt = vos_checkpidregistered(ulfid, ulpid);
    if (vos_pid_register_yes != ulrunrslt)
    {
        RETURNERRNO(vos_pid_errorcode_base, vos_pid_invalid_error);
    }

    return VOS_OK;
}

/*****************************************************************************
 �� �� ��  : vos_utreleasepid
 ��������  : �ͷ�PIDģ��
 �������  : vos_u32 ulpid
 �������  : ��
 �� �� ֵ  : vos_u32
 
 �޸���ʷ      :
  1.��    ��   : 2013��9��11��
    ��    ��   : Albort,Feng

    �޸�����   : �����ɺ���

*****************************************************************************/
vos_u32 vos_utreleasepid(vos_u32 ulpid)
{
    vos_u32 ulrunrslt = VOS_OK;
    vos_u32 ulfid     = VOS_INVALID_FID;

    vos_pid_register_info_stru* pstpidreginfo = VOS_NULL;

    ulrunrslt = vos_checkpidvalidity(ulpid);
    if (VOS_OK != ulrunrslt)
    {
        return ulrunrslt;
    }
    
    ulfid     = vos_pidmaptofid(ulpid);
    ulrunrslt = vos_checkfidvalidity(ulfid);
    if (VOS_OK != ulrunrslt)
    {
        return ulrunrslt;
    }

    /*��ȡPIDע����Ϣ*/
    pstpidreginfo = vos_getpidreginfo(ulfid, ulpid);

    pstpidreginfo->pidrcvmsgfunc = VOS_NULL;
    pstpidreginfo->pCallBack = VOS_NULL;
    pstpidreginfo->ulpid         = VOS_INVALID_PID;
   
    return VOS_OK;
}

/*****************************************************************************
 �� �� ��  : vos_utreleaseallpidoffid
 ��������  : �ͷ�һ��FIDģ�������е�PID
 �������  : vos_u32 ulfid
 �������  : ��
 �� �� ֵ  : void
 
 �޸���ʷ      :
  1.��    ��   : 2013��9��11��
    ��    ��   : Albort,Feng

    �޸�����   : �����ɺ���

*****************************************************************************/
void vos_utreleaseallpidoffid(vos_u32 ulfid)
{
    vos_freepidreginfomem(ulfid);
    
    return;
}

//PIDģ��ע��ӿ�
vos_u32 vos_registerpid(vos_u32 ulfid, vos_u32 ulpid, vos_pidrcvmsgfunc prcvmsgfunc)
{
    if (VOS_NULL == prcvmsgfunc)
    {
        RETURNERRNO(vos_pid_errorcode_base, vos_para_null_err);
    }

    if (VOS_OK != vos_checkFidAndPid(ulfid, ulpid))
    {
        RETURNERRNO(vos_pid_errorcode_base, vos_pid_invalid_error);
    }

    vos_pid_register_info_stru* pstmove = VOS_NULL;
    pstmove = vos_getpidreginfo(ulfid, VOS_INVALID_PID);
    if (VOS_NULL == pstmove)
    {
        RETURNERRNO(vos_pid_errorcode_base, vos_pid_registered_error);
    }

    pstmove->ulpid = ulpid; //����ע���PID
    pstmove->pidrcvmsgfunc = prcvmsgfunc;

    return VOS_OK;
}

#ifdef __cplusplus
}
#endif

vos_u32 vos_registerCallBack(vos_u32 ulfid, vos_u32 ulpid, CCallBack * pBack)
{
    if (VOS_NULL == pBack)
    {
        RETURNERRNO(vos_pid_errorcode_base, vos_para_null_err);
    }

    if (VOS_OK != vos_checkFidAndPid(ulfid, ulpid))
    {
        RETURNERRNO(vos_pid_errorcode_base, vos_pid_invalid_error);
    }

    vos_pid_register_info_stru* pstmove = VOS_NULL;
    pstmove = vos_getpidreginfo(ulfid, VOS_INVALID_PID);
    if (VOS_NULL == pstmove)
    {
        RETURNERRNO(vos_pid_errorcode_base, vos_pid_registered_error);
    }

    pstmove->ulpid = ulpid; //����ע���PID
    pstmove->pCallBack = pBack;

    return VOS_OK;
}

CCallBack::CCallBack(vos_u32 ulpid) :m_ulFid(0), m_ulPid(ulpid)
{

}
CCallBack::~CCallBack()
{

}
vos_u32 CCallBack::RegCallBack(vos_u32 ulfid)
{
    m_ulFid = ulfid;
    return vos_registerCallBack(ulfid, m_ulPid, this);
}

