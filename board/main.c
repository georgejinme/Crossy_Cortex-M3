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
#include "grlib/container.h"
#include "grlib/radiobutton.h"
#include "boards/dk-lm3s9d96/drivers/set_pinout.h"
#include "boards/dk-lm3s9d96/drivers/kitronix320x240x16_ssd2119_8bit.h"
#include "boards/dk-lm3s9d96/drivers/touch.h"
#include "GPIODriverConfigure.h"
#include "SysCtlConfigure.h"
#include "SysTickConfigure.h"
#include "UARTConfigure.h"
#include "bus.h"

void loginButtonClick(tWidget *pWidget);
void scoreQueryButtonClick(tWidget *pWidget);
void booksQueryButtonClick(tWidget *pWidget);
void busQueryButtonClick(tWidget *pWidget);
void ecardQueryButtonClick(tWidget *pWidget);
void semesterChoose(tWidget *pWidget, unsigned long selected);
void showMinhang2xuhui(tWidget *pWidget);

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


tCanvasWidget g_sHeading;
tCanvasWidget g_sQueryBackground;
tPushButtonWidget g_sScoreQuery;
tPushButtonWidget g_sBooksQuery;
tPushButtonWidget g_sBusQuery;
tPushButtonWidget g_sEcardQuery;

tCanvasWidget g_sScoreBackground;
tCanvasWidget g_sBooksBackground;
tCanvasWidget g_sBusBackground;
tCanvasWidget g_sEcardBackground;

tCanvasWidget g_sScore;
tCanvasWidget g_gpa;
tContainerWidget g_semester;
tRadioButtonWidget g_two1;
tRadioButtonWidget g_two2;
tRadioButtonWidget g_one1;
tRadioButtonWidget g_one2;

tCanvasWidget g_sBooks;

tPushButtonWidget g_sBus;

tCanvasWidget g_sEcard;

int c1 = 0;
int c2 = 0;
int c3 = 0;
int c4 = 0;

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
	&g_sKitronix320x240x16_SSD2119, 0, 70, 214, 170,
	(CANVAS_STYLE_FILL | CANVAS_STYLE_OUTLINE |CANVAS_STYLE_TEXT),
	ClrTurquoise, ClrTurquoise, ClrWhite, 
	&g_sFontCmss18b,"Data will be showed here", 0, 0);

Canvas(g_gpa, &g_sScoreBackground, 0, 0,
	&g_sKitronix320x240x16_SSD2119, 0, 50, 320, 20,
	(CANVAS_STYLE_FILL | CANVAS_STYLE_OUTLINE |CANVAS_STYLE_TEXT),
	ClrWhite, ClrWhite, ClrBlack, 
	&g_sFontCmss18b,"Please wait", 0, 0);

RadioButton(g_two1, &g_semester, 0, 0,
	&g_sKitronix320x240x16_SSD2119, 214, 90, 106, 35,
	RB_STYLE_TEXT, 15,
	0, ClrBlack, ClrBlack,
	&g_sFontCmss14,"2014-2015-1", 0, semesterChoose);

RadioButton(g_two2, &g_semester, 0, 0,
	&g_sKitronix320x240x16_SSD2119, 214, 125, 106, 35,
	RB_STYLE_TEXT, 15,
	0, ClrBlack, ClrBlack,
	&g_sFontCmss14,"2014-2015-2", 0, semesterChoose);

RadioButton(g_one1, &g_semester, 0, 0,
	&g_sKitronix320x240x16_SSD2119, 214, 160, 106, 35,
	RB_STYLE_TEXT, 15,
	0, ClrBlack, ClrBlack,
	&g_sFontCmss14,"2013-2014-1", 0, semesterChoose);

RadioButton(g_one2, &g_semester, 0, 0,
	&g_sKitronix320x240x16_SSD2119, 214, 195, 106, 35,
	RB_STYLE_TEXT, 15,
	0, ClrBlack, ClrBlack,
	&g_sFontCmss14,"2013-2014-2", 0, semesterChoose);

