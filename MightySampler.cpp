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

#include "DisplayManager.h"

using namespace daisy;

#define KNOB_1_PIN 15
#define KNOB_2_PIN 16
#define KNOB_3_PIN 17
#define KNOB_4_PIN 18

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

#define BUTTON_SHIFT 10
#define BUTTON_CANCEL 9

DaisySeed    hw;
SdmmcHandler sdcard;
WavStream    sampler;

DisplayManager *display = DisplayManager::GetInstance();
MidiUartHandler midi;

std::vector<Switch*> buttons;
std::vector<dsy_gpio*> leds;

Switch shiftButton;
Switch cancelButton;

bool shiftState = false;
bool cancelState = false;

double speed = 1.0;

void AudioCallback(const float *in, float *out, size_t size)
{
    int iterator = 0;

    for (auto button : buttons) {
        button->Debounce();
        bool state = button->Pressed();

        sampler.sample[iterator].SetButtonState(state);

        dsy_gpio_write(leds.at(iterator), sampler.sample[iterator].IsPlaying());
        iterator++;
    }

    shiftButton.Debounce();
    cancelButton.Debounce();

    bool newShiftState = shiftButton.Pressed();
    if (newShiftState != shiftState) {
        shiftState = newShiftState;
        display->Write("SHIFT", shiftState ? "ON" : "OFF");
    }
    
    bool newCancelState = cancelButton.Pressed();
    if (newCancelState != cancelState) {
        cancelState = newCancelState;
        display->Write("CANCEL", cancelState ? "ON" : "OFF");
    }

    speed = hw.adc.GetFloat(0);

    for(size_t i = 0; i < size; i += 2)
    {
        sampler.Stream((speed*2 - 1));

        out[i] = sampler.data[0] * 0.5f;
        out[i + 1] = sampler.data[1] * 0.5f;
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

    display->Write(strbuff);
}

void InitButtons() {

    int updateRate = 1000;
    //Initialize the button on pin 28
    for (int pin : {BUTTON_PIN_A, BUTTON_PIN_B, BUTTON_PIN_C, BUTTON_PIN_D, BUTTON_PIN_E, BUTTON_PIN_F}) {
        auto button = new Switch();
        button->Init(hw.GetPin(pin), updateRate, Switch::TYPE_MOMENTARY, Switch::POLARITY_INVERTED, Switch::PULL_UP);
        buttons.push_back(button);
    }

    shiftButton.Init(hw.GetPin(BUTTON_SHIFT), updateRate, Switch::TYPE_MOMENTARY, Switch::POLARITY_NORMAL, Switch::PULL_UP);
    cancelButton.Init(hw.GetPin(BUTTON_CANCEL), updateRate, Switch::TYPE_MOMENTARY, Switch::POLARITY_NORMAL, Switch::PULL_UP);
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

//float hardwareValue = hardware->adc.GetFloat(pinId);

int main(void)
{
    // Init hardware
    size_t blocksize = 48;
    hw.Configure();
    hw.Init();

    auto knobs = {KNOB_1_PIN, KNOB_2_PIN, KNOB_3_PIN, KNOB_4_PIN};

    AdcChannelConfig knob_init[knobs.size()];
    int iterator = 0;
    for (int pin : knobs) { 
        knob_init[iterator].InitSingle(hw.GetPin(pin));
        iterator++;
    }

    hw.adc.Init(knob_init, knobs.size());

    double samplerate = hw.AudioSampleRate();

    display->Init(&hw);
    display->Write("YMNK", "SAMPLER");

    //    hw.ClearLeds();
    SdmmcHandler::Config sd_cfg;
    sd_cfg.Defaults();
    sd_cfg.speed = SdmmcHandler::Speed::FAST;
    sd_cfg.width = SdmmcHandler::BusWidth::BITS_1;
    sdcard.Init(sd_cfg);
    
    sampler.Init();

    InitButtons();
    InitLeds();

    hw.adc.Start();

    // Init Audio
    hw.SetAudioBlockSize(blocksize);
    hw.StartAudio(AudioCallback);

    InitMidi();
    midi.StartReceive();

    // Loop forever...
    for(;;)
    {
        midi.Listen();
        // Handle MIDI Events
        while(midi.HasEvents())
        {
            HandleMidiMessage(midi.PopEvent());
        }
    }
}
