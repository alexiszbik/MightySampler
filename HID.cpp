#include "HID.h"

void HID::Init(DaisySeed &hw) {
    
    InitButtons(hw);
    InitLeds(hw);
    InitKnobs(hw);
    
}

void HID::InitKnobs(DaisySeed &hw) {
    auto knobs = {KNOB_1_PIN, KNOB_2_PIN, KNOB_3_PIN, KNOB_4_PIN};

    AdcChannelConfig knob_init[knobs.size()];
    int k = 0;
    for (int pin : knobs) { 
        knob_init[k].InitSingle(hw.GetPin(pin));
        k++;
    }

    hw.adc.Init(knob_init, knobs.size());
    hw.adc.Start();
}

void HID::InitButtons(DaisySeed &hw) {

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

void HID::InitLeds(DaisySeed &hw) {
    for (int pin : {LED_PIN_A, LED_PIN_B, LED_PIN_C, LED_PIN_D, LED_PIN_E, LED_PIN_F}) {
        auto led = new dsy_gpio();
        led->pin  = hw.GetPin(pin);
        led->mode = DSY_GPIO_MODE_OUTPUT_PP;
        led->pull = DSY_GPIO_NOPULL;
        dsy_gpio_init(led);
        leds.push_back(led);
    }
}

void HID::ProcessInput(DaisySeed &hw) {
    shiftButton.Debounce();
    cancelButton.Debounce();

    bool newShiftState = shiftButton.Pressed();
    if (newShiftState != shiftState) {
        shiftState = newShiftState;
    }
    
    bool newCancelState = cancelButton.Pressed();
    if (newCancelState != cancelState) {
        cancelState = newCancelState;
    }

    int b = 0;
    for (auto button : buttons) {
        button->Debounce();
        bool state = button->Pressed();

        buttonStates[b++] = state;
    }

    int k = 0;
    for (auto& knobValue : knobValues) {
        knobValue = hw.adc.GetFloat(k++);
    }
    
}

void HID::SetLedState(int index, bool state) {
    dsy_gpio_write(leds.at(index), state);
}