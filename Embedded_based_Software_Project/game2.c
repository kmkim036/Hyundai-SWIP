#include "user_define.h"

#define TAG_THREHSHOLD 10

IfxCpu_syncEvent g_cpuSyncEvent = 0;

int tagger = 0;

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
    PORT10_OMR |= ((0 << PCL1) | (1 << PS1));
}

/* Initialize RGB LED */
void init_RGBLED(void)
{
    /* Reset IOCR0 bits */
    PORT02_IOCR4 &= ~((0x1F) << PC7);
    PORT10_IOCR4 &= ~((0x1F) << PC5);
    PORT10_IOCR0 &= ~((0x1F) << PC3);

    /* Set PC bits in IOCR0 with push-pull(2b10000) */
    PORT02_IOCR4 |= ((0x10) << PC7);
    PORT10_IOCR4 |= ((0x10) << PC5);
    PORT10_IOCR0 |= ((0x10) << PC3);
}

void init_CCU60(void)
{
    /* CCU60 T12 Setting */

    /* Password Access to unlock WDTSCON0 */
    SCU_WDT_CPU0CON0 = ((SCU_WDT_CPU0CON0 ^ 0xFC) & ~(1 << LCK)) | (1 << ENDINIT);
    while ((SCU_WDTCPU0CON0 & (1 << LCK)) != 0)
        ;

    /* Modify Access to clear ENDINIT bit */
    SCU_WDT_CPU0CON0 = ((SCU_WDT_CPU0CON0 ^ 0xFC) | (1 << LCK)) & ~(1 << ENDINIT);
    while ((SCU_WDT_CPU0CON0 & (1 << LCK)) == 0)
        ;

    CCU60_CLC &= ~(1 << DISR); // Enable CCU60 Module

    /* Password Access to unlock WDTSCON0 */
    SCU_WDT_CPU0CON0 = ((SCU_WDT_CPU0CON0 ^ 0xFC) & ~(1 << LCK)) | (1 << ENDINIT);
    while ((SCU_WDT_CPU0CON0 & (1 << LCK)) != 0)
        ;

    /* Modify Access to clear ENDINIT bit */
    SCU_WDT_CPU0CON0 = ((SCU_WDT_CPU0CON0 ^ 0xFC) | (1 << LCK)) & ~(1 << ENDINIT);
    while ((SCU_WDT_CPU0CON0 & (1 << LCK)) == 0)
        ;

    while ((CCU60_CLC & (1 << DISS)) != 0)
        ; // Wait until module is enabled

    CCU60_TCTR0 &= ~((0x7) << T12CLK); // f_T12 = f_CCU6 / prescaler
    CCU60_TCTR0 |= ((0x2) << T12CLK); // F_CCU6 = 50 MHz, prescaler = 1024
    CCU60_TCTR0 |= (1 << T12PRE); // f_T12 = 48828 Hz

    CCU60_TCTR0 &= ~(1 << CTM); // T12 always counts up and continues counting
        // from zero after reaching the period value

    CCU60_T12PR = 162760 - 1; // Interrupt freq. = f_T12 / (period value  + 1)
    CCU60_TCTR4 |= (1 << T12STR); // Interrupt freq. = 2 Hz

    CCU60_T12 = 0; // Clear T12 counting value

    /* CCU60 T12 Interrupt Setting */
    CCU60_INP &= ~((0x3) << INPT12); // Service Request output SR0 is selected

    CCU60_IEN |= (1 << ENT12PM); // Enable Interrupt for T12 Period-Match

    /* SRC Interrupt Setting For CCU60 */
    SRC_CCU60_SR0 &= ~((0xFF) << SRPN); // Set Priority : 0x0A
    SRC_CCU60_SR0 |= ((0x0A) << SRPN);

    SRC_CCU60_SR0 &= ~((0x3) << TOS); // CPU0 services

    SRC_CCU60_SR0 |= (1 << SRE); // Service Request is enabled

    /* CCU60 T12 Start */
    CCU60_TCTR4 = (1 << T12RS); // T12 starts counting
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
    init_RGBLED();
    init_CCU60();

    while (1) {
        while (1) {
            if (tagger == 0) {
                PORT02_OMR |= (1 << PCL7); // Clear RGB LED RED
                PORT10_OMR |= (1 << PS5); // Set RGB LED GREEN
                PORT10_OMR |= (1 << PCL3); // Clear RGB LED BLUE
                if (distance < TAG_THREHSHOLD) {
                    PORT10_OMR |= ((0 << PCL2) | (1 << PS2)); // BLUE LED ON
                    PORT10_OMR |= ((1 << PCL1) | (1 << PS1)); // RED LED OFF
                    break;
                }
            } else {
                PORT02_OMR |= (1 << PS7); // Set LED RED
                PORT10_OMR |= (1 << PCL5); // Clear LED GREEN
                PORT10_OMR |= (1 << PCL3); // Clear LED BLUE
                if (distance changed) {
                    PORT10_OMR |= ((1 << PCL2) | (0 << PS2)); // BLUE LED OFF
                    PORT10_OMR |= ((0 << PCL1) | (1 << PS1)); // RED LED ON
                    break;
                }
            }
        }

        PORT02_OMR |= (1 << PCL7); // Clear LED RED
        PORT10_OMR |= (1 << PCL5); // Clear LED GREEN
        PORT10_OMR |= (1 << PCL3); // Clear LED BLUE

        while (1)
            ;
    }
    return (1);
}

__interrupt(0x0A) __vector_table(0) void CCU60_T12_ISR(void)
{
    if (tagger)
        tagger--;
    else
        tagger++;
}
