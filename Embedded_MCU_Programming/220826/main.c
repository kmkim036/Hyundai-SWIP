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

#define Ex0 // hello world
// #define Ex1   // system timer test
// #define Ex2   // signed & unsigned integer
// #define Ex3   // struct 1 / 3
// #define Ex4   // struct 2 / 3
// #define Ex5   // struct 3 / 3
// #define Ex6   // union 1 / 2
// #define Ex7   // union 2 / 2
// #define Ex8   // pointer 1 / 2
// #define Ex9   // pointer 2 / 2
// #define Ex10 // function pointer 1 / 2
// #define Ex11  // function pointer 2 / 2

#ifdef Ex0
// hello world
#include "IfxCpu.h"
#include "IfxScuWdt.h"
#include "Ifx_Types.h"

#include <stdio.h>

IfxCpu_syncEvent g_cpuSyncEvent = 0;

int core0_main(void)
{
    IfxCpu_enableInterrupts();

    // !!WATCHDOG0 AND SAFETY WATCHDOG ARE DISABLED HERE!!
    // Enable the watchdogs and service them periodically if it is required
    //
    IfxScuWdt_disableCpuWatchdog(IfxScuWdt_getCpuWatchdogPassword());
    IfxScuWdt_disableSafetyWatchdog(IfxScuWdt_getSafetyWatchdogPassword());

    // Wait for CPU sync event
    IfxCpu_emitEvent(&g_cpuSyncEvent);
    IfxCpu_waitEvent(&g_cpuSyncEvent, 1);

    printf("hello world kmkim\n");
    printf("\n");

    while (1) {
    }
    return (1);
}
#endif

#ifdef Ex1
// system timer test
#include "IfxCpu.h"
#include "IfxScuWdt.h"
#include "Ifx_Types.h"
#include <stdio.h>

#define SYSTEM_TIMER_31_0 *(unsigned int*)(0xF0000000 + 0x10)
#define SYSTEM_TIMER_PERIOD 10 // 100Mhz

unsigned int systemtick[2];
unsigned int tick_cnt;
unsigned int delay_time_ns;

IfxCpu_syncEvent g_cpuSyncEvent = 0;

int core0_main(void)
{
    unsigned int i;

    IfxCpu_enableInterrupts();

    // !!WATCHDOG0 AND SAFETY WATCHDOG ARE DISABLED HERE!!
    // Enable the watchdogs and service them periodically if it is required
    //
    IfxScuWdt_disableCpuWatchdog(IfxScuWdt_getCpuWatchdogPassword());
    IfxScuWdt_disableSafetyWatchdog(IfxScuWdt_getSafetyWatchdogPassword());

    // Wait for CPU sync event
    IfxCpu_emitEvent(&g_cpuSyncEvent);
    IfxCpu_waitEvent(&g_cpuSyncEvent, 1);

    // Delay Check
    systemtick[0] = SYSTEM_TIMER_31_0;
    for (int i = 0; i < 83; i++)
        systemtick[1] = SYSTEM_TIMER_31_0;

    // Calculate tick cnt
    tick_cnt = systemtick[1] - systemtick[0];
    printf("test\n");
    // Calculate delay time, cnt * 10ns
    delay_time_ns = tick_cnt * SYSTEM_TIMER_PERIOD;

    printf("Delay is %d \n", delay_time_ns);
    printf("\n");

    while (1) {
        i++;
        if (i == 100) {
            i = 0;
        }
    }

    return (1);
}
#endif

#ifdef Ex2
// signed & unsigned integer
#include "IfxCpu.h"
#include "IfxScuWdt.h"
#include "Ifx_Types.h"

#include <stdio.h>

IfxCpu_syncEvent g_cpuSyncEvent = 0;

int core0_main(void)
{
    IfxCpu_enableInterrupts();

    // !!WATCHDOG0 AND SAFETY WATCHDOG ARE DISABLED HERE!!
    // Enable the watchdogs and service them periodically if it is required
    //
    IfxScuWdt_disableCpuWatchdog(IfxScuWdt_getCpuWatchdogPassword());
    IfxScuWdt_disableSafetyWatchdog(IfxScuWdt_getSafetyWatchdogPassword());

    // Wait for CPU sync event
    IfxCpu_emitEvent(&g_cpuSyncEvent);
    IfxCpu_waitEvent(&g_cpuSyncEvent, 1);

    int sint;
    unsigned int uint;

    sint = 0x12345678;
    uint = 0x12345678;
    fprintf(stdout, "================== Test 1 ==================\n");
    fprintf(stdout, "Value of the signed integer :  %d\n", sint);
    fprintf(stdout, "Value of the unsigned integer :    %u\n", uint);

    sint = 0x87654321;
    uint = 0x87654321;
    fprintf(stdout, "\n================== Test 2 ==================\n");
    fprintf(stdout, "Value of the signed integer :  %d\n", sint);
    fprintf(stdout, "Value of the unsigned integer :    %u\n", uint);
    printf("\n");

    while (1) {
    }
    return (1);
}
#endif

