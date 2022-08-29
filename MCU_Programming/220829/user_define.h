/*
 * user_define.h
 *
 *  Created on: 2022. 8. 29.
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

#define PC0             0
#define PC1             11
#define PC2             19

#define PCL1 17
#define PCL2 18
#define PS1 1
#define PS2 2

// SYSTEM TIMER
#define SYSTEM_TIMER_31_0 *(unsigned int*)(0xF0000000 + 0x10)
#define SYSTEM_TIMER_PERIOD 10 // 100Mhz

// SW
#define PORT02_BASE     (0xF003A200)
#define PORT02_IOCR0    (*(volatile unsigned int*)(PORT02_BASE + 0x10))
#define PORT02_IN       (*(volatile unsigned int*)(PORT02_BASE + 0x24))

#define P0              0
#define P1              1

