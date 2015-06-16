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
#include "UARTConfigure.h"
#include <string.h>

void UART0Initial(void)
{
	SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);// 1. enable the UART0
	UARTDisable(UART0_BASE);
	UARTConfigSet(UART0_BASE, 115200, UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE);

	// 2. disable the transmitter and receiver of the UART0 before you program on UART0	by invoking UARTDisable()

	// 3. configure the UART0 with a Baud rate of 115200, 8-bit char, 1 stop bit, and no parity checking by invoking UARTConfigSet()
	//    or UARTConfigSetExpClk()

	UARTEnable(UART0_BASE);// 4. enable the transmitter and receiver of the UART0 by invoking UARTEnable()

}

void UARTStringPut(unsigned long ulBase,const char *cMessage)
{
	while (*cMessage != '\0'){
		UARTCharPut(ulBase, *(cMessage++));
	}
	// put your code here to use UART to send a string 

}
void UARTStringGet(char * data, unsigned long ulBase){
	int i = 0;
	while (1){
		char tmp = UARTCharGet(ulBase);
		if (tmp == '@') break;
		else  data[i++] = tmp;
	}
	data[i] = '\0';
}

