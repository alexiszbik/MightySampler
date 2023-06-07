
#pragma once
#ifndef DSY_WAVSTREAM_H
#define DSY_WAVSTREAM_H /**< Macro */
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

    float data[2];

    Sample sample[SPLR_COUNT];

  private:
    size_t                  readHead = 0;

    Patch patch;

    DisplayManager *display = DisplayManager::GetInstance();

    FatFSInterface fsi;
    FIL  SDFile;

};

} // namespace daisy

#endif
