#ifndef _vos_mnglist_h_
#define _vos_mnglist_h_        
/******************************************************************************

                  版权所有 (C), 2013-2023, 四川新成生物科技有限公司

 ******************************************************************************
  文 件 名   : vos_mnglist.h
  版 本 号   : 初稿
  作    者   : Albort,Feng
  生成日期   : 2013年8月26日
  最近修改   :
  功能描述   : 通用链表管理头文件
  函数列表   :
  修改历史   :
  1.日    期   : 2013年8月26日
    作    者   : Albort,Feng
    修改内容   : 创建文件

******************************************************************************/
#include "vos_basictypedefine.h"

//链表id类型定义
typedef vos_u32 vos_listid;

//定义移动链表当前操作位置宏
typedef enum
{
    vos_list_offset_head = 0x80000001, //将链表当前操作位置移到链表头
    vos_list_offset_tail = 0x7fffffff, //将链表当前操作位置移到链表尾

    vos_list_offset_butt = 2
}vos_list_offset_enum;
typedef vos_u32 vos_list_offset_enum_u32;

//链表排序方式
typedef enum
{
    vos_list_sort_default_mode = 0, //默认链表排序方式
    vos_list_sort_self_mode    = 1, //用户自定义排序方式
    vos_list_sort_fifo_mode    = 2, //按照先将先出排序方式,即新节点在链表尾
    vos_list_sort_lifo_mode    = 3, //按照后进选出排序方式,即新节点在链表头
    vos_list_sort_desc_mode    = 4, //降序
    vos_list_sort_asc_mode     = 5, //升序 

    vos_list_sort_type_butt
}vos_list_sort_type_enum;
typedef vos_u32 vos_list_sort_type_enum_u32;

#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************************
 函 数 名  : vos_createlist
 功能描述  : 创建一个新链表
 输入参数  : vos_u32 ullistnodesize
 输出参数  : vos_listid* poutid
 返 回 值  : vos_u32
 
 修改历史      :
  1.日    期   : 2013年8月26日
    作    者   : Albort,Feng

    修改内容   : 新生成函数

*****************************************************************************/
vos_u32 vos_createlist(vos_u32 ullistnodesize, vos_listid* poutid);

/*****************************************************************************
 函 数 名  : vos_getlistlength
 功能描述  : 获取链表长度
 输入参数  : vos_listid listid
             vos_u32 *plistlength
 输出参数  : 无
 返 回 值  : vos_u32(VOS_OK:成功获取链表长度，其他:获取链表失败)
 
 修改历史      :
  1.日    期   : 2014年3月4日
    作    者   : zhangchaoyong

    修改内容   : 新生成函数

*****************************************************************************/
vos_u32 vos_getlistlength(vos_listid listid, vos_u32 *plistlength);

/*****************************************************************************
 函 数 名  : vos_getnode
 功能描述  : 由关键字获取节点
 输入参数  : vos_listid listid
             vos_u32 ulkeyword
 输出参数  : void** ppout
 返 回 值  : vos_u32
 
 修改历史      :
  1.日    期   : 2013年8月26日
    作    者   : Albort,Feng

    修改内容   : 新生成函数

*****************************************************************************/
vos_u32 vos_getnode(vos_listid listid, vos_u32 ulkeyword, void** ppout);

/*****************************************************************************
 函 数 名  : vos_getlistnodesize
 功能描述  : 获取链表节点大小
 输入参数  : vos_listid listid
             vos_u32 *plistnodesize
 输出参数  : 无
 返 回 值  : vos_u32(VOS_OK:成功获取链表长度，其他:获取链表失败)
 
 修改历史      :
  1.日    期   : 2014年3月4日
    作    者   : zhangchaoyong

    修改内容   : 新生成函数

*****************************************************************************/
vos_u32 vos_getlistnodesize(vos_listid listid, vos_u32 *plistnodesize);

/*****************************************************************************
 函 数 名  : vos_deletenode
 功能描述  : 删除由关键字指定的节点
 输入参数  : vos_listid listid
             vos_u32 ulkeyword
 输出参数  : 无
 返 回 值  : vos_u32
 
 修改历史      :
  1.日    期   : 2013年8月26日
    作    者   : Albort,Feng

    修改内容   : 新生成函数

*****************************************************************************/
vos_u32 vos_deletenode(vos_listid listid, vos_u32 ulkeyword);

