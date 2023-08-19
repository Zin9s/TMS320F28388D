/*
 *  ======== main.c ========
 */

#include <xdc/std.h>

#include <xdc/runtime/Error.h>
#include <xdc/runtime/System.h>

#include <ti/sysbios/BIOS.h>

#include <ti/sysbios/knl/Task.h>

#include <device.h>
#include <f2838x_examples.h>
#include <f2838x_device.h>

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
        GPIO_togglePin(DEVICE_GPIO_PIN_LED2);
        Task_sleep(500); // [ms]
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
