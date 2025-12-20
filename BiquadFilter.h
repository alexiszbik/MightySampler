#pragma once

#include "math.h"
#include <daisysp.h>

class BiquadFilter {

public:
    void Init(float sampleRate) {
        this->sampleRate = sampleRate;
    }

    // Bandpass filter (constant 0 dB peak)
    void SetBandpass(float centerFreq, float q) {
        float w0 = 2.0f * PI_F * centerFreq / sampleRate;
        float alpha = sinf(w0) / (2.0f * q);
        float cosw0 = cosf(w0);

        b0 =   alpha;
        b1 =   0.0f;
        b2 =  -alpha;
        a0 =   1.0f + alpha;
        a1 =  -2.0f * cosw0;
        a2 =   1.0f - alpha;

        Normalize();
    }

    // Low-pass filter (standard Butterworth)
    void SetLowpass(float cutoffFreq, float q) {
        float w0 = 2.0f * PI_F * cutoffFreq / sampleRate;
        float alpha = sinf(w0) / (2.0f * q);
        float cosw0 = cosf(w0);

        b0 =  (1.0f - cosw0) / 2.0f;
        b1 =   1.0f - cosw0;
        b2 =  (1.0f - cosw0) / 2.0f;
        a0 =   1.0f + alpha;
        a1 =  -2.0f * cosw0;
        a2 =   1.0f - alpha;

        Normalize();
    }

    void SetHighpass(float cutoffFreq, float q) {
        float w0 = 2.0f * PI_F * cutoffFreq / sampleRate;
        float alpha = sinf(w0) / (2.0f * q);
        float cosw0 = cosf(w0);

        b0 =  (1.0f + cosw0) / 2.0f;
        b1 = -(1.0f + cosw0);
        b2 =  (1.0f + cosw0) / 2.0f;
        a0 =   1.0f + alpha;
        a1 =  -2.0f * cosw0;
        a2 =   1.0f - alpha;

        Normalize();
    }

    float Process(float in) {
        float out = b0 * in + z1;
        z1 = b1 * in - a1 * out + z2;
        z2 = b2 * in - a2 * out;
        return out;
    }

private:
    void Normalize() {
        b0 /= a0;
        b1 /= a0;
        b2 /= a0;
        a1 /= a0;
        a2 /= a0;
        a0 = 1.0f; // Normalized
    }

private:
    float a0, a1, a2, b0, b1, b2;
    float z1 = 0.0f, z2 = 0.0f;
    float sampleRate = 44100;
};
