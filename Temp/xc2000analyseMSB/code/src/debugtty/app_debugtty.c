#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <memory.h>
#include <termio.h>
#include <stdarg.h>
#include "app_errorcode.h"
#include "app_msgtypedefine.h"
#include "app_comstructdefine.h"
#include "app_main.h"
#include "app_debugtty.h"
#include "app_xc8002shareinfo.h"
#include "app_maintain.h"
#include "app_downmsg.h"
#include "vos_mnglist.h"
#include "vos_mngmemory.h"
#include "vos_errorcode.h"
#include "app_msg.h"
#include "vos_log.h"
#include "vos_pid.h"
#include "typedefine.pb.h"
#include "msg.h"
#include "apputils.h"
#include "vos_log.h"

#define APP_MAX_LEN_PER_SINGLE_PARA    (32)
#define APP_MAX_DEBUGTTY_PARA_NUM      (30)
#define APP_MAX_DEBUGTTY_CACHE_CMD_NUM (128)

#define APP_MAX_DBGTTY_INPUT_LENGTH    (512)
#define APP_MAX_CMD_LENGTH             (128)

#define APP_MAX_PRINT_BUF_LENGTH       (512)
#define APP_MAX_PRINT_RING_BUF_LEN     (6144)

//���Ƽ�keyֵ����
typedef enum
{
    null_key0   = 1,
    null_key1   = 27,
    null_key2   = 51,
    tab_key     = 9,
    enter_key   = 10,
    up_key      = 65,
    down_key    = 66,
    right_key   = 67,
    left_key    = 68,
    unknown_key = 91,
#ifdef COMPILER_IS_ARM_LINUX_GCC
    del_key = 127,
    backspace_key = 8,
#else
    del_key = 126,
    backspace_key = 127,
#endif
}app_debugtty_ctrl_key_enum;
typedef char app_debugtty_ctrl_key_enum_u8;

//������������Ϸ��Զ���
typedef enum
{
    app_debugtty_cmd_para_valid_hex   = 0, //�������Ϊ16����
    app_debugtty_cmd_para_valid_dec   = 1, //�������Ϊ10����
    app_debugtty_cmd_para_valid_help  = 2, //�������Ϊ��������
    app_debugtty_cmd_para_null        = 3, //�������Ϊ��

    app_debugtty_cmd_para_valid_butt
}app_debugtty_cmd_para_valid_enum;
typedef char app_debugtty_cmd_para_valid_enum_u8;

//linux�������
typedef enum
{
    app_debugtty_cmd_cd_type    = 0,
    app_debugtty_cmd_ls_type    = 1,
    app_debugtty_cmd_vi_type    = 2,
    app_debugtty_cmd_other_type = 3,
}app_debugtty_cmd_type_enum;
typedef char app_debugtty_cmd_type_enum_u8;

//������Ժ���ע�����ṹ
typedef struct
{
    char  aucfuncname[128];
    vos_u32 ulparanum;
    pdebugcmdfunc pfuncaddr; //������ַ
    pshowcmdhelp  phelpfuncaddr; //�����������Ϣ������ַ
}app_debugtty_mng_reg_func_stru;

//����cache��������ṹ
typedef struct
{
    char  auccmdinfo[APP_MAX_DEBUGTTY_CACHE_CMD_NUM][128];
    vos_u32 ulnodekey;
}app_debugtty_cache_cmd_stru;

typedef struct
{
    vos_listid ulcachelistid;
    vos_u32 ulcachedinputnum;  //�ѻ����������
    vos_u32 ulinputkeyindex;   //����������
}app_debugtty_cache_cmd_mng_stru;

//���崮�ڴ�ӡ����ṹ
typedef struct
{
    char ucnullflag; //����ձ�־
    char aucrsv[3];
    char  aucprintbuf[APP_MAX_PRINT_BUF_LENGTH];
}app_uartprint_buf_stru;

//���崮�ڴ�ӡ�������ṹ
typedef struct
{
    app_u16 usreadidx;
    app_u16 uswriteidx;
    vos_tid threadid;
    app_uartprint_buf_stru astprintbuf[APP_MAX_PRINT_RING_BUF_LEN];
}app_mng_uartprintbuf_stru;

