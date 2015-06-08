#include "inc/hw_memmap.h"		//register addresses defined in this head file			
#include "inc/hw_types.h"		//data type defined in this head file	
#include "driverlib/debug.h"	//head file for debugging	
#include "driverlib/gpio.h" 	//head file for GPIOs using Stellaris driver library	
#include "driverlib/sysctl.h"	//head file for system control using Stellaris driver library
#include "driverlib/interrupt.h"
#include "driverlib/udma.h"
#include "driverlib/rom.h"
#include "grlib/grlib.h"
#include "grlib/widget.h"
#include "grlib/canvas.h"
#include "grlib/pushbutton.h"
#include "boards/dk-lm3s9d96/drivers/set_pinout.h"
#include "boards/dk-lm3s9d96/drivers/kitronix320x240x16_ssd2119_8bit.h"
#include "boards/dk-lm3s9d96/drivers/touch.h"
#include "GPIODriverConfigure.h"
#include "SysCtlConfigure.h"
#include "SysTickConfigure.h"
#include "UARTConfigure.h"

void loginButtonClick(tWidget *pWidget);
void scoreQueryButtonClick(tWidget *pWidget);
void booksQueryButtonClick(tWidget *pWidget);
void busQueryButtonClick(tWidget *pWidget);
void ecardQueryButtonClick(tWidget *pWidget);

#ifdef ewarm
#pragma data_alignment=1024
tDMAControlTable sDMAControlTable[64];
#elif defined(ccs)
#pragma DATA_ALIGN(sDMAControlTable, 1024)
tDMAControlTable sDMAControlTable[64];
#else
tDMAControlTable sDMAControlTable[64] __attribute__ ((aligned(1024)));
#endif

tContext sContext;
extern const tDisplay g_sKitronix320x240x16_SSD2119;

// ------------------------------- initial-------------------------------------

void graphicInit(){
	ROM_SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);
	PinoutSet();

	ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_UDMA);
    SysCtlDelay(10);
    uDMAControlBaseSet(&sDMAControlTable[0]);
    uDMAEnable();

	ROM_IntMasterEnable();
	Kitronix320x240x16_SSD2119Init();
	GrContextInit(&sContext, &g_sKitronix320x240x16_SSD2119);
	TouchScreenInit();
	TouchScreenCallbackSet(WidgetPointerMessage);
}
// --------------------------------------------------------------------------------


extern tCanvasWidget g_sHeading;
extern tCanvasWidget g_sQueryBackground;
extern tPushButtonWidget g_sScoreQuery;
extern tPushButtonWidget g_sBooksQuery;
extern tPushButtonWidget g_sBusQuery;
extern tPushButtonWidget g_sEcardQuery;

extern tCanvasWidget g_sScoreBackground;
extern tCanvasWidget g_sBooksBackground;
extern tCanvasWidget g_sBusBackground;
extern tCanvasWidget g_sEcardBackground;
extern tCanvasWidget g_sScore;
extern tCanvasWidget g_sBooks;
extern tCanvasWidget g_sBus;
extern tCanvasWidget g_sEcard;

//------------------------------------homepage-----------------------------------------
Canvas(g_sHeading, WIDGET_ROOT, 0, 0,
	&g_sKitronix320x240x16_SSD2119, 0, 0, 320, 50,
	(CANVAS_STYLE_FILL |CANVAS_STYLE_OUTLINE |CANVAS_STYLE_TEXT),
	ClrLightSeaGreen, ClrLightSeaGreen, ClrWhite, 
	&g_sFontCmss22b,"Crossy", 0, 0);

RectangularButton(g_sScoreQuery, &g_sQueryBackground, 0, 0,
	&g_sKitronix320x240x16_SSD2119, 0, 50, 160, 95,
	(PB_STYLE_OUTLINE | PB_STYLE_TEXT_OPAQUE |PB_STYLE_TEXT | PB_STYLE_FILL | PB_STYLE_RELEASE_NOTIFY),
	ClrTurquoise, 0, ClrWhite, ClrWhite,
	&g_sFontCmss22b, "Score Query", 0, 0, 0, 0,scoreQueryButtonClick);

