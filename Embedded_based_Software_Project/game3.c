#include "user_define.h"

IfxCpu_syncEvent g_cpuSyncEvent = 0;

#define PWM_FREQ 6250000

int num_tones = 1;
//int tones[] = {261, 277, 294, 311, 330, 349, 370, 392};

unsigned volatile int adcResult1;
unsigned volatile int adcResult2;

void init_Buzzer(void)
{
    PORT02_IOCR0 &= ~((0x1F) << PC3); // PORT02.3 : Alternate output function 1 (push-pull)
    PORT02_IOCR0 |= ((0x11) << PC3); // PORT02.3 : GTM_TOUT3
}

void init_GTM_TOM0_PWM(void)
{
    /* GTM Enable */
    // Password Access to unlock WDTCPU0CON0
    SCU_WDT_CPU0CON0 = ((SCU_WDT_CPU0CON0 ^ 0xFC) & ~(1 << LCK)) | (1 << ENDINIT);
    while ((SCU_WDT_CPU0CON0 & (1 << LCK)) != 0)
        ;

    // Modify Access to clear ENDINIT bit
    SCU_WDT_CPU0CON0 = ((SCU_WDT_CPU0CON0 ^ 0xFC) | (1 << LCK)) & ~(1 << ENDINIT);
    while ((SCU_WDT_CPU0CON0 & (1 << LCK)) == 0)
        ;

    GTM_CLC &= ~(1 << DISR); // Enable GTM Module

    // Password Access to unlock WDTCPU0CON0
    SCU_WDT_CPU0CON0 = ((SCU_WDT_CPU0CON0 ^ 0xFC) & ~(1 << LCK)) | (1 << ENDINIT);
    while ((SCU_WDT_CPU0CON0 & (1 << LCK)) != 0)
        ;

    // Modify Access to set ENDINIT bit
    SCU_WDT_CPU0CON0 = ((SCU_WDT_CPU0CON0 ^ 0xFC) | (1 << LCK)) | (1 << ENDINIT);
    while ((SCU_WDT_CPU0CON0 & (1 << LCK)) == 0)
        ;

    while ((GTM_CLC & (1 << DISS)) != 0)
        ; // Wait until module is enabled

    /* GTM Clock Setting */
    GTM_CMU_FXCLK_CTRL &= ~((0xF) << FXCLK_SEL); // Input clock of CMU_FXCLK : CMU_GCLK_EN

    GTM_CMU_CLK_EN &= ~((0x3) << EN_FXCLK); // Enable all CMU_FXCLK
    GTM_CMU_CLK_EN |= ((0x2) << EN_FXCLK);

    /* GTM TOM0 PWM Setting */
    GTM_TOM0_TGC1_GLB_CTRL &= ~((0x3) << UPEN_CTRL3); // TOM0 channel 11 enable update of
    GTM_TOM0_TGC1_GLB_CTRL |= ((0x2) << UPEN_CTRL3); // register CM0, CM1, CLK_SRC

    GTM_TOM0_TGC1_FUPD_CTRL &= ~((0x3) << FUPD_CTRL3); // Enable force update of TOM0 channel 11
    GTM_TOM0_TGC1_FUPD_CTRL |= ((0x2) << FUPD_CTRL3);
    GTM_TOM0_TGC1_FUPD_CTRL &= ~((0x3) << RSTCN0_CH3); // Reset CN0 of TOM0 channel 11 on force update
    GTM_TOM0_TGC1_FUPD_CTRL |= ((0x2) << RSTCN0_CH3);

    GTM_TOM0_TGC1_ENDIS_CTRL &= ~((0x3) << ENDIS_CTRL3); // Enable channel 11 on an update trigger
    GTM_TOM0_TGC1_ENDIS_CTRL |= ((0x2) << ENDIS_CTRL3);

    GTM_TOM0_TGC1_OUTEN_CTRL &= ~((0x3) << OUTEN_CTRL3); // Enable channel 11 output on an update trigger
    GTM_TOM0_TGC1_OUTEN_CTRL |= ((0x2) << OUTEN_CTRL3);

    GTM_TOM0_CH11_CTRL |= (1 << SL); // High signal level for duty cycle
    GTM_TOM0_CH11_CTRL &= ~((0x7) << CLK_SRC_SR); // Clock source : CMU_FXCLK(1) = 6250 kHz
    GTM_TOM0_CH11_CTRL |= (1 << CLK_SRC_SR);

    GTM_TOM0_CH11_SR0 = 12500 - 1; // PWM freq. = 6250 kHz / 12500 = 500 Hz
    GTM_TOM0_CH11_SR1 = 0; // Duty cycle = 0

    GTM_TOUTSEL0 &= ~((0x3) << SEL3); // TOUT3 : TOM0 channel 11

    GTM_TOM0_TGC1_GLB_CTRL |= (1 << HOST_TRIG); // Trigger request signal to update
}

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

