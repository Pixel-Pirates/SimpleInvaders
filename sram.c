/*
 * sram.c
 *
 *  Created on: Mar 28, 2020
 *      Author: Raymond Bernardo
 */
//#include "driverlib.h"
#include <F28x_Project.h>

#define OE_select GPIO_WritePin(56, 0)
#define OE_deselect GPIO_WritePin(56, 1)
#define WE_select GPIO_WritePin(139, 0)
#define WE_deselect GPIO_WritePin(139, 1)

#define PORTA_MASK (((uint32_t)1 << 14) | ((uint32_t)1 << 15) | ((uint32_t)1 << 16) | ((uint32_t)1 << 22) | ((uint32_t)1 << 24) | ((uint32_t)1 << 25) | ((uint32_t)1 << 26) | ((uint32_t)1 << 27) | ((uint32_t)1 << 29))
#define PORTB_MASK (((uint32_t)1 << (41 - 32)) | ((uint32_t)1 << (63 - 32)) | ((uint32_t)1 << (52 - 32)) | ((uint32_t)1 << (52 - 32)) | ((uint32_t)1 << (40 - 32)))
#define PORTC_MASK (((uint32_t)1 << 31) | ((uint32_t)1 << 30) | ((uint32_t)1 << 3) | ((uint32_t)1 << 2) | ((uint32_t)1 << 1) | ((uint32_t)1 << 0))

#define A0
#define A1
#define A2
#define A3
#define A4
#define A5
#define A6
#define A7
#define A8
#define A9
#define A10
#define A11
#define A12
#define A13
#define A14
#define A15
#define A16
#define A17
#define A18
#define A19

inline void set_address(uint32_t addr)
{
    //GPIO14 - GPIO16 | A0 - A2
    uint32_t portA = (addr & 0b111)  << 14;
    //GPIO22 | A3
    portA |= (addr & 0b1000) << (22 - 3);
    //GPIO24 - GPIO27 | A4 - A7
    portA |= (addr & 0b11110000) << (24 - 4);
    //GPIO29 | A8
    portA |= (addr & 0b100000000) << (29 - 8);

    // GPIO97 - A9
    GPIO_WritePin(97, (addr >> 9) & 1);

    uint32_t portC = 0;
    // GPIO95 - A10
    portC |= (addr & ((uint32_t)1 << 10)) << ((95 - 64) - 10);
    // GPIO94 - A11
    portC |= (addr & ((uint32_t)1 << 11)) << ((94 - 64) - 11);
    // GPIO67 - A12
    portC |= (addr & ((uint32_t)1 << 12)) >> (12 - (67 - 64));
    // GPIO66 - A13
    portC |= (addr & ((uint32_t)1 << 13)) >> (13 - (66 - 64));


    uint32_t portB = 0;
    // GPIO41 - A14
    portB |= (addr & ((uint32_t)1 << 14)) >> (14 - (41 - 32));
    // GPIO65 - A15
    portC |= (addr & ((uint32_t)1 << 15)) >> (15 - (65 - 64));
    // GPIO64 - A16
    portC |= (addr & ((uint32_t)1 << 16)) >> (16 - (64 - 64));
    // GPIO63 - A17
    portB |= (addr & ((uint32_t)1 << 17)) << ((63 - 32) - 17);
    // GPIO52 - A18
    portB |= (addr & ((uint32_t)1 << 18)) << ((52 - 32) - 18);
    // GPIO40 - A19
    portB |= (addr & ((uint32_t)1 << 19)) >> (19 - (40 - 32));


    GpioDataRegs.GPASET.all = portA;
    GpioDataRegs.GPACLEAR.all = (~portA) & PORTA_MASK;

    GpioDataRegs.GPBSET.all = portB;
    GpioDataRegs.GPBCLEAR.all = (~portB) & PORTB_MASK;

    GpioDataRegs.GPCSET.all = portC;
    GpioDataRegs.GPCCLEAR.all = (~portC) & PORTC_MASK;
}

inline uint16_t get_data()
{
    return  GpioDataRegs.GPADAT.all & 0x7FF;
}

inline void set_data(uint16_t data)
{
    GpioDataRegs.GPASET.all = data & 0x7FF;
    GpioDataRegs.GPACLEAR.all = (~data) & 0x7FF;
}

static inline void set_data_in()
{
    GPIO_SetupPinOptions(0, GPIO_INPUT, 0);     // GPIO0 - D0
    GPIO_SetupPinOptions(1, GPIO_INPUT, 0);     // GPIO1 - D1
    GPIO_SetupPinOptions(2, GPIO_INPUT, 0);     // GPIO2 - D2
    GPIO_SetupPinOptions(3, GPIO_INPUT, 0);     // GPIO3 - D3
    GPIO_SetupPinOptions(4, GPIO_INPUT, 0);     // GPIO4 - D4
    GPIO_SetupPinOptions(5, GPIO_INPUT, 0);     // GPIO5 - D5
    GPIO_SetupPinOptions(6, GPIO_INPUT, 0);     // GPIO6 - D6
    GPIO_SetupPinOptions(7, GPIO_INPUT, 0);     // GPIO7 - D7
    GPIO_SetupPinOptions(8, GPIO_INPUT, 0);     // GPIO8 - D8
    GPIO_SetupPinOptions(9, GPIO_INPUT, 0);     // GPIO9 - D9
    GPIO_SetupPinOptions(10, GPIO_INPUT, 0);    // GPIO10 - D10
    GPIO_SetupPinOptions(11, GPIO_INPUT, 0);    // GPIO11 - D11
}