RectangularButton(g_sBooksQuery, &g_sQueryBackground, 0, 0,
	&g_sKitronix320x240x16_SSD2119, 0, 145, 160, 95,
	(PB_STYLE_OUTLINE | PB_STYLE_TEXT_OPAQUE |PB_STYLE_TEXT | PB_STYLE_FILL | PB_STYLE_RELEASE_NOTIFY),
	ClrTurquoise, 0, ClrWhite, ClrWhite,
	&g_sFontCmss22b, "Book Query", 0, 0, 0, 0,booksQueryButtonClick);

RectangularButton(g_sBusQuery, &g_sQueryBackground, 0, 0,
	&g_sKitronix320x240x16_SSD2119, 160, 50, 160, 95,
	(PB_STYLE_OUTLINE | PB_STYLE_TEXT_OPAQUE |PB_STYLE_TEXT | PB_STYLE_FILL | PB_STYLE_RELEASE_NOTIFY),
	ClrTurquoise, 0, ClrWhite, ClrWhite,
	&g_sFontCmss22b, "Bus Query", 0, 0, 0, 0,busQueryButtonClick);

RectangularButton(g_sEcardQuery, &g_sQueryBackground, 0, 0,
	&g_sKitronix320x240x16_SSD2119, 160, 145, 160, 95,
	(PB_STYLE_OUTLINE | PB_STYLE_TEXT_OPAQUE |PB_STYLE_TEXT | PB_STYLE_FILL | PB_STYLE_RELEASE_NOTIFY),
	ClrTurquoise, 0, ClrWhite, ClrWhite,
	&g_sFontCmss22b, "E-Card Query", 0, 0, 0, 0,ecardQueryButtonClick);

Canvas(g_sQueryBackground, WIDGET_ROOT, 0, 0, 
	&g_sKitronix320x240x16_SSD2119, 0, 50, 320, (240-50),
	(CANVAS_STYLE_FILL | CANVAS_STYLE_OUTLINE | CANVAS_STYLE_TEXT), 
	ClrWhite, 0, 0, 
	0, 0, 0, 0);
//----------------------------------------------------------------------------------------

//---------------------------------------score query--------------------------------------
Canvas(g_sScore, &g_sScoreBackground, 0, 0,
	&g_sKitronix320x240x16_SSD2119, 0, 60, 320, 50,
	(CANVAS_STYLE_FILL | CANVAS_STYLE_OUTLINE |CANVAS_STYLE_TEXT),
	ClrTurquoise, ClrTurquoise, ClrWhite, 
	&g_sFontCmss22b,"Jin Gang :75", 0, 0);

Canvas(g_sScoreBackground, WIDGET_ROOT,0,0, 
	&g_sKitronix320x240x16_SSD2119, 0, 50, 320, (240-50),
	CANVAS_STYLE_FILL, 
	ClrWhite, 0, 0, 
	0, 0, 0, 0);


//---------------------------------------book query----------------------------------------	
Canvas(g_sBooks, &g_sBooksBackground, 0, 0,
	&g_sKitronix320x240x16_SSD2119, 0, 60, 320, 50,
	(CANVAS_STYLE_FILL | CANVAS_STYLE_OUTLINE |CANVAS_STYLE_TEXT),
	ClrTurquoise, ClrTurquoise, ClrWhite,
	&g_sFontCmss18,"JAVA Development: Kerry Peter, 2014.11", 0, 0);

Canvas(g_sBooksBackground, WIDGET_ROOT,0,0, 
	&g_sKitronix320x240x16_SSD2119, 0, 50, 320, (240-50),
	CANVAS_STYLE_FILL, 
	ClrWhite, 0, 0, 
	0, 0, 0, 0);

