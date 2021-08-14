// # MightySampler
// ## Description
// Fairly simply sample player.
// Loads 16
//
// Play .wav file from the SD Card.
//
#include <stdio.h>
#include <string.h>
#include "daisy_pod.h"
#include "WavStream.h"
//#include "daisy_patch.h"

using namespace daisy;

//DaisyPatch   hw;
DaisyPod     hw;
SdmmcHandler sdcard;
WavStream    sampler;

void AudioCallback(float *in, float *out, size_t size)
{
    for(size_t i = 0; i < size; i += 2)
    {
        if (sampler.GetChannelCount()== 1) {
            out[i] = sampler.Stream() * 0.5f;
            out[i + 1] = out[i];
        } else {
            out[i] = sampler.Stream() * 0.5f;
            out[i + 1] = sampler.Stream() * 0.5f;
        }
    }
}


int main(void)
{
    // Init hardware
    size_t blocksize = 48;
    hw.Init();
    //    hw.ClearLeds();
    SdmmcHandler::Config sd_cfg;
    sd_cfg.Defaults();
    sdcard.Init(sd_cfg);
    sampler.Init();

    // SET LED to indicate Looping status.
    //hw.SetLed(DaisyPatch::LED_2_B, sampler.GetLooping());

    hw.led1.Set(1, 0, 0);

    // Init Audio
    hw.SetAudioBlockSize(blocksize);
    hw.StartAudio(AudioCallback);
    // Loop forever...
    for(;;)
    {
        // Prepare buffers for sampler as needed
        //sampler.Prepare();

        hw.ProcessDigitalControls();

        // Change file with encoder.
        int inc = hw.encoder.Increment();
        if(inc > 0)
        {
            size_t curfile;
            curfile = sampler.GetCurrentFile();
            if(curfile < sampler.GetNumberFiles() - 1)
            {
                sampler.Open(curfile + 1);
            }
        }
        else if(inc < 0)
        {
            size_t curfile;
            curfile = sampler.GetCurrentFile();
            if(curfile > 0)
            {
                sampler.Open(curfile - 1);
            }
        }
    }
}
