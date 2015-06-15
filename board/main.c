#include <stdlib.h>
#include <stdio.h>
#include "inc/hw_memmap.h"		//register addresses defined in this head file			
#include "inc/hw_types.h"		//data type defined in this head file
#include "inc/hw_ints.h"	
#include "driverlib/flash.h"
#include "driverlib/debug.h"	//head file for debugging	
#include "driverlib/gpio.h" 	//head file for GPIOs using Stellaris driver library	
#include "driverlib/sysctl.h"	//head file for system control using Stellaris driver library
#include "driverlib/interrupt.h"
#include "driverlib/udma.h"
#include "driverlib/rom.h"
#include "driverlib/i2s.h"
#include "grlib/grlib.h"
#include "grlib/widget.h"
#include "grlib/canvas.h"
#include "grlib/pushbutton.h"
#include "grlib/container.h"
#include "grlib/radiobutton.h"
#include "boards/dk-lm3s9d96/drivers/set_pinout.h"
#include "boards/dk-lm3s9d96/drivers/kitronix320x240x16_ssd2119_8bit.h"
#include "boards/dk-lm3s9d96/drivers/touch.h"
#include "utils/ustdlib.h"
#include "third_party/fatfs/src/ff.h"
#include "third_party/fatfs/src/diskio.h"
#include "boards/dk-lm3s9d96/drivers/sound.h"
#include "GPIODriverConfigure.h"
#include "SysCtlConfigure.h"
#include "SysTickConfigure.h"
#include "UARTConfigure.h"
#include "I2CConfigure.h"
#include "NixieTubeConfigure.h"
#include "bus.h"

void loginButtonClick(tWidget *pWidget);
void scoreQueryButtonClick(tWidget *pWidget);
void booksQueryButtonClick(tWidget *pWidget);
void busQueryButtonClick(tWidget *pWidget);
void ecardQueryButtonClick(tWidget *pWidget);
void semesterChoose(tWidget *pWidget, unsigned long selected);
void showMinhang2xuhui(tWidget *pWidget);
void schoolBusPicture(tWidget *pWidget);
void showBooksData(tWidget *pWidget);

tContext sContext;
extern const tDisplay g_sKitronix320x240x16_SSD2119;

extern char NixieTube[];
extern unsigned char volatile LEDSerial;

//---------------------------------i2s-----------------------------------------
#ifdef ewarm
#pragma data_alignment=1024
tDMAControlTable sDMAControlTable[64];
#elif defined(ccs)
#pragma DATA_ALIGN(sDMAControlTable, 1024)
tDMAControlTable sDMAControlTable[64];
#else
tDMAControlTable sDMAControlTable[64] __attribute__ ((aligned(1024)));
#endif

static FATFS g_sFatFs;
static DIR g_sDirObject;
static FILINFO g_sFileInfo;
static FIL g_sFileObject;

#define TICKS_PER_SECOND 100

#define NUM_LIST_STRINGS 48
const char *g_ppcDirListStrings[NUM_LIST_STRINGS];

#define MAX_FILENAME_STRING_LEN (8 + 1 + 3 + 1)
char g_pcFilenames[NUM_LIST_STRINGS][MAX_FILENAME_STRING_LEN];

void WaveStop(void);
#define INITIAL_VOLUME_PERCENT 60

static unsigned long g_ulBytesPlayed;
static unsigned long g_ulNextUpdate;
#define AUDIO_BUFFER_SIZE       4096
static unsigned char g_pucBuffer[AUDIO_BUFFER_SIZE];
unsigned long g_ulMaxBufferSize;
#define BUFFER_BOTTOM_EMPTY     0x00000001
#define BUFFER_TOP_EMPTY        0x00000002
#define BUFFER_PLAYING          0x00000004
static volatile unsigned long g_ulFlags;
static unsigned long g_ulBytesRemaining;
static unsigned short g_usMinutes;
static unsigned short g_usSeconds;

void BufferCallback(void *pvBuffer, unsigned long ulEvent)
{
    if(ulEvent & BUFFER_EVENT_FREE)
    {
        if(pvBuffer == g_pucBuffer)
        {
            //
            // Flag if the first half is free.
            //
            g_ulFlags |= BUFFER_BOTTOM_EMPTY;
        }
        else
        {
            //
            // Flag if the second half is free.
            //
            g_ulFlags |= BUFFER_TOP_EMPTY;
        }

        //
        // Update the byte count.
        //
        g_ulBytesPlayed += AUDIO_BUFFER_SIZE >> 1;
    }
}

