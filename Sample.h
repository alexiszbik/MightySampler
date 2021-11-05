
#ifndef SAMPLE_H
#define SAMPLE_H

#include "daisy_core.h"
#include "util/wav_format.h"

#define WAV_FILENAME_MAX \
    256 /**< Maximum LFN (set to same in FatFs (ffconf.h) */

/** @file hid_WavStream.h */

using namespace daisy;

struct FileInfo
{
    WAV_FormatTypeDef raw_data;               /**< Raw wav data */
    char              name[WAV_FILENAME_MAX]; /**< Wav filename */
};

class Sample
{
public:
    void Stream(double speed);
    void Reset();

    void Trigger(bool state);
    bool IsPlaying() { return isPlaying; }

private:
    void TableRead(double index, const size_t tableLength);

public:
    FileInfo fileInfo;

    size_t sampleSize = 0;
    int16_t* sampleData;

    size_t chanCount = 1;

    float data[2] = {0,0};

private:
    double readPos = 0;
    bool isPlaying = true;
    bool isLooping = true;

};


#endif //SAMPLE_H