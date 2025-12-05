// # MightySampler
// ## Description
// Fairly simply sample player.
// Loads 16
//
// Play .wav file from the SD Card.
//
#include <stdio.h>
#include <string.h>
#include <vector>

#include "WavStream.h"
#include "HID.h"
#include "daisy_pod.h"

#include "DaisyYMNK/DisplayManager.h"

using namespace daisy;

DaisySeed    hw;
SdmmcHandler sdcard;
WavStream    sampler;

HID hid;

DisplayManager *display = DisplayManager::GetInstance();
MidiUartHandler midi;

int lastSamplerId = 0;
char strbuff[128];

void AudioCallback(const float *in, float *out, size_t size)
{
    hid.ProcessInput(hw);

    int i = 0;
    for (auto& buttonState : hid.buttonStates) {
        if (buttonState.changed) {
            sampler.sample[i].SetState(buttonState.state);
            buttonState.changed = false;
            if (buttonState.state) {
                lastSamplerId = i;
            }
        }

        hid.SetLedState(i, sampler.sample[i].IsPlaying());
        i++;
    }

    float ratio = sampler.sample[lastSamplerId].getPositionRatio();
    string progress = "";
    ratio *= 10.0f;
    if (ratio != 0) {
        for (int i = 0; i < 10; i++) {
            if (i < round(ratio)) {
                char c = 'X';
                progress += c; 
            } else {
                char c = '-';
                progress += c; 
            }
        }
    }
    
    //display->Write({sampler.sample[lastSamplerId].getName(), progress.c_str()});

    //for testing only
    sampler.sample[0].parameters.at(Volume).value = hid.knobValues.at(0);
    sampler.sample[0].parameters.at(Speed).value = hid.knobValues.at(1);
    sampler.sample[0].parameters.at(FXSend).value = hid.knobValues.at(2);

    for(size_t i = 0; i < size; i += 2)
    {
        sampler.Stream();

        out[i] = sampler.data[0] * 0.5f;
        out[i + 1] = sampler.data[1] * 0.5f;
    }
}

void HandleMidiMessage(MidiEvent m)
{
    switch(m.type)
    {
        case NoteOn:
        {
            sprintf(strbuff,
                    "Note On :%d %d %d",
                    m.channel,
                    m.data[0],
                    m.data[1]);
            display->Write({strbuff});

            int sampleId = m.data[0];
            if (sampleId < 6) {
                sampler.sample[sampleId].SetState(m.data[1] > 0, true);
            }
            

            break;
        }

        case NoteOff:
        {
            sprintf(strbuff,
                    "Note Off :%d %d %d",
                    m.channel,
                    m.data[0],
                    m.data[1]);
            display->Write({strbuff});

            int sampleId = m.data[0];
            if (sampleId < 6) {
                sampler.sample[sampleId].SetState(false, true);
            }
            

            break;
        }
        
        case ControlChange:
        {
            sprintf(strbuff,
                    "CC : %d %d %d",
                    m.channel,
                    m.data[0],
                    m.data[1]);
            display->Write({strbuff});
            break;
        }
        default: break;
    }

    
}

void InitMidi()
{
    MidiUartHandler::Config midi_config;
    midi.Init(midi_config);
}

int main(void)
{
    // Init hardware
    size_t blocksize = 8;
    hw.Configure();
    hw.Init();

    display->Init(&hw);
    display->Write({"YMNK", "SAMPLER"});

    SdmmcHandler::Config sd_cfg;
    sd_cfg.Defaults();
    sd_cfg.speed = SdmmcHandler::Speed::FAST;
    sd_cfg.width = SdmmcHandler::BusWidth::BITS_4;
    sdcard.Init(sd_cfg);
    
    sampler.Init(hw.AudioSampleRate());

    hid.Init(hw);

    // Init Audio
    hw.SetAudioBlockSize(blocksize);
    hw.StartAudio(AudioCallback);

    InitMidi();
    midi.StartReceive();

    int d_count = 0;

    display->Write({"YMNK", "READY"});

    // Loop forever...
    for(;;)
    {
        midi.Listen();
        // Handle MIDI Events
        while(midi.HasEvents())
        {
            HandleMidiMessage(midi.PopEvent());
        }

        d_count ++;

        if (d_count > 4) {
            display->Update();
            d_count = 0;
        }
    }
}