#define RIFF_CHUNK_ID_RIFF      0x46464952
#define RIFF_CHUNK_ID_FMT       0x20746d66
#define RIFF_CHUNK_ID_DATA      0x61746164

#define RIFF_TAG_WAVE           0x45564157

#define RIFF_FORMAT_UNKNOWN     0x0000
#define RIFF_FORMAT_PCM         0x0001
#define RIFF_FORMAT_MSADPCM     0x0002
#define RIFF_FORMAT_IMAADPCM    0x0011

typedef struct
{
    //
    // Sample rate in bytes per second.
    //
    unsigned long ulSampleRate;

    //
    // The average byte rate for the wav file.
    //
    unsigned long ulAvgByteRate;

    //
    // The size of the wav data in the file.
    //
    unsigned long ulDataSize;

    //
    // The number of bits per sample.
    //
    unsigned short usBitsPerSample;

    //
    // The wav file format.
    //
    unsigned short usFormat;

    //
    // The number of audio channels.
    //
    unsigned short usNumChannels;
}
tWaveHeader;
static tWaveHeader g_sWaveHeader;

FRESULT WaveOpen(FIL *psFileObject, const char *pcFileName, tWaveHeader *pWaveHeader)
{
    unsigned long *pulBuffer;
    unsigned short *pusBuffer;
    unsigned long ulChunkSize;
    unsigned short usCount;
    unsigned long ulBytesPerSample;
    FRESULT Result;

    pulBuffer = (unsigned long *)g_pucBuffer;
    pusBuffer = (unsigned short *)g_pucBuffer;

    Result = f_open(psFileObject, pcFileName, FA_READ);
    if(Result != FR_OK)
    {
		//sprintf(NixieTube,"1234");
        return(Result);
    }

    //
    // Read the first 12 bytes.
    //
    Result = f_read(psFileObject, g_pucBuffer, 12, &usCount);
    if(Result != FR_OK)
    {
        f_close(psFileObject);
        return(Result);
    }

    //
    // Look for RIFF tag.
    //
    if((pulBuffer[0] != RIFF_CHUNK_ID_RIFF) || (pulBuffer[2] != RIFF_TAG_WAVE))
    {
        f_close(psFileObject);
        return(FR_INVALID_NAME);
    }

    //
    // Read the next chunk header.
    //
    Result = f_read(psFileObject, g_pucBuffer, 8, &usCount);
    if(Result != FR_OK)
    {
        f_close(psFileObject);
        return(Result);
    }

    if(pulBuffer[0] != RIFF_CHUNK_ID_FMT)
    {
        f_close(psFileObject);
        return(FR_INVALID_NAME);
    }

    //
    // Read the format chunk size.
    //
    ulChunkSize = pulBuffer[1];

    if(ulChunkSize > 16)
    {
        f_close(psFileObject);
        return(FR_INVALID_NAME);
    }

    //
    // Read the next chunk header.
    //
    Result = f_read(psFileObject, g_pucBuffer, ulChunkSize, &usCount);
    if(Result != FR_OK)
    {
        f_close(psFileObject);
        return(Result);
    }

    pWaveHeader->usFormat = pusBuffer[0];
    pWaveHeader->usNumChannels =  pusBuffer[1];
    pWaveHeader->ulSampleRate = pulBuffer[1];
    pWaveHeader->ulAvgByteRate = pulBuffer[2];
    pWaveHeader->usBitsPerSample = pusBuffer[7];

    //
    // Reset the byte count.
    //
    g_ulBytesPlayed = 0;
    g_ulNextUpdate = 0;

    //
    // Calculate the Maximum buffer size based on format.  There can only be
    // 1024 samples per ping pong buffer due to uDMA.
    //
    ulBytesPerSample = (pWaveHeader->usBitsPerSample *
                        pWaveHeader->usNumChannels) >> 3;

    if(((AUDIO_BUFFER_SIZE >> 1) / ulBytesPerSample) > 1024)
    {
        //
        // The maximum number of DMA transfers was more than 1024 so limit
        // it to 1024 transfers.
        //
        g_ulMaxBufferSize = 1024 * ulBytesPerSample;
    }
    else
    {
        //
        // The maximum number of DMA transfers was not more than 1024.
        //
        g_ulMaxBufferSize = AUDIO_BUFFER_SIZE >> 1;
    }

    //
    // Only mono and stereo supported.
    //
    if(pWaveHeader->usNumChannels > 2)
    {
        f_close(psFileObject);
        return(FR_INVALID_NAME);
    }

    //
    // Read the next chunk header.
    //
    Result = f_read(psFileObject, g_pucBuffer, 8, &usCount);
    if(Result != FR_OK)
    {
        f_close(psFileObject);
        return(Result);
    }

    if(pulBuffer[0] != RIFF_CHUNK_ID_DATA)
    {
        f_close(psFileObject);
        return(Result);
    }

    //
    // Save the size of the data.
    //
    pWaveHeader->ulDataSize = pulBuffer[1];

    g_usSeconds = pWaveHeader->ulDataSize/pWaveHeader->ulAvgByteRate;
    g_usMinutes = g_usSeconds/60;
    g_usSeconds -= g_usMinutes*60;

    //
    // Set the number of data bytes in the file.
    //
    g_ulBytesRemaining = pWaveHeader->ulDataSize;

    //
    // Adjust the average bit rate for 8 bit mono files.
    //
    if((pWaveHeader->usNumChannels == 1) && (pWaveHeader->usBitsPerSample == 8))
    {
        pWaveHeader->ulAvgByteRate <<=1;
    }

    //
    // Set the format of the playback in the sound driver.
    //
    SoundSetFormat(pWaveHeader->ulSampleRate, pWaveHeader->usBitsPerSample,
                   pWaveHeader->usNumChannels);

    return(FR_OK);
}

