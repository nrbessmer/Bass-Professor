#pragma once
#include "DualOscillator.h"
#include "WavetableOscillator.h"
#include "FmOscillator.h"
#include "MultiModeFilter.h"
#include "Utils.h"

// One band of the 3-band voice (Body / Character / Air). Owns its own
// oscillator pair and per-band level/pan/pitch. The voice creates three of
// these, renders each, band-limits each through the crossover, and merges.
//
// Keeping both oscillator engines per band (rather than one shared) is what
// makes the architecture genuinely "three independent lanes" as the spec
// describes — each band can run a different model and pitch.
class SynthBand
{
public:
    void prepare(double sr)
    {
        analog.prepare(sr);
        wavetable.prepare(sr);
        fm.prepare(sr);
        filterL.prepare(sr);
        filterR.prepare(sr);
    }

    void resetPhases()
    {
        analog.resetPhases();
        wavetable.resetPhases();
        fm.resetPhases();
        filterL.reset();
        filterR.reset();
    }

    // --- Per-band filter (optional; applied after crossover, before merge) ---
    void setFilterEnabled(bool b) { filterEnabled = b; }
    bool isFilterEnabled() const { return filterEnabled; }
    void setFilterParams(MultiModeFilter::Type type, float cutoffHz, float resonance, float drive)
    {
        filterL.setType(type);
        filterR.setType(type);
        filterL.setDrive(drive);
        filterR.setDrive(drive);
        filterL.setCutoffAndResonance(cutoffHz, resonance);
        filterR.setCutoffAndResonance(cutoffHz, resonance);
    }

    // Applies the per-band filter in place if enabled. Called by the voice on
    // the band's already-crossover-limited signal.
    void applyFilter(float& l, float& r)
    {
        if (!filterEnabled)
            return;
        l = filterL.processSample(l);
        r = filterR.processSample(r);
    }

    // --- Per-band configuration (set from the voice each block) ---
    // model: 0 = Analog, 1 = Wavetable, 2 = FM
    void setModel(int m)         { model = juce::jlimit(0, 2, m); }
    void setGain(float g)        { gain = juce::jlimit(0.0f, 1.0f, g); }
    void setPan(float p)         { pan = juce::jlimit(-1.0f, 1.0f, p); }
    void setActive(bool b)       { active = b; }
    void setPitchOffsetSemitones(float s) { pitchOffset = s; }

    // Independent per-band drive/saturation (0..1), applied to the oscillator
    // output before gain — NOT tied to the per-band filter. The voice's
    // crossover, applied immediately after, band-limits the generated
    // harmonics, so driving the Body band adds weight without splattering
    // harmonics into the upper bands.
    void setDrive(float d) { drive = juce::jlimit(0.0f, 1.0f, d); }

    // Pass-throughs so the voice can drive the underlying oscillators with the
    // shared controls (waveforms, unison, wavetable morph, etc).
    DualOscillator&       analogOsc()    { return analog; }
    WavetableOscillator&  wavetableOsc() { return wavetable; }
    FmOscillator&         fmOsc()        { return fm; }

    void setBaseFrequency(float hz)
    {
        const float f = hz * DspUtils::semitonesToRatio(pitchOffset);
        analog.setBaseFrequency(f);
        wavetable.setBaseFrequency(f);
        fm.setBaseFrequency(f);
    }

    bool isActive() const { return active; }

    // Renders this band's raw (pre-crossover) stereo output, applying per-band
    // gain and pan. Crossover band-limiting is applied by the voice afterward.
    void renderSample(float pitchModSemis, float pwModOffset, float& outL, float& outR)
    {
        if (!active)
        {
            outL = 0.0f;
            outR = 0.0f;
            return;
        }

        float l = 0.0f, r = 0.0f;
        if (model == 1)
            wavetable.renderSample(pitchModSemis, pwModOffset, l, r);
        else if (model == 2)
            fm.renderSample(pitchModSemis, pwModOffset, l, r);
        else
            analog.renderSample(pitchModSemis, pwModOffset, l, r);

        // Per-band drive: tanh saturation normalized so low drive ≈ unity.
        if (drive > 0.0f)
        {
            const float amount = 1.0f + drive * 8.0f;
            const float norm = std::tanh(amount);
            l = DspUtils::softClip(l * amount) / norm;
            r = DspUtils::softClip(r * amount) / norm;
        }

        // Per-band pan applied on top of whatever unison spread produced.
        const float panL = 0.5f - 0.5f * pan;
        const float panR = 0.5f + 0.5f * pan;
        outL = (l * panL + r * (1.0f - panR)) * gain; // simple constant-ish pan blend
        outR = (r * panR + l * (1.0f - panL)) * gain;
    }

private:
    DualOscillator analog;
    WavetableOscillator wavetable;
    FmOscillator fm;
    MultiModeFilter filterL, filterR;

    int model = 0; // 0 Analog, 1 Wavetable, 2 FM
    bool active = true;
    bool filterEnabled = false;
    float gain = 1.0f;
    float pan = 0.0f;
    float pitchOffset = 0.0f;
    float drive = 0.0f;
};
