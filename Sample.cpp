#include "Sample.h"
#include "daisy.h"

void Sample::Reset() {
    readPos = 0;
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
    TableRead(readPos, sampleSize);

    readPos += speed;

    if (speed > 0) {
        if (readPos >= sampleSize) {
            readPos = readPos - trunc(readPos);
        }
    } else {
        if (readPos < 0) {
            readPos += (sampleSize - 1);
        }
    }
}