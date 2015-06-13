#include <string.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_ints.h"
#include "driverlib/flash.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/rom.h"
#include "driverlib/sysctl.h"
#include "driverlib/udma.h"
#include "driverlib/systick.h"
#include "driverlib/i2s.h"
#include "utils/ustdlib.h"
#include "third_party/fatfs/src/ff.h"
#include "third_party/fatfs/src/diskio.h"
#include "boards/dk-lm3s9d96/drivers/sound.h"

//******************************************************************************
//
// The DMA control structure table.
//
//******************************************************************************
#ifdef ewarm
#pragma data_alignment=1024
tDMAControlTable sDMAControlTable[64];
#elif defined(ccs)
#pragma DATA_ALIGN(sDMAControlTable, 1024)
tDMAControlTable sDMAControlTable[64];
#else
tDMAControlTable sDMAControlTable[64] __attribute__ ((aligned(1024)));
#endif

//******************************************************************************
//
// The following are data structures used by FatFs.
//
//******************************************************************************
static FATFS g_sFatFs;
static DIR g_sDirObject;
static FILINFO g_sFileInfo;
static FIL g_sFileObject;

//******************************************************************************
//
// The number of SysTick ticks per second.
//
//******************************************************************************
#define TICKS_PER_SECOND 100

//******************************************************************************
//
// Storage for the filename listbox widget string table.
//
//******************************************************************************
#define NUM_LIST_STRINGS 48
const char *g_ppcDirListStrings[NUM_LIST_STRINGS];

//******************************************************************************
//
// Storage for the names of the files in the current directory.  Filenames
// are stored in format "filename.ext".
//
//******************************************************************************
#define MAX_FILENAME_STRING_LEN (8 + 1 + 3 + 1)
char g_pcFilenames[NUM_LIST_STRINGS][MAX_FILENAME_STRING_LEN];

//******************************************************************************
//
// State information for keep track of time.
//
//******************************************************************************
static unsigned long g_ulBytesPlayed;
static unsigned long g_ulNextUpdate;

//******************************************************************************
//
// Buffer management and flags.
//
//******************************************************************************
#define AUDIO_BUFFER_SIZE       4096
static unsigned char g_pucBuffer[AUDIO_BUFFER_SIZE];
unsigned long g_ulMaxBufferSize;

//
// Flags used in the g_ulFlags global variable.
//
#define BUFFER_BOTTOM_EMPTY     0x00000001
#define BUFFER_TOP_EMPTY        0x00000002
#define BUFFER_PLAYING          0x00000004
static volatile unsigned long g_ulFlags;

//
// Globals used to track playback position.
//
static unsigned long g_ulBytesRemaining;
static unsigned short g_usMinutes;
static unsigned short g_usSeconds;

//******************************************************************************
//
// Handler for bufffers being released.
//
//******************************************************************************

void BufferCallback(void *pvBuffer, unsigned long ulEvent);
#define RIFF_CHUNK_ID_RIFF      0x46464952
#define RIFF_CHUNK_ID_FMT       0x20746d66
#define RIFF_CHUNK_ID_DATA      0x61746164

#define RIFF_TAG_WAVE           0x45564157

#define RIFF_FORMAT_UNKNOWN     0x0000
#define RIFF_FORMAT_PCM         0x0001
#define RIFF_FORMAT_MSADPCM     0x0002
#define RIFF_FORMAT_IMAADPCM    0x0011

//******************************************************************************
//
// The wav file header information.
//
//******************************************************************************
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

FRESULT WaveOpen(FIL *psFileObject, const char *pcFileName, tWaveHeader *pWaveHeader);
void WaveClose(FIL *psFileObject);
void Convert8Bit(unsigned char *pucBuffer, unsigned long ulSize);
void WaveStop(void);
unsigned short WaveRead(FIL *psFileObject, tWaveHeader *pWaveHeader, unsigned char *pucBuffer);
unsigned long WavePlay(FIL *psFileObject, tWaveHeader *pWaveHeader);
void SysTickHandler(void);
