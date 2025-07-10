

/**
 * main.c
 */


#include <stdbool.h>
#include <stdint.h>
#include "inc/hw_memmap.h"
#include "driverlib/gpio.h"
#include "driverlib/ssi.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "utils/uartstdio.h"

#define PIN1_SET (1 << 1)
#define PIN2_SET (1 << 2)
#define PIN3_SET (1 << 3)


void uartInit(void)
{
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    GPIOPinConfigure(GPIO_PA0_U0RX);
    GPIOPinConfigure(GPIO_PA1_U0TX);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
    UARTClockSourceSet(UART0_BASE, UART_CLOCK_PIOSC);
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
    UARTStdioConfig(0, 115200, 16000000);
    UARTConfigSetExpClk(UART0_BASE, SysCtlClockGet(), 115200, (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));
    UARTprintf("UART0 init successfully\n");
}

void gpioInit(void)
{
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3);
    UARTprintf("GPIO init successfully\n");
}

void spiInit(void)
{
    SysCtlPeripheralEnable(SYSCTL_PERIPH_SSI0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    GPIOPinConfigure(GPIO_PA2_SSI0CLK);
    GPIOPinConfigure(GPIO_PA3_SSI0FSS);
    GPIOPinConfigure(GPIO_PA4_SSI0RX);
    GPIOPinConfigure(GPIO_PA5_SSI0TX);
    GPIOPinTypeSSI(GPIO_PORTA_BASE, GPIO_PIN_5 | GPIO_PIN_4 | GPIO_PIN_3 | GPIO_PIN_2);
    SSIConfigSetExpClk(SSI1_BASE, ui32SysClock, SSI_FRF_MOTO_MODE_0, SSI_MODE_SLAVE, 1000000, 8);        // 8-bit data
    SSIEnable(SSI1_BASE);
    UARTprintf("SSI0 init successfully\n");
}

int main()
{
    uint32_t ui32SysClock;
    uint32_t receivedData;

    ui32SysClock = SysCtlClockFreqSet(SYSCTL_XTAL_16MHZ | SYSCTL_OSC_MAIN | SYSCTL_USE_PLL | SYSCTL_CFG_VCO_320, 50000000);
    uartInit();
    spiInit();
    gpioInit();
    while (SSIDataGetNonBlocking(SSI1_BASE, &receivedData)) {}

    while(1)
    {
        if (SSIDataGetNonBlocking(SSI1_BASE, &receivedData))
        {
            if ((uint8_t)receivedData == 0x50)
            {
                GPIOPinWrite(LED_PORT, LED_PIN, LED_PIN);
            }
            else
            {
                GPIOPinWrite(LED_PORT, LED_PIN, 0);
            }
        }
    }
}
