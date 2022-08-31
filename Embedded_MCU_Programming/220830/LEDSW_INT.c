#include "user_define.h"

IfxCpu_syncEvent g_cpuSyncEvent = 0;

/* Initialize LED (RED) */
void init_LED(void)
{
    /* Reset PC1 & PC2 in IOCR0*/
    PORT10_IOCR0 &= ~((0x1F) << PC1);
    PORT10_IOCR0 &= ~((0x1F) << PC2);

    /* Set PC1 & PC2 with push-pull(2b10000) */
    PORT10_IOCR0 |= ((0x10) << PC1);
    PORT10_IOCR0 |= ((0x10) << PC2);
}

/* Initialize Switch */
void init_Switch(void)
{
    /* Reset PC1 and PC0 in IOCR0*/
    PORT02_IOCR0 &= ~((0x1F) << PC1);
    PORT02_IOCR0 &= ~((0x1F) << PC0);

    /* Set PC1 and PC0 with pull-up(2b0xx10) */
    PORT02_IOCR0 |= ((0x2) << PC1);
    PORT02_IOCR0 |= ((0x2) << PC0);
}

/* Initialize External Request Unit (ERU) */
void init_ERU(void)
{
    /* ERU Input Channel 2 Setting */
    /* Password Access to unlock WDTSCON0 */
    SCU_WDTSCON0 = ((SCU_WDTSCON0 ^ 0xFC) & ~(1 << LCK)) | (1 << ENDINIT);
    while ((SCU_WDTSCON0 & (1 << LCK)) != 0)
        ;

    // Modify Access to clear ENDINIT bit
    SCU_WDTSCON0 = ((SCU_WDTSCON0 ^ 0xFC) | (1 << LCK)) & ~(1 << ENDINIT);
    while ((SCU_WDTSCON0 & (1 << LCK)) == 0)
        ;

    SCU_EICR1 &= ~(((0x7) << EXIS0) | ((0x7) << EXIS1)); // External input clear
    SCU_EICR1 &= ~(((0x7) << INP0) | ((0x7) << INP1)); // Input Node Pointer Clear
    SCU_IGCR0 &= ~(((0x3) << IGP0) | ((0x3) << IGP1)); // Interrupt Gating Patten 2, 3 Clear

    SCU_EICR1 |= (0x1 << EXIS0) | // P02.1 select
        (0x2 << EXIS1); // P02.0 select

    SCU_EICR1 |= (0x1 << FEN0) | // Falling edge enable
        (0x1 << FEN1);

    //    SCU_EICR1 |= (0x1 << LDEN0)      |                       // Level Detection edge enable
    //                 (0x1 << LDEN1)      ;

    SCU_EICR1 |= (0x1 << EIEN0) | // The trigger event is enabled
        (0x1 << EIEN1);

    SCU_EICR1 |= (0x0 << INP0) | // Trigger Input Channel 2 -> Output Channel 0
        (0x1 << INP1); // Trigger Input Channel 3 -> Output Channel 1

    SCU_IGCR0 |= (0x1 << IGP0) | // Input Channel 2 activated, pattern is not considered
        (0x1 << IGP1); // Input Channel 3 activated, pattern is not considered

    /* Password Access to unlock WDTSCON0 */
    SCU_WDTSCON0 = ((SCU_WDTSCON0 ^ 0xFC) & ~(1 << LCK)) | (1 << ENDINIT);
    while ((SCU_WDTSCON0 & (1 << LCK)) != 0)
        ;

    /* Modify Access to set ENDINIT bit */
    SCU_WDTSCON0 = ((SCU_WDTSCON0 ^ 0xFC) | (1 << LCK)) | (1 << ENDINIT);
    while ((SCU_WDTSCON0 & (1 << LCK)) == 0)
        ;

    /* SRC Interrupt Setting For ECU */
    SRC_SCUERU0 &= ~((0xFF) << SRPN); // Set Priority : 0x0A
    SRC_SCUERU0 |= ((0x0A) << SRPN);

    SRC_SCUERU0 |= (1 << SRE); // Service Request is enabled

    SRC_SCUERU1 &= ~((0xFF) << SRPN); // Set Priority : 0x0B
    SRC_SCUERU1 |= ((0x0B) << SRPN);

    SRC_SCUERU1 &= ~((0x3) << TOS); // CPU0 services

    SRC_SCUERU1 |= (1 << SRE); // Service Request is enabled
}

