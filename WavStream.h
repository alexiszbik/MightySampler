#ifndef DSY_WAVSTREAM_H
#define DSY_WAVSTREAM_H 

#include "daisy_core.h"
#include "util/wav_format.h"
#include "Sample.h"
#include "DaisyYMNK/DisplayManager.h"

namespace daisy
{
class WavStream
{
  public:
    WavStream();
    ~WavStream() {}

    /** Initializes the WavStream, loading up to max_files of wav files from an SD Card. */
    void Init(double sampleRate);

    /** Opens the file at index sel for reading.
    \param sel File to open
     */
    int Open(size_t sel);

    /** Closes whatever file is currently open.
    \return &
     */
    int Close();

    /** \return The next sample if playing, otherwise returns 0 */
    void Stream();

    static constexpr uint8_t dataChanCount = 2;

    float data[dataChanCount];

    vector<Sample> samples;

  private:
    size_t readHead = 0;

    Patch patch;

    DisplayManager *display = DisplayManager::GetInstance();

    FatFSInterface fsi;
    FIL  SDFile;

    bool isInit = false;

};

} // namespace daisy

#endif
