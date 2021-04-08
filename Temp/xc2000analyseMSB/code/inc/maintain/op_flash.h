#ifndef __A5_GPIO_H__
#define __A5_GPIO_H__

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string>
#include "vos_basictypedefine.h"

using namespace std;

#define MidFPGA_CMD_ERASE   (0x01)
#define MidFPGA_CMD_READ    (0x02)
#define MidFPGA_CMD_WRITE   (0x03)
#define MidFPGA_DEV         "/dev/mtd8"

void a5_ioCtrl(int value, string gpio);

void Enable_244();
void Disable_244();
void Enable_WP();
void Disable_WP();
void Enable_PB();
void Disable_PB();
vos_u32 m25p16_op(vos_u16 cmd, char* buff, vos_u32 len);
#endif