//--------------------------------------bus query---------------------------------------------
Canvas(g_sBus, &g_sBusBackground, 0, 0,
	&g_sKitronix320x240x16_SSD2119, 0, 60, 320, 50,
	(CANVAS_STYLE_FILL | CANVAS_STYLE_OUTLINE |CANVAS_STYLE_TEXT),
	ClrTurquoise, ClrTurquoise, ClrWhite, 
	&g_sFontCmss20b,"6:00 Minhang to Xuhui", 0, 0);

Canvas(g_sBusBackground, WIDGET_ROOT,0,0, 
	&g_sKitronix320x240x16_SSD2119, 0, 50, 320, (240-50),
	CANVAS_STYLE_FILL, 
	ClrWhite, 0, 0, 
	0, 0, 0, 0);

//-----------------------------------------ecard query----------------------------------------
Canvas(g_sEcard, &g_sEcardBackground, 0, 0,
	&g_sKitronix320x240x16_SSD2119, 0, 60, 320, 50,
	(CANVAS_STYLE_FILL | CANVAS_STYLE_OUTLINE |CANVAS_STYLE_TEXT),
	ClrTurquoise, ClrTurquoise, ClrWhite, 
	&g_sFontCmss20b,"remaining: 6.30 RMB", 0, 0);


Canvas(g_sEcardBackground, WIDGET_ROOT,0,0, 
	&g_sKitronix320x240x16_SSD2119, 0, 50, 320, (240-50),
	CANVAS_STYLE_FILL, 
	ClrWhite, 0, 0, 
	0, 0, 0, 0);


void scoreQueryButtonClick(tWidget *pWidget){
	WidgetAdd(WIDGET_ROOT, (tWidget *)&g_sScoreBackground);
	WidgetAdd(WIDGET_ROOT, (tWidget *)&g_sScore);
   	WidgetPaint(WIDGET_ROOT);
	UARTStringPut(UART0_BASE,"scoreQuery\n");
}

void booksQueryButtonClick(tWidget *pWidget){
	WidgetAdd(WIDGET_ROOT, (tWidget *)&g_sBooksBackground);
	WidgetAdd(WIDGET_ROOT, (tWidget *)&g_sBooks);
    WidgetPaint(WIDGET_ROOT);
	UARTStringPut(UART0_BASE,"booksQuery\n");
}

void busQueryButtonClick(tWidget *pWidget){
	WidgetAdd(WIDGET_ROOT, (tWidget *)&g_sBusBackground);
	WidgetAdd(WIDGET_ROOT, (tWidget *)&g_sBus);
    WidgetPaint(WIDGET_ROOT);
	UARTStringPut(UART0_BASE,"busQuery\n");
}

void ecardQueryButtonClick(tWidget *pWidget){
	WidgetAdd(WIDGET_ROOT, (tWidget *)&g_sEcardBackground);
	WidgetAdd(WIDGET_ROOT, (tWidget *)&g_sEcard);
    WidgetPaint(WIDGET_ROOT);
	UARTStringPut(UART0_BASE,"ecardQuery\n"); 	
}
																	   
int main(void)					
{
    graphicInit();

	ClockInitial();
	GPIOInitial();
	SysTickInitial();
	UART0Initial();
	//UARTStringPut(UART0_BASE,"Initial Done!\r\n");
	//UARTStringPut(UART0_BASE,"Press the KEY to continue~\r\n");

	WidgetAdd(WIDGET_ROOT, (tWidget *)&g_sHeading);
	WidgetAdd(WIDGET_ROOT, (tWidget *)&g_sScoreQuery);
	WidgetAdd(WIDGET_ROOT, (tWidget *)&g_sBooksQuery);
	WidgetAdd(WIDGET_ROOT, (tWidget *)&g_sBusQuery);
	WidgetAdd(WIDGET_ROOT, (tWidget *)&g_sEcardQuery);
	WidgetPaint(WIDGET_ROOT);

	while(1){
		WidgetMessageQueueProcess();
	};	   
}
