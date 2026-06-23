#pragma once
#include <array>
#include <vector>
#include <cmath>
#include <juce_core/juce_core.h>
#include "Utils.h"

// A wavetable oscillator that matches DualOscillator's render interface so the
// voice can host either engine interchangeably. Tables are generated
// procedurally at prepare() time (nothing to ship), and each waveform is
// stored as a bank of band-limited mip levels — one per octave — so high
// notes don't alias. The "Position" control scans/morphs continuously
// between adjacent waveforms in the set.
class WavetableOscillator
{
public:
    // Mirror of DualOscillator::Waveform value ordering is not required here;
    // the wavetable set is its own thing. Tables, in scan order:
    //   0 Sine -> 1 Triangle -> 2 Saw -> 3 Square -> 4 "Formant-ish" -> 5 Bass harmonics
    static constexpr int numTables = 6;

    void prepare(double newSampleRate)
    {
        sampleRate = newSampleRate;
        if (!tablesBuilt)
        {
            buildTables();
            tablesBuilt = true;
        }
    }

    void setBaseFrequency(float hz) { baseFrequency = juce::jmax(0.0001f, hz); }

    // "position" 0..1 scans across the table set; "warp" 0..1 reserved for a
    // secondary shaping (here: a simple symmetry skew of the read phase).
    void setMorph(float position01, float warp01)
    {
        morphPosition = juce::jlimit(0.0f, 1.0f, position01);
        warp = juce::jlimit(0.0f, 1.0f, warp01);
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
            voices[i].phase = (float) i / (float) juce::jmax(1, numVoices) * 0.97f;
    }

    // pwModOffset is accepted for interface compatibility with DualOscillator;
    // here it nudges the warp (phase symmetry) for a comparable "moving" feel.
    void renderSample(float pitchModSemitones, float pwModOffset, float& outL, float& outR)
    {
        outL = 0.0f;
        outR = 0.0f;

        const float pitchModRatio = DspUtils::semitonesToRatio(pitchModSemitones);
        const float effWarp = juce::jlimit(0.0f, 1.0f, warp + pwModOffset);

        for (int i = 0; i < numVoices; ++i)
        {
            auto& v = voices[i];
            const float freq = baseFrequency * v.detuneRatio * pitchModRatio;
            const float inc = freq / (float) sampleRate;

            const int mip = mipLevelForFrequency(freq);
            const float warped = applyWarp(v.phase, effWarp);
            const float sample = readMorphed(warped, mip);

            const float panL = 0.5f - 0.5f * v.pan;
            const float panR = 0.5f + 0.5f * v.pan;
            outL += sample * panL;
            outR += sample * panR;

            v.phase += inc;
            if (v.phase >= 1.0f) v.phase -= 1.0f;
        }

        const float norm = 1.0f / std::sqrt((float) juce::jmax(1, numVoices));
        outL *= norm;
        outR *= norm;
    }

private:
    static constexpr int tableSize = 2048;
    static constexpr int numMips = 10; // ~ covers the MIDI range in octave steps
    static constexpr int maxUnison = 8;

    struct UnisonVoice
    {
        float phase = 0.0f;
        float pan = 0.0f;
        float detuneRatio = 1.0f;
    };

    // tables[tableIndex][mipLevel][sampleIndex]
    std::array<std::array<std::vector<float>, numMips>, numTables> tables;
    bool tablesBuilt = false;

    double sampleRate = 44100.0;
    float baseFrequency = 110.0f;
    float morphPosition = 0.0f;
    float warp = 0.0f;

    std::array<UnisonVoice, maxUnison> voices;
    int numVoices = 1;
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

    static float applyWarp(float phase, float warp)
    {
        // Symmetry skew: warp=0.5 is linear; toward 0 or 1 compresses one
        // half of the cycle, shifting harmonic content (a cheap "phase
        // distortion"-like motion control).
        if (std::abs(warp - 0.5f) < 1.0e-4f)
            return phase;

        const float k = juce::jlimit(0.05f, 0.95f, warp);
        if (phase < k)
            return 0.5f * phase / k;
        return 0.5f + 0.5f * (phase - k) / (1.0f - k);
    }

