#include <string.h>
#include "WavStream.h"
#include "daisy.h"

//#include "oled_display.h"

DSY_SDRAM_BSS int16_t bigBuff[44100*120];

using namespace daisy;

WavStream::WavStream() {
}

void WavStream::Init()
{
    // First check for all .wav files, and add them to the list until its full or there are no more.
    // Only checks '/'
    FRESULT result = FR_OK;
    FILINFO fno;
    DIR     dir;
    char *  fn;
    sampleCount = 0;
    // Init Fatfs
    dsy_fatfs_init();
    // Mount SD Card
   
    f_mount(&SDFatFS, SDPath, 1);

    //return "post fmount";
    // Open Dir and scan for files.
    if(f_opendir(&dir, SDPath) != FR_OK)
    {
        return;
    }
    do
    {
        result = f_readdir(&dir, &fno);
        // Exit if bad read or NULL fname
        if(result != FR_OK || fno.fname[0] == 0)
            break;
        // Skip if its a directory or a hidden file.
        if(fno.fattrib & (AM_HID | AM_DIR))
            continue;
        // Now we'll check if its .wav and add to the list.
        fn = fno.fname;
        if(sampleCount < kMaxFiles - 1)
        {
            if(strstr(fn, ".wav") || strstr(fn, ".WAV"))
            {
                strcpy(sample[sampleCount].fileInfo.name, fn);
                sampleCount++;
                // For now lets break anyway to test.
                //                break;
            }
        }
        else
        {
            break;
        }
    } while(result == FR_OK);
    f_closedir(&dir);
    
    // Now we'll go through each file and load the WavInfo.
    for(size_t i = 0; i < sampleCount; i++)
    {
        size_t bytesread;
        if(f_open(&SDFile, sample[i].fileInfo.name, (FA_OPEN_EXISTING | FA_READ))
           == FR_OK)
        {
            // Populate the WAV Info
            if(f_read(&SDFile,
                      (void *)&sample[i].fileInfo.raw_data,
                      sizeof(WAV_FormatTypeDef),
                      &bytesread)
               != FR_OK)
            {
                // Maybe add return type
                return;
            }
            
            f_close(&SDFile);
        }
    }

    Open(0);
    Open(1);
}

size_t WavStream::GetChannelCount() {
    return sample[0].fileInfo.raw_data.NbrChannels;
}

int WavStream::Open(size_t sel)
{
    display->Write("loading", sample[sel].fileInfo.name);

    f_open(&SDFile, sample[sel].fileInfo.name, (FA_OPEN_EXISTING | FA_READ));
    
    f_lseek(&SDFile,
            sizeof(WAV_FormatTypeDef)
                + sample[sel].fileInfo.raw_data.SubChunk1Size);

    UINT bytesread = 0;
    size_t fileSize = 0;

    size_t chanCount = sample[sel].fileInfo.raw_data.NbrChannels;

    while(f_eof(&SDFile) == 0) {
        UINT sizeToRead = 44100 * 2 * sizeof(bigBuff[0]);
        f_read(&SDFile, &bigBuff[fileSize + readHead], sizeToRead, &bytesread);
        fileSize += bytesread / 2;
    }

    sample[sel].sampleData = &bigBuff[readHead];

    readHead += fileSize;

    fileSize = fileSize / chanCount;
    
    sample[sel].sampleSize = fileSize;
    sample[sel].chanCount = chanCount;
    sample[sel].Reset();

    f_close(&SDFile);

    return 0;
}

int WavStream::Close()
{
    return f_close(&SDFile);
}

void WavStream::Stream(double speed)
{
    sample[0].Stream(speed);
    sample[1].Stream(speed);
    
    data[0] = sample[0].data[0] + sample[1].data[0];
    data[1] = sample[0].data[1] + sample[1].data[1];
}