int SW1 = 0;
int SW2 = 0;

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

    init_ERU(); // Initialize ERU
    init_LED(); // Initialize LED
    init_Switch(); // Initialize Switch

    volatile int cycle;

    while (1) {
        /*
        if(SW1 && SW2) {
            PORT10_OMR |= (1 << PS1); // LED RED on
            PORT10_OMR |= (1 << PCL2); // LED BLUE off
            for (int i = 0; i < 4; i++) {
                for (cycle = 0; cycle < 10000000; cycle++)
                    ; // Delay
                PORT10_OMR |= ((1 << PCL1) | (1 << PS1)); // Toggle LED RED
                PORT10_OMR |= ((1 << PCL2) | (1 << PS2)); // Toggle LED BLUE
                SW1 = 0;
                SW2 = 0;
            }
        } else if (SW1) {
            PORT10_OMR |= ((1<<PCL1) | (1<<PS1));           // Toggle LED RED
            SW1 = 0;
        } else if(SW2) {
            PORT10_OMR |= ((1<<PCL2) | (1<<PS2));           // Toggle LED BLUE
            SW2 = 0;
        }
        */
        /*
        if(SW1){
            if(SW2) {
                PORT10_OMR |= (1 << PS1); // LED RED on
                PORT10_OMR |= (1 << PCL2); // LED BLUE off
                for (int i = 0; i < 4; i++) {
                    for (cycle = 0; cycle < 10000000; cycle++)
                    ; // Delay
                    PORT10_OMR |= ((1 << PCL1) | (1 << PS1)); // Toggle LED RED
                    PORT10_OMR |= ((1 << PCL2) | (1 << PS2)); // Toggle LED BLUE
                    SW1 = 0;
                    SW2 = 0;
                }
            } else {
                PORT10_OMR |= ((1<<PCL1) | (1<<PS1));           // Toggle LED RED
                for (cycle = 0; cycle < 10000000; cycle++)
                ;
                SW1 = 0;
            }
        } else if (SW2) {
            if(SW1) {
                PORT10_OMR |= (1 << PS1); // LED RED on
                PORT10_OMR |= (1 << PCL2); // LED BLUE off
                for (int i = 0; i < 4; i++) {
                    for (cycle = 0; cycle < 10000000; cycle++)
                    ; // Delay
                    PORT10_OMR |= ((1 << PCL1) | (1 << PS1)); // Toggle LED RED
                    PORT10_OMR |= ((1 << PCL2) | (1 << PS2)); // Toggle LED BLUE
                    SW1 = 0;
                    SW2 = 0;
                }
            } else {
                PORT10_OMR |= ((1<<PCL2) | (1<<PS2));           // Toggle LED BLUE
                for (cycle = 0; cycle < 10000000; cycle++)
                ;
                SW2 = 0;
            }
        }
        */
        if (SW1) {
            for (cycle = 0; cycle < 1000000; cycle++)
                ;
            if (SW2) {
                PORT10_OMR |= (1 << PS1); // LED RED on
                PORT10_OMR |= (1 << PCL2); // LED BLUE off
                for (int i = 0; i < 4; i++) {
                    for (cycle = 0; cycle < 10000000; cycle++)
                        ; // Delay
                    PORT10_OMR |= ((1 << PCL1) | (1 << PS1)); // Toggle LED RED
                    PORT10_OMR |= ((1 << PCL2) | (1 << PS2)); // Toggle LED BLUE
                    SW1 = 0;
                    SW2 = 0;
                }
            } else {
                PORT10_OMR |= ((1 << PCL1) | (1 << PS1)); // Toggle LED RED
                SW1 = 0;
            }
        } else if (SW2) {
            for (cycle = 0; cycle < 1000000; cycle++)
                ;
            if (SW1) {
                PORT10_OMR |= (1 << PS1); // LED RED on
                PORT10_OMR |= (1 << PCL2); // LED BLUE off
                for (int i = 0; i < 4; i++) {
                    for (cycle = 0; cycle < 10000000; cycle++)
                        ; // Delay
                    PORT10_OMR |= ((1 << PCL1) | (1 << PS1)); // Toggle LED RED
                    PORT10_OMR |= ((1 << PCL2) | (1 << PS2)); // Toggle LED BLUE
                    SW1 = 0;
                    SW2 = 0;
                }
            } else {
                PORT10_OMR |= ((1 << PCL2) | (1 << PS2)); // Toggle LED BLUE
                SW2 = 0;
            }
        }
    }
    return (1);
}

__interrupt(0x0A) __vector_table(0) void ERU0_ISR(void)
{
    SW1++;
}

__interrupt(0x0B) __vector_table(0) void ERU1_ISR(void)
{
    SW2++;
}