void WaveClose(FIL *psFileObject)
{
    //
    // Close out the file.
    //
    f_close(psFileObject);
}

void Convert8Bit(unsigned char *pucBuffer, unsigned long ulSize)
{
    unsigned long ulIdx;

    for(ulIdx = 0; ulIdx < ulSize; ulIdx++)
    {
        //
        // In place conversion of 8 bit unsigned to 8 bit signed.
        //
        *pucBuffer = ((short)(*pucBuffer)) - 128;
        pucBuffer++;
    }
}

void WaveStop(void)
{
    //
    // Stop playing audio.
    //
    g_ulFlags &= ~BUFFER_PLAYING;
}

unsigned short WaveRead(FIL *psFileObject, tWaveHeader *pWaveHeader, unsigned char *pucBuffer)
{
    unsigned long ulBytesToRead;
    unsigned short usCount;

    //
    // Either read a half buffer or just the bytes remaining if we are at the
    // end of the file.
    //
    if(g_ulBytesRemaining < g_ulMaxBufferSize)
    {
        ulBytesToRead = g_ulBytesRemaining;
    }
    else
    {
        ulBytesToRead = g_ulMaxBufferSize;
    }

    //
    // Read in another buffer from the sd card.
    //
    if(f_read(&g_sFileObject, pucBuffer, ulBytesToRead, &usCount) != FR_OK)
    {
        return(0);
    }

    //
    // Decrement the number of data bytes remaining to be read.
    //
    g_ulBytesRemaining -= usCount;

    //
    // Need to convert the audio from unsigned to signed if 8 bit
    // audio is used.
    //
    if(pWaveHeader->usBitsPerSample == 8)
    {
        Convert8Bit(pucBuffer, usCount);
    }

    return(usCount);
}

