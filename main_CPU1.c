/*
 *  ======== main.c ========
 */

#include <xdc/std.h>

#include <xdc/runtime/Error.h>
#include <xdc/runtime/System.h>

#include <ti/sysbios/BIOS.h>

#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Swi.h>
#include <ti/sysbios/hal/Hwi.h>

#include "device.h"
#include "driverlib.h"
#include "LEDCounter.h"

#include <f2838x_examples.h>
#include <f2838x_device.h>

//
// Defines
//
#define WAITSTEP     asm(" RPT #255 || NOP")

//
// Globals
//
uint32_t TaskACount = 0;
uint32_t TaskBCount = 0;
uint32_t TimerACount = 0;
uint32_t Swi0Count = 0;
uint16_t pass = 0;
uint16_t fail = 0;
extern ti_sysbios_knl_Swi_Handle swi0;
uint32_t EPwm1IsrCount = 0;

// Refer to C:\ti\c2000\C2000Ware_5_00_00_00\device_support\f2838x\examples\cpu1\cla\cla_ex1_asin.c
void CLA_initCpu1Cla1(void);

// Refer to C:\ti\c2000\C2000Ware_5_00_00_00\device_support\f2838x\examples\cpu1\cla\cla_ex3_background_task.c
void CLA_configClaMemory(void);

void init_EPwm1(void);

//
//Task 1 (C) Variables
// NOTE: Do not initialize the Message RAM variables globally, they will be
// reset during the message ram initialization phase in the CLA memory
// configuration routine
//
#pragma DATA_SECTION(iCLA1Task1Count,"Cla1ToCpuMsgRAM");
int32_t iCLA1Task1Count;
#pragma DATA_SECTION(iLedD1Signal,"Cla1ToCpuMsgRAM");
int32_t iLedD1Signal;

//
//Task 2 (C) Variables
//
int32_t iLedD2Signal = 0;

//
//Task 3 (C) Variables
//

//
//Task 4 (C) Variables
//

//
//Task 5 (C) Variables
//

//
//Task 6 (C) Variables
//

//
//Task 7 (C) Variables
//

//
//Task 8 (C) Variables
//
#pragma DATA_SECTION(iCLA1Task8Count,"Cla1ToCpuMsgRAM");
int32_t iCLA1Task8Count;


/*
 *  ======== Swi Fxn ========
 */
void Swi0_ISR(void)
{
    Swi0Count++;
    if ((Swi0Count % 500) == 0){
        if(iLedD2Signal == 0)   iLedD2Signal = 1;
        else                    iLedD2Signal = 0;
    }
}


/*
 *  ======== hwiFxn ========
 */
void EPwm1_ISR(UArg arg0)
{
    EPwm1IsrCount++;
    CLA_forceTasks(CLA1_BASE,CLA_TASKFLAG_1);

    //
    // Clear INT flag for this timer
    //
    EPwm1Regs.ETCLR.bit.INT = 1;    // Event Trigger Clear Register
}

/*
 *  ======== taskFxn ========
 */
void TaskA(UArg a0, UArg a1)
{
    while(1)
    {
        TaskACount++;
        GPIO_writePin(DEVICE_GPIO_PIN_LED1,iLedD1Signal);
        Task_sleep(1);
    }
}

void TaskB(UArg a0, UArg a1)
{
    while(1)
    {
        TaskBCount++;
        Swi_post(swi0);
        GPIO_writePin(DEVICE_GPIO_PIN_LED2, iLedD2Signal);
        Task_sleep(1);
    }
}

void TimerA(void)
{
    TimerACount++;
}


/*
 *  ======== main ========
 */
int main()
{ 
    //
    // Initialize device clock and peripherals
    //
    Device_init();

    //
    // Initialize GPIO and configure the GPIO pin as a push-pull output
    //
    Device_initGPIO();
    GPIO_setPadConfig(DEVICE_GPIO_PIN_LED1, GPIO_PIN_TYPE_STD);
    GPIO_setDirectionMode(DEVICE_GPIO_PIN_LED1, GPIO_DIR_MODE_OUT);
    GPIO_setPadConfig(DEVICE_GPIO_PIN_LED2, GPIO_PIN_TYPE_STD);
    GPIO_setDirectionMode(DEVICE_GPIO_PIN_LED2, GPIO_DIR_MODE_OUT);


    //
    // Initialize EPwm1
    //
    init_EPwm1();

    //
    // Initialize cla memory
    //
    CLA_configClaMemory();
    CLA_initCpu1Cla1();

    CLA_forceTasks(CLA1_BASE, CLA_TASKFLAG_8);

    BIOS_start();    /* does not return */
    return(0);
}