/*****************************************************************************
 函 数 名  : vos_deletelist
 功能描述  : 删除整个链表
 输入参数  : vos_listid listid
 输出参数  : 无
 返 回 值  : vos_u32
 
 修改历史      :
  1.日    期   : 2013年8月27日
    作    者   : Albort,Feng

    修改内容   : 新生成函数

*****************************************************************************/
vos_u32 vos_deletelist(vos_listid listid);

/*****************************************************************************
 函 数 名  : vos_mallocnode
 功能描述  : 申请一个链表节点
 输入参数  : vos_listid listid
             vos_u32 ensortmd  (非默认方式下调用者需要输入有效keyword)
             vos_u32 ulkeyword
 输出参数  : void** ppoutmem
 返 回 值  : void*
 
 修改历史      :
  1.日    期   : 2013年8月26日
    作    者   : Albort,Feng

    修改内容   : 新生成函数

*****************************************************************************/
vos_u32 vos_mallocnode(vos_listid listid, vos_u32 ensortmd, vos_u32 ulkeyword,
                       void** ppoutmem);

/*****************************************************************************
 函 数 名  : vos_listseek
 功能描述  : 设置链表当前操作位置
 输入参数  : vos_listid listid
             vos_i32 sloffset
 输出参数  : 无
 返 回 值  : vos_u32
 
 修改历史      :
  1.日    期   : 2013年8月27日
    作    者   : Albort,Feng

    修改内容   : 新生成函数

*****************************************************************************/
vos_u32 vos_listseek(vos_listid listid, vos_i32 sloffset);

/*****************************************************************************
 函 数 名  : vos_getlistheadnode
 功能描述  : 获取链表头结点
 输入参数  : vos_listid listid
             void** ppoutnode
 输出参数  : 无
 返 回 值  : vos_u32
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2014年5月31日
    作    者   : Albort,Feng
    修改内容   : 新生成函数

*****************************************************************************/
vos_u32 vos_getlistheadnode(vos_listid listid, void** ppoutnode);

/*****************************************************************************
 函 数 名  : vos_getlisttailnode
 功能描述  : 获取链表尾结点
 输入参数  : vos_listid listid
             void** ppoutnode
 输出参数  : 无
 返 回 值  : vos_u32
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2014年5月31日
    作    者   : Albort,Feng
    修改内容   : 新生成函数

*****************************************************************************/
vos_u32 vos_getlisttailnode(vos_listid listid, void** ppoutnode);

/*****************************************************************************
 函 数 名  : vos_getcurrentopnode
 功能描述  : 获取当前操作位置的下一个节点
 输入参数  : vos_listid listid
 输出参数  : void** ppoutnode
 返 回 值  : vos_u32
 
 修改历史      :
  1.日    期   : 2013年8月27日
    作    者   : Albort,Feng

    修改内容   : 新生成函数

*****************************************************************************/
vos_u32 vos_getcurrentopnode(vos_listid listid, void** ppoutnode);

/*****************************************************************************
 函 数 名  : vos_getnextnode
 功能描述  : 获取链表下一个节点
 输入参数  : vos_listid listid
             void** ppoutnode
 输出参数  : 无
 返 回 值  : vos_u32
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2014年7月9日
    作    者   : Albort,Feng
    修改内容   : 新生成函数

*****************************************************************************/
vos_u32 vos_getnextnode(vos_listid listid, void** ppoutnode);

/*****************************************************************************
 函 数 名  : vos_getprenode
 功能描述  : 获取链表前一个节点
 输入参数  : vos_listid listid
             void** ppoutnode
 输出参数  : 无
 返 回 值  : vos_u32
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2014年7月9日
    作    者   : Albort,Feng
    修改内容   : 新生成函数

*****************************************************************************/
vos_u32 vos_getprenode(vos_listid listid, void** ppoutnode);

//链表模块初始化接口
vos_u32 vos_initlistmd(void);

#ifdef __cplusplus
}
#endif

#endif