unsigned long
WavePlay(FIL *psFileObject, tWaveHeader *pWaveHeader)
{
    static unsigned short usCount;

    //
    // Mark both buffers as empty.
    //
    g_ulFlags = BUFFER_BOTTOM_EMPTY | BUFFER_TOP_EMPTY;

    //
    // Indicate that the application is about to start playing.
    //
    g_ulFlags |= BUFFER_PLAYING;

    while(1)
    {
        //
        // Must disable I2S interrupts during this time to prevent state
        // problems.
        //
        IntDisable(INT_I2S0);

        //
        // If the refill flag gets cleared then fill the requested side of the
        // buffer.
        //
        if(g_ulFlags & BUFFER_BOTTOM_EMPTY)
        {
            //
            // Read out the next buffer worth of data.
            //
            usCount = WaveRead(psFileObject, pWaveHeader, g_pucBuffer);

            //
            // Start the playback for a new buffer.
            //
            SoundBufferPlay(g_pucBuffer, usCount, BufferCallback);

            //
            // Bottom half of the buffer is now not empty.
            //
            g_ulFlags &= ~BUFFER_BOTTOM_EMPTY;
        }

        if(g_ulFlags & BUFFER_TOP_EMPTY)
        {
            //
            // Read out the next buffer worth of data.
            //
            usCount = WaveRead(psFileObject, pWaveHeader,
                               &g_pucBuffer[AUDIO_BUFFER_SIZE >> 1]);

            //
            // Start the playback for a new buffer.
            //
            SoundBufferPlay(&g_pucBuffer[AUDIO_BUFFER_SIZE >> 1],
                            usCount, BufferCallback);

            //
            // Top half of the buffer is now not empty.
            //
            g_ulFlags &= ~BUFFER_TOP_EMPTY;

            //
            // Update the current time display.
            //
            //DisplayTime(0);
        }

        //
        // If something reset this while playing then stop playing and break
        // out of the loop.
        //
        if((g_ulFlags & BUFFER_PLAYING) == 0)
        {
            //
            // Change the text to indicate that the button is now for play.
            //
            //strcpy(g_psPlayText, "Play");
            //WidgetPaint((tWidget *)&g_sPlayBtn);

            //
            // Update the new file information if necessary.
            //
            //UpdateFileInfo();

            break;
        }

        //
        // Audio playback is done once the count is below a full buffer.
        //
        if((usCount < g_ulMaxBufferSize) || (g_ulBytesRemaining == 0))
        {
            //
            // Change the text to indicate that the button is now for play.
            //
            //strcpy(g_psPlayText, "Play");
            //WidgetPaint((tWidget *)&g_sPlayBtn);

            //
            // No longer playing audio.
            //
            g_ulFlags &= ~BUFFER_PLAYING;

            //
            // Wait for the buffer to empty.
            //
            while(g_ulFlags != (BUFFER_TOP_EMPTY | BUFFER_BOTTOM_EMPTY))
            {
            }

            //
            // Update the real display time.
            //
            //DisplayTime(1);

            break;
        }

        //
        // Must disable I2S interrupts during this time to prevent state
        // problems.
        //
        IntEnable(INT_I2S0);

        //
        // Process any messages in the widget message queue.
        //
        //WidgetMessageQueueProcess();
    }

    //
    // Close out the file.
    //
    WaveClose(psFileObject);

    return(0);
}

void SysTickHandler(void)
{
    //
    // Call the FatFs tick timer.
    //
    disk_timerproc();
}

//-----------------------------------------------------------------------------

// ------------------------------- initial-------------------------------------

void graphicInit(){
	ROM_SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);
	PinoutSet();

	ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_UDMA);
    SysCtlDelay(10);
    ROM_uDMAControlBaseSet(&sDMAControlTable[0]);
    ROM_uDMAEnable();

	ROM_SysTickPeriodSet(SysCtlClockGet() / TICKS_PER_SECOND);
    ROM_SysTickEnable();
    ROM_SysTickIntEnable();

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

tPushButtonWidget g_sBooks;
tCanvasWidget g_details;

tPushButtonWidget g_sBus;
tPushButtonWidget g_schoolBus;

tCanvasWidget g_sEcard;
tCanvasWidget g_remaining;

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
RectangularButton(g_sBooks, &g_sBooksBackground, 0, 0,
	&g_sKitronix320x240x16_SSD2119, 0, 210, 50, 30,
	(PB_STYLE_OUTLINE | PB_STYLE_TEXT_OPAQUE |PB_STYLE_TEXT | PB_STYLE_FILL | PB_STYLE_RELEASE_NOTIFY),
	ClrTurquoise, 0, ClrWhite, ClrWhite, 
	&g_sFontCmss14,"Show", 0, 0,0,0,showBooksData);

Canvas(g_details, &g_sBooksBackground,0,0, 
	&g_sKitronix320x240x16_SSD2119, 0, 50, 320, 160,
	(CANVAS_STYLE_FILL | CANVAS_STYLE_OUTLINE |CANVAS_STYLE_TEXT),
	ClrTurquoise, ClrTurquoise, ClrWhite, 
	&g_sFontCmss18b,"The books details will be showed here", 0, 0);