//
// CLA_configClaMemory - Configure CLA memory sections
//
void CLA_configClaMemory(void)
{
    extern uint32_t Cla1funcsRunStart, Cla1funcsLoadStart, Cla1funcsLoadSize;
    extern uint32_t Cla1ConstRunStart, Cla1ConstLoadStart, Cla1ConstLoadSize;
    EALLOW;

#ifdef _FLASH
    //
    // Copy the program and constants from FLASH to RAM before configuring
    // the CLA
    //
    memcpy((uint32_t *)&Cla1funcsRunStart, (uint32_t *)&Cla1funcsLoadStart,
           (uint32_t)&Cla1funcsLoadSize);
    memcpy((uint32_t *)&Cla1ConstRunStart, (uint32_t *)&Cla1ConstLoadStart,
        (uint32_t)&Cla1ConstLoadSize );
#endif //_FLASH

    EDIS;
}

//
// CLA_initCpu1Cla1 - Initialize CLA1 task vectors and end-of-task interrupts
//
void CLA_initCpu1Cla1(void)
{

    EALLOW;

    //
    // Compute all CLA task vectors
    // On Type-1 CLAs the MVECT registers accept full 16-bit task addresses as
    // opposed to offsets used on older Type-0 CLAs
    //
#pragma diag_suppress=770
    CLA_mapTaskVector(CLA1_BASE,CLA_MVECT_1,(uint16_t)&Cla1Task1);
    CLA_mapTaskVector(CLA1_BASE,CLA_MVECT_8,(uint16_t)&Cla1Task8);
#pragma diag_warning=770

    //
    // Enable the IACK instruction to start a task on CLA in software
    // for all  8 CLA tasks. Also, globally enable all 8 tasks (or a
    // subset of tasks) by writing to their respective bits in the
    // MIER register
    //
    CLA_enableIACK(CLA1_BASE);
    CLA_enableTasks(CLA1_BASE, CLA_TASKFLAG_ALL);


    //
    // Initialize and wait for CLA1ToCPUMsgRAM
    //
    MemCfg_initSections(MEMCFG_SECT_MSGCLA1TOCPU);
    while(!MemCfg_getInitStatus(MEMCFG_SECT_MSGCLA1TOCPU)){};

    //
    // Initialize and wait for CPUToCLA1MsgRAM
    //
    MemCfg_initSections(MEMCFG_SECT_MSGCPUTOCLA1);
    while(!MemCfg_getInitStatus(MEMCFG_SECT_MSGCPUTOCLA1)){};

    //
    // Configure LSRAMs
    //
    MemCfg_setLSRAMControllerSel(MEMCFG_SECT_LS0, MEMCFG_LSRAMCONTROLLER_CPU_ONLY);
    MemCfg_setLSRAMControllerSel(MEMCFG_SECT_LS1, MEMCFG_LSRAMCONTROLLER_CPU_ONLY);
    MemCfg_setLSRAMControllerSel(MEMCFG_SECT_LS2, MEMCFG_LSRAMCONTROLLER_CPU_ONLY);
    MemCfg_setLSRAMControllerSel(MEMCFG_SECT_LS3, MEMCFG_LSRAMCONTROLLER_CPU_ONLY);
    MemCfg_setLSRAMControllerSel(MEMCFG_SECT_LS4, MEMCFG_LSRAMCONTROLLER_CPU_ONLY);

    //
    // Select LS5RAM to be the programming space for the CLA
    // First configure the CLA to be the master for LS5 and then
    // set the space to be a program block
    //
    MemCfg_setLSRAMMasterSel(MEMCFG_SECT_LS5, MEMCFG_LSRAMMASTER_CPU_CLA1);
    MemCfg_setCLAMemType(MEMCFG_SECT_LS5, MEMCFG_CLA_MEM_PROGRAM);

    //
    // Next configure LS6RAM and LS7RAM as data spaces for the CLA
    // First configure the CLA to be the master for LS0(1) and then
    // set the spaces to be code blocks
    //
    MemCfg_setLSRAMMasterSel(MEMCFG_SECT_LS6, MEMCFG_LSRAMMASTER_CPU_CLA1);
    MemCfg_setCLAMemType(MEMCFG_SECT_LS6, MEMCFG_CLA_MEM_DATA);

    MemCfg_setLSRAMMasterSel(MEMCFG_SECT_LS7, MEMCFG_LSRAMMASTER_CPU_CLA1);
    MemCfg_setCLAMemType(MEMCFG_SECT_LS7, MEMCFG_CLA_MEM_DATA);

    //
    // Configure GSRAMs
    //
    MemCfg_setGSRAMControllerSel(MEMCFG_SECT_GS0, MEMCFG_GSRAMCONTROLLER_CPU1);
    MemCfg_setGSRAMControllerSel(MEMCFG_SECT_GS1, MEMCFG_GSRAMCONTROLLER_CPU1);
    MemCfg_setGSRAMControllerSel(MEMCFG_SECT_GS2, MEMCFG_GSRAMCONTROLLER_CPU1);
    MemCfg_setGSRAMControllerSel(MEMCFG_SECT_GS3, MEMCFG_GSRAMCONTROLLER_CPU1);
    MemCfg_setGSRAMControllerSel(MEMCFG_SECT_GS4, MEMCFG_GSRAMCONTROLLER_CPU1);
    MemCfg_setGSRAMControllerSel(MEMCFG_SECT_GS5, MEMCFG_GSRAMCONTROLLER_CPU1);
    MemCfg_setGSRAMControllerSel(MEMCFG_SECT_GS6, MEMCFG_GSRAMCONTROLLER_CPU1);
    MemCfg_setGSRAMControllerSel(MEMCFG_SECT_GS7, MEMCFG_GSRAMCONTROLLER_CPU1);
    MemCfg_setGSRAMControllerSel(MEMCFG_SECT_GS8, MEMCFG_GSRAMCONTROLLER_CPU1);
    MemCfg_setGSRAMControllerSel(MEMCFG_SECT_GS9, MEMCFG_GSRAMCONTROLLER_CPU1);
    MemCfg_setGSRAMControllerSel(MEMCFG_SECT_GS10, MEMCFG_GSRAMCONTROLLER_CPU1);
    MemCfg_setGSRAMControllerSel(MEMCFG_SECT_GS11, MEMCFG_GSRAMCONTROLLER_CPU1);
    MemCfg_setGSRAMControllerSel(MEMCFG_SECT_GS12, MEMCFG_GSRAMCONTROLLER_CPU1);
    MemCfg_setGSRAMControllerSel(MEMCFG_SECT_GS13, MEMCFG_GSRAMCONTROLLER_CPU1);
    MemCfg_setGSRAMControllerSel(MEMCFG_SECT_GS14, MEMCFG_GSRAMCONTROLLER_CPU1);
    MemCfg_setGSRAMControllerSel(MEMCFG_SECT_GS15, MEMCFG_GSRAMCONTROLLER_CPU1);

    EDIS;
}