#ifdef Ex3
// struct 1 / 3
#include "IfxCpu.h"
#include "IfxScuWdt.h"
#include "Ifx_Types.h"

#include <stdio.h>

IfxCpu_syncEvent g_cpuSyncEvent = 0;

//declaration
struct mydata {
    int a;
    int b;
};

//declaration & initialization
struct mydata2 {
    int a;
    char b;
    short c;
} object2;

//declaration with typedef
typedef struct {
    char c;
    int a;
    short b;
} mydata3;

int core0_main(void)
{
    IfxCpu_enableInterrupts();

    // !!WATCHDOG0 AND SAFETY WATCHDOG ARE DISABLED HERE!!
    // Enable the watchdogs and service them periodically if it is required
    //
    IfxScuWdt_disableCpuWatchdog(IfxScuWdt_getCpuWatchdogPassword());
    IfxScuWdt_disableSafetyWatchdog(IfxScuWdt_getSafetyWatchdogPassword());

    // Wait for CPU sync event
    IfxCpu_emitEvent(&g_cpuSyncEvent);
    IfxCpu_waitEvent(&g_cpuSyncEvent, 1);

    struct mydata object1;
    // object2;
    struct mydata2 object3;
    mydata3 object4;

    fprintf(stdout, "mydata struct size : %d\n", sizeof(object1));
    fprintf(stdout, "mydata2 struct size : %d\n", sizeof(object2));
    fprintf(stdout, "mydata2 struct size : %d\n", sizeof(object3));
    fprintf(stdout, "mydata3 struct size : %d\n", sizeof(object4));
    printf("\n");

    while (1) {
    }
    return (1);
}
#endif

#ifdef Ex4
// struct 2 / 3
#include "IfxCpu.h"
#include "IfxScuWdt.h"
#include "Ifx_Types.h"

#include <stdio.h>

IfxCpu_syncEvent g_cpuSyncEvent = 0;

struct mydata2 {
    int a;
    char b;
    short c;
} object2;

//declaration with typedef
typedef struct {
    char c;
    int a;
    short b;
} mydata3;

typedef struct {
    short b;
    double d;
} mydata4;

int core0_main(void)
{
    IfxCpu_enableInterrupts();

    // !!WATCHDOG0 AND SAFETY WATCHDOG ARE DISABLED HERE!!
    // Enable the watchdogs and service them periodically if it is required
    //
    IfxScuWdt_disableCpuWatchdog(IfxScuWdt_getCpuWatchdogPassword());
    IfxScuWdt_disableSafetyWatchdog(IfxScuWdt_getSafetyWatchdogPassword());

    // Wait for CPU sync event
    IfxCpu_emitEvent(&g_cpuSyncEvent);
    IfxCpu_waitEvent(&g_cpuSyncEvent, 1);

    mydata3 object4;
    mydata4 object5;

    fprintf(stdout, "mydata2 size : %d\n", sizeof(object2));
    fprintf(stdout, "mydata2 int size   : %d\n", sizeof(object2.a));
    fprintf(stdout, "mydata2 char size  : %d\n", sizeof(object2.b));
    fprintf(stdout, "mydata2 short size : %d\n", sizeof(object2.c));
    fprintf(stdout, "==========================\n");
    fprintf(stdout, "mydata3 size : %d\n", sizeof(object4));
    fprintf(stdout, "mydata3 char size  : %d\n", sizeof(object4.c));
    fprintf(stdout, "mydata3 int size   : %d\n", sizeof(object4.a));
    fprintf(stdout, "mydata3 short size : %d\n", sizeof(object4.b));
    fprintf(stdout, "==========================\n");
    fprintf(stdout, "mydata4 size : %d\n", sizeof(object5));
    fprintf(stdout, "mydata4 short size  : %d\n", sizeof(object5.b));
    fprintf(stdout, "mydata4 double size : %d\n", sizeof(object5.d));
    fprintf(stdout, "==========================\n");
    fprintf(stdout, "mydata2 address : %p\n", &object2);
    fprintf(stdout, "mydata2 int address   : %p\n", &(object2.a));
    fprintf(stdout, "mydata2 char address  : %p\n", &(object2.b));
    fprintf(stdout, "mydata2 short address : %p\n", &(object2.c));
    fprintf(stdout, "==========================\n");
    fprintf(stdout, "mydata3 address : %p\n", &object4);
    fprintf(stdout, "mydata3 char address  : %p\n", &(object4.c));
    fprintf(stdout, "mydata3 int address   : %p\n", &(object4.a));
    fprintf(stdout, "mydata3 short address : %p\n", &(object4.b));
    fprintf(stdout, "==========================\n");
    fprintf(stdout, "mydata4 address : %p\n", &object5);
    fprintf(stdout, "mydata4 short address  : %p\n", &(object5.b));
    fprintf(stdout, "mydata4 double address : %p\n", &(object5.d));
    printf("\n");

    while (1) {
    }
    return (1);
}
#endif

