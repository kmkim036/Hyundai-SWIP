/*
 * user_define.h
 *
 *  Created on: 2022. 8. 30.
 *      Author: user
 */

#ifndef USER_DEFINE_H_
#define USER_DEFINE_H_

#endif /* USER_DEFINE_H_ */

#include <stdio.h>

#include "IfxCpu.h"
#include "IfxScuWdt.h"
#include "Ifx_Types.h"

// LED
#define PORT10_BASE (0xF003B000)
#define PORT10_IOCR0 (*(volatile unsigned int*)(PORT10_BASE + 0x10))
#define PORT10_OMR (*(volatile unsigned int*)(PORT10_BASE + 0x04))

#define PC0 0
#define PC1 11
#define PC2 19

#define PCL1 17
#define PCL2 18
#define PS1 1
#define PS2 2

// SYSTEM TIMER
#define SYSTEM_TIMER_31_0 *(unsigned int*)(0xF0000000 + 0x10)
#define SYSTEM_TIMER_PERIOD 10 // 100Mhz

// SW
#define PORT02_BASE (0xF003A200)
#define PORT02_IOCR0 (*(volatile unsigned int*)(PORT02_BASE + 0x10))
#define PORT02_IN (*(volatile unsigned int*)(PORT02_BASE + 0x24))

#define P0 0
#define P1 1

/* Define SCU Registers for Interrupt */
#define SCU_BASE (0xF0036000)
#define SCU_WDTSCON0 (*(volatile unsigned int*)(SCU_BASE + 0x0F0))
#define SCU_EICR1 (*(volatile unsigned int*)(SCU_BASE + 0x214))
#define SCU_IGCR0 (*(volatile unsigned int*)(SCU_BASE + 0x22C))

#define LCK 1
#define ENDINIT 0

#define IGP0 14
#define INP0 12
#define EIEN0 11
#define LDEN0 10
#define REN0 9
#define FEN0 8
#define EXIS0 4

#define IGP1 30
#define INP1 28
#define EIEN1 27
#define LDEN1 26
#define REN1 25
#define FEN1 24
#define EXIS1 20

/* Define SRC Registers for Interrupt */
#define SRC_BASE (0xF0038000)
#define SRC_SCUERU0 (*(volatile unsigned int*)(SRC_BASE + 0xCD4))
#define SRC_SCUERU1 (*(volatile unsigned int*)(SRC_BASE + 0xCD8))

#define TOS 11
#define SRE 10
#define SRPN 0

#define SYSTEM_TIMER_0_31_0 *(unsigned int *)(0xF0000000+0x10)

/* CCU60 Registers */
#define CCU60_BASE          (0xF0002A00)
#define CCU60_CLC           (*(volatile unsigned int*)(CCU60_BASE + 0x00))
#define CCU60_T12           (*(volatile unsigned int*)(CCU60_BASE + 0x20))
#define CCU60_T12PR         (*(volatile unsigned int*)(CCU60_BASE + 0x24))
#define CCU60_TCTR0         (*(volatile unsigned int*)(CCU60_BASE + 0x70))
#define CCU60_TCTR4         (*(volatile unsigned int*)(CCU60_BASE + 0x78))
#define CCU60_INP           (*(volatile unsigned int*)(CCU60_BASE + 0xAC))
#define CCU60_IEN           (*(volatile unsigned int*)(CCU60_BASE + 0xB0))

#define CCU61_BASE          (0xF0002B00)

typedef struct CCU6 {
    unsigned int CLC;
    unsigned int REG_04_1C[(0x20-0x04)/4];      // 0x04 ~ 0x1C
    unsigned int T12;                           // 0x20
    unsigned int T12PR;                         // 0x24
    unsigned int REG_28_6C[(0x70-0x28)/4];      // 0x28 ~ 0x6C
    unsigned int TCTR0;                         // 0x70
    unsigned int REG_74;                        // 0x74
    unsigned int TCTR4;                         // 0x78
    unsigned int REG_7C_A8[(0xAC-0x7C)/4];      // 0x7C ~ 0xA8
    unsigned int INP;                           // 0xAC
    unsigned int IEN;                           // 0xB0
} myCCU6;

#define CCU60 (*(volatile myCCU6 *)CCU60_BASE)
// FIXME: add CCU61
#define CCU61 (*(volatile myCCU6 *)CCU61_BASE)

#define DISS                1
#define DISR                0
#define CTM                 7
#define T12PRE              3
#define T12CLK              0
#define T12STR              6
#define T12RS               1
#define INPT12              10
#define ENT12PM             7

/* Define PORT10 Registers for LED */
#define PORT10_BASE     (0xF003B000)
#define PORT10_IOCR0    (*(volatile unsigned int*)(PORT10_BASE + 0x10))
#define PORT10_OMR      (*(volatile unsigned int*)(PORT10_BASE + 0x04))

#define PC0             3
#define PC1             11
#define PC2             19

#define PCL1            17
#define PCL2            18
#define PS1             1
#define PS2             2


