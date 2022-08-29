/**********************************************************************************************************************
 * \file Cpu0_Main.c
 * \copyright Copyright (C) Infineon Technologies AG 2019
 * 
 * Use of this file is subject to the terms of use agreed between (i) you or the company in which ordinary course of 
 * business you are acting and (ii) Infineon Technologies AG or its licensees. If and as long as no such terms of use
 * are agreed, use of this file is subject to following:
 * 
 * Boost Software License - Version 1.0 - August 17th, 2003
 * 
 * Permission is hereby granted, free of charge, to any person or organization obtaining a copy of the software and 
 * accompanying documentation covered by this license (the "Software") to use, reproduce, display, distribute, execute,
 * and transmit the Software, and to prepare derivative works of the Software, and to permit third-parties to whom the
 * Software is furnished to do so, all subject to the following:
 * 
 * The copyright notices in the Software and this entire statement, including the above license grant, this restriction
 * and the following disclaimer, must be included in all copies of the Software, in whole or in part, and all 
 * derivative works of the Software, unless such copies or derivative works are solely in the form of 
 * machine-executable object code generated by a source language processor.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT SHALL THE 
 * COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN 
 * CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS 
 * IN THE SOFTWARE.
 *********************************************************************************************************************/
#include "user_define.h"

#define LED

#ifdef LED

IfxCpu_syncEvent g_cpuSyncEvent = 0;

/* Initialize LED (RED & BLUE) */
void init_LED(void)
{
    /* Reset PC1 & PC2 in IOCR0*/
    PORT10_IOCR0 &= ~((0x1F) << PC1);
    PORT10_IOCR0 &= ~((0x1F) << PC2);

    /* Set PC1 & PC2 with push-pull(2b10000) */
    PORT10_IOCR0 |= ((0x10) << PC1);
    PORT10_IOCR0 |= ((0x10) << PC2);

    // init LED RED ON
    PORT10_OMR |= ((0<<PCL1) | (1<<PS1));
}

int core0_main(void)
{
    IfxCpu_enableInterrupts();
    
    /* !!WATCHDOG0 AND SAFETY WATCHDOG ARE DISABLED HERE!!
     * Enable the watchdogs and service them periodically if it is required
     */
    IfxScuWdt_disableCpuWatchdog(IfxScuWdt_getCpuWatchdogPassword());
    IfxScuWdt_disableSafetyWatchdog(IfxScuWdt_getSafetyWatchdogPassword());
    
    /* Wait for CPU sync event */
    IfxCpu_emitEvent(&g_cpuSyncEvent);
    IfxCpu_waitEvent(&g_cpuSyncEvent, 1);
    
    volatile int cycle;

    init_LED();

/*
    unsigned int systemtick[2];
    unsigned int tick_cnt;
    unsigned int delay_time_ns;
    volatile int i;

    // Delay Check
    systemtick[0] = SYSTEM_TIMER_31_0;
    for (i = 0; i < 10000000; i++)
        systemtick[1] = SYSTEM_TIMER_31_0;

    // Calculate tick cnt
    tick_cnt = systemtick[1] - systemtick[0];

    // Calculate delay time, cnt * 10ns
    delay_time_ns = tick_cnt * SYSTEM_TIMER_PERIOD;
    printf("Delay is %d \n\n", delay_time_ns);
*/

    while(1)
    {
//        PORT10_OMR |= ((1<<PCL1) | (1<<PS1));           // Toggle LED RED
//        for(cycle = 0; cycle < 10000000 ; cycle++);     // Delay

        for(cycle = 0; cycle < 10000000 ; cycle++);     // Delay
        PORT10_OMR |= ((1<<PCL1) | (1<<PS1));           // Toggle LED RED
        PORT10_OMR |= ((1<<PCL2) | (1<<PS2));           // Toggle LED BLUE


    }
    return (1);
}

#endif
