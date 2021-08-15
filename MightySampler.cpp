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

Parameter p_knob1, p_knob2;

double speed = 1.0;

void AudioCallback(float *in, float *out, size_t size)
{
    for(size_t i = 0; i < size; i += 2)
    {
        sampler.Stream(speed);

        if (sampler.GetChannelCount()== 1) {
            out[i] = sampler.data[0] * 0.5f;
            out[i + 1] = out[i];
        } else {
            out[i] = sampler.data[0] * 0.5f;
            out[i + 1] = sampler.data[1] * 0.5f;
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

    p_knob1.Init(hw.knob1, 0, 1, Parameter::LINEAR);
    p_knob2.Init(hw.knob2, 0, 1, Parameter::LINEAR);

    hw.StartAdc();

    // Init Audio
    hw.SetAudioBlockSize(blocksize);
    hw.StartAudio(AudioCallback);
    // Loop forever...
    for(;;)
    {
        // Prepare buffers for sampler as needed
        //sampler.Prepare();

        hw.ProcessDigitalControls();

        speed = (p_knob1.Process() - 0.5) * 4.0;
        //speed = p_knob1.Process();

        // Change file with encoder.
        int inc = hw.encoder.Increment();
        if(inc > 0)
        {
            hw.StopAudio();
            hw.DelayMs(100);

            size_t curfile;
            curfile = sampler.GetCurrentFile();
            if(curfile < sampler.GetNumberFiles() - 1)
            {
                sampler.Open(curfile + 1);
            }
            hw.DelayMs(100);
            hw.StartAudio(AudioCallback);
        }
        else if(inc < 0)
        {
            hw.StopAudio();
            hw.DelayMs(100);

            size_t curfile;
            curfile = sampler.GetCurrentFile();
            if(curfile > 0)
            {
                sampler.Open(curfile - 1);
            }

            hw.DelayMs(100);
            hw.StartAudio(AudioCallback);
        }
    }
}
