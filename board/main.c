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

void loginPage(void);
void homePage(void);
void scorePage(void);
void booksPage(void);
void busPage(void);
void ecardPage(void);

tContext myDevice;
extern const tDisplay g_sKitronix320x240x16_SSD2119;

// ------------------------------- initial-------------------------------------
void initSysClock(){
	ROM_SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);
	ROM_IntMasterEnable();
}
void graphicInit(){
	PinoutSet();
	Kitronix320x240x16_SSD2119Init();
	GrContextInit(&myDevice, &g_sKitronix320x240x16_SSD2119);
	TouchScreenInit();
	TouchScreenCallbackSet(WidgetPointerMessage);
}
// --------------------------------------------------------------------------------

// ------------------------------ loginPage -------------------------------------
extern tCanvasWidget background;
extern tCanvasWidget heading;
extern tPushButtonWidget logInButton;
void loginButtonClick(tWidget *pWidget);

Canvas(heading, &background, 0, &logInButton,&g_sKitronix320x240x16_SSD2119, 0, 0, 320, 50,(CANVAS_STYLE_FILL | CANVAS_STYLE_OUTLINE |CANVAS_STYLE_TEXT),ClrLightSeaGreen, ClrLightSeaGreen, ClrWhite, &g_sFontCmss22b,"Crossy", 0, 0);
Canvas(background, WIDGET_ROOT, 0, &heading, &g_sKitronix320x240x16_SSD2119, 0, 50, 320, (240-50),CANVAS_STYLE_FILL, ClrWhite, 0, 0, 0, 0, 0, 0);
RectangularButton(logInButton, &heading, 0, 0,&g_sKitronix320x240x16_SSD2119, 80, 160, 160, 40,(PB_STYLE_OUTLINE | PB_STYLE_TEXT_OPAQUE |PB_STYLE_TEXT | PB_STYLE_FILL | PB_STYLE_RELEASE_NOTIFY),ClrLightSeaGreen, 0, ClrWhite, ClrWhite,&g_sFontCmss22b, "Login", 0, 0, 0, 0,loginButtonClick);

void loginButtonClick(tWidget *pWidget){
	homePage();
}

void loginPage(){
	WidgetAdd(WIDGET_ROOT, (tWidget *)&background);
	WidgetPaint(WIDGET_ROOT);
}
//---------------------------------------------------------------------------------

//----------------------------------homePage---------------------------------------
extern tPushButtonWidget scoreQuery;
extern tPushButtonWidget booksQuery;
extern tPushButtonWidget busQuery;
extern tPushButtonWidget ecardQuery;
void scoreQueryButtonClick(tWidget *pWidget);
void booksQueryButtonClick(tWidget *pWidget);
void busQueryButtonClick(tWidget *pWidget);
void ecardQueryButtonClick(tWidget *pWidget);

RectangularButton(scoreQuery, &background, &booksQuery, 0 ,&g_sKitronix320x240x16_SSD2119, 0, 60, 160, 90,(PB_STYLE_OUTLINE | PB_STYLE_TEXT_OPAQUE |PB_STYLE_TEXT | PB_STYLE_FILL | PB_STYLE_RELEASE_NOTIFY),ClrTurquoise, 0, ClrWhite, ClrWhite,&g_sFontCmss22b, "Score Query", 0, 0, 0, 0,scoreQueryButtonClick);
RectangularButton(booksQuery, &background, &busQuery, 0 ,&g_sKitronix320x240x16_SSD2119, 0, 150, 160, 90,(PB_STYLE_OUTLINE | PB_STYLE_TEXT_OPAQUE |PB_STYLE_TEXT | PB_STYLE_FILL | PB_STYLE_RELEASE_NOTIFY),ClrTurquoise, 0, ClrWhite, ClrWhite,&g_sFontCmss22b, "Book Query", 0, 0, 0, 0,booksQueryButtonClick);
RectangularButton(busQuery, &background, &ecardQuery, 0 ,&g_sKitronix320x240x16_SSD2119, 160, 60, 160, 90,(PB_STYLE_OUTLINE | PB_STYLE_TEXT_OPAQUE |PB_STYLE_TEXT | PB_STYLE_FILL | PB_STYLE_RELEASE_NOTIFY),ClrTurquoise, 0, ClrWhite, ClrWhite,&g_sFontCmss22b, "Bus Query", 0, 0, 0, 0,busQueryButtonClick);
RectangularButton(ecardQuery, &background, 0, 0 ,&g_sKitronix320x240x16_SSD2119, 160, 150, 160, 90,(PB_STYLE_OUTLINE | PB_STYLE_TEXT_OPAQUE |PB_STYLE_TEXT | PB_STYLE_FILL | PB_STYLE_RELEASE_NOTIFY),ClrTurquoise, 0, ClrWhite, ClrWhite,&g_sFontCmss22b, "E-Card Query", 0, 0, 0, 0,ecardQueryButtonClick);

