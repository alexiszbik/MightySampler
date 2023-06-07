#ifndef HID_H
#define HID_H

#include <stdio.h>
#include <string.h>
#include <vector>

#include "daisy_pod.h"

#define BUTTON_COUNT 6
#define KNOB_COUNT 4

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

using namespace std;
using namespace daisy;

class HID {

public: 
    void Init(DaisySeed &hw);
    void ProcessInput(DaisySeed &hw);
    
    void SetLedState(int index, bool state);

private:
    void InitKnobs(DaisySeed &hw);
    void InitButtons(DaisySeed &hw);
    void InitLeds(DaisySeed &hw);

public:
    vector<bool> buttonStates = vector<bool>(BUTTON_COUNT, false);
    vector<float> knobValues = vector<float>(KNOB_COUNT, 0);

    bool shiftState = false;
    bool cancelState = false;

private:
    vector<Switch*> buttons;
    vector<dsy_gpio*> leds;

    Switch shiftButton;
    Switch cancelButton;
};


#endif //HID_H