Canvas(g_sBooksBackground, WIDGET_ROOT,0,0, 
	&g_sKitronix320x240x16_SSD2119, 0, 50, 320, (240-50),
	CANVAS_STYLE_FILL, 
	ClrWhite, 0, 0, 
	0, 0, 0, 0);

//--------------------------------------bus query---------------------------------------------
RectangularButton(g_sBus, &g_sBusBackground, 0, 0,
	&g_sKitronix320x240x16_SSD2119, 0, 210, 40, 30,
	(PB_STYLE_OUTLINE | PB_STYLE_TEXT_OPAQUE |PB_STYLE_TEXT | PB_STYLE_FILL | PB_STYLE_RELEASE_NOTIFY),
	ClrTurquoise, 0, ClrWhite, ClrWhite, 
	&g_sFontCmss14,"Show", 0, 0,0,0,showMinhang2xuhui);

RectangularButton(g_schoolBus, &g_sBusBackground, 0, 0,
	&g_sKitronix320x240x16_SSD2119, 280, 210, 40, 30,
	(PB_STYLE_OUTLINE | PB_STYLE_TEXT_OPAQUE |PB_STYLE_TEXT | PB_STYLE_FILL | PB_STYLE_RELEASE_NOTIFY),
	ClrTurquoise, 0, ClrWhite, ClrWhite, 
	&g_sFontCmss14,"Photo", 0, 0,0,0,schoolBusPicture);

Canvas(g_sBusBackground, WIDGET_ROOT,0,0, 
	&g_sKitronix320x240x16_SSD2119, 0, 50, 320, (240-50),
	CANVAS_STYLE_FILL, 
	ClrWhite, 0, 0, 
	0, 0, 0, 0);

//-----------------------------------------ecard query----------------------------------------
Canvas(g_sEcard, &g_sEcardBackground, 0, 0,
	&g_sKitronix320x240x16_SSD2119, 0, 70, 320, 50,
	(CANVAS_STYLE_FILL | CANVAS_STYLE_OUTLINE |CANVAS_STYLE_TEXT),
	ClrTurquoise, ClrTurquoise, ClrWhite, 
	&g_sFontCmss20b,"remaining: 6.30 RMB", 0, 0);

Canvas(g_remaining, &g_sEcardBackground, 0, 0,
	&g_sKitronix320x240x16_SSD2119, 0, 50, 320, 20,
	(CANVAS_STYLE_FILL | CANVAS_STYLE_OUTLINE |CANVAS_STYLE_TEXT),
	ClrWhite, ClrWhite, ClrBlack, 
	&g_sFontCmss18,"The remaining of your ecard is: ", 0, 0);

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
	sprintf(NixieTube,data);
	I2C0DeviceRefresh();
}

void semesterChoose(tWidget *pWidget, unsigned long selected){
	char *data;
	int i = 0;
	if (pWidget == (tWidget *) (&g_two1) && c1 == 0){
		i = 0;
		c1 = 1;
		c2 = 0;
		c3 = 0;
		c4 = 0;
		UARTStringPut(UART0_BASE,"2014-2015-1\n");
		while (1){
			UARTStringGet(data, UART0_BASE);
			if (*data == '#') break;
			GrContextForegroundSet(&sContext, ClrBlack);
			GrContextFontSet(&sContext, &g_sFontCmss12);
			GrStringDraw(&sContext, data, -1, 0, 70 + i * 13, false);
			GrFlush(&sContext);
			++i;
		}
	} 
	else if (pWidget == (tWidget *) (&g_two2) && c2 == 0){
		i = 0;
		c1 = 0;
		c2 = 1;
		c3 = 0;
		c4 = 0;
		UARTStringPut(UART0_BASE,"2014-2015-2\n");																		 	
		while (1){
			UARTStringGet(data, UART0_BASE);
			if (*data == '#') break;
			GrContextForegroundSet(&sContext, ClrBlack);
			GrContextFontSet(&sContext, &g_sFontCmss12);
			GrStringDraw(&sContext, data, -1, 0, 70 + i * 13, false);
			GrFlush(&sContext);
			++i;
		}
	}
	else if (pWidget == (tWidget *) (&g_one1) && c3 == 0){
		i = 0;
		c1 = 0;
		c2 = 0;
		c3 = 1;
		c4 = 0;
		UARTStringPut(UART0_BASE,"2013-2014-1\n");
		while (1){
			UARTStringGet(data, UART0_BASE);
			if (*data == '#') break;
			GrContextForegroundSet(&sContext, ClrBlack);
			GrContextFontSet(&sContext, &g_sFontCmss12);
			GrStringDraw(&sContext, data, -1, 0, 70 + i * 13, false);
			GrFlush(&sContext);
			++i;
		}
	}
	else if (pWidget == (tWidget *) (&g_one2) && c4 == 0){
		i = 0;
		c1 = 0;
		c2 = 0;
		c3 = 0;
		c4 = 1;
		UARTStringPut(UART0_BASE,"2013-2014-2\n");
		while (1){
			UARTStringGet(data, UART0_BASE);
			if (*data == '#') break;
			GrContextForegroundSet(&sContext, ClrBlack);
			GrContextFontSet(&sContext, &g_sFontCmss12);
			GrStringDraw(&sContext, data, -1, 0, 70 + i * 13, false);
			GrFlush(&sContext);
			++i;
		}
	}
}

