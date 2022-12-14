#include "user_define.h"

#define PWM_FREQ 6250000
#define TAG_THREHSHOLD 5

/* Global Variables */
IfxCpu_syncEvent g_cpuSyncEvent = 0;

volatile unsigned int timer_cnt_ultra;
volatile unsigned int timer_cnt_mode;

volatile unsigned char irq_ultra_sensor;
volatile unsigned int start_time;
volatile unsigned int end_time;
volatile unsigned int interval_time;

int distance;
int distance_cmp;
int distance_now;

int tagger = 0;

void init_LED(void)
{
    /* Reset PC1 & PC2 in IOCR0*/
    PORT10_IOCR0 &= ~((0x1F) << PC1);
    PORT10_IOCR0 &= ~((0x1F) << PC2);

    /* Set PC1 & PC2 with push-pull(2b10000) */
    PORT10_IOCR0 |= ((0x10) << PC1);
    PORT10_IOCR0 |= ((0x10) << PC2);
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

void init_ultrasonic(void)
{
    /* Init TRIG Pin - P15.4 (Output) */
    PORT15_IOCR4 &= ~((0x1F) << PC4);
    PORT15_IOCR4 |= ((0x10) << PC4);

    /* Init ECHO Pin - P15.5 (Input) */
    PORT15_IOCR4 &= ~((0x1F) << PC5);
}

void init_ERU(void)
{
    /* ERU Input Channel 4 Setting */
    /* Password Access to unlock WDTSCON0 */
    SCU_WDTSCON0 = ((SCU_WDTSCON0 ^ 0xFC) & ~(1 << LCK)) | (1 << ENDINIT);
    while ((SCU_WDTSCON0 & (1 << LCK)) != 0)
        ;

    // Modify Access to clear ENDINIT bit
    SCU_WDTSCON0 = ((SCU_WDTSCON0 ^ 0xFC) | (1 << LCK)) & ~(1 << ENDINIT);
    while ((SCU_WDTSCON0 & (1 << LCK)) == 0)
        ;

    SCU_EICR2 &= ~((0x7) << EXIS0); // External input 3 is selected
    SCU_EICR2 |= ((0x3) << EXIS0);

    SCU_EICR2 |= (1 << REN0); // Rising edge enable
    SCU_EICR2 |= (1 << FEN0); // Falling edge enable

    SCU_EICR2 |= (1 << EIEN0); // The trigger event is enabled

    SCU_EICR2 &= ~((0x7) << INP0); // An event from input ETL 4 triggers output OGU 0

    SCU_IGCR0 &= ~((0x3) << IGP0); // IOUT(0) is activated in response to a trigger event
    SCU_IGCR0 |= (1 << IGP0); // The pattern is not considered

    /* Password Access to unlock WDTSCON0 */
    SCU_WDTSCON0 = ((SCU_WDTSCON0 ^ 0xFC) & ~(1 << LCK)) | (1 << ENDINIT);
    while ((SCU_WDTSCON0 & (1 << LCK)) != 0)
        ;

    /* Modify Access to set ENDINIT bit */
    SCU_WDTSCON0 = ((SCU_WDTSCON0 ^ 0xFC) | (1 << LCK)) | (1 << ENDINIT);
    while ((SCU_WDTSCON0 & (1 << LCK)) == 0)
        ;

    /* SRC Interrupt Setting For ECU */
    SRC_SCUERU0 &= ~((0xFF) << SRPN); // Set Priority : 0x0B
    SRC_SCUERU0 |= ((0x0B) << SRPN);

    SRC_SCUERU0 &= ~((0x3) << TOS); // CPU0 services

    SRC_SCUERU0 |= (1 << SRE); // Service Request is enabled
}

void init_CCU60(void)
{
    /* CCU60 T12 Setting */

    /* Password Access to unlock WDTSCON0 */
    SCU_WDT_CPU0CON0 = ((SCU_WDT_CPU0CON0 ^ 0xFC) & ~(1 << LCK)) | (1 << ENDINIT);
    while ((SCU_WDT_CPU0CON0 & (1 << LCK)) != 0)
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
    SCU_WDT_CPU0CON0 = ((SCU_WDT_CPU0CON0 ^ 0xFC) | (1 << LCK)) | (1 << ENDINIT);
    while ((SCU_WDT_CPU0CON0 & (1 << LCK)) == 0)
        ;

    while ((CCU60_CLC & (1 << DISS)) != 0)
        ; // Wait until module is enabled

    CCU60_TCTR0 &= ~((0x7) << T12CLK); // f_T12 = f_CCU6 / prescaler
    CCU60_TCTR0 |= (1 << T12CLK); // F_CCU6 = 100 MHz, prescaler = 2
    CCU60_TCTR0 &= ~(1 << T12PRE); // f_T12 = 50 MHz

    CCU60_TCTR0 &= ~(1 << CTM); // T12 always counts up and continues counting
        // from zero after reaching the period value

    CCU60_T12PR = 500 - 1; // Interrupt freq. = f_T12 / (period value + 1)
    CCU60_TCTR4 |= (1 << T12STR); // Interrupt freq. = 100 kHz (10 us)

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

    irq_ultra_sensor = 0;

    /* Initialization */
    init_ultrasonic();
    init_ERU();
    init_CCU60();
    init_LED();
    init_RGBLED();
    init_Buzzer();
    init_GTM_TOM0_PWM();

    PORT10_OMR = (1 << PCL2); // LED BLUE OFF
    PORT10_OMR = (1 << PCL1); // LED RED OFF

    while (1) {
        if (tagger == 0) {
            PORT02_OMR |= (1 << PCL7); // Clear RGB LED RED
            PORT10_OMR |= (1 << PS5); // Set RGB LED GREEN
            PORT10_OMR |= (1 << PCL3); // Clear RGB LED BLUE

            if (distance < TAG_THREHSHOLD && distance != 0) {
                PORT10_OMR = (1 << PS2); // LED BLUE ON
                PORT10_OMR = (1 << PCL1); // LED RED OFF
                PORT02_OMR |= (1 << PCL7); // Clear LED RED
                PORT10_OMR |= (1 << PCL5); // Clear LED GREEN
                PORT10_OMR |= (1 << PCL3); // Clear LED BLUE

                int pwm_cnt = PWM_FREQ / 261;
                GTM_TOM0_CH11_SR0 = pwm_cnt;
                GTM_TOM0_CH11_SR1 = pwm_cnt / 2;
                GTM_CMU_CLK_EN |= ((0x2) << EN_FXCLK); // enable
                for (int j = 0; j < 50000000; j++)
                    ;
                GTM_CMU_CLK_EN &= ~((0x2) << EN_FXCLK); // disable
                break;
            }
        } else {
            PORT02_OMR |= (1 << PS7); // Set LED RED
            PORT10_OMR |= (1 << PCL5); // Clear LED GREEN
            PORT10_OMR |= (1 << PCL3); // Clear LED BLUE

            if (distance != 0 && distance_cmp != 0) {
                distance_now = distance;
                if (distance_now - distance_cmp > 10 || distance_cmp - distance_now > 10) {
                    PORT10_OMR = (1 << PCL2); // LED BLUE OFF
                    PORT10_OMR = (1 << PS1); // LED RED ON
                    PORT02_OMR |= (1 << PCL7); // Clear LED RED
                    PORT10_OMR |= (1 << PCL5); // Clear LED GREEN
                    PORT10_OMR |= (1 << PCL3); // Clear LED BLUE

                    int pwm_cnt = PWM_FREQ / 392;
                    GTM_TOM0_CH11_SR0 = pwm_cnt;
                    GTM_TOM0_CH11_SR1 = pwm_cnt / 2;
                    GTM_CMU_CLK_EN |= ((0x2) << EN_FXCLK); // enable
                    for (int j = 0; j < 50000000; j++)
                        ;
                    GTM_CMU_CLK_EN &= ~((0x2) << EN_FXCLK); // disable
                    break;
                }
            }
        }
    }

    while (1)
        ;

    return (1);
}

__interrupt(0x0A) __vector_table(0) void CCU60_T12_ISR(void)
{
    timer_cnt_ultra++;
    if (timer_cnt_ultra == 1) {
        /* Set TRIG Pin */
        PORT15_OMR |= (1 << PS4);
    } else if (timer_cnt_ultra == 2) {
        /* Clear TRIG Pin */
        /* Generate 10us Pulse */
        PORT15_OMR |= (1 << PCL4);
    } else if (timer_cnt_ultra == 1000) {
        /* TRIG Period: 100ms : 10000*/
        timer_cnt_ultra = 0;
    }

    timer_cnt_mode++;
    if (timer_cnt_mode == 300000) {
        timer_cnt_mode = 0;
        if (tagger) {
            tagger--;
        } else {
            distance_cmp = distance;
            tagger++;
        }
    }
}

__interrupt(0x0B) __vector_table(0) void ERU0_ISR(void)
{
    if ((PORT15_IN & (1 << P5)) == 0) {
        // Falling edge

        /* Get distance */
        end_time = timer_cnt_ultra;

        // 10us
        interval_time = end_time - start_time; // clock per 0.02us

        // 340m/s -> 340_00cm/s -> 340_00
        // 1us    -> 1000000/340_00 = 29
        // ToF(Time of Flight) = distance * 2
        // distance = ToF us / 58 cm
        // distance = ToF 10us / 5.8 cm
        // distance = ToF 10us 17/100 cm

        //distance = ((interval_time/2)*34000)/1000_00;    // cm
        distance = (interval_time * 17) / 100; // cm
        irq_ultra_sensor = 1;
    } else {
        // Rising edge
        start_time = timer_cnt_ultra;
    }
}