Container(g_semester, &g_sScoreBackground, 0, 0,
	&g_sKitronix320x240x16_SSD2119, 214, 70, 106, 170,
	(CTR_STYLE_FILL | CTR_STYLE_OUTLINE |CTR_STYLE_TEXT),
	ClrWhite, ClrWhite, ClrBlack,
	&g_sFontCmss14,"Semester Choices");

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
RectangularButton(g_sBus, &g_sBusBackground, 0, 0,
	&g_sKitronix320x240x16_SSD2119, 0, 50, 320, 30,
	(PB_STYLE_OUTLINE | PB_STYLE_TEXT_OPAQUE |PB_STYLE_TEXT | PB_STYLE_FILL | PB_STYLE_RELEASE_NOTIFY),
	ClrTurquoise, 0, ClrWhite, ClrWhite, 
	&g_sFontCmss20b,"Min Hang To Xu Hui / Weekday", 0, 0,0,0,showMinhang2xuhui);

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

//-------------------------------------------------------------------------------------------
void scoreQueryButtonClick(tWidget *pWidget){
	char *data;
	UARTStringPut(UART0_BASE,"scoreQuery\n"); 
	WidgetAdd(WIDGET_ROOT, (tWidget *)&g_sScoreBackground);
	WidgetAdd(WIDGET_ROOT, (tWidget *)&g_sScore);
	WidgetAdd(WIDGET_ROOT, (tWidget *)&g_gpa);
	WidgetAdd((tWidget *)&g_semester, (tWidget *)&g_two1);
	WidgetAdd((tWidget *)&g_semester, (tWidget *)&g_two2);
	WidgetAdd((tWidget *)&g_semester, (tWidget *)&g_one1);
	WidgetAdd((tWidget *)&g_semester, (tWidget *)&g_one2);
	WidgetAdd(WIDGET_ROOT, (tWidget *)&g_semester);

	WidgetRemove((tWidget *)&g_sScoreQuery);
	WidgetRemove((tWidget *)&g_sBooksQuery);
	WidgetRemove((tWidget *)&g_sBusQuery);
	WidgetRemove((tWidget *)&g_sEcardQuery);
	WidgetRemove((tWidget *)&g_sQueryBackground);

	UARTStringGet(data, UART0_BASE);
	CanvasTextSet(&g_gpa, data);
	WidgetPaint(WIDGET_ROOT);
}

void semesterChoose(tWidget *pWidget, unsigned long selected){
	char *data;
	int i = 0;
	if (pWidget == (tWidget *) (&g_two1) && c1 == 0){
		tContext sContext1;
		GrContextInit(&sContext1, &g_sKitronix320x240x16_SSD2119);
		i = 0;
		c1 = 1;
		c2 = 0;
		c3 = 0;
		c4 = 0;
		UARTStringPut(UART0_BASE,"2014-2015-1\n");

		while (1){
			UARTStringGet(data, UART0_BASE);
			if (*data == '#') break;
			GrContextForegroundSet(&sContext1, ClrBlack);
			GrContextFontSet(&sContext1, &g_sFontCmss12);
			GrStringDraw(&sContext1, data, -1, 0, 70 + i * 13, false);
			GrFlush(&sContext1);
			++i;
		}
	} 
	else if (pWidget == (tWidget *) (&g_two2) && c2 == 0){
		tContext sContext2;
		GrContextInit(&sContext2, &g_sKitronix320x240x16_SSD2119);
		i = 0;
		c1 = 0;
		c2 = 1;
		c3 = 0;
		c4 = 0;
		UARTStringPut(UART0_BASE,"2014-2015-2\n");																		 	
		while (1){
			UARTStringGet(data, UART0_BASE);
			if (*data == '#') break;
			GrContextForegroundSet(&sContext2, ClrBlack);
			GrContextFontSet(&sContext2, &g_sFontCmss12);
			GrStringDraw(&sContext2, data, -1, 0, 70 + i * 13, false);
			GrFlush(&sContext2);
			++i;
		}
	}
	else if (pWidget == (tWidget *) (&g_one1) && c3 == 0){
		tContext sContext3;
		GrContextInit(&sContext3,&g_sKitronix320x240x16_SSD2119);
		i = 0;
		c1 = 0;
		c2 = 0;
		c3 = 1;
		c4 = 0;
		UARTStringPut(UART0_BASE,"2013-2014-1\n");
		while (1){
			UARTStringGet(data, UART0_BASE);
			if (*data == '#') break;
			GrContextForegroundSet(&sContext3, ClrBlack);
			GrContextFontSet(&sContext3, &g_sFontCmss12);
			GrStringDraw(&sContext3, data, -1, 0, 70 + i * 13, false);
			GrFlush(&sContext3);
			++i;
		}
	}
	else if (pWidget == (tWidget *) (&g_one2) && c4 == 0){
		tContext sContext4;
		GrContextInit(&sContext4, &g_sKitronix320x240x16_SSD2119);
		i = 0;
		c1 = 0;
		c2 = 0;
		c3 = 0;
		c4 = 1;
		UARTStringPut(UART0_BASE,"2013-2014-2\n");
		while (1){
			UARTStringGet(data, UART0_BASE);
			if (*data == '#') break;
			GrContextForegroundSet(&sContext4, ClrBlack);
			GrContextFontSet(&sContext4, &g_sFontCmss12);
			GrStringDraw(&sContext4, data, -1, 0, 70 + i * 13, false);
			GrFlush(&sContext4);
			++i;
		}
	}
}

