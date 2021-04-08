#ifndef _vos_mngmemory_h_
#define _vos_mngmemory_h_   

#include "vos_basictypedefine.h"

//申请内存类型
typedef enum
{
    vos_mem_type_static  = 0,  //静态内存,长时间未使用系统不会释放内存
    vos_mem_type_dynamic = 1,  //动态内存,长时间未使用系统会自动释放内存

    vos_mem_type_butt
}vos_mem_type_enum;
typedef vos_u32 vos_mem_type_enum_u32;

#define DELETE_MEMEORY(POINT) \
    if (NULL != POINT) \
            { \
        vos_freememory(POINT);\
        POINT = NULL; \
            }

#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************************
 函 数 名  : vos_mallocmemory
 功能描述  : 申请内存接口
 输入参数  : vos_u32 ulmemsize
             vos_u32 enmemtype
 输出参数  : void** ppoutmem
 返 回 值  : vos_u32
 
 修改历史      :
  1.日    期   : 2013年8月27日
    作    者   : Albort,Feng

    修改内容   : 新生成函数

*****************************************************************************/
vos_u32 vos_mallocmemory(vos_u32 ulmemsize, vos_u32 enmemtype,
                         void** ppoutmem);

/*****************************************************************************
 函 数 名  : vos_freememory
 功能描述  : 释放内存
 输入参数  : void* pmemaddr
 输出参数  : 无
 返 回 值  : void
 
 修改历史      :
  1.日    期   : 2013年8月28日
    作    者   : Albort,Feng

    修改内容   : 新生成函数

*****************************************************************************/
void vos_freememory(void* pmemaddr);

/*****************************************************************************
 函 数 名  : vos_getusedmemsize
 功能描述  : 获取vos管理的已使用内存大小
 输入参数  : void
 输出参数  : 无
 返 回 值  : vos_u32
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2014年8月7日
    作    者   : Albort,Feng
    修改内容   : 新生成函数

*****************************************************************************/
vos_u32 vos_getusedmemsize(void);

/*****************************************************************************
 函 数 名  : vos_calccrc32
 功能描述  : 计算一块内存32bit的CRC值
 输入参数  : char* pmemaddr
             vos_u32 ulmemsize
 输出参数  : vos_u32* poutcrc
 返 回 值  : vos_u32
 
 修改历史      :
  1.日    期   : 2013年8月28日
    作    者   : Albort,Feng

    修改内容   : 新生成函数

*****************************************************************************/
vos_u32 vos_calccrc32(char* pmemaddr, vos_u32 ulmemsize, vos_u32* poutcrc);

//初始化内存管理模块
vos_u32 vos_initmemorymd(void);

#ifdef __cplusplus
}
#endif

#endif