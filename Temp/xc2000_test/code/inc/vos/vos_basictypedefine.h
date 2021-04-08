//checked
#ifndef _vos_basictypedef_h_
#define _vos_basictypedef_h_    

#define STATIC

#define VOS_INVALID_U32 (0xffffffff)

#define VOS_INVALID_FID    VOS_INVALID_U32
#define VOS_INVALID_PID    VOS_INVALID_U32

#define VOS_NULL  (0x00000000)
#define VOS_FALSE (0)
#define VOS_TRUE  (!(VOS_FALSE))

#define VOS_YES   (1)
#define VOS_NO    (0)

#define VOS_OK    0          //执行成功

#define VOS_COM_PID (0xffffffff)

#define VOS_NULL_SOCKET (0)

#define VOS_IP_BUF_LEN          (32)
#define VOS_MAX_PATH              (1024)

typedef unsigned short     vos_u16;
typedef unsigned long      vos_u32;
typedef signed long      vos_i32;

#define COUNTOF(ARR) (sizeof(ARR)/sizeof(ARR[0]))

#define APP_NULL_HANDLE (-1)
#define APP_COM_ERR     (-1)
#define APP_INVALID_U32 (VOS_INVALID_U32)

#define APP_FALSE (VOS_FALSE)
#define APP_TRUE  (VOS_TRUE)

#define APP_YES     (VOS_YES)
#define APP_NO      (VOS_NO)

#define APP_COM_PID (VOS_COM_PID)

#define APP_1M_SIZE (1024*1024)


#define APP_CLOSE (0)
#define APP_OPEN  (1)


#define APP_DWNMACH_CMDBUF_LEN    (2048*6)
#define APP_MAX_CMD_RSLT_INFO_LEN (200)  //命令结果信息最大长度

#define APP_DWNMACH_CMDBUF_RSVLEN (64*6)
#define APP_DWNMACH_CMDBUF_TOTAL_LEN (APP_DWNMACH_CMDBUF_LEN + APP_DWNMACH_CMDBUF_RSVLEN)

typedef vos_u32  app_u32;
typedef vos_u16  app_u16;
typedef vos_i32  app_i32;
typedef double  app_f64;

//状态机的状态迁移方向定义
typedef enum
{
    app_statemach_shift_dir_up = 0,  //状态向上迁移
    app_statemach_shift_dir_down = 1,  //状态向下迁移
    app_statemach_shift_dir_nomv = 2,  //原地不动
    app_statemach_run_over = 3,  //状态机运行完成

    app_statemach_shift_dir_butt
}app_statemach_shift_dir_enum;
#endif