//--------------------------------------------------------------------------------------

void booksQueryButtonClick(tWidget *pWidget){
	WidgetAdd(WIDGET_ROOT, (tWidget *)&g_sBooksBackground);
	WidgetAdd(WIDGET_ROOT, (tWidget *)&g_sBooks);
	WidgetAdd(WIDGET_ROOT, (tWidget *)&g_details);
	WidgetRemove((tWidget *)&g_sScoreQuery);
	WidgetRemove((tWidget *)&g_sBooksQuery);
	WidgetRemove((tWidget *)&g_sBusQuery);
	WidgetRemove((tWidget *)&g_sEcardQuery);
	WidgetRemove((tWidget *)&g_sQueryBackground);
    WidgetPaint(WIDGET_ROOT);
	UARTStringPut(UART0_BASE,"booksQuery\n");
}

void showBooksData(tWidget *pWidget){
	char *data;
	int i = 0;
	UARTStringPut(UART0_BASE,"show\n");
	while (1){
		UARTStringGet(data, UART0_BASE);
		if (*data == '#') break;
		UARTStringPut(UART0_BASE, data);
		GrContextForegroundSet(&sContext, ClrBlack);
		GrContextFontSet(&sContext, &g_sFontCmss14);
		GrStringDraw(&sContext, "I am sorry about that this function has a terrible bug", -1, 0, 50 + i * 15, false);
		GrFlush(&sContext);
		++i;
	}
}

//--------------------------------------------------------------------------------------
void busQueryButtonClick(tWidget *pWidget){
	WidgetAdd(WIDGET_ROOT, (tWidget *)&g_sBusBackground);
	WidgetAdd(WIDGET_ROOT, (tWidget *)&g_sBus);
	WidgetAdd(WIDGET_ROOT, (tWidget *)&g_schoolBus);
	WidgetRemove((tWidget *)&g_sScoreQuery);
	WidgetRemove((tWidget *)&g_sBooksQuery);
	WidgetRemove((tWidget *)&g_sBusQuery);
	WidgetRemove((tWidget *)&g_sEcardQuery);
	WidgetRemove((tWidget *)&g_sQueryBackground);
    WidgetPaint(WIDGET_ROOT);
	UARTStringPut(UART0_BASE,"busQuery\n");
}
void showMinhang2xuhui(tWidget *pWidget){
	UARTStringPut(UART0_BASE,"minhang2xuhui\n");
	GrContextForegroundSet(&sContext, ClrBlack);
	GrContextFontSet(&sContext, &g_sFontCmss12b);
	GrStringDraw(&sContext, "Minhang To Xuhui / Weekday", -1, 0, 50, false);
	GrContextFontSet(&sContext, &g_sFontCmss12);
	GrStringDraw(&sContext, "06:40 direct  08:00 direct  10:10 undirect  12:15 undirect", -1, 0, 60, false);
	GrStringDraw(&sContext, "14:10 direct  16:00 direct  17:00 undirect  18:30 direct",   -1, 0, 70, false);
	GrStringDraw(&sContext, "20:40 direct",                                               -1, 0, 80, false);
	GrContextFontSet(&sContext, &g_sFontCmss12b);
	GrStringDraw(&sContext, "Minhang To Xuhui / Weekend", -1, 0, 100, false);
	GrContextFontSet(&sContext, &g_sFontCmss12);
	GrStringDraw(&sContext, "07:30 direct  12:30 direct  16:30 direct", -1, 0, 110, false);
	GrContextFontSet(&sContext, &g_sFontCmss12b);
	GrStringDraw(&sContext, "Xuhui To Minhang / Weekday", -1, 0, 130, false);
	GrContextFontSet(&sContext, &g_sFontCmss12);
	GrStringDraw(&sContext, "06:40 undirect  07:00 undirect  07:10 undirect  07:30 undirect", -1, 0, 140, false);
	GrStringDraw(&sContext, "09:00 direct    10:10 direct    12:00 undirect  13:00 direct",   -1, 0, 150, false);
	GrStringDraw(&sContext, "15:00 direct    17:00 direct    18:00 direct    20:00 direct",   -1, 0, 160, false);
	GrStringDraw(&sContext, "21:30 direct",   -1, 0, 170, false);
	GrContextFontSet(&sContext, &g_sFontCmss12b);
	GrStringDraw(&sContext, "Xuhui To Minhang / Weekend", -1, 0, 190, false);
	GrContextFontSet(&sContext, &g_sFontCmss12);
	GrStringDraw(&sContext, "08:30 direct  13:30 direct  17:30 direct", -1, 0, 200, false);
	GrFlush(&sContext);
}

