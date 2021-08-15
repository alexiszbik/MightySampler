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

#define WAV_FILENAME_MAX \
    256 /**< Maximum LFN (set to same in FatFs (ffconf.h) */

/** @file hid_WavStream.h */

namespace daisy
{
// TODO: add bitrate, samplerate, length, etc.
/** Struct containing details of Wav File. */
struct FileInfo
{
    WAV_FormatTypeDef raw_data;               /**< Raw wav data */
    char              name[WAV_FILENAME_MAX]; /**< Wav filename */
};

/* 
TODO:
- Make template-y to reduce memory usage.
*/


/** Wav Player that will load .wav files from an SD Card,
and then provide a method of accessing the samples with
double-buffering. */
class WavStream
{
  public:
    WavStream() {}
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
    float Stream(double speed = 1);

    /** \return The number of files loaded by the WavStream */
    inline size_t GetNumberFiles() const { return file_cnt_; }

    /** \return currently selected file.*/
    inline size_t GetCurrentFile() const { return file_sel_; }

    size_t GetChannelCount(); 

  private:
    enum BufferState
    {
        BUFFER_STATE_IDLE,
        BUFFER_STATE_PREPARE_0,
        BUFFER_STATE_PREPARE_1,
    };

    BufferState GetNextBuffState();

    static constexpr size_t kMaxFiles   = 8;
    static constexpr size_t kBufferSize = 512;
    FileInfo             file_info_[kMaxFiles];
    size_t                  file_cnt_, file_sel_;
    BufferState             buff_state_;
    int16_t                 buff_[kBufferSize];
    double                  read_ptr_;
    bool                    looping_, playing_;
    size_t fileSize = 0;

};

} // namespace daisy

#endif
