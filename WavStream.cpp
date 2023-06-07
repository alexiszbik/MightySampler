#include <string.h>
#include "WavStream.h"
#include "daisy.h"

#include "Patch.h"

DSY_SDRAM_BSS int16_t bigBuff[44100*10];

using namespace daisy;

#define SDPath "/"

//Orignal .wav reading algorithm : https://gist.github.com/yurapyon/cc44f014491158d2bf2d71c90dd9bf70

struct header_wav {
    unsigned char riff[4];
    uint32_t size;
    unsigned char wave[4];
    unsigned char fmt[4];
    uint32_t fmtlen;
    uint16_t format;
    uint16_t chan_ct;
    uint32_t sample_rate;
    uint32_t byte_rate;
    uint16_t block_align;
    uint16_t bits_per_sample;
    unsigned char fact[4];
    uint32_t fact_size;
    uint32_t fact_num_samples;
    unsigned char data[4];
    uint32_t data_size;
};

#define end_buf2_to_int(_buf) ((_buf)[0] | ((_buf)[1] << 8))
#define end_buf4_to_int(_buf) ((_buf)[0] | ((_buf)[1] << 8) | ((_buf)[2] << 16) | ((_buf)[3] << 24))

WavStream::WavStream() {
    for (short i = 0; i < SPLR_COUNT; i++) {
        sample[i].desc = &patch.buttonDesc[i];
    }
}

void WavStream::Init(double sampleRate)
{
    // First check for all .wav files, and add them to the list until its full or there are no more.
    // Only checks '/'
    FRESULT result = FR_OK;
    FILINFO fno;
    DIR     dir;
    char *  fn;

    fsi.Init(FatFSInterface::Config::MEDIA_SD);

    // Get a reference to the SD card file system
    FATFS& SDFatFS = fsi.GetSDFileSystem();

    // Mount SD Card
    display->Write({"Loading SD ..."}, true);
    f_mount(&SDFatFS, "/", 1);

    // Open Dir and scan for files.
    if(f_opendir(&dir, SDPath) != FR_OK)
    {
        display->Write({"Cannot open Dir ..."}, true);
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

        fn = fno.fname;
        if(strstr(fn, ".ymnk") || strstr(fn, ".YMNK"))
        {
            display->Write({fn}, true);
            patch.loadFile(fn, SDFile);        
        }
    } while(result == FR_OK);
    f_closedir(&dir);
    
    // Now we'll go through each file and load the WavInfo.
    //TODO
    for(size_t i = 0; i < SPLR_COUNT; i++)
    {
        display->Write({"open", patch.buttonDesc[i].sampleName}, true);

        UINT bytesread;
        if(f_open(&SDFile, patch.buttonDesc[i].sampleName, (FA_OPEN_EXISTING | FA_READ))
           == FR_OK)
        {
            strcpy(sample[i].fileInfo.name, patch.buttonDesc[i].sampleName);

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

            Open(i);
        }
    }

    for (short i = 0; i < SPLR_COUNT; i++) {
        sample[i].Init(sampleRate);
    }
}

int WavStream::Open(size_t sel)
{
    display->Write({"loading", sample[sel].fileInfo.name}, true);

    f_open(&SDFile, sample[sel].fileInfo.name, (FA_OPEN_EXISTING | FA_READ));

    struct header_wav header;
    UINT br = 0;
    unsigned char buf4[4];
    unsigned char buf2[2];

    f_read(&SDFile, &header.riff, sizeof(header.riff), &br);

    f_read(&SDFile, &buf4, sizeof(buf4),  &br);
    header.size = end_buf4_to_int(buf4);

    f_read(&SDFile, &header.wave, sizeof(header.wave),  &br);

    f_read(&SDFile, &header.fmt, sizeof(header.fmt),  &br);

    f_read(&SDFile, &buf4, sizeof(buf4),  &br);
    header.fmtlen = end_buf4_to_int(buf4);

    f_read(&SDFile, &buf2, sizeof(buf2),  &br);
    header.format = end_buf2_to_int(buf2);

    f_read(&SDFile, &buf2, sizeof(buf2),  &br);
    header.chan_ct = end_buf2_to_int(buf2);

    f_read(&SDFile, &buf4, sizeof(buf4), &br);
    header.sample_rate = end_buf4_to_int(buf4);

    f_read(&SDFile, &buf4, sizeof(buf4), &br);
    header.byte_rate = end_buf4_to_int(buf4);

    f_read(&SDFile, &buf2, sizeof(buf2), &br);
    header.block_align = end_buf2_to_int(buf2);

    f_read(&SDFile, &buf2, sizeof(buf2), &br);
    header.bits_per_sample = end_buf2_to_int(buf2);

    if (header.bits_per_sample != 8
        && header.bits_per_sample != 16 
        && header.bits_per_sample != 32) {
        display->Write({"unsupported", "bitrate"}, true);
    }

    if (header.format == 1) {
        // do nothing
    } else if (header.format == 3) {
        if (header.fmtlen != 16) {
            //printf("unimplemented data stuff whateve ikdkkkdf\n");
        }

        f_read(&SDFile, &header.fact, sizeof(header.fact), &br);

        f_read(&SDFile, &buf4, sizeof(buf4), &br);
        header.fact_size = end_buf4_to_int(buf4);

        f_read(&SDFile, &buf4, sizeof(buf4), &br);
        header.fact_num_samples = end_buf4_to_int(buf4);

        // f*** whatever else give me the data

        unsigned char hack = 'x';
        while (hack != 'd')
            f_read(&SDFile, &hack, sizeof(hack), &br);

        f_lseek(&SDFile, -1);
    } else {
        //printf("header format tag unknown (#^# good bye\n");
    }

    f_read(&SDFile, &header.data, sizeof(header.data), &br);

    f_read(&SDFile, &buf4, sizeof(buf4), &br);
    header.data_size = end_buf4_to_int(buf4);

    uint32_t sample_ct = header.data_size / header.block_align ;
    int bytes_per_chan = header.block_align / header.chan_ct;

    sample[sel].sampleSize = sample_ct;
    sample[sel].chanCount = header.chan_ct;
    sample[sel].sampleRate = (double)header.sample_rate;
    sample[sel].Reset();

    if (header.format == 1) {   // PCM

        UINT bytesread = 0;
        size_t fileSize = 0;

        while(f_eof(&SDFile) == 0) {
            UINT sizeToRead = 128;
            f_read(&SDFile, &bigBuff[fileSize + readHead], sizeToRead, &bytesread);
            fileSize += bytesread / bytes_per_chan;
        }

        sample[sel].sampleData = &bigBuff[readHead];

        readHead += fileSize;
    }

    f_close(&SDFile);

    return 0;
}

int WavStream::Close()
{
    return f_close(&SDFile);
}

void WavStream::Stream()
{
    data[0] = 0;
    data[1] = 0;
    
    for (size_t sampler = 0; sampler < SPLR_COUNT; sampler++) {

        sample[sampler].Stream();

        for (size_t channel = 0; channel < 2; channel++) {        
            data[channel] += sample[sampler].data[channel];
        }
    }
    
}