void schoolBusPicture(tWidget *pWidget){
	UARTStringPut(UART0_BASE,"schoolBus\n");
	GrImageDraw(&sContext, g_pucImage,0, 50);
	GrFlush(&sContext);
}

//---------------------------------------------------------------------------------------
void ecardQueryButtonClick(tWidget *pWidget){
	char *data;
	UARTStringPut(UART0_BASE,"ecardQuery\n");
	WidgetAdd(WIDGET_ROOT, (tWidget *)&g_sEcardBackground);
	WidgetAdd(WIDGET_ROOT, (tWidget *)&g_sEcard);
	WidgetAdd(WIDGET_ROOT, (tWidget *)&g_remaining);
	WidgetRemove((tWidget *)&g_sScoreQuery);
	WidgetRemove((tWidget *)&g_sBooksQuery);
	WidgetRemove((tWidget *)&g_sBusQuery);
	WidgetRemove((tWidget *)&g_sEcardQuery);
	WidgetRemove((tWidget *)&g_sQueryBackground);
 
	UARTStringGet(data, UART0_BASE);
	CanvasTextSet(&g_sEcard, data);
	WidgetPaint(WIDGET_ROOT); 
	sprintf(NixieTube,data);
	I2C0DeviceRefresh();	
}
//----------------------------------------------------------------------------------------------
																	   
int main(void)					
{
	FRESULT fresult;

    graphicInit();
	ClockInitial();
	GPIOInitial();
	SysTickInitial();
	UART0Initial();
	I2C0MasterInitial();
	UARTStringPut(UART0_BASE,"Initial Done\n");
	sprintf(NixieTube,"CROS");

	SoundInit(0);
	SoundVolumeSet(INITIAL_VOLUME_PERCENT);
	WidgetAdd(WIDGET_ROOT, (tWidget *)&g_sHeading);
	WidgetAdd(WIDGET_ROOT, (tWidget *)&g_sScoreQuery);
	WidgetAdd(WIDGET_ROOT, (tWidget *)&g_sBooksQuery);
	WidgetAdd(WIDGET_ROOT, (tWidget *)&g_sBusQuery);
	WidgetAdd(WIDGET_ROOT, (tWidget *)&g_sEcardQuery);
	WidgetPaint(WIDGET_ROOT);

	fresult = f_mount(0, &g_sFatFs);
    if(fresult != FR_OK)
    {
        return(1);
    }

	if (WaveOpen(&g_sFileObject, "test.wav",&g_sWaveHeader) == FR_OK){
		WavePlay(&g_sFileObject, &g_sWaveHeader);
	}

	while(1){
		I2C0DeviceRefresh();
		WidgetMessageQueueProcess();
	};	   
}
