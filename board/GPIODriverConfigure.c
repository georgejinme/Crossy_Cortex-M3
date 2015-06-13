#include "inc/hw_types.h"
#include "inc/hw_memmap.h"	
#include "inc/hw_sysctl.h"
#include "inc/hw_gpio.h"
#include "inc/hw_timer.h"
#include "inc/hw_ints.h"
#include "inc/hw_watchdog.h"		  	
#include "inc/hw_uart.h"
#include "inc/hw_i2c.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/debug.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/systick.h"
#include "driverlib/timer.h"
#include "driverlib/interrupt.h"
#include "driverlib/watchdog.h"
#include "driverlib/uart.h"
#include "driverlib/i2c.h"
#include "GPIODriverConfigure.h"


void GPIOInitial(void)
{
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);					// Enable GPIO Port A for the usage of UART0
  	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);  					// Enable GPIO Port B for the usage of I2C, and RIGHT and LEFT keys
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);					// Enable GPIO Port E for the usage of PRESS and DOWN keys
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);					// Enable GPIO Port F for the usage of LED0, LED1 and UP key

	GPIOPinConfigure(GPIO_PB2_I2C0SCL);							  	// Configures the alternate function of a GPIO pin
	GPIOPinConfigure(GPIO_PB3_I2C0SDA);								// Configures the alternate function of a GPIO pin
	GPIOPinConfigure(GPIO_PF2_LED1);								// Configures the alternate function of a GPIO pin
	GPIOPinConfigure(GPIO_PF3_LED0);								// Configures the alternate function of a GPIO pin
	GPIOPadConfigSet(I2C0_PIN_BASE, I2C0SCL_PIN |I2C0SDA_PIN, 		// Enable the I2C pins for Open Drain operation
					 GPIO_STRENGTH_8MA, GPIO_PIN_TYPE_OD);
	GPIOPinTypeI2C(I2C0_PIN_BASE, I2C0SCL_PIN | I2C0SDA_PIN);		// Configures pin(s) for use by the I2C peripheral

	GPIOPinTypeGPIOOutput(LED0_BASE,LED0_PIN);						// PF2 is set as GPIO Output pins to control LED0
	GPIOPinTypeGPIOOutput(LED1_BASE,LED1_PIN);						// PF3 is set as GPIO Output pins to control LED1
	LEDOff(LED_ALL);

	GPIOPinTypeGPIOInput(KEY_LEFT_BASE,  KEY_LEFT_PIN);				// PB6 is set as GPIO Input for LEFT key
	GPIOPinTypeGPIOInput(KEY_RIGHT_BASE, KEY_RIGHT_PIN);			// PB4 is set as GPIO Input for RIGHT key

	GPIOPinTypeGPIOInput(KEY_PRESS_BASE, KEY_PRESS_PIN);			// PE5 is set as GPIO Input for PRESS key
	GPIOPinTypeGPIOInput(KEY_DOWN_BASE,  KEY_DOWN_PIN);				// PE4 is set as GPIO Input for DOWN key

	GPIOPinTypeGPIOInput(KEY_UP_BASE, KEY_UP_PIN);					// PF1 is set as GPIO Input for UP key
	GPIOPinTypeUART(UART0_PIN_BASE, UART0RX_PIN | UART0TX_PIN);		// Configures pin(s) for use by the UART0 peripheral
}
		
void LEDOn(unsigned char LEDNum)
{
	switch (LEDNum)
	{
		case	LED_0:	HWREG(LED0_BASE+GPIO_O_DATA+(LED0_PIN<<2))=~LED0_PIN;break;
		case	LED_1:	HWREG(LED1_BASE+GPIO_O_DATA+(LED1_PIN<<2))=~LED1_PIN;break;
		case	LED_ALL:
		{
			HWREG(LED0_BASE+GPIO_O_DATA+(LED0_PIN<<2))=~LED0_PIN;
			HWREG(LED1_BASE+GPIO_O_DATA+(LED1_PIN<<2))=~LED1_PIN;
			break;
		}
		default:	break;
	}
}

void LEDOff(unsigned char LEDNum)
{
	switch (LEDNum)
	{
		case	LED_0:	HWREG(LED0_BASE+GPIO_O_DATA+(LED0_PIN<<2))=LED0_PIN;break;
		case	LED_1:	HWREG(LED1_BASE+GPIO_O_DATA+(LED1_PIN<<2))=LED1_PIN;break;
		case	LED_ALL:
		{
			HWREG(LED0_BASE+GPIO_O_DATA+(LED0_PIN<<2))=LED0_PIN;
			HWREG(LED1_BASE+GPIO_O_DATA+(LED1_PIN<<2))=LED1_PIN;
			break;
		}
		default:	break;
	}
}

void LEDOverturn(unsigned char LEDNum)
{
	switch (LEDNum)
	{
		case	LED_0:	HWREG(LED0_BASE+GPIO_O_DATA+(LED0_PIN<<2)) =
						~HWREG(LED0_BASE+GPIO_O_DATA+(LED0_PIN<<2));
						break;
		case	LED_1:	HWREG(LED1_BASE+GPIO_O_DATA+(LED1_PIN<<2)) =
						~HWREG(LED1_BASE+GPIO_O_DATA+(LED1_PIN<<2));
						break;
		case	LED_ALL:
		{
			HWREG(LED0_BASE+GPIO_O_DATA+(LED0_PIN<<2)) =
			~HWREG(LED0_BASE+GPIO_O_DATA+(LED0_PIN<<2));
			HWREG(LED1_BASE+GPIO_O_DATA+(LED1_PIN<<2)) =
			~HWREG(LED1_BASE+GPIO_O_DATA+(LED1_PIN<<2));
			break;
		}
		default:	break;
	}
}		

unsigned char KeyPress(unsigned char KeyNum)
{
	switch	(KeyNum)
	{
		case	KEY_PRESS:
		{
			if (HWREG(KEY_PRESS_BASE+GPIO_O_DATA+(KEY_PRESS_PIN<<2)))
				return 0;
			else
				return 1;
		}
		case	KEY_LEFT:
		{
			if (HWREG(KEY_LEFT_BASE+GPIO_O_DATA+(KEY_LEFT_PIN<<2)))
				return 0;
			else
				return 1;
		}
		case	KEY_RIGHT:
		{
			if (HWREG(KEY_RIGHT_BASE+GPIO_O_DATA+(KEY_RIGHT_PIN<<2)))
				return 0;
			else
				return 1;
		}
		case	KEY_UP:
		{
			if (HWREG(KEY_UP_BASE+GPIO_O_DATA+(KEY_UP_PIN<<2)))
				return 0;
			else
				return 1;
		}
		case	KEY_DOWN:
		{
			if (HWREG(KEY_DOWN_BASE+GPIO_O_DATA+(KEY_DOWN_PIN<<2)))
				return 0;
			else
				return 1;
		}
		default:	break;
	}
	return 0;
}
