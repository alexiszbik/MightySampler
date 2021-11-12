// # MightySampler
// ## Description
// Fairly simply sample player.
// Loads 16
//
// Play .wav file from the SD Card.
//
#include <stdio.h>
#include <string.h>
#include "./WavStream.h"
#include "daisy_pod.h"
#include "dev/oled_ssd130x.h"
//#include "daisy_patch.h"

using MyOledDisplay = OledDisplay<SSD130xI2c128x64Driver>;

using namespace daisy;

#define BUTTON_PIN 22

//DaisyPatch   hw;
DaisySeed     hw;
SdmmcHandler sdcard;
WavStream    sampler;

MyOledDisplay display;
MidiUartHandler midi;

double speed = 1.0;

void AudioCallback(const float *in, float *out, size_t size)
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

char strbuff[128];

// Typical Switch case for Message Type.
void HandleMidiMessage(MidiEvent m)
{

    //sprintf(strbuff, "midi");
    

    
    switch(m.type)
    {
        case NoteOn:
        {
            sprintf(strbuff,
                    "Note :%d %d %d",
                    m.channel,
                    m.data[0],
                    m.data[1]);

            
        }
        break;
        case ControlChange:
        {
            sprintf(strbuff,
                    "CC : %d %d %d",
                    m.channel,
                    m.data[0],
                    m.data[1]);
            break;
        }
        default: break;
    }

    display.Fill(false);
    display.SetCursor(0, 0);
    display.WriteString(strbuff, Font_11x18, true);

    display.Update();
}

void displayWrite(const char* message) {
    display.Fill(false);
    display.SetCursor(0, 0);
    display.WriteString(message, Font_11x18, true);
    display.Update();

}

void InitMidi()
{
    MidiUartHandler::Config midi_config;
    midi.Init(midi_config);
}



int main(void)
{
    // Init hardware
    size_t blocksize = 48;
    hw.Configure();
    hw.Init();

    /** Configure the Display */
    MyOledDisplay::Config disp_cfg;
    disp_cfg.driver_config.transport_config.Defaults();
    disp_cfg.driver_config.transport_config.i2c_config.pin_config.scl = hw.GetPin(11);
    disp_cfg.driver_config.transport_config.i2c_config.pin_config.sda = hw.GetPin(12);
    
    /** And Initialize */
    display.Init(disp_cfg);

    displayWrite("pre");

    //    hw.ClearLeds();
    SdmmcHandler::Config sd_cfg;
    sd_cfg.Defaults();
    sd_cfg.speed = SdmmcHandler::Speed::SLOW;
    sdcard.Init(sd_cfg);

    displayWrite("sd config");
    const char* init = sampler.Init();

    displayWrite(init);

    // Init Audio
    hw.SetAudioBlockSize(blocksize);
    hw.StartAudio(AudioCallback);

    InitMidi();
    midi.StartReceive();
    
    sampler.Trigger(0, true);

    // Loop forever...
    for(;;)
    {

        midi.Listen();
        // Handle MIDI Events
        while(midi.HasEvents())
        {
            HandleMidiMessage(midi.PopEvent());
        }

        sampler.CheckPlaying();

        // this is temp
        //speed = (p_knob1.Process() - 0.5) * 4.0;

        // Change file with encoder.
        // this is temp
        /*
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
        }*/
    }
}
