#include "LayerPlayer.h"
#include "daisy.h"

LayerPlayer::LayerPlayer(LayerData* layerData, Patch* patch) {
    this->layerData = layerData;
    const uint8_t sampleId = layerData->sampleId;
    desc = &patch->sampleDescs.at(sampleId);
    sampleData = &desc->sampleData;
}

void LayerPlayer::Init(double playingSampleRate) {
    this->playingSampleRate = playingSampleRate;
    Reset();
}

void LayerPlayer::Reset() {
    isPlaying = false;
    if (layerData->isReverse) {
        readPos = desc->sampleData.sampleSize - 1;
    } else {
        readPos = 0;
    }
}

const char* LayerPlayer::getName() {
    return desc->sampleName;
}

float LayerPlayer::getPositionRatio() {
    return isPlaying ? (float)readPos/(float)sampleData->sampleSize : 0;
}

void LayerPlayer::SetIsPlaying(bool state) {
    if (state && !isPlaying) {
        Reset();
    }
    isPlaying = state;
}

void LayerPlayer::SetState(bool state, bool fromMidi) {
    if (state == previousButtonState)
        return;

    switch (layerData->playMode) {
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

void LayerPlayer::TableRead(double index, size_t tableLength)
{
    const int q = (int)index;   // do we need floor ? cause index >= 0
    const double r = index - q;

    const int nextIndex = (q + 1) % tableLength;
    const int sampleChanCount = sampleData->sampleChanCount;

    for (uint8_t channel = 0; channel < dataChanCount; ++channel)
    {
        const uint8_t channelStride = channel % sampleChanCount;

        const float a = s162f(sampleData->data[q * sampleChanCount + channelStride]);
        const float b = s162f(sampleData->data[nextIndex * sampleChanCount + channelStride]);

        data[channel] = (1.0 - r) * a + r * b;

        // LOFI reading, for later
        // data[channel] = s162f(sampleData[((int)q) * chanCount + channelStride]);
    }
}

void LayerPlayer::Stream()
{
    double srSpeed = sampleData->sampleRate/playingSampleRate;
    double speed = (double)parameters.at(Speed).value * srSpeed;

    if (layerData->isReverse) speed *= -1.;

    size_t sampleSize = sampleData->sampleSize;

    float volume = parameters.at(Volume).value;
    volume*=volume;

    if (!isPlaying) {
        for (uint8_t c = 0; c < dataChanCount; c++) {
            data[c] = 0;
        } 
    } else {

        TableRead(/*(layerData->isReverse ? sampleSize : 0) -*/readPos, sampleSize);

        for (uint8_t c = 0; c < dataChanCount; c++) {
            data[c] *= volume;
        }

        readPos += speed;

        if (speed > 0) {
            if (readPos >= sampleSize) {
                if (isLooping) {
                    readPos = readPos - trunc(readPos);
                } else {
                    Reset();
                }
            }
        } else {
            if (readPos < 0) {
                if (isLooping) {
                    readPos += (sampleSize - 1);
                } else {
                    Reset();
                }
            }
        }
    }
}