void init_VADC(void)
{
    /* VADC Enable */
    /* Password Access to unlock WDTCPU0CON0 */
    SCU_WDT_CPU0CON0 = ((SCU_WDT_CPU0CON0 ^ 0xFC) & ~(1 << LCK)) | (1 << ENDINIT);
    while ((SCU_WDT_CPU0CON0 & (1 << LCK)) != 0)
        ;

    /* Modify Access to clear ENDINIT bit */
    SCU_WDT_CPU0CON0 = ((SCU_WDT_CPU0CON0 ^ 0xFC) | (1 << LCK)) & ~(1 << ENDINIT);
    while ((SCU_WDT_CPU0CON0 & (1 << LCK)) == 0)
        ;

    VADC_CLC &= ~(1 << DISR); // Enable VADC Module

    /* Password Access to unlock WDTSCPU0CON0 */
    SCU_WDT_CPU0CON0 = ((SCU_WDT_CPU0CON0 ^ 0xFC) & ~(1 << LCK)) | (1 << ENDINIT);
    while ((SCU_WDT_CPU0CON0 & (1 << LCK)) != 0)
        ;

    /* Modify Access to clear ENDINIT bit */
    SCU_WDT_CPU0CON0 = ((SCU_WDT_CPU0CON0 ^ 0xFC) | (1 << LCK)) | (1 << ENDINIT);
    while ((SCU_WDT_CPU0CON0 & (1 << LCK)) == 0)
        ;

    while ((VADC_CLC & (1 << DISS)) != 0)
        ; // Wait until module is enabled

    VADC_G4ARBPR |= ((0x3) << PRIO0); // Highest Priority for Request Source 0
    VADC_G4ARBPR &= ~(1 << CSM0); // Conversion Start Mode : Wait-for-start mode
    VADC_G4ARBPR |= (1 << ASEN0); // Arbitration Source Input 0 Enable

    VADC_G4QMR0 &= ~((0x3) << ENGT); // Enable Conversion Requests
    VADC_G4QMR0 |= ((0x1) << ENGT);
    VADC_G4QMR0 |= (1 << FLUSH); // Clear all Queue Entries

    VADC_G4ARBCFG |= ((0x3) << ANONC); // Analog Converter : Normal Operation
    VADC_G4ICLASS0 &= ~((0x7) << CMS); // Group-specific Class 0
    // Conversion Mode : Standard Conversion (12-bit)

    /* VADC Group 4 Channel 7 Setting */
    VADC_G4CHCTR7 |= (1 << RESPOS); // Read Results Right-aligned
    VADC_G4CHCTR7 &= ~((0xF) << RESREG); // Store Result in Group Result Register G4RES1
    VADC_G4CHCTR7 |= (1 << RESREG);
    VADC_G4CHCTR7 &= ~((0x3) << ICLSEL); // Use Group-specific Class 0

    /* VADC Group 4 Channel 6 Setting */
    VADC_G4CHCTR6 |= (1 << RESPOS); // Read Results Right-aligned
    VADC_G4CHCTR6 &= ~((0xF) << RESREG); // Store Result in Group Result Register G4RES1
    VADC_G4CHCTR6 |= (1 << RESREG);
    VADC_G4CHCTR6 &= ~((0x3) << ICLSEL); // Use Group-specific Class 0
}

void VADC_startConversion(void)
{
    /* No fill and Start Queue */
    VADC_G4QINR0 &= ~(0x1F); // Request Channel Number : 7
    VADC_G4QINR0 |= (0x07);
    VADC_G4QINR0 &= ~(1 << RF); // No fill : it is converted once
    VADC_G4QMR0 |= (1 << TREV); // Generate a Trigger Event
}

unsigned int VADC_readResult(void)
{
    unsigned int result;

    while ((VADC_G4RES1 & (1 << VF)) == 0)
        ; // Wait until New Result Available

    result = (VADC_G4RES1 & ((0xFFFF) << RESULT)); // Read Result

    return result;
}

unsigned int GetVADC4(int channel)
{
    unsigned int result;

    VADC_G4QINR0 = channel;
    VADC_G4QMR0 |= (1 << TREV); // Generate a Trigger Event

    while ((VADC_G4RES1 & (1 << VF)) == 0)
        ; // Wait until New Result Available

    result = (VADC_G4RES1 & ((0xFFFF) << RESULT)); // Read Result

    return result;
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

    init_Buzzer();
    init_GTM_TOM0_PWM();
    init_RGBLED();
    init_VADC();

    int temp = GetVADC4(6);

    for (int i = 0; i < 500000; i++) {
        adcResult1 = GetVADC4(6); //cd sensor(light sensor)
    }

    adcResult1 %= 3000;
    adcResult2 = GetVADC4(7); //potential meter

    //printf("light: %d pot: %d\n", adcResult1, adcResult2);

    GTM_CMU_CLK_EN &= ~((0x2) << EN_FXCLK); // buzzer pwm disable

    while (1) {
        adcResult2 = GetVADC4(7); //potential meter
        if (adcResult1 >= adcResult2) {
            PORT02_OMR |= (1 << PS7); // Set LED RED
            PORT10_OMR |= (1 << PCL5); // Clear LED GREEN
            PORT10_OMR |= (1 << PCL3); // Clear LED BLUE
            // printf("adcResult1 : %d  adcResult2 : %d \n", adcResult1, adcResult2);

            for (int i = 0; i < num_tones; i++) {
                int pwm_cnt = PWM_FREQ / 261;

                GTM_TOM0_CH11_SR0 = pwm_cnt;
                GTM_TOM0_CH11_SR1 = pwm_cnt / 2;

                GTM_CMU_CLK_EN |= ((0x2) << EN_FXCLK); // enable

                for (int j = 0; j < 50000000; j++)
                    ;

                GTM_CMU_CLK_EN &= ~((0x2) << EN_FXCLK); // disable
            }
            //break;
        } else {
            PORT02_OMR |= (1 << PCL7); // Clear LED RED
            PORT10_OMR |= (1 << PCL5); // Clear LED GREEN
            PORT10_OMR |= (1 << PS3); // Set LED BLUE
        }
    }
    return (1);
}
