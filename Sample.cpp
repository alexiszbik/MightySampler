#include "Sample.h"
#include "daisy.h"

void Sample::Reset() {
    isPlaying = false;
    readPos = 0;
}

void Sample::SetIsPlaying(bool state) {
    if (state && !isPlaying) {
        readPos = 0;
    }
    isPlaying = state;
}

void Sample::SetButtonState(bool state) {

    if (state != previousButtonState) {
        if (desc->playMode == Trigger) {
            if (state) {
                SetIsPlaying(!IsPlaying());
            }
        } else if (desc->playMode == Gate) {
            SetIsPlaying(state);
        }
    }

    previousButtonState = state;
}

//Table read
void Sample::TableRead(double index, const size_t tableLength) {

    const double p = index;
    const double q = floorf(p);
    const double r = p - q;

    int nextIndex = q + 1;
    if (nextIndex >= (int)tableLength) {
        nextIndex = 0;
    } else if (nextIndex < 0) {
        nextIndex = (tableLength - 1);
    }

    //There is something to do about reading it wrong
    for (size_t channel = 0; channel < chanCount; channel++) {
        data[channel] = (1.0 - r) * s162f(sampleData[((int)q) * chanCount + channel]) + (r * s162f(sampleData[nextIndex * chanCount + channel]));
    }
}

void Sample::Stream(double speed)
{
    double srSpeed = sampleRate/playingSampleRate;
    speed = speed * srSpeed;

    if (!isPlaying) {
        data[0] = 0;
        data[1] = 0;
        
    } else {

        TableRead(readPos, sampleSize);

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