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

#include "SysCtlConfigure.h"	 

unsigned long TheSysClock = 16000000UL;


void ClockInitial(void)
{
 	
	SysCtlLDOSet(SYSCTL_LDO_2_50V);			
	SysCtlClockSet(SYSCTL_USE_OSC |			
                   SYSCTL_OSC_MAIN |		
                   SYSCTL_XTAL_16MHZ |		
                   SYSCTL_SYSDIV_1);	

	TheSysClock = SysCtlClockGet();			//  get the system clock frequency
}

