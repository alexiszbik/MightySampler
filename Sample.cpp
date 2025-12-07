#include "Sample.h"
#include "daisy.h"

Sample::Sample() {
}

void Sample::Init(double playingSampleRate) {
    this->playingSampleRate = playingSampleRate;
}

void Sample::Reset() {
    isPlaying = false;
    readPos = 0;
}

const char* Sample::getName() {
    return desc->sampleName;
}

float Sample::getPositionRatio() {
    return isPlaying ? (float)readPos/(float)sampleData.sampleSize : 0;
}

void Sample::SetIsPlaying(bool state) {
    if (state && !isPlaying) {
        readPos = 0;
    }
    isPlaying = state;
}

void Sample::SetState(bool state, bool fromMidi) {
    if (state == previousButtonState)
        return;

    switch (desc->playMode) {
        case Trigger:
            if (fromMidi) {
                SetIsPlaying(state);
            } else if (state) {
                SetIsPlaying(!IsPlaying());
            }
            break;

        case Gate:
            SetIsPlaying(state);
            break;

        case OneShot:
            if (state) {
                isPlaying = false;
                isLooping = false;
                SetIsPlaying(true);
            }
            break;
    }

    previousButtonState = state;
}

//Table read
void Sample::TableRead(double index, const size_t tableLength) {

    const double p = index;
    const double q = floor(p);
    const double r = p - q;

    int nextIndex = q + 1;
    if (nextIndex >= (int)tableLength) {
        nextIndex = 0;
    } else if (nextIndex < 0) {
        nextIndex = (tableLength - 1);
    }

    for (uint8_t channel = 0; channel < dataChanCount; channel++) {
        uint8_t channelStride = channel % sampleData.sampleChanCount;
        data[channel] = (1.0 - r) * s162f(sampleData.sampleData[((int)q) * sampleData.sampleChanCount + channelStride]) + (r * s162f(sampleData.sampleData[nextIndex * sampleData.sampleChanCount + channelStride]));
        
        //LOFI reading, for later
        //data[channel] = s162f(sampleData[((int)q) * chanCount + channelStride]);
    }
}

void Sample::Stream()
{
    double srSpeed = sampleData.sampleRate/playingSampleRate;
    double speed = (double)parameters.at(Speed).value * srSpeed;

    float volume = parameters.at(Volume).value;
    volume*=volume;

    if (!isPlaying) {
        for (uint8_t c = 0; c < dataChanCount; c++) {
            data[c] = 0;
        } 
    } else {

        TableRead(readPos, sampleData.sampleSize);

        for (uint8_t c = 0; c < dataChanCount; c++) {
            data[c] *= volume;
        }

        readPos += speed;

        if (speed > 0) {
            if (readPos >= sampleData.sampleSize) {
                if (isLooping) {
                    readPos = readPos - trunc(readPos);
                } else {
                    Reset();
                }
            }
        } else {
            if (readPos < 0) {
                if (isLooping) {
                    readPos += (sampleData.sampleSize - 1);
                } else {
                    Reset();
                }
            }
        }
    }
}