/**
 * main.c
 */
#include <F28x_Project.h>
#include "inv.h"
#include "inv2.h"
#include "sram.h"
#include "fatfs/src/tff.h"
#include "uart.h"

static FATFS g_sFatFs;
static FIL g_sFileObject;
static FIL g_invader;
FRESULT fresult;

void writeAll(uint16_t color1, uint16_t color2, uint16_t color3);
void writeImage();
void writeSquare(uint32_t x_size, uint32_t y_size, FIL* obj);
void writeX(uint32_t x_size, uint32_t y_size, uint32_t x_pos, uint32_t y_pos, uint16_t factor, FIL* obj);
void writeInvader(uint32_t x_pos, uint32_t y_pos, uint16_t type);

#define INV_BEGIN_X   80
#define INV_END_X     140
#define INV_BEGIN_Y   140
#define INV_ROW     6
#define INV_PER_ROW 11

int main(void)
{
    sram_init();
    volatile unsigned long time = 0;
    InitSysCtrl();

    GPIO_SetupPinOptions(32, GPIO_OUTPUT, 0);
    GPIO_WritePin(32, 0);

    scia_msg("\rOpen SD Card\n");
    fresult = f_mount(0, &g_sFatFs);
    if(fresult != FR_OK) scia_msg("\rDid not mount\n");

//    fresult = f_open(&g_invader, "ismall.txt", FA_READ);
//    if(fresult != FR_OK) scia_msg("\rrs.txt did not open\n");

    fresult = f_open(&g_sFileObject, "bs.txt", FA_READ);
    if(fresult != FR_OK) scia_msg("\rvga.txt did not open\n");

    writeImage();
    GPIO_WritePin(32, 1);
    writeImage();
    GPIO_WritePin(32, 0);
    f_close(&g_sFileObject);

    uint32_t x_start = 120;
    uint16_t dir = 1, animation = 0;

    while(1)
    {
//        writeAll(RED, GREEN, BLUE);
//        writeX(13, 10, 120, 300, 3, &g_invader);
        for(uint32_t rows = 0; rows < INV_ROW; rows++)
        {
            for(uint32_t enemy = 0; enemy < INV_PER_ROW; enemy++)
            {
                writeInvader(x_start+(enemy*40), INV_BEGIN_Y+30*rows, animation & 2);
            }
        }
        if(dir)
        {
            x_start++;
            if(x_start == INV_END_X)
                dir = 0;
        }
        else
        {
            x_start--;
            if(x_start == INV_BEGIN_X)
                dir = 1;
        }

        GPIO_WritePin(32, 1);
//        writeX(13, 10, 120, 300, 3, &g_invader);
        for(uint32_t rows = 0; rows < INV_ROW; rows++)
        {
            for(uint32_t enemy = 0; enemy < INV_PER_ROW; enemy++)
            {
                writeInvader(x_start+(enemy*40), INV_BEGIN_Y+30*rows, animation & 2);
            }
        }
        if(dir)
        {
            x_start++;
            if(x_start == INV_END_X)
                dir = 0;
        }
        else
        {
            x_start--;
            if(x_start == INV_BEGIN_X)
                dir = 1;
        }
        GPIO_WritePin(32, 0);
        animation++;
    }

	return 0;
}

void writeInvader(uint32_t x_pos, uint32_t y_pos, uint16_t type)
{
    uint32_t addr = 0;
    sram_write_multi_start();

    for(uint32_t x = 0; x < 33; x++)
    {
        for(uint32_t y = 0; y < 25; y++)
        {
            addr = ((x_pos + x) << 9) | (y_pos + y);
            if(type)
                sram_write_multi(addr, inv[y][x]);
            else
                sram_write_multi(addr, inv2[y][x]);
        }
    }
    sram_write_multi_end();
}

void writeImage()
{
    //SPI-C
    //CS - Select GPIO125 (J2 12)
    //DO - MISO GPIO123 (J2 18)
    //SCK - CLK GPIO124 (J2 13)
    //DI - MOSI GPIO122 (J2 17)
    //CD - VCC

    unsigned short usBytesRead;

    uint32_t addr = 0;
    uint16_t buf[2] = {0, 0};

    sram_write_multi_start();

    for(uint32_t x = 0; x < 640; x++)
    {
        for(uint32_t y = 0; y < 480; y++)
        {
            addr = (x << 9) | y;
            fresult = f_read(&g_sFileObject, buf, 2, &usBytesRead);
            sram_write_multi(addr, buf[1] << 8 | buf[0]);
        }
    }

    f_lseek(&g_sFileObject, 0);
    sram_write_multi_end();
}

void writeSquare(uint32_t x_size, uint32_t y_size, FIL* obj)
{
    unsigned short usBytesRead;

    uint32_t addr = 0;
    uint16_t buf[2] = {0, 0};

    sram_write_multi_start();

    for(uint32_t x = 0; x < 33; x++)
    {
        for(uint32_t y = 0; y < 25; y++)
        {
            addr = ((120+x) << 9) | (300+y);
            fresult = f_read(obj, buf, 2, &usBytesRead);
            sram_write_multi(addr, buf[1] << 8 | buf[0]);
        }
    }

    f_lseek(obj, 0);
    sram_write_multi_end();
}

void writeX(uint32_t x_size, uint32_t y_size, uint32_t x_pos, uint32_t y_pos, uint16_t factor, FIL* obj)
{
    unsigned short usBytesRead;

    uint32_t addr = 0;
    uint16_t buf[2] = {0, 0};

    sram_write_multi_start();

    for(uint32_t x = 0; x < factor*x_size; x+=factor)
    {
        for(uint32_t y = 0; y < factor*y_size; y+=factor)
        {
            fresult = f_read(obj, buf, 2, &usBytesRead);
            for(uint16_t i = 0; i < factor; i++)
            {
                for(uint16_t j = 0; j < factor; j++)
                {
                    addr = ((x_pos+x+i) << 9) | (y_pos+y+j);
                    sram_write_multi(addr, buf[1] << 8 | buf[0]);
                }
            }
        }
    }

    f_lseek(obj, 0);
    sram_write_multi_end();
}

void writeAll(uint16_t color1, uint16_t color2, uint16_t color3)
{
    uint32_t addr = 0;
    uint16_t color = 0;

    sram_write_multi_start();

    for(uint32_t x = 0; x < 640; x++)
    {
        if(x < 213)
            color = color1;
        else if(x < 426)
            color = color2;
        else
            color = color3;

        for(uint32_t y = 0; y < 480; y++)
        {
            addr = (x << 9) | y;
            sram_write_multi(addr, color);
        }
    }

    sram_write_multi_end();
}
