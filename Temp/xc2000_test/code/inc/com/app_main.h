//checked
#ifndef _app_main_h_
#define _app_main_h_



#ifdef __cplusplus
extern "C" {
#endif

void app_terminateprocess(void);

#ifdef COMPILER_IS_LINUX_GCC

vos_u32 app_getterminateflag(void);
#endif

#ifdef __cplusplus
}
#endif

#endif