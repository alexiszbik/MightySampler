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
    const char* Init();

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
    inline size_t GetCurrentFile() const { return file_sel_; }

    void Trigger(size_t sampleId, bool state);

    size_t GetChannelCount();

    void CheckPlaying();

    static constexpr size_t kMaxFiles   = 8;
    float data[2];
    bool isPlaying[kMaxFiles];

  private:
    Sample                  sample[kMaxFiles];

    size_t                  sampleCount, file_sel_;
    
    size_t                  fileSize = 0;

};

} // namespace daisy

#endif
