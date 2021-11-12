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

#include "./WavStream.h"
#include "daisy_pod.h"
#include "dev/oled_ssd130x.h"
//#include "daisy_patch.h"

using MyOledDisplay = OledDisplay<SSD130xI2c128x64Driver>;

using namespace daisy;

#define BUTTON_PIN_A 21
#define BUTTON_PIN_B 22
#define BUTTON_PIN_C 23
#define BUTTON_PIN_D 24
#define BUTTON_PIN_E 25
#define BUTTON_PIN_F 26

#define LED_PIN_A 27
#define LED_PIN_B 28
#define LED_PIN_C 29
#define LED_PIN_D 30
#define LED_PIN_E 7
#define LED_PIN_F 8

DaisySeed    hw;
SdmmcHandler sdcard;
WavStream    sampler;

MyOledDisplay display;
MidiUartHandler midi;

std::vector<Switch*> buttons;
std::vector<dsy_gpio*> leds;

double speed = 1.0;

void AudioCallback(const float *in, float *out, size_t size)
{
    int iterator = 0;
    for (auto button : buttons) {
        button->Debounce();
        bool state = button->Pressed();

        dsy_gpio_write(leds.at(iterator), state);
        iterator++;
    }

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

void HandleMidiMessage(MidiEvent m)
{
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

void InitButtons() {

    int updateRate = 1000;
    //Initialize the button on pin 28
    for (int pin : {BUTTON_PIN_A, BUTTON_PIN_B, BUTTON_PIN_C, BUTTON_PIN_D, BUTTON_PIN_E, BUTTON_PIN_F}) {
        auto button = new Switch();
        button->Init(hw.GetPin(pin), updateRate, Switch::TYPE_MOMENTARY, Switch::POLARITY_INVERTED, Switch::PULL_UP);
        buttons.push_back(button);
    }
    
}

void InitLeds() {
    for (int pin : {LED_PIN_A, LED_PIN_B, LED_PIN_C, LED_PIN_D, LED_PIN_E, LED_PIN_F}) {
        auto led = new dsy_gpio();
        led->pin  = hw.GetPin(pin);
        led->mode = DSY_GPIO_MODE_OUTPUT_PP;
        led->pull = DSY_GPIO_NOPULL;
        dsy_gpio_init(led);
        leds.push_back(led);

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

    //    hw.ClearLeds();
    SdmmcHandler::Config sd_cfg;
    sd_cfg.Defaults();
    sd_cfg.speed = SdmmcHandler::Speed::SLOW;
    sdcard.Init(sd_cfg);

    displayWrite("sd config");
    const char* init = sampler.Init();

    displayWrite(init);

    InitButtons();
    InitLeds();

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
