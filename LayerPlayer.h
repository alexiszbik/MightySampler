#pragma once

#include "daisy_core.h"
#include "util/wav_format.h"
#include "DaisyYMNK/DaisyYMNK.h"

#include "Patch.h"
#include "SamplerParameters.h"

#define WAV_FILENAME_MAX 256

using namespace daisy;

class LayerPlayer
{
public:
    LayerPlayer(LayerData* layerData, Patch* patch);

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

    double playingSampleRate = 48000;
    
    float data[dataChanCount] = {0,0};

private:
    LayerData* layerData = nullptr;
    SampleData* sampleData = nullptr;
    SampleDesc* desc = nullptr;
    
    bool previousButtonState = false;

    double readPos = 0;
    bool isPlaying = false;
    bool isLooping = true;

    double playSpeed;
};

