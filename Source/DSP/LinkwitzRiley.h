#pragma once
#include <cmath>
#include <juce_core/juce_core.h>

// One 2nd-order Linkwitz-Riley section = two cascaded Butterworth biquads of
// the same type. A 4th-order LR crossover (24 dB/oct) is what we use to split
// the spectrum: low and high outputs sum back to (near) flat with coherent
// phase, which is exactly what you want when three bands recombine.
//
// For a 3-band split we run two crossover points:
//   input -> [LP @ fLow]                 = Body
//         -> [HP @ fLow] -> [LP @ fHigh] = Character
//         -> [HP @ fHigh]                = Air
// The mid band is bandpassed by cascading the low crossover's high output
// through the high crossover's low. This keeps the bands summing cleanly.

class LinkwitzRiley
{
public:
    enum class Type { LowPass, HighPass };

    void prepare(double sr)
    {
        sampleRate = sr;
        reset();
    }

    void reset()
    {
        for (auto& s : z) s = 0.0f;
    }

    void setType(Type t) { type = t; }

    void setCutoff(float freqHz)
    {
        freqHz = juce::jlimit(10.0f, (float) (sampleRate * 0.49), freqHz);
        // Two identical Butterworth (Q = 1/sqrt(2)) biquads cascaded = LR4.
        const float w0 = juce::MathConstants<float>::twoPi * freqHz / (float) sampleRate;
        const float cosw0 = std::cos(w0);
        const float sinw0 = std::sin(w0);
        const float Q = 0.70710678f;
        const float alpha = sinw0 / (2.0f * Q);

        float b0, b1, b2;
        if (type == Type::LowPass)
        {
            b0 = (1.0f - cosw0) * 0.5f;
            b1 = 1.0f - cosw0;
            b2 = (1.0f - cosw0) * 0.5f;
        }
        else
        {
            b0 = (1.0f + cosw0) * 0.5f;
            b1 = -(1.0f + cosw0);
            b2 = (1.0f + cosw0) * 0.5f;
        }
        const float a0 = 1.0f + alpha;
        const float a1 = -2.0f * cosw0;
        const float a2 = 1.0f - alpha;

        nb0 = b0 / a0; nb1 = b1 / a0; nb2 = b2 / a0;
        na1 = a1 / a0; na2 = a2 / a0;
    }

    // Process one sample through two cascaded biquads (transposed direct form II).
    float processSample(float x)
    {
        x = biquad(x, 0);
        x = biquad(x, 2);
        return x;
    }

private:
    double sampleRate = 44100.0;
    Type type = Type::LowPass;

    float nb0 = 1.0f, nb1 = 0.0f, nb2 = 0.0f, na1 = 0.0f, na2 = 0.0f;
    // Two biquads -> two pairs of state variables.
    float z[4] = { 0, 0, 0, 0 };

    float biquad(float x, int stateOffset)
    {
        float& z1 = z[stateOffset];
        float& z2 = z[stateOffset + 1];
        const float y = nb0 * x + z1;
        z1 = nb1 * x - na1 * y + z2;
        z2 = nb2 * x - na2 * y;
        return y;
    }
};