//--------------------------------------------------------------------------------------

void booksQueryButtonClick(tWidget *pWidget){
	WidgetAdd(WIDGET_ROOT, (tWidget *)&g_sBooksBackground);
	WidgetAdd(WIDGET_ROOT, (tWidget *)&g_sBooks);
	WidgetRemove((tWidget *)&g_sScoreQuery);
	WidgetRemove((tWidget *)&g_sBooksQuery);
	WidgetRemove((tWidget *)&g_sBusQuery);
	WidgetRemove((tWidget *)&g_sEcardQuery);
	WidgetRemove((tWidget *)&g_sQueryBackground);
    WidgetPaint(WIDGET_ROOT);
	UARTStringPut(UART0_BASE,"booksQuery\n");
}

//--------------------------------------------------------------------------------------
void busQueryButtonClick(tWidget *pWidget){
	WidgetAdd(WIDGET_ROOT, (tWidget *)&g_sBusBackground);
	WidgetAdd(WIDGET_ROOT, (tWidget *)&g_sBus);
	WidgetRemove((tWidget *)&g_sScoreQuery);
	WidgetRemove((tWidget *)&g_sBooksQuery);
	WidgetRemove((tWidget *)&g_sBusQuery);
	WidgetRemove((tWidget *)&g_sEcardQuery);
	WidgetRemove((tWidget *)&g_sQueryBackground);
    WidgetPaint(WIDGET_ROOT);
	UARTStringPut(UART0_BASE,"busQuery\n");
}
void showMinhang2xuhui(tWidget *pWidget){
	tContext sContext1;
	GrContextInit(&sContext1, &g_sKitronix320x240x16_SSD2119);
	UARTStringPut(UART0_BASE,"minhang2xuhui\n");
	GrContextForegroundSet(&sContext1, ClrBlack);
	GrContextFontSet(&sContext1, &g_sFontCmss14);
	GrStringDraw(&sContext1, "07:00 undirect            07:30 undirect", -1, 0, 100, false);
	GrStringDraw(&sContext1, "09:00 direct              10:10 direct", -1, 0, 120, false);
	GrStringDraw(&sContext1, "12:00 undirect            13:00 direct", -1, 0, 140, false);
	GrStringDraw(&sContext1, "15:00 direct              17:00 direct", -1, 0, 160, false);
	GrStringDraw(&sContext1, "18:00 direct              20:00 direct", -1, 0, 180, false);
	GrStringDraw(&sContext1, "21:30 direct", -1, 0, 200, false);
	GrFlush(&sContext1);
}

//---------------------------------------------------------------------------------------
void ecardQueryButtonClick(tWidget *pWidget){
	WidgetAdd(WIDGET_ROOT, (tWidget *)&g_sEcardBackground);
	WidgetAdd(WIDGET_ROOT, (tWidget *)&g_sEcard);
	WidgetRemove((tWidget *)&g_sScoreQuery);
	WidgetRemove((tWidget *)&g_sBooksQuery);
	WidgetRemove((tWidget *)&g_sBusQuery);
	WidgetRemove((tWidget *)&g_sEcardQuery);
	WidgetRemove((tWidget *)&g_sQueryBackground);
    WidgetPaint(WIDGET_ROOT);
	UARTStringPut(UART0_BASE,"ecardQuery\n"); 	
}
//----------------------------------------------------------------------------------------------
																	   
int main(void)					
{
    graphicInit();

	ClockInitial();
	GPIOInitial();
	SysTickInitial();
	UART0Initial();
	UARTStringPut(UART0_BASE,"Initial Done\n");

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
