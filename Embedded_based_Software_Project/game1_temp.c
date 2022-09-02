#include "IfxCpu.h"
#include "IfxScuWdt.h"
#include "Ifx_Types.h"
#include <stdio.h>

#include <peh.h>
/* Address of Registers */

// SCU Registers
#define SCU_BASE (0xF0036000)
#define SCU_WDT_CPU0CON0 (*(volatile unsigned int*)(SCU_BASE + 0x100))

#define LCK 1
#define ENDINIT 0

// GTM Registers
// GTM - CMU
#define GTM_BASE (0xF0100000)
#define GTM_CMU_CLK_EN (*(volatile unsigned int*)(GTM_BASE + 0x00300))
#define GTM_CMU_FXCLK_CTRL (*(volatile unsigned int*)(GTM_BASE + 0x00344))

#define EN_FXCLK 22
#define FXCLK_SEL 0

// GTM - TOM0
#define GTM_TOM0_TGC0_GLB_CTRL (*(volatile unsigned int*)(GTM_BASE + 0x08030))
#define GTM_TOM0_TGC0_ENDIS_CTRL (*(volatile unsigned int*)(GTM_BASE + 0x08070))
#define GTM_TOM0_TGC0_OUTEN_CTRL (*(volatile unsigned int*)(GTM_BASE + 0x08078))
#define GTM_TOM0_TGC0_FUPD_CTRL (*(volatile unsigned int*)(GTM_BASE + 0x08038))

#define GTM_TOM0_CH1_CTRL (*(volatile unsigned int*)(GTM_BASE + 0x08040))
#define GTM_TOM0_CH1_SR0 (*(volatile unsigned int*)(GTM_BASE + 0x08044))
#define GTM_TOM0_CH1_SR1 (*(volatile unsigned int*)(GTM_BASE + 0x08048))

#define GTM_TOM0_CH2_CTRL (*(volatile unsigned int*)(GTM_BASE + 0x08080))
#define GTM_TOM0_CH2_SR0 (*(volatile unsigned int*)(GTM_BASE + 0x08084))
#define GTM_TOM0_CH2_SR1 (*(volatile unsigned int*)(GTM_BASE + 0x08088))

#define HOST_TRIG 0

#define UPEN_CTRL1 18
#define UPEN_CTRL2 20

#define ENDIS_CTRL1 2
#define ENDIS_CTRL2 4
#define OUTEN_CTRL1 2
#define OUTEN_CTRL2 4
#define RSTCN0_CH1 18
#define RSTCN0_CH2 20
#define FUPD_CTRL1 2
#define FUPD_CTRL2 4

#define CLK_SRC_SR 12
#define SL 11

// GTM
#define GTM_CLC (*(volatile unsigned int*)(GTM_BASE + 0x9FD00))
#define GTM_TOUTSEL6 (*(volatile unsigned int*)(GTM_BASE + 0x9FD48))

#define DISS 1
#define DISR 0
#define SEL7 14
#define SEL8 16

// PORT10 Registers
#define PORT10_BASE (0xF003B000)
#define PORT10_IOCR0 (*(volatile unsigned int*)(PORT10_BASE + 0x10))

#define PC1 11
#define PC2 19

/* Function Prototype */
void init_LED(void);

IfxCpu_syncEvent g_cpuSyncEvent = 0;

volatile unsigned int systick;
volatile unsigned int systick_curr;
volatile unsigned int systick_prev;
volatile unsigned int potential_meter;
volatile unsigned int light_sensor;
volatile unsigned int potential_meter_duty;
volatile unsigned int light_sensor_duty;
unsigned volatile int adcResult;
unsigned volatile int timer_bool = 0;

void init_RGBLED(void);
void init_VADC(void);
void VADC_startConversion(void);
unsigned int VADC_readResult(void);

unsigned int GetVADC4(int channel);

volatile int RED_ON_FLAG;
volatile int BLUE_ON_FLAG;
volatile int timer;
volatile int win = 0;
volatile int SW1 = 0;
volatile int SW2 = 0;
int led_mode;

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

    CCU60_T12PR = 24414 - 1; // Interrupt freq. = f_T12 / (period value  + 1)
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

