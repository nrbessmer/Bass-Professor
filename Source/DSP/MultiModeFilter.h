#pragma once
#include <cmath>
#include <juce_core/juce_core.h>
#include "Utils.h"

class MultiModeFilter
{
public:
    enum class Type { LowPass, HighPass, BandPass, Notch };

    void prepare(double newSampleRate)
    {
        sampleRate = newSampleRate;
        reset();
    }

    void reset()
    {
        ic1eq = 0.0f;
        ic2eq = 0.0f;
    }

    void setType(Type t) { type = t; }
    void setDrive(float d) { drive = juce::jlimit(0.0f, 1.0f, d); }

    // cutoffHz should already include keytrack/envelope/LFO modulation.
    void setCutoffAndResonance(float cutoffHz, float resonanceQ)
    {
        cutoffHz = juce::jlimit(20.0f, 20000.0f, cutoffHz);
        resonanceQ = juce::jmax(0.1f, resonanceQ);

        const float g = std::tan(juce::MathConstants<float>::pi * cutoffHz / (float) sampleRate);
        const float k = 1.0f / resonanceQ;

        a1 = 1.0f / (1.0f + g * (g + k));
        a2 = g * a1;
        a3 = g * a2;
        this->g = g;
        this->k = k;
    }

    float processSample(float input)
    {
        // Pre-filter saturation: gives the filter input some analog-style
        // grit/compression, especially useful for sub-bass content.
        if (drive > 0.0f)
        {
            const float driveAmount = 1.0f + drive * 6.0f;
            input = DspUtils::softClip(input * driveAmount) / std::tanh(driveAmount);
        }

        // Zavalishin TPT state-variable filter — one process step yields
        // simultaneous LP/HP/BP outputs with good behaviour across the
        // full cutoff/resonance range.
        const float v3 = input - ic2eq;
        const float v1 = a1 * ic1eq + a2 * v3;
        const float v2 = ic2eq + a2 * ic1eq + a3 * v3;

        ic1eq = 2.0f * v1 - ic1eq;
        ic2eq = 2.0f * v2 - ic2eq;

        const float lp = v2;
        const float bp = v1;
        const float hp = input - k * bp - lp;
        const float notch = input - k * bp;

        switch (type)
        {
            case Type::LowPass:  return lp;
            case Type::HighPass: return hp;
            case Type::BandPass: return bp;
            case Type::Notch:    return notch;
            default:             return lp;
        }
    }

private:
    double sampleRate = 44100.0;
    Type type = Type::LowPass;
    float drive = 0.0f;

    float cutoffHz = 1000.0f;
    float resonanceQ = 0.7f;
    float g = 0.0f, k = 1.0f;
    float a1 = 0.0f, a2 = 0.0f, a3 = 0.0f;

    float ic1eq = 0.0f, ic2eq = 0.0f;
};
