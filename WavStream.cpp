#include <string.h>
#include "WavStream.h"
#include "daisy.h"

#include "Patch.h"

DSY_SDRAM_BSS int16_t bigBuff[44100*60];

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
    display->WriteNow("Loading SD ...");
    f_mount(&SDFatFS, "/", 1);

    // Open Dir and scan for files.
    if(f_opendir(&dir, SDPath) != FR_OK)
    {
        display->WriteNow("Cannot open Dir ...");
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

        //Let's load the patch!
        if(strstr(fn, ".ymnk") || strstr(fn, ".YMNK"))
        {
            display->WriteNow(fn);
            patchReader.loadFile(fn, SDFile);        
        }
    } while(result == FR_OK);
    f_closedir(&dir);

    display->WriteNow("init", "layers");
    System::Delay(500);

    for (size_t i = 0; i < patch.layers.size(); i++) {
        layerPlayers.push_back(LayerPlayer(&patch.layers.at(i), &patch));
    }

    display->WriteNow("loading", "resources");
    System::Delay(500);
    
    // Now we'll go through each file and load the WavInfo.
    //TODO
    for(size_t i = 0; i < patch.sampleDescs.size(); i++)
    {
        display->WriteNow("open", patch.sampleDescs[i].sampleName);

         //System::Delay(500);
        

        if(f_open(&SDFile, patch.sampleDescs[i].sampleName, (FA_OPEN_EXISTING | FA_READ))
           == FR_OK)
        {
            f_close(&SDFile);
            Open(i);
        }
    }

    for (uint8_t i = 0; i < layerPlayers.size(); i++) {
        layerPlayers[i].Init(sampleRate);
    }

    isInit = true;
}

int WavStream::Open(size_t sel)
{
    SampleDesc* desc = &patch.sampleDescs[sel];
    display->WriteNow("loading", desc->sampleName);

    f_open(&SDFile, desc->sampleName, (FA_OPEN_EXISTING | FA_READ));

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
        display->WriteNow("unsupported", "bitrate");
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

    desc->sampleData.sampleSize = sample_ct;
    desc->sampleData.sampleChanCount = header.chan_ct;
    desc->sampleData.sampleRate = (double)header.sample_rate;

    if (header.format == 1) {   // PCM

        UINT bytesread = 0;
        size_t fileSize = 0;

        while(f_eof(&SDFile) == 0) {
            UINT sizeToRead = 128;
            f_read(&SDFile, &bigBuff[fileSize + readHead], sizeToRead, &bytesread);
            fileSize += bytesread / bytes_per_chan;
        }

        desc->sampleData.data = &bigBuff[readHead];

        readHead += fileSize;
    }

    f_close(&SDFile);

    return 0;
}
/*
int WavStream::Open(size_t sel)
{
    SampleDesc* desc = &patch.sampleDescs[sel];
    display->WriteNow("loading", desc->sampleName);

    if(f_open(&SDFile, desc->sampleName, (FA_OPEN_EXISTING | FA_READ)) != FR_OK)
        return -1;

    UINT br = 0;
    unsigned char buf4[4];
    unsigned char buf2[2];

    // ---- RIFF header ----
    f_read(&SDFile, buf4, 4, &br); // "RIFF"
    f_read(&SDFile, buf4, 4, &br); // RIFF size
    f_read(&SDFile, buf4, 4, &br); // "WAVE"

    uint16_t format = 0;
    uint16_t chan_ct = 0;
    uint32_t sample_rate = 0;
    uint16_t block_align = 0;
    uint16_t bits_per_sample = 0;
    uint32_t data_size = 0;

    // ---- Scan all chunks ----
    while(true)
    {
        char chunk_id[4] = {0};
        if(f_read(&SDFile, chunk_id, 4, &br) != FR_OK || br < 4) break;
        f_read(&SDFile, buf4, 4, &br);
        uint32_t chunk_size = end_buf4_to_int(buf4);

        if(memcmp(chunk_id, "fmt ", 4) == 0)
        {
            // fmt chunk
            f_read(&SDFile, buf2, 2, &br);
            format = end_buf2_to_int(buf2);

            f_read(&SDFile, buf2, 2, &br);
            chan_ct = end_buf2_to_int(buf2);

            f_read(&SDFile, buf4, 4, &br);
            sample_rate = end_buf4_to_int(buf4);

            f_read(&SDFile, buf4, 4, &br);
            uint32_t byte_rate = end_buf4_to_int(buf4);

            f_read(&SDFile, buf2, 2, &br);
            block_align = end_buf2_to_int(buf2);

            f_read(&SDFile, buf2, 2, &br);
            bits_per_sample = end_buf2_to_int(buf2);

            if(chunk_size > 16)
                f_lseek(&SDFile, f_tell(&SDFile) + (chunk_size - 16));
        }
        else if(memcmp(chunk_id, "data", 4) == 0)
        {
            data_size = chunk_size;
            break; // found data
        }
        else
        {
            // Skip any other chunk (smpl, LIST, cue , fact, etc.)
            uint32_t skip = chunk_size + (chunk_size & 1); // align to 2 bytes
            f_lseek(&SDFile, f_tell(&SDFile) + skip);
        }
    }

    if(data_size == 0)
    {
        display->WriteNow("no data", "chunk");
        f_close(&SDFile);
        return -1;
    }

    if(bits_per_sample != 8 && bits_per_sample != 16 && bits_per_sample != 32)
    {
        display->WriteNow("unsupported", "bit depth");
        f_close(&SDFile);
        return -1;
    }

    // ---- Fill sample desc ----
    uint32_t sample_ct = data_size / block_align;
    desc->sampleData.sampleSize      = sample_ct;
    desc->sampleData.sampleChanCount = chan_ct;
    desc->sampleData.sampleRate      = (double)sample_rate;

    // ---- Read PCM data ----
    if(format == 1) // PCM
    {
        UINT bytesread = 0;
        size_t framesRead = 0;
        const UINT bytesPerFrame = block_align;

        while(!f_eof(&SDFile))
        {
            UINT bytesToRead = 128 * bytesPerFrame; // read 128 frames at a time
            f_read(&SDFile,
                   (uint8_t*)&bigBuff[readHead + framesRead],
                   bytesToRead,
                   &bytesread);

            framesRead += bytesread / bytesPerFrame;
        }

        desc->sampleData.data = &bigBuff[readHead];
        readHead += framesRead;
    }

    f_close(&SDFile);
    return 0;
}
*/

int WavStream::Close()
{
    return f_close(&SDFile);
}

void WavStream::Stream()
{
    for (uint8_t c = 0; c < dataChanCount; c++) { 
        data[c] = 0;
    }  

    if (!isInit) {
        return;
    }
    
    for (size_t sampler = 0; sampler < layerPlayers.size(); sampler++) {

        layerPlayers[sampler].Stream();

        for (uint8_t c = 0; c < dataChanCount; c++) {        
            data[c] += layerPlayers[sampler].data[c];
        }
    }
    
}