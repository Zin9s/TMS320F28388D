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

#include <device.h>
#include <f2838x_examples.h>
#include <f2838x_device.h>

//
// Globals
//
uint32_t TaskA_100mscount = 0;
uint32_t TaskB_10mscount = 0;
uint32_t Swi0IntCount = 0;
uint32_t TimerCIntCount = 0;
uint16_t TimerC1msCount = 0;
uint32_t TimerDIntCount = 0;
uint32_t TimerD1msCount = 0;
uint32_t EPwm1IntCount = 0;
uint32_t LED1count = 0;
uint32_t LED2count = 0;
int32_t diffCount = 0;

//
// Swi Handler
//
extern ti_sysbios_knl_Swi_Handle swi0;

void init_EPwm1(void);


/*
 *  ======== taskFxn ========
 */
void TaskA(void)
{
    while(1)
    {
        TaskA_100mscount++;
        Task_sleep(100); // [ms]
    }
}

void TaskB(void)
{
    while(1)
    {
        TaskB_10mscount++;
        Task_sleep(10); // [ms]
    }
}

/*
 *  ======== timerFxn ========
 */
// TimerC() -> task timer0
// TimerD() -> bios timer1
// TimerC() and TimerD() is running every 31.25us.
void TimerC(void)
{
    Swi_post(swi0);
    TimerCIntCount++;
    if ((TimerCIntCount % 32) == 0)
    {
        TimerC1msCount++;
    }
}

void TimerD(void)
{
    TimerDIntCount++;
    if ((TimerDIntCount % 32) == 0)
    {
        TimerD1msCount++;
    }
    diffCount = Swi0IntCount - EPwm1IntCount;
}

/*
 *  ======== swiFxn ========
 */
void Swi0_ISR(UArg arg0, UArg arg1)
{
    Swi0IntCount++;
    LED2count++;

    //
    // Toggle LED2 when the count reaches 16000
    //
    if (LED2count == 16000)
    {
        GPIO_togglePin(DEVICE_GPIO_PIN_LED2);
        LED2count=0;
    }
}

/*
 *  ======== hwiFxn ========
 */
void EPwm1_ISR(UArg arg0)
{
    EPwm1IntCount++;
    LED1count++;
    //
    // Toggle LED1 every 500ms
    //
    if (LED1count == 16000)
    {
        GPIO_togglePin(DEVICE_GPIO_PIN_LED1);
        LED1count=0;
    }

    //
    // Clear INT flag for this timer
    //
    EPwm1Regs.ETCLR.bit.INT = 1;    // Event Trigger Clear Register
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

/*
 *  ======== main ========
 */
Int main()
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

    BIOS_start();    /* does not return */
    return(0);
}