#ifdef Ex5
// struct 3 / 3
#include "IfxCpu.h"
#include "IfxScuWdt.h"
#include "Ifx_Types.h"

#include <stdio.h>

IfxCpu_syncEvent g_cpuSyncEvent = 0;

struct bit_field {
    unsigned int a : 1;
    unsigned int b : 3;
    unsigned int c : 7;
} flag;

int core0_main(void)
{
    IfxCpu_enableInterrupts();

    // !!WATCHDOG0 AND SAFETY WATCHDOG ARE DISABLED HERE!!
    // Enable the watchdogs and service them periodically if it is required
    //
    IfxScuWdt_disableCpuWatchdog(IfxScuWdt_getCpuWatchdogPassword());
    IfxScuWdt_disableSafetyWatchdog(IfxScuWdt_getSafetyWatchdogPassword());

    // Wait for CPU sync event
    IfxCpu_emitEvent(&g_cpuSyncEvent);
    IfxCpu_waitEvent(&g_cpuSyncEvent, 1);

    flag.a = 1; // 1bit (1)
    flag.b = 15; // 4bit (1111)
    flag.c = 255; // 8bit (1111 1111)

    fprintf(stdout, "flag.a : %u\n", flag.a);
    fprintf(stdout, "flag.b : %u\n", flag.b); // only 3bit
    fprintf(stdout, "flag.c : %u\n", flag.c); // only 7bit
    printf("\n");

    while (1) {
    }
    return (1);
}
#endif

#ifdef Ex6
// union 1 / 2
#include "IfxCpu.h"
#include "IfxScuWdt.h"
#include "Ifx_Types.h"

#include <stdio.h>

IfxCpu_syncEvent g_cpuSyncEvent = 0;

union Box {
    int a;
    short b;
    char c;
};

int core0_main(void)
{
    IfxCpu_enableInterrupts();

    // !!WATCHDOG0 AND SAFETY WATCHDOG ARE DISABLED HERE!!
    // Enable the watchdogs and service them periodically if it is required
    //
    IfxScuWdt_disableCpuWatchdog(IfxScuWdt_getCpuWatchdogPassword());
    IfxScuWdt_disableSafetyWatchdog(IfxScuWdt_getSafetyWatchdogPassword());

    // Wait for CPU sync event
    IfxCpu_emitEvent(&g_cpuSyncEvent);
    IfxCpu_waitEvent(&g_cpuSyncEvent, 1);

    union Box b1;

    fprintf(stdout, "Box union size : %d\n", sizeof(b1));

    b1.a = 0x12345678;
    fprintf(stdout, "Box.a data: 0x%x\n", b1.a);
    fprintf(stdout, "Box.b data: 0x%x\n", b1.b);
    fprintf(stdout, "Box.c data: 0x%x\n", b1.c);
    fprintf(stdout, "====================\n");
    fprintf(stdout, "Box.a address: %p\n", &(b1.a));
    fprintf(stdout, "Box.b address: %p\n", &(b1.b));
    fprintf(stdout, "Box.c address: %p\n", &(b1.c));
    printf("\n");

    while (1) {
    }
    return (1);
}
#endif

#ifdef Ex7
// union 2 / 2
#include "IfxCpu.h"
#include "IfxScuWdt.h"
#include "Ifx_Types.h"

#include <stdio.h>

IfxCpu_syncEvent g_cpuSyncEvent = 0;

