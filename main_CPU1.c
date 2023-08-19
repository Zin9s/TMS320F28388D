/*
 *  ======== main.c ========
 */

#include <xdc/std.h>

#include <xdc/runtime/Error.h>
#include <xdc/runtime/System.h>

#include <ti/sysbios/BIOS.h>

#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Swi.h>

#include <device.h>
#include <f2838x_examples.h>
#include <f2838x_device.h>

//
// Globals
//
uint32_t Swi0IntCount = 0;
uint32_t TimerCIntCount = 0;
uint16_t TimerC1msCount = 0;
uint16_t LEDcount = 0;

//
// Swi Handler
//
extern ti_sysbios_knl_Swi_Handle swi0;


void Swi0_ISR(UArg arg0, UArg arg1);


/*
 *  ======== taskFxn ========
 */
void TaskA(void)
{
    while(1)
    {
        GPIO_togglePin(DEVICE_GPIO_PIN_LED1);
        Task_sleep(200); // [ms]
    }
}

void TaskB(void)
{
    while(1)
    {
//        Swi_post(swi0);
        Task_sleep(10); // [ms]
    }
}

/*
 *  ======== timerFxn ========
 */
void TimerC(void)
{
    Swi_post(swi0);
    TimerCIntCount++;
    if ((TimerCIntCount % 32) == 0)
    {
        TimerC1msCount++;
    }
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

    BIOS_start();    /* does not return */
    return(0);
}

void Swi0_ISR(UArg arg0, UArg arg1)
{
    Swi0IntCount++;
    LEDcount++;

    //
    // Toggle LED2 when the count reaches 32000
    //
    if (LEDcount == 32000)
    {
        GPIO_togglePin(DEVICE_GPIO_PIN_LED2);
        LEDcount=0;
    }

}