void scoreQueryButtonClick(tWidget *pWidget){
	scorePage();
}

void booksQueryButtonClick(tWidget *pWidget){
    booksPage();
}

void busQueryButtonClick(tWidget *pWidget){
    busPage();
}

void ecardQueryButtonClick(tWidget *pWidget){
	ecardPage();	
}

void homePage(){
	WidgetRemove((tWidget *)&logInButton);
	WidgetAdd((tWidget *)&background, (tWidget *)&scoreQuery);
	WidgetAdd((tWidget *)&background, (tWidget *)&booksQuery);
	WidgetAdd((tWidget *)&background, (tWidget *)&busQuery);
	WidgetAdd((tWidget *)&background, (tWidget *)&ecardQuery); 
	WidgetPaint((tWidget *)&background);
}

void removeAllButton(){
	WidgetRemove((tWidget *)&scoreQuery);
	WidgetRemove((tWidget *)&booksQuery);
	WidgetRemove((tWidget *)&busQuery);
	WidgetRemove((tWidget *)&ecardQuery); 
}
// --------------------------------------------------------------------------------------

// -------------------------------------ScorePage----------------------------------------
extern tCanvasWidget score;
Canvas(score, &background, 0, 0,&g_sKitronix320x240x16_SSD2119, 0, 60, 320, 50,(CANVAS_STYLE_FILL | CANVAS_STYLE_OUTLINE |CANVAS_STYLE_TEXT),ClrTurquoise, ClrTurquoise, ClrWhite, &g_sFontCmss22b,"Jin Gang :75", 0, 0);

void scorePage(){
   removeAllButton();
   WidgetAdd((tWidget *)&background, (tWidget *)&score);
   WidgetPaint((tWidget *)&background);
}
// ---------------------------------------------------------------------------------------

// -------------------------------------booksPage-----------------------------------------
extern tCanvasWidget books;
Canvas(books, &background, 0, 0,&g_sKitronix320x240x16_SSD2119, 0, 60, 320, 50,(CANVAS_STYLE_FILL | CANVAS_STYLE_OUTLINE |CANVAS_STYLE_TEXT),ClrTurquoise, ClrTurquoise, ClrWhite, &g_sFontCmss18,"JAVA Development: Kerry Peter, 2014.11", 0, 0);

void booksPage(){
   removeAllButton();
   WidgetAdd((tWidget *)&background, (tWidget *)&books);
   WidgetPaint((tWidget *)&background);
}
// ---------------------------------------------------------------------------------------

// -------------------------------------busPage-------------------------------------------
extern tCanvasWidget bus;
Canvas(bus, &background, 0, 0,&g_sKitronix320x240x16_SSD2119, 0, 60, 320, 50,(CANVAS_STYLE_FILL | CANVAS_STYLE_OUTLINE |CANVAS_STYLE_TEXT),ClrTurquoise, ClrTurquoise, ClrWhite, &g_sFontCmss20b,"6:00 Minhang to Xuhui", 0, 0);

void busPage(){
   removeAllButton();
   WidgetAdd((tWidget *)&background, (tWidget *)&bus);
   WidgetPaint((tWidget *)&background);
}
// ---------------------------------------------------------------------------------------

// -------------------------------------ecardPage-------------------------------------------
extern tCanvasWidget ecard;
Canvas(ecard, &background, 0, 0,&g_sKitronix320x240x16_SSD2119, 0, 60, 320, 50,(CANVAS_STYLE_FILL | CANVAS_STYLE_OUTLINE |CANVAS_STYLE_TEXT),ClrTurquoise, ClrTurquoise, ClrWhite, &g_sFontCmss20b,"remaining: 6.30 RMB", 0, 0);

void ecardPage(){
   removeAllButton();
   WidgetAdd((tWidget *)&background, (tWidget *)&ecard);
   WidgetPaint((tWidget *)&background);
}
// ---------------------------------------------------------------------------------------

int main(void)					
{
	initSysClock();
    graphicInit();

	loginPage();

	while(1){
		WidgetMessageQueueProcess();
	};	   
}