union IP_addr {
    unsigned int ipv4;
    struct {
        unsigned int addr4 : 8;
        unsigned int addr3 : 8;
        unsigned int addr2 : 8;
        unsigned int addr1 : 8;
    };
};

int core0_main(void)
{
    IfxCpu_enableInterrupts();

    // !!WATCHDOG0 AND SAFETY WATCHDOG ARE DISABLED HERE!!
    // Enable the watchdogs and service them periodically if it is required
    //
    IfxScuWdt_disableCpuWatchdog(IfxScuWdt_getCpuWatchdogPassword());
    IfxScuWdt_disableSafetyWatchdog(IfxScuWdt_getSafetyWatchdogPassword());

    // Wait for CPU sync event
    IfxCpu_emitEvent(&g_cpuSyncEvent);
    IfxCpu_waitEvent(&g_cpuSyncEvent, 1);

    union IP_addr host;
    host.addr4 = 127;
    host.addr3 = 0;
    host.addr2 = 0;
    host.addr1 = 1;

    fprintf(stdout, "host ipv4 : 0x%x\n", host.ipv4); // little-endian
    fprintf(stdout, "host ipv4 : %u.%u.%u.%u\n", host.addr4, host.addr3, host.addr2, host.addr1);
    printf("\n");

    while (1) {
    }
    return (1);
}
#endif

#ifdef Ex8
// pointer 1 / 2
#include "IfxCpu.h"
#include "IfxScuWdt.h"
#include "Ifx_Types.h"

#include <stdio.h>

IfxCpu_syncEvent g_cpuSyncEvent = 0;

int core0_main(void)
{
    IfxCpu_enableInterrupts();

    // !!WATCHDOG0 AND SAFETY WATCHDOG ARE DISABLED HERE!!
    // Enable the watchdogs and service them periodically if it is required
    //
    IfxScuWdt_disableCpuWatchdog(IfxScuWdt_getCpuWatchdogPassword());
    IfxScuWdt_disableSafetyWatchdog(IfxScuWdt_getSafetyWatchdogPassword());

    // Wait for CPU sync event
    IfxCpu_emitEvent(&g_cpuSyncEvent);
    IfxCpu_waitEvent(&g_cpuSyncEvent, 1);

    fprintf(stdout, "===== Size of data type =====\n");
    fprintf(stdout, "Size of char : %d\n", sizeof(char));
    fprintf(stdout, "Size of short : %d\n", sizeof(short));
    fprintf(stdout, "Size of int : %d\n", sizeof(int));
    fprintf(stdout, "Size of double : %d\n", sizeof(double));
    fprintf(stdout, "Size of float : %d\n", sizeof(float));
    fprintf(stdout, "Size of long : %d\n", sizeof(long));
    fprintf(stdout, "Size of long long : %d\n", sizeof(long long));

    fprintf(stdout, "\n===== Size of pointer data type ======\n");
    fprintf(stdout, "Size of char * : %d\n", sizeof(char*));
    fprintf(stdout, "Size of short * : %d\n", sizeof(short*));
    fprintf(stdout, "Size of int * : %d\n", sizeof(int*));
    fprintf(stdout, "Size of double * : %d\n", sizeof(double*));
    fprintf(stdout, "Size of float * : %d\n", sizeof(float*));
    fprintf(stdout, "Size of long * : %d\n", sizeof(long*));
    fprintf(stdout, "Size of long long * : %d\n", sizeof(long long*));
    printf("\n");

    while (1) {
    }
    return (1);
}
#endif

#ifdef Ex9
// pointer 2 / 2
#include "IfxCpu.h"
#include "IfxScuWdt.h"
#include "Ifx_Types.h"

#include <stdio.h>

IfxCpu_syncEvent g_cpuSyncEvent = 0;