    int mipLevelForFrequency(float freq) const
    {
        // Pick the most detailed mip whose top harmonic stays below Nyquist.
        // Each mip up halves the harmonic count, so step by octaves from a
        // reference of ~27.5 Hz (A0).
        if (freq <= 27.5f) return 0;
        const float octavesAbove = std::log2(freq / 27.5f);
        return juce::jlimit(0, numMips - 1, (int) octavesAbove);
    }

    float readMorphed(float phase, int mip) const
    {
        // Map morphPosition across the table set and crossfade between the
        // two nearest tables.
        const float scaled = morphPosition * (float) (numTables - 1);
        const int tA = juce::jlimit(0, numTables - 1, (int) scaled);
        const int tB = juce::jmin(numTables - 1, tA + 1);
        const float frac = scaled - (float) tA;

        const float a = readTable(tA, mip, phase);
        const float b = readTable(tB, mip, phase);
        return a + (b - a) * frac;
    }

    float readTable(int tableIndex, int mip, float phase) const
    {
        const auto& tbl = tables[(size_t) tableIndex][(size_t) mip];
        const float pos = phase * (float) tableSize;
        const int i0 = (int) pos % tableSize;
        const int i1 = (i0 + 1) % tableSize;
        const float frac = pos - std::floor(pos);
        return tbl[(size_t) i0] + (tbl[(size_t) i1] - tbl[(size_t) i0]) * frac;
    }

    // --- Procedural table generation (additive, band-limited per mip) ---
    void buildTables()
    {
        for (int t = 0; t < numTables; ++t)
            for (int m = 0; m < numMips; ++m)
                buildOneTable(t, m);
    }

    void buildOneTable(int tableIndex, int mip)
    {
        auto& tbl = tables[(size_t) tableIndex][(size_t) mip];
        tbl.assign(tableSize, 0.0f);

        // Max harmonic for this mip: halve per octave from a generous base.
        const int baseMaxHarmonic = tableSize / 2;
        const int maxHarmonic = juce::jmax(1, baseMaxHarmonic >> mip);

        for (int n = 0; n < tableSize; ++n)
        {
            const float ph = (float) n / (float) tableSize;
            tbl[(size_t) n] = sampleHarmonic(tableIndex, ph, maxHarmonic);
        }

        normalize(tbl);
    }

    static float sampleHarmonic(int tableIndex, float phase, int maxHarmonic)
    {
        const float twoPi = juce::MathConstants<float>::twoPi;
        float v = 0.0f;

        switch (tableIndex)
        {
            case 0: // Sine
                v = std::sin(twoPi * phase);
                break;
            case 1: // Triangle (odd harmonics, 1/n^2, alternating sign)
                for (int h = 1; h <= maxHarmonic; h += 2)
                {
                    const float sign = ((h - 1) / 2) % 2 == 0 ? 1.0f : -1.0f;
                    v += sign * std::sin(twoPi * h * phase) / (float) (h * h);
                }
                break;
            case 2: // Saw (all harmonics, 1/n)
                for (int h = 1; h <= maxHarmonic; ++h)
                    v += std::sin(twoPi * h * phase) / (float) h;
                break;
            case 3: // Square (odd harmonics, 1/n)
                for (int h = 1; h <= maxHarmonic; h += 2)
                    v += std::sin(twoPi * h * phase) / (float) h;
                break;
            case 4: // Formant-ish: a couple of emphasized harmonic bands
                for (int h = 1; h <= maxHarmonic; ++h)
                {
                    const float formant1 = std::exp(-std::pow((float) h - 4.0f, 2.0f) / 6.0f);
                    const float formant2 = std::exp(-std::pow((float) h - 11.0f, 2.0f) / 14.0f);
                    v += (formant1 + 0.6f * formant2) * std::sin(twoPi * h * phase) / (float) h;
                }
                break;
            case 5: // Bass harmonics: strong fundamental + controlled upper content
            default:
                for (int h = 1; h <= maxHarmonic; ++h)
                {
                    const float weight = 1.0f / (float) (h * h) + (h <= 3 ? 0.5f / (float) h : 0.0f);
                    v += weight * std::sin(twoPi * h * phase);
                }
                break;
        }
        return v;
    }

    static void normalize(std::vector<float>& tbl)
    {
        float peak = 0.0f;
        for (float x : tbl) peak = std::max(peak, std::abs(x));
        if (peak > 1.0e-6f)
        {
            const float g = 1.0f / peak;
            for (float& x : tbl) x *= g;
        }
    }
};