/*
 *  ======== initFxn ========
 */
#define TBCLK 200000000
#define PWMFREQ 32000
uint32_t EPWM1_TIMER_TBPRD = 0;

void init_EPwm1(void)
{
    EALLOW;     // It is needed to write to EALLOW protected registers.

    //
    // Setup TBCLK
    //
    EPWM1_TIMER_TBPRD = TBCLK/PWMFREQ/2;        // 3125 = 200MHz/32kHz/2
    EPwm1Regs.TBPRD = EPWM1_TIMER_TBPRD;       // Set timer period 3125 TBCLKs
    EPwm1Regs.TBPHS.bit.TBPHS = 0x0000;        // Phase is 0
    EPwm1Regs.TBCTR = 0x0000;                  // Clear counter

    //
    // Setup counter mode
    //
    EPwm1Regs.TBCTL.bit.CTRMODE = TB_COUNT_UPDOWN; // Count up and down
    EPwm1Regs.TBCTL.bit.PHSEN = TB_DISABLE;        // Disable phase loading

    //
    // Configure EPWM to run at SYSCLK
    //
    ClkCfgRegs.PERCLKDIVSEL.bit.EPWMCLKDIV = SYSCTL_EPWMCLK_DIV_1;    // 200MHz
    EPwm1Regs.TBCTL.bit.HSPCLKDIV = TB_DIV1;       // Clock ratio to SYSCLKOUT
    EPwm1Regs.TBCTL.bit.CLKDIV = TB_DIV1;

    //
    // Interrupt where we will change the Compare Values
    //
    EPwm1Regs.ETSEL.bit.INTSEL = ET_CTR_ZERO;     // Select INT on Zero event
    EPwm1Regs.ETSEL.bit.INTEN = 1;                // Enable INT
    EPwm1Regs.ETPS.bit.INTPRD = ET_1ST;           // Generate INT on 1 event

    EDIS;       // It is needed to disable write to EALLOW protected registers.
}