int core0_main(void)
{
    IfxCpu_enableInterrupts();

    // !!WATCHDOG0 AND SAFETY WATCHDOG ARE DISABLED HERE!!
    // Enable the watchdogs and service them periodically if it is required
    //
    IfxScuWdt_disableCpuWatchdog(IfxScuWdt_getCpuWatchdogPassword());
    IfxScuWdt_disableSafetyWatchdog(IfxScuWdt_getSafetyWatchdogPassword());

    // Wait for CPU sync event
    IfxCpu_emitEvent(&g_cpuSyncEvent);
    IfxCpu_waitEvent(&g_cpuSyncEvent, 1);

    int a = 0x12345678;
    int* a_ptr = &a;

    fprintf(stdout, "========== Test 1 ==========\n");
    fprintf(stdout, "Address of a :                %p\n", &a);
    fprintf(stdout, "Address of *a_ptr :           %p\n", &a_ptr);
    fprintf(stdout, "Stored address in *a_ptr :    %p\n", a_ptr);
    fprintf(stdout, "Data pointed by a_ptr :       0x%x\n", *a_ptr);

    fprintf(stdout, "\n========== Test 2 ==========\n");
    fprintf(stdout, "Data pointed by int ptr :      0x%x\n", *a_ptr);
    fprintf(stdout, "Data pointed by short ptr :    0x%x\n", *(short*)a_ptr);
    fprintf(stdout, "Data pointed by char ptr :     0x%x\n", *(char*)a_ptr);

    char* ch_ptr = (char*)a_ptr;

    fprintf(stdout, "\n========== Test 3 ==========\n");
    fprintf(stdout, "data(0) : 0x%x\n", *(ch_ptr++));
    fprintf(stdout, "data(1) : 0x%x\n", *(ch_ptr++));
    fprintf(stdout, "data(2) : 0x%x\n", *(ch_ptr++));
    fprintf(stdout, "data(3) : 0x%x\n", *(ch_ptr));
    printf("\n");

    while (1) {
    }
    return (1);
}
#endif

#ifdef Ex10
// function pointer 1 / 2
#include "IfxCpu.h"
#include "IfxScuWdt.h"
#include "Ifx_Types.h"

#include <stdio.h>

IfxCpu_syncEvent g_cpuSyncEvent = 0;

int add(int num1, int num2);
int sub(int num1, int num2);
int mul(int num1, int num2);

int core0_main(void)
{
    IfxCpu_enableInterrupts();

    // !!WATCHDOG0 AND SAFETY WATCHDOG ARE DISABLED HERE!!
    // Enable the watchdogs and service them periodically if it is required
    //
    IfxScuWdt_disableCpuWatchdog(IfxScuWdt_getCpuWatchdogPassword());
    IfxScuWdt_disableSafetyWatchdog(IfxScuWdt_getSafetyWatchdogPassword());

    // Wait for CPU sync event
    IfxCpu_emitEvent(&g_cpuSyncEvent);
    IfxCpu_waitEvent(&g_cpuSyncEvent, 1);

    int (*func)(int, int);

    func = add;
    fprintf(stdout, "add(3, 5) : %d\n", func(3, 5));

    func = sub;
    fprintf(stdout, "sub(3, 5) : %d\n", func(3, 5));

    func = mul;
    fprintf(stdout, "mul(3, 5) : %d\n", func(3, 5));
    printf("\n");

    while (1) {
    }
    return (1);
}

int add(int num1, int num2)
{
    return num1 + num2;
}

int sub(int num1, int num2)
{
    return num1 - num2;
}

int mul(int num1, int num2)
{
    return num1 * num2;
}
#endif

#ifdef Ex11
// function pointer 2 / 2
#include "IfxCpu.h"
#include "IfxScuWdt.h"
#include "Ifx_Types.h"

#include <stdio.h>

IfxCpu_syncEvent g_cpuSyncEvent = 0;

int add(int num1, int num2);
int sub(int num1, int num2);
int mul(int num1, int num2);

int core0_main(void)
{
    IfxCpu_enableInterrupts();

    // !!WATCHDOG0 AND SAFETY WATCHDOG ARE DISABLED HERE!!
    // Enable the watchdogs and service them periodically if it is required
    //
    IfxScuWdt_disableCpuWatchdog(IfxScuWdt_getCpuWatchdogPassword());
    IfxScuWdt_disableSafetyWatchdog(IfxScuWdt_getSafetyWatchdogPassword());

    // Wait for CPU sync event
    IfxCpu_emitEvent(&g_cpuSyncEvent);
    IfxCpu_waitEvent(&g_cpuSyncEvent, 1);

    int (*func_ptr[3])(int, int) = { add, sub, mul };

    for (int i = 0; i < 3; i++) {
        fprintf(stdout, "Result of func_ptr[%d] : %d\n", i, func_ptr[i](3, 5));
    }
    printf("\n");

    while (1) {
    }
    return (1);
}

int add(int num1, int num2)
{
    return num1 + num2;
}

int sub(int num1, int num2)
{
    return num1 - num2;
}

int mul(int num1, int num2)
{
    return num1 * num2;
}
#endif
