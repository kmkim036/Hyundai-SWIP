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
