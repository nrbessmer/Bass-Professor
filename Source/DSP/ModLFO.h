#pragma once
#include <cmath>
#include <juce_core/juce_core.h>

class ModLFO
{
public:
    enum class Shape { Sine, Triangle, Square, Saw, SampleHold };

    void prepare(double newSampleRate) { sampleRate = newSampleRate; }
    void setShape(Shape s) { shape = s; }
    void setRate(float hz) { rateHz = juce::jmax(0.001f, hz); }

    void resetPhase() { phase = 0.0f; lastHeld = 0.0f; }

    // Returns a value in -1..1
    float renderSample()
    {
        float out = 0.0f;
        switch (shape)
        {
            case Shape::Sine:
                out = std::sin(juce::MathConstants<float>::twoPi * phase);
                break;
            case Shape::Triangle:
                out = phase < 0.5f ? (4.0f * phase - 1.0f) : (3.0f - 4.0f * phase);
                break;
            case Shape::Square:
                out = phase < 0.5f ? 1.0f : -1.0f;
                break;
            case Shape::Saw:
                out = 2.0f * phase - 1.0f;
                break;
            case Shape::SampleHold:
                out = lastHeld;
                break;
        }

        const float inc = (float) (rateHz / sampleRate);
        const float prevPhase = phase;
        phase += inc;
        if (phase >= 1.0f)
        {
            phase -= 1.0f;
            if (shape == Shape::SampleHold)
                lastHeld = rng.nextFloat() * 2.0f - 1.0f;
        }
        juce::ignoreUnused(prevPhase);

        return out;
    }

private:
    double sampleRate = 44100.0;
    Shape shape = Shape::Sine;
    float rateHz = 2.0f;
    float phase = 0.0f;
    float lastHeld = 0.0f;
    juce::Random rng { (juce::int64) (juce::Random::getSystemRandom().nextInt64()) };
};
