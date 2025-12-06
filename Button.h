#pragma once

#include "daisy_seed.h"

using namespace daisy;

class Button {
public:
    void init(DaisySeed& hw, int pin, int rate = updateRate) noexcept;

    [[nodiscard]] bool process() noexcept;
    inline bool getState() const noexcept { return currentState; }

private:
    Switch sw;
    bool currentState = false;

    static constexpr int updateRate = 1000;
};
