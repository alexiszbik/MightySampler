/* Current Limitations:
- 1x Playback speed only
- 16-bit, mono files only (otherwise fun weirdness can happen).
- Only 1 file playing back at a time.
- Not sure how this would interfere with trying to use the SDCard/FatFs outside of
this module. However, by using the extern'd SDFile, etc. I think that would break things.
*/
#pragma once
#ifndef DSY_WAVSTREAM_H
#define DSY_WAVSTREAM_H /**< Macro */
#include "daisy_core.h"
#include "util/wav_format.h"
#include "Sample.h"
#include "DisplayManager.h"

namespace daisy
{
/** Wav Player that will load .wav files from an SD Card,
and then provide a method of accessing the samples with
double-buffering. */
class WavStream
{
  public:
    WavStream();
    ~WavStream() {}

    /** Initializes the WavStream, loading up to max_files of wav files from an SD Card. */
    void Init();

    /** Opens the file at index sel for reading.
    \param sel File to open
     */
    int Open(size_t sel);

    /** Closes whatever file is currently open.
    \return &
     */
    int Close();

    /** \return The next sample if playing, otherwise returns 0 */
    void Stream(double speed = 1);
    
    inline size_t GetNumberFiles() const { return sampleCount; }

    static constexpr size_t kMaxFiles   = 8;
    float data[2];

    Sample                  sample[kMaxFiles];

  private:
    size_t                  sampleCount;
    
    size_t                  readHead = 0;

    Patch patch;

    DisplayManager *display = DisplayManager::GetInstance();

};

} // namespace daisy

#endif
