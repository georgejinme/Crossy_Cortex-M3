#include "inc/hw_types.h"
#include "inc/hw_memmap.h"	
#include "inc/hw_sysctl.h"
#include "inc/hw_gpio.h"
#include "inc/hw_timer.h"
#include "inc/hw_ints.h"
#include "inc/hw_watchdog.h"		  	
#include "inc/hw_uart.h"
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
#include "GPIODriverConfigure.h"


void GPIOInitial(void)
{
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);					//UART0
  	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);  					//KEY RIGHT | KEY LEFT
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);					//KEY PRESS | KEY UP
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);					//KEY DOWN  | LED1 | LED0
		
	GPIOPinConfigure(GPIO_PA0_U0RX);
	GPIOPinConfigure(GPIO_PA1_U0TX);
	GPIOPinConfigure(GPIO_PF2_LED1);
	GPIOPinConfigure(GPIO_PF3_LED0);

	GPIOPinTypeGPIOOutput(LED0_BASE,LED0_PIN);						//Set LED0
	GPIOPinTypeGPIOOutput(LED1_BASE,LED1_PIN);						//Set LED1
	LEDOff(LED_ALL);

	GPIOPinTypeGPIOInput(KEY_PRESS_BASE, KEY_PRESS_PIN);			//Set Key Press
	GPIOPinTypeGPIOInput(KEY_LEFT_BASE,  KEY_LEFT_PIN);				//Set Key Left
	GPIOPinTypeGPIOInput(KEY_RIGHT_BASE, KEY_RIGHT_PIN);			//Set Key Right
	GPIOPinTypeGPIOInput(KEY_UP_BASE,    KEY_UP_PIN);				//Set Key Up
	GPIOPinTypeGPIOInput(KEY_DOWN_BASE,  KEY_DOWN_PIN);				//Set Key Down

	GPIOPinTypeUART(UART0_PIN_BASE, UART0RX_PIN | UART0TX_PIN);		//Set UART0
	
	GPIOIntTypeSet(KEY_LEFT_BASE,KEY_LEFT_PIN,GPIO_FALLING_EDGE);	//Left中断
	GPIOPinIntEnable(KEY_LEFT_BASE, KEY_LEFT_PIN);
	GPIOIntTypeSet(KEY_RIGHT_BASE,KEY_RIGHT_PIN,GPIO_FALLING_EDGE);	//Right中断
	GPIOPinIntEnable(KEY_RIGHT_BASE,KEY_RIGHT_PIN);	
	GPIOIntTypeSet(KEY_UP_BASE,KEY_UP_PIN,GPIO_FALLING_EDGE);		//Up中断
	GPIOPinIntEnable(KEY_UP_BASE,KEY_UP_PIN);	
	GPIOIntTypeSet(KEY_DOWN_BASE,KEY_DOWN_PIN,GPIO_FALLING_EDGE);	//Down中断
	GPIOPinIntEnable(KEY_DOWN_BASE,KEY_DOWN_PIN);
	
	IntEnable(INT_GPIOE);											//KEY_PRESS | KEY_UP INTS
	IntEnable(INT_GPIOB);											//KEY_LEFT | KEY_RIGHT INTS
	IntEnable(INT_GPIOF);											//KEY_DOWN INT
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
