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

IfxCpu_syncEvent g_cpuSyncEvent = 0;

void init_LED(void)
{
    /* Reset PC1 in IOCR0*/
    PORT10_IOCR0 &= ~((0x1F) << PC1);

    /* Set PC1 with push-pull(2b10000) */
    PORT10_IOCR0 |= ((0x10) << PC1);

    /* Reset PC2 in IOCR0*/
    PORT10_IOCR0 &= ~((0x1F) << PC2);

    /* Set PC1 with push-pull(2b10000) */
    PORT10_IOCR0 |= ((0x10) << PC2);

    /* LED OFF */
    PORT10_OMR = ((1<<PCL1) | (1<<PCL2));
}

void init_CCU6(myCCU6 *CCU6, int ms, int int_priority)
{
    /* CCU60 T12 Setting */

    /* Password Access to unlock WDTSCON0 */
    SCU_WDT_CPU0CON0 = ((SCU_WDT_CPU0CON0 ^ 0xFC) & ~(1 << LCK)) | (1 << ENDINIT);
    while((SCU_WDT_CPU0CON0 & (1 << LCK)) != 0);

    /* Modify Access to clear ENDINIT bit */
    SCU_WDT_CPU0CON0 = ((SCU_WDT_CPU0CON0 ^ 0xFC) | (1 << LCK)) & ~ (1 << ENDINIT);
    while((SCU_WDT_CPU0CON0 & (1 << LCK)) == 0);

    CCU6->CLC &= ~(1 << DISR);                     // Enable CCU61 Module

    /* Password Access to unlock WDTSCON0 */
    SCU_WDT_CPU0CON0 = ((SCU_WDT_CPU0CON0 ^ 0xFC) & ~(1 << LCK)) | (1 << ENDINIT);
    while((SCU_WDT_CPU0CON0 & (1 << LCK)) != 0);

    /* Modify Access to clear ENDINIT bit */
    SCU_WDT_CPU0CON0 = ((SCU_WDT_CPU0CON0 ^ 0xFC) | (1 << LCK)) | (1 << ENDINIT);
    while((SCU_WDT_CPU0CON0 & (1 << LCK)) == 0);

    while((CCU6->CLC & (1 << DISS)) != 0);       // Wait until module is enabled

    CCU6->TCTR0 &= ~((0x7) << T12CLK);           // f_T12 = f_CCU6 / prescaler, 100MHz
    CCU6->TCTR0 |= ((0x3) << T12CLK);            // F_CCU6 = 100 MHz, prescaler = 2048
    CCU6->TCTR0 |= (1 << T12PRE);                // f_T12 = 48828 Hz

    CCU6->TCTR0 &= ~(1 << CTM);                  // T12 always counts up and continues counting
                                                 // from zero after reaching the period value

    // FIXME: 0.5s -> ms
    CCU6->T12PR = 48828/2 - 1;                   // Interrupt freq. = f_T12 / (period value  + 1)
    CCU6->T12PR = (48828*ms)/1000 - 1;
    CCU6->TCTR4 |= (1 << T12STR);                // Interrupt freq. = 2 Hz

    CCU6->T12 = 0;                               // Clear T12 counting value

    /* CCU6 T12 Interrupt Setting */
    CCU6->INP &= ~((0x3) << INPT12);             // Service Request output SR0 is selected

    CCU6->IEN |= (1 << ENT12PM);                 // Enable Interrupt for T12 Period-Match

    /* SRC Interrupt Setting For CCU6 */
    if( (int)CCU6 == CCU60_BASE )
    {
        SRC_CCU60_SR0 &= ~((0xFF) << SRPN);           // Set Priority
        SRC_CCU60_SR0 |= (int_priority << SRPN);
        SRC_CCU60_SR0 &= ~((0x3) << TOS);             // CPU0 services
        SRC_CCU60_SR0 |= (1 << SRE);                  // Service Request is enabled
    }
    else
    {
        SRC_CCU61_SR0 &= ~((0xFF) << SRPN);           // Set Priority
        SRC_CCU61_SR0 |= (int_priority << SRPN);
        SRC_CCU61_SR0 &= ~((0x3) << TOS);             // CPU0 services
        SRC_CCU61_SR0 |= (1 << SRE);                  // Service Request is enabled
    }

    /* CCU6 T12 Start */
    CCU6->TCTR4 = (1 << T12RS);                  // T12 starts counting
}

// 10ms timer
// 500ms => 50
// 1000ms => 100

int timer_counts = 0;

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

    init_LED();
    
    init_CCU6((myCCU6 *)&CCU60,10,0x0A);

    while(1)
    {
        if(timer_counts == 50)
            PORT10_OMR |= ((1<<PCL1) | (1<<PS1));           // Toggle LED RED
        else if(timer_counts == 100) {
            PORT10_OMR |= ((1<<PCL1) | (1<<PS1));           // Toggle LED RED
            PORT10_OMR |= ((1<<PCL2) | (1<<PS2));           // Toggle LED BLUE
            timer_counts = 0;
        }
    }
    return (1);
}

__interrupt( 0x0A ) __vector_table( 0 )
void CCU60_T12_ISR(void)
{
    timer_counts++;
}
