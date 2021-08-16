#include <string.h>
#include "WavStream.h"
#include "fatfs.h"
#include "daisy.h"

DSY_SDRAM_BSS int16_t bigBuff[44100*60];

using namespace daisy;

void WavStream::Init()
{
    // First check for all .wav files, and add them to the list until its full or there are no more.
    // Only checks '/'
    FRESULT result = FR_OK;
    FILINFO fno;
    DIR     dir;
    char *  fn;
    file_sel_ = 0;
    sampleCount = 0;
    // Init Fatfs
    dsy_fatfs_init();
    // Mount SD Card
    f_mount(&SDFatFS, SDPath, 1);
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
    readPos = 0;
}

size_t WavStream::GetChannelCount() {
    return sample[file_sel_].fileInfo.raw_data.NbrChannels;
}

int WavStream::Open(size_t sel)
{
    if(sel != file_sel_)
    {
        f_close(&SDFile);
        file_sel_ = sel < sampleCount ? sel : sampleCount - 1;
    }
    // Set Buffer Position
    f_open(&SDFile, sample[file_sel_].fileInfo.name, (FA_OPEN_EXISTING | FA_READ));
    
    f_lseek(&SDFile,
            sizeof(WAV_FormatTypeDef)
                + sample[file_sel_].fileInfo.raw_data.SubChunk1Size);

    UINT bytesread = 0;
    fileSize = 0;

    while(f_eof(&SDFile) == 0) {
        UINT sizeToRead = 44100 * 2 * sizeof(bigBuff[0]);
        f_read(&SDFile, &bigBuff[fileSize], sizeToRead, &bytesread);
        fileSize += bytesread / 2;
    }

    fileSize = fileSize / GetChannelCount();

    return 0;
}

int WavStream::Close()
{
    return f_close(&SDFile);
}

//Table read
void WavStream::TableRead(double index, const size_t tableLength) {

    const double p = index;
    const double q = floorf(p);
    const double r = p - q;

    int nextIndex = q + 1;
    if (nextIndex >= (int)tableLength) {
        nextIndex = 0;
    } else if (nextIndex < 0) {
        nextIndex = (tableLength - 1);
    }

    size_t chanCount = GetChannelCount();
    //There is something to do about reading it wrong
    for (size_t channel = 0; channel < chanCount; channel++) {
        data[channel] = (1.0 - r) * s162f(bigBuff[((int)q) * chanCount + channel]) + (r * s162f(bigBuff[nextIndex * chanCount + channel]));
    }
}


void WavStream::Stream(double speed)
{
    TableRead(readPos, fileSize);

    readPos += speed;

    if (speed > 0) {
        if (readPos >= fileSize) {
            readPos = readPos - trunc(readPos);
        }
    } else {
        if (readPos < 0) {
            readPos += (fileSize - 1);
        }
    }
}
