#pragma once
#include <array>
#include <cmath>
#include <juce_core/juce_core.h>
#include "Utils.h"

class DualOscillator
{
public:
    enum class Waveform { Saw, Square, Triangle, Sine, Noise };

    void prepare(double newSampleRate)
    {
        sampleRate = newSampleRate;
    }

    void setBaseFrequency(float hz) { baseFrequency = hz; }
    void setWaveforms(Waveform a, Waveform b) { waveA = a; waveB = b; }
    void setOscBParams(float gain, float pitchOffsetSemitones)
    {
        oscBGain = gain;
        oscBPitchSemitones = pitchOffsetSemitones;
    }
    void setPulseWidth(float pw) { pulseWidth = juce::jlimit(0.05f, 0.95f, pw); }
    void setHardSync(bool enabled) { hardSyncEnabled = enabled; }

    void setUnison(int voiceCount, float detuneCents, float spread)
    {
        numVoices = juce::jlimit(1, maxUnison, voiceCount);
        unisonDetuneCents = detuneCents;
        unisonSpreadAmt = spread;
        updateUnisonSpread();
    }

    // Call on note-on for a clean retrigger. Each unison voice gets a small
    // phase offset so they don't all start perfectly aligned (avoids a
    // static, comb-filtered attack).
    void resetPhases()
    {
        for (int i = 0; i < maxUnison; ++i)
        {
            voices[i].phaseA = (float) i / (float) juce::jmax(1, numVoices) * 0.97f;
            voices[i].phaseB = voices[i].phaseA;
        }
    }

    void renderSample(float pitchModSemitones, float pulseWidthModOffset, float& outL, float& outR)
    {
        outL = 0.0f;
        outR = 0.0f;

        const float effectivePw = juce::jlimit(0.05f, 0.95f, pulseWidth + pulseWidthModOffset);
        const float pitchModRatio = DspUtils::semitonesToRatio(pitchModSemitones);
        const float oscBRatio = DspUtils::semitonesToRatio(oscBPitchSemitones);

        for (int i = 0; i < numVoices; ++i)
        {
            auto& v = voices[i];

            const float freqA = baseFrequency * v.detuneRatio * pitchModRatio;
            const float freqB = freqA * oscBRatio;

            const float incA = (float) (freqA / sampleRate);
            const float incB = (float) (freqB / sampleRate);

            const float sampleA = generateWaveform(waveA, v.phaseA, incA, effectivePw);
            float sampleB;

            if (hardSyncEnabled)
            {
                // Osc A is the sync master: B's phase is recomputed as a multiple
                // of A's phase so B always completes whole cycles within A's period.
                float ratio = juce::jmax(0.01f, freqB / juce::jmax(1.0f, freqA));
                float syncedPhaseB = std::fmod(v.phaseA * ratio, 1.0f);
                sampleB = generateWaveform(waveB, syncedPhaseB, incB, effectivePw);
            }
            else
            {
                sampleB = generateWaveform(waveB, v.phaseB, incB, effectivePw);
            }

            const float mixed = sampleA * (1.0f - oscBGain) + sampleB * oscBGain;

            const float panL = 0.5f - 0.5f * v.pan;
            const float panR = 0.5f + 0.5f * v.pan;
            outL += mixed * panL;
            outR += mixed * panR;

            v.phaseA += incA;
            if (v.phaseA >= 1.0f) v.phaseA -= 1.0f;

            if (!hardSyncEnabled)
            {
                v.phaseB += incB;
                if (v.phaseB >= 1.0f) v.phaseB -= 1.0f;
            }
        }

        // Normalize so adding unison voices doesn't blow up gain.
        const float norm = 1.0f / std::sqrt((float) juce::jmax(1, numVoices));
        outL *= norm;
        outR *= norm;
    }

private:
    struct UnisonVoice
    {
        float phaseA = 0.0f, phaseB = 0.0f;
        float pan = 0.0f;
        float detuneRatio = 1.0f;
    };

    static constexpr int maxUnison = 8;
    std::array<UnisonVoice, maxUnison> voices;
    int numVoices = 1;

    double sampleRate = 44100.0;
    float baseFrequency = 110.0f;
    Waveform waveA = Waveform::Saw, waveB = Waveform::Square;
    float oscBGain = 0.5f;
    float oscBPitchSemitones = 0.0f;
    bool hardSyncEnabled = false;
    float pulseWidth = 0.5f;
    float unisonDetuneCents = 10.0f;
    float unisonSpreadAmt = 0.5f;
    juce::Random rng { (juce::int64) (juce::Random::getSystemRandom().nextInt64()) };

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
            // -1..1 spread position across the voice count
            const float pos = (float) i / (float) (numVoices - 1) * 2.0f - 1.0f;
            voices[i].pan = pos * unisonSpreadAmt;
            const float cents = pos * unisonDetuneCents;
            voices[i].detuneRatio = DspUtils::centsToRatio(cents);
        }
    }

    float generateWaveform(Waveform w, float phase, float inc, float pw)
    {
        switch (w)
        {
            case Waveform::Saw:
            {
                float v = 2.0f * phase - 1.0f;
                v -= DspUtils::polyBlep(phase, inc);
                return v;
            }
            case Waveform::Square:
            {
                float v = phase < pw ? 1.0f : -1.0f;
                v += DspUtils::polyBlep(phase, inc);
                float t2 = phase + (1.0f - pw);
                t2 -= std::floor(t2);
                v -= DspUtils::polyBlep(t2, inc);
                return v;
            }
            case Waveform::Triangle:
            {
                float t = phase - std::floor(phase);
                return t < 0.5f ? (4.0f * t - 1.0f) : (3.0f - 4.0f * t);
            }
            case Waveform::Sine:
                return std::sin(juce::MathConstants<float>::twoPi * phase);
            case Waveform::Noise:
            default:
                return rng.nextFloat() * 2.0f - 1.0f;
        }
    }
};
