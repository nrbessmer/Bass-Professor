#pragma once
#include <array>
#include <cmath>
#include <juce_core/juce_core.h>
#include "Utils.h"

// Two-operator phase-modulation FM (the "FM" most synths actually ship —
// technically phase modulation, which is better-behaved than true frequency
// modulation). A modulator sine modulates the carrier's phase; ratio sets the
// modulator:carrier frequency relationship, index sets depth, and modulator
// self-feedback adds the saw-like richness classic to DX-style operators.
//
// Interface matches DualOscillator / WavetableOscillator so the voice can host
// it interchangeably.
class FmOscillator
{
public:
    void prepare(double newSampleRate) { sampleRate = newSampleRate; }

    void setBaseFrequency(float hz) { baseFrequency = juce::jmax(0.0001f, hz); }

    void setFmParams(float ratio, float index, float feedback)
    {
        fmRatio = juce::jmax(0.0f, ratio);
        fmIndex = juce::jmax(0.0f, index);
        fmFeedback = juce::jlimit(0.0f, 1.0f, feedback);
    }

    void setUnison(int voiceCount, float detuneCents, float spread)
    {
        numVoices = juce::jlimit(1, maxUnison, voiceCount);
        unisonDetuneCents = detuneCents;
        unisonSpreadAmt = spread;
        updateUnisonSpread();
    }

    void resetPhases()
    {
        for (int i = 0; i < maxUnison; ++i)
        {
            voices[i].carrierPhase = (float) i / (float) juce::jmax(1, numVoices) * 0.97f;
            voices[i].modPhase = 0.0f;
            voices[i].lastModOut = 0.0f;
        }
    }

    // pwModOffset is repurposed (interface compatibility) as a small additive
    // index wobble, giving the LFO->PW destination a meaningful FM effect.
    void renderSample(float pitchModSemitones, float pwModOffset, float& outL, float& outR)
    {
        outL = 0.0f;
        outR = 0.0f;

        const float pitchRatio = DspUtils::semitonesToRatio(pitchModSemitones);
        const float effIndex = juce::jmax(0.0f, fmIndex + pwModOffset * 4.0f);
        const float twoPi = juce::MathConstants<float>::twoPi;

        for (int i = 0; i < numVoices; ++i)
        {
            auto& v = voices[i];
            const float carrierFreq = baseFrequency * v.detuneRatio * pitchRatio;
            const float modFreq = carrierFreq * fmRatio;

            const float carrierInc = carrierFreq / (float) sampleRate;
            const float modInc = modFreq / (float) sampleRate;

            // Modulator with self-feedback.
            const float modPhaseFb = v.modPhase + fmFeedback * v.lastModOut;
            const float modOut = std::sin(twoPi * modPhaseFb);
            v.lastModOut = modOut;

            // Carrier phase-modulated by the (scaled) modulator output.
            const float carrierSample = std::sin(twoPi * v.carrierPhase + effIndex * modOut);

            const float panL = 0.5f - 0.5f * v.pan;
            const float panR = 0.5f + 0.5f * v.pan;
            outL += carrierSample * panL;
            outR += carrierSample * panR;

            v.carrierPhase += carrierInc;
            if (v.carrierPhase >= 1.0f) v.carrierPhase -= 1.0f;
            v.modPhase += modInc;
            if (v.modPhase >= 1.0f) v.modPhase -= 1.0f;
        }

        const float norm = 1.0f / std::sqrt((float) juce::jmax(1, numVoices));
        outL *= norm;
        outR *= norm;
    }

private:
    static constexpr int maxUnison = 8;

    struct FmVoice
    {
        float carrierPhase = 0.0f;
        float modPhase = 0.0f;
        float lastModOut = 0.0f;
        float pan = 0.0f;
        float detuneRatio = 1.0f;
    };

    std::array<FmVoice, maxUnison> voices;
    int numVoices = 1;

    double sampleRate = 44100.0;
    float baseFrequency = 110.0f;
    float fmRatio = 1.0f;
    float fmIndex = 1.0f;
    float fmFeedback = 0.0f;

    float unisonDetuneCents = 10.0f;
    float unisonSpreadAmt = 0.5f;

    void updateUnisonSpread()
    {
        if (numVoices <= 1)
        {
            voices[0].pan = 0.0f;
            voices[0].detuneRatio = 1.0f;
            return;
        }
        for (int i = 0; i < numVoices; ++i)
        {
            const float pos = (float) i / (float) (numVoices - 1) * 2.0f - 1.0f;
            voices[i].pan = pos * unisonSpreadAmt;
            voices[i].detuneRatio = DspUtils::centsToRatio(pos * unisonDetuneCents);
        }
    }
};
