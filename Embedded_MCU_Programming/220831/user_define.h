/*
 * user_define.h
 *
 *  Created on: 2022. 8. 31.
 *      Author: user
 */

#ifndef USER_DEFINE_H_
#define USER_DEFINE_H_

#endif /* USER_DEFINE_H_ */

#include "IfxCpu.h"
#include "IfxScuWdt.h"
#include "Ifx_Types.h"
#include <stdio.h>

/* SCU Registers */
#define SCU_BASE (0xF0036000)
#define SCU_WDT_CPU0CON0 (*(volatile unsigned int*)(SCU_BASE + 0x100))

#define LCK 1
#define ENDINIT 0

/* VADC Registers */
#define VADC_BASE (0xF0020000)
#define VADC_CLC (*(volatile unsigned int*)(VADC_BASE + 0x000))
#define VADC_GLOBCFG (*(volatile unsigned int*)(VADC_BASE + 0x080))
#define VADC_G4ARBCFG (*(volatile unsigned int*)(VADC_BASE + 0x1480))
#define VADC_G4ARBPR (*(volatile unsigned int*)(VADC_BASE + 0x1484))
#define VADC_G4ICLASS0 (*(volatile unsigned int*)(VADC_BASE + 0x14A0))
#define VADC_G4QMR0 (*(volatile unsigned int*)(VADC_BASE + 0x1504))
#define VADC_G4QINR0 (*(volatile unsigned int*)(VADC_BASE + 0x1510))
#define VADC_G4CHCTR6 (*(volatile unsigned int*)(VADC_BASE + 0x1618))
#define VADC_G4CHCTR7 (*(volatile unsigned int*)(VADC_BASE + 0x161C))
#define VADC_G4RES1 (*(volatile unsigned int*)(VADC_BASE + 0x1704))

#define DISS 1
#define DISR 0
#define ANONC 0
#define ASEN0 24
#define CSM0 3
#define PRIO0 0
#define CMS 8
#define STCS 0
#define FLUSH 10
#define TREV 9
#define ENGT 0
#define RF 5
#define REQCHNR 0
#define RESPOS 21
#define RESREG 16
#define ICLSEL 0
#define VF 31
#define RESULT 0

/* Define PORT02/10 Registers for RGB LED */
#define PORT02_BASE (0xF003A200)
#define PORT02_IOCR4 (*(volatile unsigned int*)(PORT02_BASE + 0x14))
#define PORT02_OMR (*(volatile unsigned int*)(PORT02_BASE + 0x04))

#define PC7 27
#define PCL7 23
#define PS7 7

#define PORT10_BASE (0xF003B000)
#define PORT10_IOCR4 (*(volatile unsigned int*)(PORT10_BASE + 0x14))
#define PORT10_IOCR0 (*(volatile unsigned int*)(PORT10_BASE + 0x10))
#define PORT10_OMR (*(volatile unsigned int*)(PORT10_BASE + 0x04))

#define PC5 11
#define PC3 27
#define PC2 19
#define PC1 11
#define PCL5 21
#define PCL3 19
#define PS5 5
#define PS3 3

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
