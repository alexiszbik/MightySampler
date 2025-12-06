#include "Button.h"

void Button::init(DaisySeed& hw, int pin, int rate) noexcept {
    sw.Init(hw.GetPin(pin),
            rate,
            Switch::TYPE_MOMENTARY,
            Switch::POLARITY_NORMAL,
            Switch::PULL_UP);
}

bool Button::process() noexcept {
    sw.Debounce();
    const bool newState = sw.Pressed();

    if(newState != currentState) {
        currentState = newState;
        return true;
    }
    return false;
}