#ifdef __cplusplus
    extern "C" {
#endif

STATIC vos_listid g_ulmngdebugttyregfunclistid  = APP_INVALID_U32;
STATIC app_debugtty_cache_cmd_mng_stru g_stcachecmdmng;
STATIC app_mng_uartprintbuf_stru* g_pstmnguartprtbuf = VOS_NULL;
STATIC char* g_pucexelinuxcmdrslt = VOS_NULL;
STATIC volatile vos_u32 g_ultotalcmdnum = 0;
vos_u32 ADtimer = 0;
vos_u32 ADtime = 0;
vos_u32 ADmode = 0;
/*****************************************************************************
 �� �� ��  : app_showDebugttyHelpInfo
 ��������  : ��ʾ���Դ��ڰ�����Ϣ
 �������  : void
 �������  : ��
 �� �� ֵ  : void
 ���ú���  : 
 ��������  : 
 
 �޸���ʷ      :
  1.��    ��   : 2014��7��10��
    ��    ��   : Albort,Feng
    �޸�����   : �����ɺ���

*****************************************************************************/
void app_showDebugttyHelpInfo(void)
{
    printf("\r\n h/?: show registered commands and tty help info.\r\n");
    printf(" command h/help: show this command help info.\r\n");
    
    printf(" reboot: reboot system.\r\n");
    
    return;
}

/*****************************************************************************
 �� �� ��  : app_registerDebugCmdfunc
 ��������  : ע��ά������
 �������  : char* pcmdfuncaddr
             pdebugcmdfunc pfuncaddr
             pshowcmdhelp phfuncaddr
             vos_u32 ulparanum
 �������  : ��
 �� �� ֵ  : void
 ���ú���  : 
 ��������  : 
 
 �޸���ʷ      :
  1.��    ��   : 2014��7��9��
    ��    ��   : Albort,Feng
    �޸�����   : �����ɺ���

*****************************************************************************/
void app_registerDebugCmdfunc(char* pucfuncname, pdebugcmdfunc pcmdfuncaddr,
                                pshowcmdhelp phfuncaddr, vos_u32 ulparanum)
{
    vos_u32 ulrunrslt = VOS_OK;
    
    app_debugtty_mng_reg_func_stru* pstnode = VOS_NULL;

    g_ultotalcmdnum += 1;
    
    ulrunrslt = vos_mallocnode(g_ulmngdebugttyregfunclistid,
                               vos_list_sort_fifo_mode,
                               g_ultotalcmdnum, (void**)(&pstnode));
    if (VOS_OK != ulrunrslt)
    {
        WRITE_ERR_LOG(
                   "Call vos_mallocnode() failed(0x%x)\r\n",
                   ulrunrslt);

        return;
    }

    memcpy((char*)(pstnode->aucfuncname), pucfuncname,
           (strlen(pucfuncname) + 1));

    pstnode->pfuncaddr = pcmdfuncaddr;
    pstnode->ulparanum = ulparanum;
    pstnode->phelpfuncaddr = phfuncaddr;
    
    return;
}

/*****************************************************************************
 �� �� ��  : app_debugttyfuncnameMaptoHelpaddr
 ��������  : ά��ģ�麯������ӳ��Ϊ������Ϣ������ַ
 �������  : char* pstfuncname
 �������  : ��
 �� �� ֵ  : pdebugcmdfunc
 ���ú���  : 
 ��������  : 
 
 �޸���ʷ      :
  1.��    ��   : 2014��7��9��
    ��    ��   : Albort,Feng
    �޸�����   : �����ɺ���

*****************************************************************************/
pshowcmdhelp app_debugttyfuncnameMaptoHelpaddr(char* pstfuncname)
{
    app_debugtty_mng_reg_func_stru* pstnode = VOS_NULL;

    (void)vos_listseek(g_ulmngdebugttyregfunclistid, vos_list_offset_head);
    (void)vos_getcurrentopnode(g_ulmngdebugttyregfunclistid,
                                   (void**)(&pstnode));
    
    while (VOS_NULL != pstnode)
    {
        if (0 == strcmp((const char*)(pstnode->aucfuncname), (const char*)(pstfuncname)))
        {
            break;
        }

        (void)vos_getcurrentopnode(g_ulmngdebugttyregfunclistid,
                                       (void**)(&pstnode));
    }

    if (VOS_NULL == pstnode)
    {
        return VOS_NULL;
    }

    return pstnode->phelpfuncaddr;
}

/*****************************************************************************
 �� �� ��  : app_debugttyFuncNameMaptoAddr
 ��������  : ά��ģ�麯������ӳ��Ϊ������ַ
 �������  : char* pstfuncname
 �������  : ��
 �� �� ֵ  : pdebugcmdfunc
 ���ú���  : 
 ��������  : 
 
 �޸���ʷ      :
  1.��    ��   : 2014��7��9��
    ��    ��   : Albort,Feng
    �޸�����   : �����ɺ���

*****************************************************************************/
pdebugcmdfunc app_debugttyFuncNameMaptoAddr(char* pstfuncname)
{
    app_debugtty_mng_reg_func_stru* pstnode = VOS_NULL;

    (void)vos_listseek(g_ulmngdebugttyregfunclistid, vos_list_offset_head);
    (void)vos_getcurrentopnode(g_ulmngdebugttyregfunclistid,
                                   (void**)(&pstnode));
    
    while (VOS_NULL != pstnode)
    {
        if (0 == strcmp((const char*)(pstnode->aucfuncname), (const char*)(pstfuncname)))
        {
            break;
        }

        (void)vos_getcurrentopnode(g_ulmngdebugttyregfunclistid,
                                       (void**)(&pstnode));
    }

    if (VOS_NULL == pstnode)
    {
        return VOS_NULL;
    }

    return pstnode->pfuncaddr;
}

/*****************************************************************************
 �� �� ��  : app_debugttyShowAllCmdInfo
 ��������  : ��ʾ������ע��������Ϣ
 �������  : void
 �������  : ��
 �� �� ֵ  : void
 ���ú���  : 
 ��������  : 
 
 �޸���ʷ      :
  1.��    ��   : 2014��7��9��
    ��    ��   : Albort,Feng
    �޸�����   : �����ɺ���

*****************************************************************************/
void app_debugttyShowAllCmdInfo(void)
{
    vos_u32 ulfindfuncflag = APP_FALSE;
    
    app_debugtty_mng_reg_func_stru* pstnode = VOS_NULL;

    (void)vos_listseek(g_ulmngdebugttyregfunclistid, vos_list_offset_head);
    (void)vos_getcurrentopnode(g_ulmngdebugttyregfunclistid,
                                   (void**)(&pstnode));

    while (VOS_NULL != pstnode)
    {
        ulfindfuncflag = APP_TRUE;
        
        printf(" %s\r\n", (char*)(pstnode->aucfuncname));

        fflush(stdout);
        
        (void)vos_getcurrentopnode(g_ulmngdebugttyregfunclistid,
                                       (void**)(&pstnode));
    }

    if (APP_FALSE == ulfindfuncflag)
    {
        printf("\r\n No any command.\r\n");
    }

    fflush(stdout);

    return;
}

/*****************************************************************************
 �� �� ��  : app_getSpsCacheInputCmd
 ��������  : ��ʾ������ض�������Ϣ
 �������  : vos_u32 ulkeytype
 �������  : char* pucout
 �� �� ֵ  : void
 ���ú���  : 
 ��������  : 
 
 �޸���ʷ      :
  1.��    ��   : 2014��7��9��
    ��    ��   : Albort,Feng
    �޸�����   : �����ɺ���

*****************************************************************************/
void app_getSpsCacheInputCmd(vos_u32 ulkeytype, char* pucout)
{
    app_debugtty_cache_cmd_stru* pstnode = VOS_NULL;

    if (up_key == ulkeytype)
    {
        (void)vos_getnextnode(g_stcachecmdmng.ulcachelistid, (void**)(&pstnode));
    }else if (down_key == ulkeytype)
    {
        (void)vos_getprenode(g_stcachecmdmng.ulcachelistid, (void**)(&pstnode));
    }

    if (VOS_NULL == pstnode)
    {
        return;
    }
    
    memcpy(pucout, (char*)(pstnode->auccmdinfo),
           (strlen((char*)(pstnode->auccmdinfo)) + 1));
    
    return;
}

/*****************************************************************************
 �� �� ��  : app_cacheNewTtyInput
 ��������  : �����µĴ�������
 �������  : char* pucnewinput
 �������  : ��
 �� �� ֵ  : void
 ���ú���  : 
 ��������  : 
 
 �޸���ʷ      :
  1.��    ��   : 2014��7��9��
    ��    ��   : Albort,Feng
    �޸�����   : �����ɺ���

*****************************************************************************/
void app_cacheNewTtyInput(char* pucnewinput)
{
    vos_u32 ulnodekey = 0;
    vos_u32 ulrunrslt = VOS_OK;
    
    app_debugtty_cache_cmd_stru* pstnode = VOS_NULL;

    if(0 == strcmp("", pucnewinput))
    {
        (void)vos_listseek(g_stcachecmdmng.ulcachelistid, vos_list_offset_head);
        
        return;
    }
    
    if (APP_MAX_DEBUGTTY_CACHE_CMD_NUM <= g_stcachecmdmng.ulcachedinputnum)
    {//����ﵽ���,ɾ����ɵĻ�����Ϣ
        (void)vos_getlisttailnode(g_stcachecmdmng.ulcachelistid,
                                      (void**)(&pstnode));
        if (VOS_NULL != pstnode)
        {
            ulnodekey = pstnode->ulnodekey;

            vos_deletenode(g_stcachecmdmng.ulcachelistid, ulnodekey);
        }

        pstnode = VOS_NULL;
    }

    (void)vos_getlistheadnode(g_stcachecmdmng.ulcachelistid,
                                  (void**)(&pstnode));

    if (VOS_NULL != pstnode)
    {
        if (0 == strcmp((char*)(pstnode->auccmdinfo), pucnewinput))
        {//���ϴ�������ͬ������
            (void)vos_listseek(g_stcachecmdmng.ulcachelistid, vos_list_offset_head);
            return;
        }
    }
    
    g_stcachecmdmng.ulinputkeyindex -= 1;
    if (0 == g_stcachecmdmng.ulinputkeyindex)
    {
        g_stcachecmdmng.ulinputkeyindex = 0xffffffff;
    }

    ulrunrslt = vos_mallocnode(g_stcachecmdmng.ulcachelistid,
                               vos_list_sort_lifo_mode,
                               g_stcachecmdmng.ulinputkeyindex,
                               (void**)(&pstnode));
    if (VOS_OK != ulrunrslt)
    {
        WRITE_ERR_LOG(
                   "Call vos_mallocnode() failed(0x%x)\r\n",
                   ulrunrslt);
    }

    pstnode->ulnodekey = g_stcachecmdmng.ulinputkeyindex;

    memcpy((char*)(pstnode->auccmdinfo), pucnewinput,
           (strlen(pucnewinput) + 1));

    (void)vos_listseek(g_stcachecmdmng.ulcachelistid, vos_list_offset_head);
    
    return;
}

/*****************************************************************************
 �� �� ��  : app_procTabKeyDream
 ��������  : TAB�����봦����
 �������  : char* pucsonstr
 �������  : ��
 �� �� ֵ  : void
 ���ú���  : 
 ��������  : 
 
 �޸���ʷ      :
  1.��    ��   : 2014��7��10��
    ��    ��   : Albort,Feng
    �޸�����   : �����ɺ���

*****************************************************************************/
void app_procTabKeyDream(char *pucsonstr)
{
    vos_u32 ulloopvar = 0;
    char* pucmatch  = VOS_NULL;
    char* pucrlstmv = VOS_NULL;
    char* puccmpbg  = VOS_NULL;
    char  aucmatchcmd[APP_MAX_CMD_LENGTH] = {'\0'};
    char  aucattach[APP_MAX_CMD_LENGTH]   = {'\0'};
    
    app_debugtty_mng_reg_func_stru* pstnode = VOS_NULL;

    puccmpbg = pucsonstr + (strlen(pucsonstr) + 1);
    while ((' ' != *puccmpbg) && (puccmpbg != pucsonstr))
    {
        puccmpbg -= 1;
    }

    if ((puccmpbg == pucsonstr) && (('.' != *pucsonstr) && ('/' != *(pucsonstr + 1))))
    {//ֻ��Ӧ�ó����Լ�������ƥ��
        (void)vos_listseek(g_ulmngdebugttyregfunclistid, vos_list_offset_head);
        (void)vos_getcurrentopnode(g_ulmngdebugttyregfunclistid,
                                   (void**)(&pstnode));
    
        while (VOS_NULL != pstnode)
        {
            if (VOS_NULL != strstr((char*)(pstnode->aucfuncname), pucsonstr))
            {
                if (0 == strcmp("", (char*)aucattach))
                {
                    memcpy((char*)aucattach, (char*)(pstnode->aucfuncname),
                           (strlen((char*)(pstnode->aucfuncname)) + 1));
                }else
                {
                    for (ulloopvar = 0; '\0' != aucattach[ulloopvar]; ulloopvar++)
                    {
                        if (aucattach[ulloopvar] != pstnode->aucfuncname[ulloopvar])
                        {
                            aucattach[ulloopvar] = '\0';
                            
                            break;
                        }
                    }
                }

                if (0 != strcmp("", (char*)aucmatchcmd))
                {
                    printf("\r                                                   ");
                
                    printf("\r %s\r\n", (char*)aucmatchcmd);

                    fflush(stdout);
                }
                
                memcpy((char*)aucmatchcmd, (char*)(pstnode->aucfuncname),
                           (strlen((char*)(pstnode->aucfuncname)) + 1));
            }

            (void)vos_getcurrentopnode(g_ulmngdebugttyregfunclistid,
                                           (void**)(&pstnode));
        }

        if (0 != strcmp(pucsonstr, (char*)aucmatchcmd))
        {
            printf("\r                                                   ");
        
            printf("\r %s\r\n", (char*)aucmatchcmd);

            fflush(stdout);
        }

        if (0 != strcmp("", (char*)aucattach))
        {//��linux����
            memcpy(pucsonstr, (char*)aucattach, (strlen((char*)aucattach) + 1));
        }

        return;
    }

    //ֻ��linux����Ĳ���ƥ��
    if (('.' == *pucsonstr) && ('/' == *(pucsonstr + 1)))
    {
        puccmpbg += 2;
    }else
    {
        puccmpbg += 1;
    }
    
    if ('\0' == *puccmpbg)
    {
        return;
    }
    
    memset((char*)aucattach, 0x00, APP_MAX_CMD_LENGTH);
    
    //����linuxԭʼ�����tab����չ
    pucrlstmv = g_pucexelinuxcmdrslt;
    while ('\0' != *pucrlstmv)
    {
        if (' ' == *pucrlstmv)
        {
            *pucrlstmv = '\0';
        }

        pucrlstmv += 1;
    }

    pucrlstmv = g_pucexelinuxcmdrslt;
    while (('\0' != *pucrlstmv) && ('\0' != *(pucrlstmv + 1)))
    {
        pucmatch = strstr(pucrlstmv, puccmpbg);
        
        if ((VOS_NULL != pucmatch)
         && (pucrlstmv == pucmatch))
        {
            if (0 == strcmp("", (char*)aucattach))
            {
                memcpy((char*)aucattach, pucrlstmv, (strlen(pucrlstmv) + 1));
            }else
            {
                for (ulloopvar = 0; '\0' != aucattach[ulloopvar]; ulloopvar++)
                {
                    if (aucattach[ulloopvar] != *(pucrlstmv + ulloopvar))
                    {
                        aucattach[ulloopvar] = '\0';
                        
                        break;
                    }
                }
            }
        }

        while ('\0' != *pucrlstmv)
        {
            pucrlstmv += 1;
        }

        pucrlstmv += 1;
    }

    if (0 != strcmp("", (char*)aucattach))
    {
        memcpy(puccmpbg, (char*)aucattach, (strlen((char*)aucattach) + 1));
    }
    
    return;
}

/*****************************************************************************
 �� �� ��  : app_delStringTrim
 ��������  : ɾ���ַ���ͷβ�Ķ���ո�
 �������  : char* pstr
 �������  : ��
 �� �� ֵ  : void
 ���ú���  : 
 ��������  : 
 
 �޸���ʷ      :
  1.��    ��   : 2014��7��9��
    ��    ��   : Albort,Feng
    �޸�����   : �����ɺ���

*****************************************************************************/
void app_delStringTrim(char* pstr)
{
    char  auctmpbuf[1024] = {'\0'};
    char* pmv = VOS_NULL;

    pmv = pstr;
    while (('\0' != (*pmv)) && ((' '== (*pmv)) || ('\t'== (*pmv))))
    {
        pmv += 1;
    }

    memcpy((char*)auctmpbuf, pmv, (strlen(pmv) + 1));

    pmv = &(auctmpbuf[strlen((char*)auctmpbuf)]);

    while (pmv != &(auctmpbuf[0]))
    {
        if ((' ' == (*pmv)) || ('\t' == (*pmv)) || ('\0' == (*pmv)))
        {
            *pmv = '\0';
        }else
        {
            break;
        }

        pmv -= 1;
    }

    memset(pstr, 0x00, (strlen(pstr) + 1));
    
    memcpy(pstr, (char*)auctmpbuf, (strlen((char*)auctmpbuf) + 1));
    
    return;
}

/*****************************************************************************
 �� �� ��  : app_checkValidInputPara
 ��������  : ��������Ϸ���У��
 �������  : char* pucpara
 �������  : ��
 �� �� ֵ  : vos_u32
 ���ú���  : 
 ��������  : 
 
 �޸���ʷ      :
  1.��    ��   : 2014��7��11��
    ��    ��   : Albort,Feng
    �޸�����   : �����ɺ���

*****************************************************************************/
vos_u32 app_checkValidInputPara(char* pucpara)
{
    vos_u32 ulrunrslt   = app_debugtty_cmd_para_valid_dec;
    char  ucchar      = 0;
    vos_u32 ulindex     = 0;
    const vos_u32 ulmaxthrd = 512;

    if (0 == strcmp("", pucpara))
    {
        return app_debugtty_cmd_para_null;
    }

    if (('0' == (*pucpara)) && (('x' == (*(pucpara +1))) || ('X' == (*(pucpara +1)))))
    {//ʮ�����Ƽ��
        ulrunrslt = app_debugtty_cmd_para_valid_hex;
        
        for (ulindex = 2;
             (('\0' != (*(pucpara + ulindex))) && (ulmaxthrd > ulindex));
             ulindex++)
        {
            ucchar = *(pucpara + ulindex);
            
            if ((('0' >  ucchar) || ('9' < ucchar))
             && (('a' != ucchar) && ('A' != ucchar))
             && (('b' != ucchar) && ('B' != ucchar))
             && (('c' != ucchar) && ('C' != ucchar))
             && (('d' != ucchar) && ('D' != ucchar))
             && (('e' != ucchar) && ('E' != ucchar))
             && (('f' != ucchar) && ('F' != ucchar)))
            {
                ulrunrslt = app_debugtty_cmd_para_valid_butt;
                break;
            }
        }

        return ulrunrslt;
    }

    ulrunrslt = app_debugtty_cmd_para_valid_dec;
    
    //����ʮ���Ƽ��
    for(ulindex = 0;
        (('\0' != (*(pucpara + ulindex))) && (ulmaxthrd > ulindex)); ulindex++)
    {
        ucchar = *(pucpara + ulindex);
        
        if (('0' > ucchar)  || ('9' < ucchar))
        {
            ulrunrslt = app_debugtty_cmd_para_valid_butt;
            break;
        }
    }
    
    return ulrunrslt;
}

/*****************************************************************************
 �� �� ��  : app_excuteLinuxCmd
 ��������  : ִ��linuxϵͳԭʼ�����
 �������  : vos_u32 ulcmdtype
             char* puccmdstr
 �������  : ��
 �� �� ֵ  : void
 ���ú���  : 
 ��������  : 
 
 �޸���ʷ      :
  1.��    ��   : 2014��7��29��
    ��    ��   : Albort,Feng
    �޸�����   : �����ɺ���

*****************************************************************************/
void app_excuteLinuxCmd(vos_u32 ulcmdtype, char* puccmdstr)
{
    char* puccmdmv  = VOS_NULL;
    char* pucrsltmv = VOS_NULL;
    vos_u32 ulrunrslt = VOS_OK;
    char aucbuf[1024] = {'\0'};
    
    FILE *ptr = VOS_NULL;

    if ((app_debugtty_cmd_cd_type == ulcmdtype)
     || (app_debugtty_cmd_ls_type == ulcmdtype))
    {
        if (app_debugtty_cmd_cd_type == ulcmdtype)
        {
            puccmdmv = puccmdstr;
            while (' ' != *puccmdmv)
            {
                puccmdmv += 1;
            }
            
            puccmdmv += 1;
            
            ulrunrslt = chdir(puccmdmv);
            if (VOS_OK != ulrunrslt)
            {
                printf("\r cd %s excute failed\r\n", puccmdmv);
                return;
            }
        }

        memset(g_pucexelinuxcmdrslt, '\0', 1024*2);

        ptr = popen("ls", "r");
        if (VOS_NULL == ptr)
        {
            printf("open pop failed\r\n");
            fflush(stdout);
            
            return;
        }
        
        while(fgets((char*)aucbuf, 1024, ptr) != VOS_NULL)
        {
            pucrsltmv = (char*)aucbuf;
            
            while ('\0' != *pucrsltmv)
            {
                if (('\r' == *pucrsltmv)
                 || ('\n' == *pucrsltmv))
                {
                    *pucrsltmv = '\0';
                }

                pucrsltmv += 1;
            }
            
            strcat(g_pucexelinuxcmdrslt, aucbuf);

            strcat(g_pucexelinuxcmdrslt, " ");

            memset((char*)aucbuf, 0x00, 1024);
        }

        pclose(ptr);
        ptr = NULL;

        if (app_debugtty_cmd_ls_type == ulcmdtype)
        {
            printf(" %s\r\n", g_pucexelinuxcmdrslt);
        }
    }else if (app_debugtty_cmd_vi_type == ulcmdtype)
    {
        ulrunrslt = system(puccmdstr);
        if (VOS_OK != ulrunrslt)
        {
            printf("\r %s excute failed\r\n", puccmdstr);
            return;
        }
    }else
    {
        ptr = popen(puccmdstr, "r");
        if (VOS_NULL == ptr)
        {
            printf("open pop failed\r\n");
            fflush(stdout);
            
            return;
        }

        while(fgets((char*)aucbuf, 1024, ptr) !=NULL )
        {
            printf("%s", (char*)aucbuf);

            memset((char*)aucbuf, 0x00, 1024);
        }

        pclose(ptr);
        ptr = VOS_NULL;

        printf("\r\n->");
    }

    if (VOS_OK != ulrunrslt)
    {
        printf("%s excute failed\r\n", puccmdstr);
    }
    
    return;
}

/*****************************************************************************
 �� �� ��  : app_strnumToIntnum
 ��������  : �ַ�������ת������������
 �������  : char* pucstrnum
             vos_u32* pulintnum
 �������  : ��
 �� �� ֵ  : vos_u32
 ���ú���  : 
 ��������  : 
 
 �޸���ʷ      :
  1.��    ��   : 2014��7��31��
    ��    ��   : Albort,Feng
    �޸�����   : �����ɺ���

*****************************************************************************/
vos_u32 app_strnumToIntnum(char* pucstrnum, vos_u32* pulintnum)
{
    vos_u32 ulrunrslt     = VOS_OK;
    vos_u32 ulparaerrflag = APP_FALSE;
    char* puctmpbuf     = VOS_NULL;

    ulrunrslt = app_checkValidInputPara(pucstrnum);

    switch (ulrunrslt)
    {
        case app_debugtty_cmd_para_valid_hex:
            *pulintnum = strtol((const char*)pucstrnum, (char**)&puctmpbuf, 16);
            break;
        case app_debugtty_cmd_para_valid_dec:
            *pulintnum = atoi(pucstrnum);
            break;
        case app_debugtty_cmd_para_null:
            break;
        default:
            printf("\r input parameter error!\r\n");
            ulparaerrflag = APP_TRUE;
            break;
    }

    if (APP_TRUE == ulparaerrflag)
    {
        return (vos_u32)APP_COM_ERR;
    }
    
    return VOS_OK;
}
/*****************************************************************************
 �� �� ��  : app_printThreadEnter
 ��������  : ���ڴ�ӡ�߳���ں���
 �������  : void* para
 �������  : ��
 �� �� ֵ  : vos_u32
 ���ú���  : 
 ��������  : 
 
 �޸���ʷ      :
  1.��    ��   : 2014��10��15��
    ��    ��   : Albort,Feng
    �޸�����   : �����ɺ���

*****************************************************************************/
vos_u32 app_printThreadEnter(void* para)
{
    vos_u32 ulreadidx = (vos_u32)(g_pstmnguartprtbuf->usreadidx);

    if (APP_MAX_PRINT_RING_BUF_LEN <= ulreadidx)
    {
        ulreadidx = 0;
    }

    while (APP_YES != g_pstmnguartprtbuf->astprintbuf[ulreadidx].ucnullflag)
    {
        printf("%s", (char *)(g_pstmnguartprtbuf->astprintbuf[ulreadidx].aucprintbuf));

        fflush(stdout);

        g_pstmnguartprtbuf->astprintbuf[ulreadidx].ucnullflag = APP_YES;

        ulreadidx += 1;
        if (APP_MAX_PRINT_RING_BUF_LEN <= ulreadidx)
        {
            ulreadidx = 0;
        }
    }

    g_pstmnguartprtbuf->usreadidx = (app_u16)ulreadidx;

    //������Ϣ��ӡ��ɺ��߳��Լ����Լ�����
    vos_suspendthread(g_pstmnguartprtbuf->threadid);

    return VOS_OK;
}

/*****************************************************************************
 �� �� ��  : app_cmdwinThreadEnter
 ��������  : ��������߳����
 �������  : void* para
 �������  : ��
 �� �� ֵ  : vos_u32
 ���ú���  : 
 ��������  : 
 
 �޸���ʷ      :
  1.��    ��   : 2014��7��9��
    ��    ��   : Albort,Feng
    �޸�����   : �����ɺ���

*****************************************************************************/
vos_u32 app_cmdwinThreadEnter(void *para)
{
    vos_u32       ulrunrslt      = VOS_OK;
    vos_u32       ulinputlen     = 0;
    vos_u32       ulcounter      = 0;
    vos_u32       ullinuxcmdflag = APP_FALSE;
    vos_u32       ulcmdtype      = APP_FALSE;
    vos_u32       ulinputchar    = 0;
    vos_u32       uldelbeginflag = APP_FALSE;
    char        auccmdinput[APP_MAX_DBGTTY_INPUT_LENGTH]    = { '\0' };
    char        auccmdname[APP_MAX_CMD_LENGTH]     = { '\0' };
    vos_u32       ulindex        = 0;
    pdebugcmdfunc pfuncaddr      = VOS_NULL;
    pshowcmdhelp  phfuncaddr     = VOS_NULL;
    vos_u32       ulfirstflag    = APP_TRUE;
    char        *phead         = VOS_NULL;
    char        *ptail         = VOS_NULL;
    char        auccmdpara[APP_MAX_DEBUGTTY_PARA_NUM][APP_MAX_LEN_PER_SINGLE_PARA]     = { 0 };

#ifdef COMPILER_IS_LINUX_GCC
    if (APP_TRUE != app_getterminateflag())
#endif
    {
        printf("\r->");
        fflush(stdout);
    }

    memset((char *)auccmdinput, 0x00, APP_MAX_DBGTTY_INPUT_LENGTH);
    memset((char *)auccmdname,  0x00, APP_MAX_CMD_LENGTH);

    for (ulcounter = 0; APP_MAX_DEBUGTTY_PARA_NUM > ulcounter; ulcounter++)
    {
        memset((char *)&(auccmdpara[ulcounter][0]), 0x00, APP_MAX_LEN_PER_SINGLE_PARA);
    }

    while (1)
    {
        ulinputchar = 0;
        fflush(stdin);
        ulinputchar = getchar();
        fflush(stdin);
        //fflush(stdout);

        if (null_key1 == ulinputchar)
        {
            uldelbeginflag = APP_TRUE;

            continue;
        }

        if ((unknown_key == ulinputchar) && (APP_TRUE == uldelbeginflag))
        {
            continue;
        }

        if ((null_key2 == ulinputchar) && (APP_TRUE == uldelbeginflag))
        {
            continue;
        }

        uldelbeginflag = APP_FALSE;

        if (enter_key == ulinputchar)
        { //enter��
#ifdef COMPILER_IS_ARM_LINUX_GCC
            //printf("\r\n");
            fflush(stdout);
#endif
            break;
        }

        if ((tab_key   != ulinputchar) && (enter_key != ulinputchar)
            && (up_key    != ulinputchar) && (down_key  != ulinputchar)
            && (right_key != ulinputchar) && (left_key  != ulinputchar)
            && (null_key0 != ulinputchar) && (null_key1 != ulinputchar)
            && (backspace_key != ulinputchar) && (del_key != ulinputchar))
        {
#ifdef COMPILER_IS_ARM_LINUX_GCC
            //printf("%c", (char)ulinputchar);
            fflush(stdout);
#endif
            if (ulindex < APP_MAX_CMD_LENGTH)
            {
                auccmdinput[ulindex++] = (char)ulinputchar;
                continue;
            } else
            {
                return VOS_OK;
            }
        }

        if (tab_key == ulinputchar)
        {
            app_procTabKeyDream((char *)auccmdinput);
            printf("\r                                                          ");
            fflush(stdout);

            printf("\r->%s", (char *)auccmdinput);
            fflush(stdout);

            ulindex = strlen((char *)auccmdinput);

            continue;
        }

        if ((up_key == ulinputchar) || (down_key == ulinputchar))
        {
            app_getSpsCacheInputCmd(ulinputchar, (char *)auccmdinput);

            ulindex = strlen((char *)auccmdinput);

            printf("\r                                                          ");
            fflush(stdout);

            printf("\r->%s", (char *)auccmdinput);
            fflush(stdout);

            continue;
        }

        if ((backspace_key == ulinputchar) || (del_key == ulinputchar))
        {
            if (0 < ulindex)
            {
                auccmdinput[--ulindex] = '\0';
            }

            printf("\r                                                          ");
            fflush(stdout);

            printf("\r->%s", (char *)auccmdinput);
            fflush(stdout);

            continue;
        }
    }

    app_delStringTrim((char *)auccmdinput);

    app_cacheNewTtyInput((char *)auccmdinput);

    /*����������������*/
    ulinputlen = strlen((char *)auccmdinput) + 1;
    ulcounter  = 0;
    phead      = (char *)auccmdinput;
    ptail      = (char *)auccmdinput;
    ulindex    = 0;

    while (('\0' != (*ptail)) && (ulinputlen > ulcounter))
    {
        while (('\0' != (*ptail)) && (' ' != (*ptail)) && ('\t' != (*ptail)))
        {
            ulcounter += 1;
            ptail     += 1;
        }

        if (APP_TRUE == ulfirstflag)
        {
            ulfirstflag = APP_FALSE;

            *ptail = '\0';

            strcpy((char *)auccmdname, phead);

            if ((0 != strcmp("h", (const char *)auccmdname))
                && (0 != strcmp("?", (const char *)auccmdname)))
            {
                if (VOS_NULL == app_debugttyFuncNameMaptoAddr((char *)auccmdname))
                {
                    if (VOS_NULL == app_debugttyfuncnameMaptoHelpaddr((char *)auccmdname))
                    { //ִ��linux���������
                        ullinuxcmdflag = APP_TRUE;
                        if (0 == strcmp("cd", (const char *)auccmdname))
                        {
                            ulcmdtype = app_debugtty_cmd_cd_type;
                        } else if (0 == strcmp("vi", (const char *)auccmdname))
                        {
                            ulcmdtype = app_debugtty_cmd_vi_type;
                        } else if ((0 == strcmp("ls", (const char *)auccmdname))
                                   || (0 == strcmp("dir", (const char *)auccmdname)))
                        {
                            ulcmdtype = app_debugtty_cmd_ls_type;
                        } else
                        {
                            ulcmdtype = app_debugtty_cmd_other_type;
                        }

                        *ptail = ' ';
                        break;
                    }
                }
            }

            ptail     += 1;
            ulcounter += 1;

            if ('\0' == (*ptail))
            {
                break;
            }
        } else
        {
            if (APP_MAX_DEBUGTTY_PARA_NUM <= ulindex)
            {
                printf(" input too many parameter(max= d)\r\n");

                return VOS_OK;
            }

            *ptail = '\0';

            ptail     += 1;
            ulcounter += 1;

            memcpy(&(auccmdpara[ulindex++][0]), phead, (strlen(phead) + 1));
        }

        /*�˳�����ո�*/
        while (('\0' != (*ptail)) && ((' ' == (*ptail)) || ('\t' == (*ptail))))
        {
            ptail     += 1;
            ulcounter += 1;

            if (ulinputlen <= ulcounter)
            {
                break;
            }
        }

        phead = ptail;
    }

    if (0 == strcmp("", (const char *)(char *)auccmdname))
    { //���������û����������̫��
        return VOS_OK;
    }

    if (APP_TRUE == ullinuxcmdflag)
    {
        if (0 == strcmp("reboot", (const char *)(char *)auccmdname))
        {
			SENDREBOOT2DOWNM();
			
#ifdef COMPILER_IS_LINUX_GCC
            app_terminateprocess();
            return VOS_OK;
#endif
            vos_threadsleep(500);
        }

        app_excuteLinuxCmd(ulcmdtype, (char *)auccmdinput);

        return VOS_OK;
    }

    if ((0 == strcmp("h", (const char *)auccmdname))
        || (0 == strcmp("?", (const char *)auccmdname)))
    { //��ʾtty����İ�����Ϣ
        app_debugttyShowAllCmdInfo();
        app_showDebugttyHelpInfo();

        return VOS_OK;
    }

    if (0 == strcmp("h", (const char *)&(auccmdpara[0][0])))
    { //��ʾָ������İ�����Ϣ
        phfuncaddr = app_debugttyfuncnameMaptoHelpaddr((char *)auccmdname);
        if (VOS_NULL == phfuncaddr)
        {
            printf(" Not register function help info.\r\n");
            fflush(stdout);
        } else
        {
            phfuncaddr();
        }

        return VOS_OK;
    }

    /*���������ִ��*/
    pfuncaddr = app_debugttyFuncNameMaptoAddr((char *)auccmdname);
    if (VOS_NULL == pfuncaddr)
    {
        printf("\r\n This %s command hasn't been registered.\r\n",
               (char *)auccmdname);

        fflush(stdout);
    } else
    {
        ulrunrslt = pfuncaddr(&(auccmdpara[0][0]), &(auccmdpara[1][0]),
                              &(auccmdpara[2][0]), &(auccmdpara[3][0]),
                              &(auccmdpara[4][0]), &(auccmdpara[5][0]),
                              &(auccmdpara[6][0]), &(auccmdpara[7][0]),
                              &(auccmdpara[8][0]), &(auccmdpara[9][0]),
                              &(auccmdpara[10][0]), &(auccmdpara[11][0]),
                              &(auccmdpara[12][0]), &(auccmdpara[13][0]),
                              &(auccmdpara[14][0]), &(auccmdpara[15][0]),
                              &(auccmdpara[16][0]), &(auccmdpara[17][0]),
                              &(auccmdpara[18][0]), &(auccmdpara[19][0]),
                              &(auccmdpara[20][0]), &(auccmdpara[21][0]),
                              &(auccmdpara[22][0]), &(auccmdpara[23][0]),
                              &(auccmdpara[24][0]), &(auccmdpara[25][0]),
                              &(auccmdpara[26][0]), &(auccmdpara[27][0]),
                              &(auccmdpara[28][0]), &(auccmdpara[29][0])
                             );
        if (VOS_OK != ulrunrslt)
        {
            printf(" return code= 0x%x\r\n", (int)ulrunrslt);
            fflush(stdout);
        } 
        else
        {
            printf(" Start time: %s\n", GetCurTimeStr());
            fflush(stdout);
        }
    }

    return VOS_OK;
}
//zxc���������ӡ
void app_debugprint(app_dwnmach_action_res_msg_stru* pstmsg)
{
    switch(pstmsg->encmdtype)
    {
        case 2:
            break;
        case 3:
            break;
        default:
            break;
    }
}

/*****************************************************************************
 �� �� ��  : app_procDebugDwnmachResmsg
 ��������  : ά��ģ�鴦����λ����Ӧ��Ϣ
 �������  : app_dwnmach_action_res_msg_stru* pstmsg
 �������  : ��
 �� �� ֵ  : vos_u32
 ���ú���  : 
 ��������  : 
 
 �޸���ʷ      :
  1.��    ��   : 2014��7��9��
    ��    ��   : Albort,Feng
    �޸�����   : �����ɺ���

*****************************************************************************/
vos_u32 app_procDebugDwnmachResmsg(app_dwnmach_action_res_msg_stru* pstmsg)
{
    printf("\r\n The 0x%x command(subsystem= %d) has been responded(result= %d)\r\n",
           pstmsg->encmdtype, pstmsg->endwnmachsubsys, pstmsg->encmdexerslt);
    
    printf(" End time: %s\n->", GetCurTimeStr());

    fflush(stdout);
 
    app_debugprint(pstmsg);
    return VOS_OK;
}

/*****************************************************************************
 �� �� ��  : app_procDebugSubctrlResmsg
 ��������  : ά��ģ�������ϵͳ��Ӧ��Ϣ������
 �������  : ��
 �������  : ��
 �� �� ֵ  : void
 ���ú���  : 
 ��������  : 
 
 �޸���ʷ      :
  1.��    ��   : 2014��7��18��
    ��    ��   : Albort,Feng
    �޸�����   : �����ɺ���

*****************************************************************************/
vos_u32 app_procDebugSubctrlResmsg(app_com_dev_act_result_stru* pstresmsg)
{
    printf("The command has been responded(acttype = %d,result = %d)\r\n",pstresmsg->enacttype,pstresmsg->enactrslt);
 
    printf(" End time: %s\n->", GetCurTimeStr());
    fflush(stdout);
    
    return VOS_OK;
}

/*****************************************************************************
 �� �� ��  : app_procDebugOpExtDevactionResmsg
 ��������  : �������ⲿ�豸����������Ӧ��Ϣ����
 �������  : app_op_extdev_res_msg_stru* pstresmsg
 �������  : ��
 �� �� ֵ  : vos_u32
 ���ú���  : 
 ��������  : 
 
 �޸���ʷ      :
  1.��    ��   : 2014��9��19��
    ��    ��   : Albort,Feng
    �޸�����   : �����ɺ���

*****************************************************************************/
vos_u32 app_procDebugOpExtDevactionResmsg(app_op_extdev_res_msg_stru* pstresmsg)
{
    app_midmach_op_lpbck_res_msg_stru* pstlpbckres = VOS_NULL;
    
    switch (pstresmsg->enextdevname)
    {
        case app_midmach_extdev_name_fpga:
            switch (pstresmsg->enopmode)
            {
                case app_op_extdev_action_reset:
                    if (xc8002_exe_cmd_sucess == pstresmsg->enexerslt)
                    {
                        printf("\r Reset mid machine's FPGA successfully.\r\n");
                        
                    }else
                    {
                        printf("\r Reset mid machine's FPGA failed.\r\n");
                    }
                    break;

                case app_op_extdev_action_set_lpbck_md:
                    pstlpbckres = (app_midmach_op_lpbck_res_msg_stru*)pstresmsg;
                    if (xc8002_exe_cmd_sucess == pstlpbckres->enexerslt)
                    {
                        printf("\r Set uart%d loopback mode successfully.\r\n",
                               pstlpbckres->enuartname);
                    }else
                    {
                        printf("\r Set uart%d loopback mode failed.\r\n",
                               pstlpbckres->enuartname);
                    }
                    break;

                case app_op_extdev_action_clr_lpbck_md:
                    pstlpbckres = (app_midmach_op_lpbck_res_msg_stru*)pstresmsg;
                    if (xc8002_exe_cmd_sucess == pstlpbckres->enexerslt)
                    {
                        printf("\r Clear uart%d loopback mode successfully.\r\n",
                               pstlpbckres->enuartname);
                    }else
                    {
                        printf("\r Clear uart%d loopback mode failed.\r\n",
                               pstlpbckres->enuartname);
                    }
                    break;

                case app_op_extdev_action_query_wk_status:
                    pstlpbckres = (app_midmach_op_lpbck_res_msg_stru*)pstresmsg;
                    if (app_midmach_extdev_status_normal == pstlpbckres->enuartname)
                    {
                        printf("\r Mid machine's FPGA is normal.\r\n");
                    }else
                    {
                        printf("\r Mid machine's FPGA is abnormal.\r\n");
                    }
                    break;
                    
                default:
                    break;
            }
            break;

        case app_midmach_extdev_name_drvebi:
            break;

        default:
            break;
    }

    fflush(stdout);
    return VOS_OK;
}

/*****************************************************************************
 �� �� ��  : app_debugttyPidmsgRcvCallback
 ��������  : ά��ģ��PID��Ϣ���ջص�����
 �������  : vos_msg_header_stru* pstvosmsg
 �������  : ��
 �� �� ֵ  : vos_u32
 ���ú���  : 
 ��������  : 
 
 �޸���ʷ      :
  1.��    ��   : 2014��7��9��
    ��    ��   : Albort,Feng
    �޸�����   : �����ɺ���

*****************************************************************************/
vos_u32 app_debugttyPidmsgRcvCallback(vos_msg_header_stru* pstvosmsg)
{
    vos_u32 ulrunrslt = VOS_OK;
   
    switch (pstvosmsg->usmsgtype)
    {
        case app_res_dwnmach_act_cmd_msg_type:
            ulrunrslt = app_procDebugDwnmachResmsg((app_dwnmach_action_res_msg_stru*)pstvosmsg);
            break;

        case app_com_device_res_exe_act_msg_type:
            ulrunrslt = app_procDebugSubctrlResmsg((app_com_dev_act_result_stru*)&(pstvosmsg->aucmsgload[0]));
            break;

        case app_operate_extdev_res_msg_type:
            ulrunrslt = app_procDebugOpExtDevactionResmsg((app_op_extdev_res_msg_stru*)pstvosmsg);
            break;
        case MSG_ResReset:
        {
            CMsg stMsg;
            stMsg.ParseMsg(pstvosmsg);
            STResResult * pPara = (STResResult *)stMsg.pPara;
            printf("Callback  Sucess.Msgtype=%lu, Result=%d\n", pstvosmsg->usmsgtype, pPara->uiresult());
            return VOS_OK;
        }
        case vos_pidmsgtype_timer_timeout:
        {
            (app_ad_fpga_work_mode_tst == ADmode) ? staticTestAD() : dynamicTsetAD();
            startTimer(ADtimer, ADtime);
            break;
        }
        default:
            printf("DEBUG -- callback Unknown msg type = %lu\n",pstvosmsg->usmsgtype);
            ulrunrslt = GENERRNO(app_maintain_errorcode_base, app_unknown_msg_type_err);            
            break;
    }
    if(VOS_OK != ulrunrslt)
    {
        WRITE_ERR_LOG(" failed! smsgtype = %d \n", pstvosmsg->usmsgtype);
    }
    return ulrunrslt;
}

/*****************************************************************************
 �� �� ��  : app_initDebugttymd
 ��������  : ά��ģ���ʼ������
 �������  : void
 �������  : ��
 �� �� ֵ  : vos_u32
 ���ú���  : 
 ��������  : 
 
 �޸���ʷ      :
  1.��    ��   : 2014��7��9��
    ��    ��   : Albort,Feng
    �޸�����   : �����ɺ���

*****************************************************************************/
vos_u32 app_initDebugttymd(void)
{
    vos_u32 ulrunrslt = VOS_OK;
    vos_u32 ulloopvar = 0;
    vos_multi_thread_para_stru stthdinfo;
    struct termios new_settings;
    ulrunrslt = vos_mallocmemory(sizeof(app_mng_uartprintbuf_stru),
                                 vos_mem_type_static,
                                 (void**)(&g_pstmnguartprtbuf));
    if (VOS_OK != ulrunrslt)
    {
        return ulrunrslt;
    }
    
    for (ulloopvar = 0; APP_MAX_PRINT_RING_BUF_LEN > ulloopvar; ulloopvar++)
    {
        g_pstmnguartprtbuf->astprintbuf[ulloopvar].ucnullflag = APP_YES;
    }

    g_pstmnguartprtbuf->usreadidx  = 0;
    g_pstmnguartprtbuf->uswriteidx = 0;
    ulrunrslt = vos_mallocmemory(1024*2,vos_mem_type_static,
                                 (void**)(&g_pucexelinuxcmdrslt));
    if (VOS_OK != ulrunrslt)
    {
        vos_freememory(g_pstmnguartprtbuf);
        g_pstmnguartprtbuf = VOS_NULL;
        WRITE_ERR_LOG("Call vos_mallocmemory() failed(0x%x)\r\n", ulrunrslt);
        return ulrunrslt;
    }

    ulrunrslt = vos_createlist(sizeof(app_debugtty_cache_cmd_stru),
                               &(g_stcachecmdmng.ulcachelistid));
    if (VOS_OK != ulrunrslt)
    {
        vos_freememory(g_pstmnguartprtbuf);
        g_pstmnguartprtbuf = VOS_NULL;
        vos_freememory(g_pucexelinuxcmdrslt);
        g_pucexelinuxcmdrslt = VOS_NULL;
        
        WRITE_ERR_LOG(
                   "Call vos_createlist() failed(0x%x)\r\n",
                   ulrunrslt);

        return ulrunrslt;
    }

    g_stcachecmdmng.ulcachedinputnum = 0;
    g_stcachecmdmng.ulinputkeyindex  = 0xffffffff;
    
    ulrunrslt = vos_createlist(sizeof(app_debugtty_mng_reg_func_stru),
                               &g_ulmngdebugttyregfunclistid);
    if (VOS_OK != ulrunrslt)
    {
        vos_freememory(g_pstmnguartprtbuf);
        g_pstmnguartprtbuf = VOS_NULL;
        vos_freememory(g_pucexelinuxcmdrslt);
        g_pucexelinuxcmdrslt = VOS_NULL;
        
        vos_deletelist(g_stcachecmdmng.ulcachelistid);
        g_stcachecmdmng.ulcachelistid = APP_INVALID_U32;
        
        WRITE_ERR_LOG(
                   "Call vos_createlist() failed(0x%x)\r\n",
                   ulrunrslt);

        return ulrunrslt;
    }

    stthdinfo.pthrdfunc    = app_printThreadEnter;
    stthdinfo.pthrdinpara  = VOS_NULL;
    stthdinfo.ulstacksize  = 512*512;
    stthdinfo.ulthreadpri  = vos_thread_pri_45;
    stthdinfo.arglen       = 0;
    stthdinfo.eninitstatus = vos_thread_init_status_running;
   
    ulrunrslt = vos_createthread(&stthdinfo);
    if (VOS_OK != ulrunrslt)
    {
        vos_freememory(g_pstmnguartprtbuf);
        g_pstmnguartprtbuf = VOS_NULL;
        
        vos_freememory(g_pucexelinuxcmdrslt);
        g_pucexelinuxcmdrslt = VOS_NULL;
        
        vos_deletelist(g_stcachecmdmng.ulcachelistid);
        g_stcachecmdmng.ulcachelistid = APP_INVALID_U32;
        
        vos_deletelist(g_ulmngdebugttyregfunclistid);
        g_ulmngdebugttyregfunclistid = APP_INVALID_U32;
        
        WRITE_ERR_LOG(
                   "Create the command win thread failed(0x%x)\r\n",
                   ulrunrslt);

        return ulrunrslt;
    }

    g_pstmnguartprtbuf->threadid = stthdinfo.ulthrdid;

    tcgetattr(0,&new_settings);
    new_settings.c_lflag &= (~ICANON);
    new_settings.c_cc[VTIME] = 0;
    new_settings.c_cc[VMIN]  = 1;
    tcsetattr(0,TCSANOW,&new_settings);
    
    stthdinfo.pthrdfunc    = app_cmdwinThreadEnter;
    stthdinfo.pthrdinpara  = VOS_NULL;
    stthdinfo.ulstacksize  = 1024*512;
    stthdinfo.ulthreadpri  = vos_thread_pri_40;
    stthdinfo.arglen       = 0;
    stthdinfo.eninitstatus = vos_thread_init_status_running;
    
    ulrunrslt = vos_createthread(&stthdinfo);
    if (VOS_OK != ulrunrslt)
    {
        vos_terminatethread(g_pstmnguartprtbuf->threadid);
        
        vos_freememory(g_pstmnguartprtbuf);
        g_pstmnguartprtbuf = VOS_NULL;
        
        vos_freememory(g_pucexelinuxcmdrslt);
        g_pucexelinuxcmdrslt = VOS_NULL;
        
        vos_deletelist(g_stcachecmdmng.ulcachelistid);
        g_stcachecmdmng.ulcachelistid = APP_INVALID_U32;
        
        vos_deletelist(g_ulmngdebugttyregfunclistid);
        g_ulmngdebugttyregfunclistid = APP_INVALID_U32;
        
        WRITE_ERR_LOG(
                   "Create the command win thread failed(0x%x)\r\n",
                   ulrunrslt);

        return ulrunrslt;
    }
    
    ulrunrslt = vos_registerpid(app_self_oam_fid,Pid_DebugTty,
                                app_debugttyPidmsgRcvCallback);
    if (VOS_OK != ulrunrslt)
    {
        vos_terminatethread(g_pstmnguartprtbuf->threadid);
        
        vos_freememory(g_pstmnguartprtbuf);
        g_pstmnguartprtbuf = VOS_NULL;

        vos_freememory(g_pucexelinuxcmdrslt);
        g_pucexelinuxcmdrslt = VOS_NULL;
        
        vos_terminatethread(stthdinfo.ulthrdid);

        vos_deletelist(g_stcachecmdmng.ulcachelistid);
        g_stcachecmdmng.ulcachelistid = APP_INVALID_U32;
        
        vos_deletelist(g_ulmngdebugttyregfunclistid);
        g_ulmngdebugttyregfunclistid = APP_INVALID_U32;
        
        WRITE_ERR_LOG("Register the %d pid failed(0x%x)\r\n", Pid_DebugTty, ulrunrslt);

        return ulrunrslt;
    }
    ADtimer = createTimer(Pid_DebugTty, "debug");
    return VOS_OK;
}

#ifdef __cplusplus
}
#endif