static inline void set_data_out()
{
    GPIO_SetupPinOptions(0, GPIO_OUTPUT, 0);     // GPIO0 - D0
    GPIO_SetupPinOptions(1, GPIO_OUTPUT, 0);     // GPIO1 - D1
    GPIO_SetupPinOptions(2, GPIO_OUTPUT, 0);     // GPIO2 - D2
    GPIO_SetupPinOptions(3, GPIO_OUTPUT, 0);     // GPIO3 - D3
    GPIO_SetupPinOptions(4, GPIO_OUTPUT, 0);     // GPIO4 - D4
    GPIO_SetupPinOptions(5, GPIO_OUTPUT, 0);     // GPIO5 - D5
    GPIO_SetupPinOptions(6, GPIO_OUTPUT, 0);     // GPIO6 - D6
    GPIO_SetupPinOptions(7, GPIO_OUTPUT, 0);     // GPIO7 - D7
    GPIO_SetupPinOptions(8, GPIO_OUTPUT, 0);     // GPIO8 - D8
    GPIO_SetupPinOptions(9, GPIO_OUTPUT, 0);     // GPIO9 - D9
    GPIO_SetupPinOptions(10, GPIO_OUTPUT, 0);    // GPIO10 - D10
    GPIO_SetupPinOptions(11, GPIO_OUTPUT, 0);    // GPIO11 - D11
}

bool outputIn = false;

uint16_t sram_read(uint32_t addr)
{
    set_address(addr);

    //set_data_in();
    if (!outputIn) {
        set_data_in();
        outputIn = true;
    }
    OE_select;
    //F28x_usDelay(1);
    return get_data();
}

void sram_write(uint32_t addr, uint16_t data)
{
    set_address(addr);
    OE_deselect;
    WE_select;
    set_data_out();
    set_data(data);
    //F28x_usDelay(1);
    WE_deselect;
}

void sram_write_multi_start()
{
    OE_deselect;
    WE_select;
    set_data_out();
}

void sram_write_multi_end()
{
    WE_deselect;
}

void sram_write_multi(uint32_t addr, uint16_t data)
{
    set_address(addr);
    set_data(data);
    //F28x_usDelay(1);
}

void sram_init()
{
    set_data_in();

    GPIO_SetupPinOptions(14, GPIO_OUTPUT, 0);    // GPIO14 - A0
    GPIO_SetupPinOptions(15, GPIO_OUTPUT, 0);    // GPIO15 - A1
    GPIO_SetupPinOptions(16, GPIO_OUTPUT, 0);    // GPIO16 - A2
    GPIO_SetupPinOptions(22, GPIO_OUTPUT, 0);    // GPIO22 - A3
    GPIO_SetupPinOptions(24, GPIO_OUTPUT, 0);    // GPIO24 - A4
    GPIO_SetupPinOptions(25, GPIO_OUTPUT, 0);    // GPIO25 - A5
    GPIO_SetupPinOptions(26, GPIO_OUTPUT, 0);    // GPIO26 - A6
    GPIO_SetupPinOptions(27, GPIO_OUTPUT, 0);    // GPIO27 - A7
    GPIO_SetupPinOptions(29, GPIO_OUTPUT, 0);    // GPIO29 - A8
    GPIO_SetupPinOptions(97, GPIO_OUTPUT, 0);    // GPIO97 - A9
    GPIO_SetupPinOptions(95, GPIO_OUTPUT, 0);    // GPIO95 - A10
    GPIO_SetupPinOptions(94, GPIO_OUTPUT, 0);    // GPIO94 - A11
    GPIO_SetupPinOptions(67, GPIO_OUTPUT, 0);    // GPIO67 - A12
    GPIO_SetupPinOptions(66, GPIO_OUTPUT, 0);    // GPIO66 - A13
    GPIO_SetupPinOptions(41, GPIO_OUTPUT, 0);    // GPIO41 - A14
    GPIO_SetupPinOptions(65, GPIO_OUTPUT, 0);    // GPIO65 - A15
    GPIO_SetupPinOptions(64, GPIO_OUTPUT, 0);    // GPIO64 - A16
    GPIO_SetupPinOptions(63, GPIO_OUTPUT, 0);    // GPIO63 - A17
    GPIO_SetupPinOptions(52, GPIO_OUTPUT, 0);    // GPIO52 - A18
    GPIO_SetupPinOptions(40, GPIO_OUTPUT, 0);    // GPIO40 - A19

    GPIO_SetupPinOptions(139, GPIO_OUTPUT, 0);    // GPIO139 - WE
    GPIO_SetupPinOptions(56, GPIO_OUTPUT, 0);    // GPIO56 - OE
    OE_deselect;
    WE_deselect;

    //scia_msg("TEST - READ WRITE\n\r");

//    uint32_t i;
//    for (i = 0; i < ((uint32_t)1 << 20); i++)
//    {
//        sram_write(i, (uint16_t)i + (i % 50));
//    }
//
//    int incorrect = 0;
//    for (i = 0; i < ((uint32_t)1 << 20); i++)
//    {
//        if (sram_read(i) != ((uint16_t)i + (i % 50)) & 0xFF)
//        {
//            //printNum((i & 0xff) + (i % 50));
//            //scia_msg(" != ");
//            //printNum(sram_read(i));
//            //scia_msg(" Incorrect read!\n\r");
//            incorrect++;
//        }
//    }
//    scia_msg("Incorrect: ");
//    printNum(incorrect);
//    scia_msg("\n\r");
//
//    scia_msg("Total: ");
//    printNum((uint32_t)1 << 20);
//    scia_msg("\n\r");
}

