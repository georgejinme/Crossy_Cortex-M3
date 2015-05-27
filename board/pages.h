#include "inc/hw_memmap.h"		//register addresses defined in this head file			
#include "inc/hw_types.h"		//data type defined in this head file	
#include "driverlib/debug.h"	//head file for debugging	
#include "driverlib/gpio.h" 	//head file for GPIOs using Stellaris driver library	
#include "driverlib/sysctl.h"	//head file for system control using Stellaris driver library
#include "driverlib/interrupt.h"
#include "driverlib/rom.h"
#include "grlib/grlib.h"
#include "grlib/widget.h"
#include "grlib/canvas.h"
#include "grlib/pushbutton.h"
#include "boards/dk-lm3s9d96/drivers/set_pinout.h"
#include "boards/dk-lm3s9d96/drivers/kitronix320x240x16_ssd2119_8bit.h"
#include "boards/dk-lm3s9d96/drivers/touch.h"	

tContext myDevice;
extern tCanvasWidget background;
extern tCanvasWidget Heading;
extern tPushButtonWidget logInButton;

Canvas(background, WIDGET_ROOT, 0, &Heading, &g_sKitronix320x240x16_SSD2119, 0, 0, 320, 240,CANVAS_STYLE_FILL, ClrWhite, 0, 0, 0, 0, 0, 0);
void graphicInit(){
	PinoutSet();
	Kitronix320x240x16_SSD2119Init();
	GrContextInit(&myDevice, &g_sKitronix320x240x16_SSD2119);
}	

void homePage(){
	//Canvas(background, WIDGET_ROOT, 0, &Heading, &g_sKitronix320x240x16_SSD2119, 0, 0, 320, 240,CANVAS_STYLE_FILL, ClrWhite, 0, 0, 0, 0, 0, 0);
	//Canvas(Heading, &background, 0, &logInButton, &myDevice, 0, 0, 320, 23,(CANVAS_STYLE_FILL | CANVAS_STYLE_OUTLINE |CANVAS_STYLE_TEXT),ClrDarkBlue, ClrWhite, ClrWhite, &g_sFontCm20b, "SJTU Assistant", 0, 0);
	//GrContextForegroundSet(&myDevice, ClrWhite);
	//GrContextFontSet(&myDevice, &g_sFontCm18b);
	//GrStringDrawCentered(&myDevice, "SJTU Assistant!", 20, 160, 120, true);

	GrFlush(&myDevice);
}

						


