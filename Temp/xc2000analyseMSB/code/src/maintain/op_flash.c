#include "op_flash.h"


#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <time.h>
#include <sys/ioctl.h>
#include <sys/mount.h>
#include <string.h>
#include <mtd/mtd-user.h>


#define GPIO_EXPORT "/sys/class/gpio/export"


void a5_ioCtrl(int value, string gpio)
{
    FILE *p = NULL;
    string GPIO_Dir = "/sys/class/gpio/gpio" + gpio + "/direction";
    string GPIO_Val = "/sys/class/gpio/gpio" + gpio + "/value";

    p = fopen(GPIO_EXPORT, "w");
    fprintf(p, "%s", gpio.c_str());
    fclose(p);

    p = fopen(GPIO_Dir.c_str(), "w");
    fprintf(p, "out");
    fclose(p);

    p = fopen(GPIO_Val.c_str(), "w");
    if (value == 0)
    {
        fprintf(p, "%d", 0);
        printf("=================== 打开%s引脚\n", gpio.c_str());
    }
    else
    {
        fprintf(p, "%d", 1);
        printf("=================== 关闭%s引脚\n", gpio.c_str());
    }
    fclose(p);
}

void Enable_244()
{
    string io = "26";
    a5_ioCtrl(0,io);
}

void Disable_244()
{
    string io = "26";
    a5_ioCtrl(1, io);
}

void Enable_WP()
{
    string io = "6";
    a5_ioCtrl(0, io);
}

void Disable_WP()
{
    string io = "6";
    a5_ioCtrl(1, io);
}

void Enable_PB()
{
    string io = "22";
    a5_ioCtrl(1, io);
}

void Disable_PB()
{
    string io = "22";
    a5_ioCtrl(0, io);
}

int non_region_erase(int Fd, int start, int count, int unlock)
{
    mtd_info_t meminfo;
    if (ioctl(Fd, MEMGETINFO, &meminfo) == 0)
    {
        erase_info_t erase;
        erase.start = start;
        erase.length = meminfo.erasesize;
        for (; count > 0; count--) {
            printf("\rPerforming Flash Erase of length %u at offset 0x%x",
                erase.length, erase.start);
            fflush(stdout);
            if (unlock != 0)
            {
                //Unlock the sector first.
                printf("\rPerforming Flash unlock at offset 0x%x", erase.start);
                if (ioctl(Fd, MEMUNLOCK, &erase) != 0)
                {
                    perror("\nMTD Unlock failure");
                    close(Fd);
                    return 8;
                }
            }
            if (ioctl(Fd, MEMERASE, &erase) != 0)
            {
                perror("\nMTD Erase failure");
                close(Fd);
                return 8;
            }
            erase.start += meminfo.erasesize;
        }
        printf(" done\n");
    }
    return 0;
}
vos_u32 m25p16_op(vos_u16 cmd, char* buff, vos_u32 len)
{
    int fd;

    struct mtd_info_user info;
    int regcount;
    int ret = 0;

    Enable_244();

    // Open the device
    if ((fd = open(MidFPGA_DEV, O_RDWR)) < 0)
    {
        fprintf(stderr, "File open error\n");
        Disable_244();
        return 8;
    }

    ioctl(fd, MEMGETINFO, &info);
    ioctl(fd, MEMGETREGIONCOUNT, &regcount);

    /*erase the device*/
    if (cmd == MidFPGA_CMD_ERASE)
    {
        if (regcount == 0)
        {
            non_region_erase(fd, 0, (info.size / info.erasesize), 0);
            printf("erase!\n");
        } 
    }
    else if (cmd == MidFPGA_CMD_WRITE)/*write file to this device*/
    {
        Disable_WP();//去掉写保护
        int ifd;
        if ((ifd = open("fpga_m.bin", O_RDONLY)) == -1)
        {
            printf("open input file error!\n");
            return 9;
        }

        int imglen = lseek(ifd, 0, SEEK_END);
        lseek(ifd, 0, SEEK_SET);
        printf("The input file image len is %d!\n", imglen);

        int readlen = 256;// info.writesize;
        char * write_buf = (char *)malloc(readlen);
        if (write_buf == NULL)
        {
            printf("can't allocate memory!\n");
            return 10;
        }

        int imglen1 = imglen;
        while (imglen)
        {
            int cnt, write_cnt;
            memset(write_buf, 0xff, readlen);
            if ((cnt = read(ifd, write_buf, readlen)) != readlen)
            {
                if (cnt < readlen)
                    printf("the file is end!\n");
                else
                {
                    printf("File I/O error on input file\n");
                    return 11;
                }
            }
            if ((write_cnt = write(fd, write_buf, readlen)) != readlen)
            {
                printf("write mtd device error! startaddr=%d\n", (imglen1 - imglen));
                return 12;
            }
            else
                printf("rwrite mtd device ok!startaddr=%d\n", (imglen1 - imglen));

            imglen -= cnt;
        }
        Enable_WP();//使能写保护
    }
    else if (cmd == MidFPGA_CMD_READ)/*read file from this device*/
    {
        ret = read(fd, buff, len);
        printf("read ok!\n");
    }

    Disable_244();
    return (vos_u32)ret;
}