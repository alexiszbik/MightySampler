#pragma once

#include "daisy_core.h"
#include "util/wav_format.h"
#include "DaisyYMNK/DisplayManager.h"

#include "Patch.h"
#include "SamplerParameters.h"

#define WAV_FILENAME_MAX 256

using namespace daisy;

struct FileInfo
{
    WAV_FormatTypeDef raw_data;               /**< Raw wav data */
    char              name[WAV_FILENAME_MAX]; /**< Wav filename */
};

struct SampleData
{
    size_t sampleSize = 0;
    int16_t* sampleData;
    uint8_t sampleChanCount = 1;
    double sampleRate = 44100;
};

class Sample
{
public:
    Sample();

public:
    void Init(double playingSampleRate);

    void Stream();
    void Reset();

    void SetState(bool state, bool fromMidi = false);
    bool IsPlaying() { return isPlaying; }

    const char* getName();
    float getPositionRatio();

private:
    void SetIsPlaying(bool state);
    void TableRead(double index, const size_t tableLength);

public:
    static constexpr uint8_t dataChanCount = 2;

    SamplerParameterList  parameters;
    SampleDesc* desc = nullptr;

    FileInfo fileInfo;

    SampleData sampleData;
    
    double playingSampleRate = 48000;
    
    float data[dataChanCount] = {0,0};

private:
    bool previousButtonState = false;

    double readPos = 0;
    bool isPlaying = false;
    bool isLooping = true;
};