void init_Switch(void)
{
    /* Reset PC0 & PC1 in IOCR0*/
    PORT02_IOCR0 &= ~((0x1F) << PC1);
    PORT02_IOCR0 &= ~((0x1F) << PC0);

    /* Set PC0 & PC1 with pull-up(2b0xx10) */
    PORT02_IOCR0 |= ((0x2) << PC1);
    PORT02_IOCR0 |= ((0x2) << PC0);
}

/* Initialize LED (RED & BLUE) */
void init_LED(void)
{
    /* Reset PC1 & PC2 in IOCR0*/
    PORT10_IOCR0 &= ~((0x1F) << PC1);
    PORT10_IOCR0 &= ~((0x1F) << PC2);

    /* Set PC1 & PC2 with push-pull(2b10000) */
    PORT10_IOCR0 |= ((0x10) << PC1);
    PORT10_IOCR0 |= ((0x10) << PC2);
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

    /* Initialization */
    init_RGBLED(); // Initialize PORT
    init_VADC(); // Initialize VADC
    init_LED();
    init_Switch();
    init_CCU60();

    // game 1
    PORT02_OMR |= (1 << PCL7); // Clear LED RED
    PORT10_OMR |= (1 << PCL5); // Clear LED GREEN
    PORT10_OMR |= (1 << PCL3); // Clear LED BLUE

    win = 0;
    while (1) {
        adcResult = GetVADC4(6);
        led_mode = adcResult % 3;
        timer_bool = 1;

        volatile int cycle;

        if (led_mode == 0) {
            PORT10_OMR = (1 << PS1); // LED RED ON
            PORT10_OMR = (1 << PS2); // LED BLUE ON
        } else if (led_mode == 1) {
            PORT10_OMR = (1 << PS1); // LED RED ON
            PORT10_OMR = (1 << PCL2); // LED BLUE OFF
        } else {
            PORT10_OMR = (1 << PCL1); // LED RED OFF
            PORT10_OMR = (1 << PS2); // LED BLUE ON
        }

        SW1 = 0;
        SW2 = 0;

        while (timer < 4) {
            if ((PORT02_IN & (1 << P1)) == 0) // Switch2 is pushed
                SW2 = 1;
            if ((PORT02_IN & (1 << P0)) == 0) // Switch1 is pushed
                SW1 = 1;

            if (SW1 && SW2) {
                if (led_mode == 0) {
                    win++;
                    break;
                } else
                    break;
            } else if (SW1) {
                if (led_mode == 2) {
                    win++;
                    break;
                } else
                    break;
            } else if (SW2) {
                if (led_mode == 1) {
                    win++;
                    break;
                }
            } else
                break;
        }
 
        PORT10_OMR = (1 << PCL2); // LED BLUE OFF
        PORT10_OMR = (1 << PCL1); // LED RED OFF

        if (win == 1) {
            PORT02_OMR |= (1 << PS7); // Set LED RED
            PORT10_OMR |= (1 << PCL5); // Clear LED GREEN
            PORT10_OMR |= (1 << PCL3); // Clear LED BLUE
        } else if (win == 2) {
            PORT02_OMR |= (1 << PCL7); // Clear LED RED
            PORT10_OMR |= (1 << PS5); // Set LED GREEN
            PORT10_OMR |= (1 << PCL3); // Clear LED BLUE
        } else if (win == 3) {
            PORT02_OMR |= (1 << PCL7); // Clear LED RED
            PORT10_OMR |= (1 << PCL5); // Clear LED GREEN
            PORT10_OMR |= (1 << PS3); // Set LED BLUE
        } else if (win == 4) {
            PORT02_OMR |= (1 << PS7); // Set LED RED
            PORT10_OMR |= (1 << PS5); // Set LED GREEN
            PORT10_OMR |= (1 << PS3); // Set LED BLUE
        }
    }

    return (1);
}

__interrupt(0x0A) __vector_table(0) void CCU60_T12_ISR(void)
{
    if (timer_bool == 1)
        timer++;
    // printf("get_count_fucn\n");

    //PORT10_OMR |= ((1<<PCL1) | (1<<PS1));           // Toggle LED RED
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
