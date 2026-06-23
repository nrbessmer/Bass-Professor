#pragma once
#include <cmath>
#include <juce_core/juce_core.h>

namespace DspUtils
{
    // Classic PolyBLEP correction (Valimaki/Huovilainen). t = phase / period (0..1),
    // dt = phase increment per sample (0..1). Subtract the return value at
    // discontinuities to band-limit saw/square edges.
    inline float polyBlep(float t, float dt)
    {
        if (dt <= 0.0f)
            return 0.0f;

        if (t < dt)
        {
            float x = t / dt;
            return x + x - x * x - 1.0f;
        }
        if (t > 1.0f - dt)
        {
            float x = (t - 1.0f) / dt;
            return x * x + x + x + 1.0f;
        }
        return 0.0f;
    }

    inline float midiToHz(float midiNote)
    {
        return 440.0f * std::pow(2.0f, (midiNote - 69.0f) / 12.0f);
    }

    inline float dbToGain(float db)
    {
        return std::pow(10.0f, db / 20.0f);
    }

    inline float centsToRatio(float cents)
    {
        return std::pow(2.0f, cents / 1200.0f);
    }

    inline float semitonesToRatio(float semis)
    {
        return std::pow(2.0f, semis / 12.0f);
    }

    inline float softClip(float x)
    {
        return std::tanh(x);
    }
}
