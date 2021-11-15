
#ifndef SAMPLE_H
#define SAMPLE_H

#include "daisy_core.h"
#include "util/wav_format.h"
#include "DisplayManager.h"

#define WAV_FILENAME_MAX 256

using namespace daisy;

struct FileInfo
{
    WAV_FormatTypeDef raw_data;               /**< Raw wav data */
    char              name[WAV_FILENAME_MAX]; /**< Wav filename */
};

class Sample
{
public:
    enum EPlayMode {
        Trigger,
        Gate,
        OneShot
    };

public:
    void Stream(double speed);
    void Reset();

    void SetButtonState(bool state);
    bool IsPlaying() { return isPlaying; }

private:
    void SetIsPlaying(bool state);
    void TableRead(double index, const size_t tableLength);

public:
    bool previousButtonState = false;

    FileInfo fileInfo;

    EPlayMode playMode = Trigger;

    size_t sampleSize = 0;
    int16_t* sampleData;

    size_t chanCount = 1;

    float data[2] = {0,0};

private:
    double readPos = 0;
    bool isPlaying = false;
    bool isLooping = true;

    DisplayManager *display = DisplayManager::GetInstance();

};


#endif //SAMPLE_H