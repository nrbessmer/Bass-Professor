#pragma once
#include <juce_audio_processors/juce_audio_processors.h>

namespace Params
{
    // Oscillator
    constexpr auto oscModel      = "oscModel"; // 0 = Analog (dual), 1 = Wavetable
    constexpr auto oscWaveA      = "oscWaveA";
    constexpr auto oscWaveB      = "oscWaveB";
    constexpr auto oscBGain      = "oscBGain";
    constexpr auto oscBPitch     = "oscBPitch";
    constexpr auto hardSync      = "hardSync";
    constexpr auto pulseWidth    = "pulseWidth";
    constexpr auto oscOctave     = "oscOctave";
    constexpr auto oscSemitone   = "oscSemitone";
    constexpr auto oscFineCents  = "oscFineCents";
    constexpr auto unisonVoices  = "unisonVoices";
    constexpr auto unisonDetune  = "unisonDetune";
    constexpr auto unisonSpread  = "unisonSpread";

    // --- AM Injector (cross-band amplitude modulation: Body modulates upper bands) ---
    constexpr auto amAmount = "amAmount";
    constexpr auto amTarget = "amTarget"; // 0 = Off, 1 = Character, 2 = Air, 3 = Character+Air

    // Wavetable-specific (only audible when oscModel == Wavetable)
    constexpr auto wtPosition    = "wtPosition";
    constexpr auto wtWarp        = "wtWarp";

    // FM-specific (only audible when oscModel == FM)
    constexpr auto fmRatio       = "fmRatio";
    constexpr auto fmIndex       = "fmIndex";
    constexpr auto fmFeedback    = "fmFeedback";

    // --- 3-Band architecture ---
    constexpr auto bandsEnable     = "bandsEnable";   // off = single-band (legacy) behaviour
    constexpr auto crossoverLow    = "crossoverLow";  // Body/Character split
    constexpr auto crossoverHigh   = "crossoverHigh"; // Character/Air split
    // Body
    constexpr auto bodyModel    = "bodyModel";
    constexpr auto bodyGain     = "bodyGain";
    constexpr auto bodyPan      = "bodyPan";
    constexpr auto bodyPitch    = "bodyPitch";
    constexpr auto bodyActive   = "bodyActive";
    constexpr auto bodyFilterOn   = "bodyFilterOn";
    constexpr auto bodyFilterType = "bodyFilterType";
    constexpr auto bodyFilterCut  = "bodyFilterCut";
    constexpr auto bodyFilterRes  = "bodyFilterRes";
    constexpr auto bodyDrive       = "bodyDrive";
    // Character
    constexpr auto charModel    = "charModel";
    constexpr auto charGain     = "charGain";
    constexpr auto charPan      = "charPan";
    constexpr auto charPitch    = "charPitch";
    constexpr auto charActive   = "charActive";
    constexpr auto charFilterOn   = "charFilterOn";
    constexpr auto charFilterType = "charFilterType";
    constexpr auto charFilterCut  = "charFilterCut";
    constexpr auto charFilterRes  = "charFilterRes";
    constexpr auto charDrive       = "charDrive";
    // Air
    constexpr auto airModel     = "airModel";
    constexpr auto airGain      = "airGain";
    constexpr auto airPan       = "airPan";
    constexpr auto airPitch     = "airPitch";
    constexpr auto airActive    = "airActive";
    constexpr auto airFilterOn   = "airFilterOn";
    constexpr auto airFilterType = "airFilterType";
    constexpr auto airFilterCut  = "airFilterCut";
    constexpr auto airFilterRes  = "airFilterRes";
    constexpr auto airDrive       = "airDrive";

    // Filter
    constexpr auto filterType       = "filterType";
    constexpr auto filterCutoff     = "filterCutoff";
    constexpr auto filterResonance  = "filterResonance";
    constexpr auto filterDrive      = "filterDrive";
    constexpr auto filterEnvAmount  = "filterEnvAmount";
    constexpr auto filterKeyTrack   = "filterKeyTrack";

    // Amp envelope
    constexpr auto ampAttack   = "ampAttack";
    constexpr auto ampDecay    = "ampDecay";
    constexpr auto ampSustain  = "ampSustain";
    constexpr auto ampRelease  = "ampRelease";

    // Mod envelope (drives filter cutoff via filterEnvAmount)
    constexpr auto modAttack   = "modAttack";
    constexpr auto modDecay    = "modDecay";
    constexpr auto modSustain  = "modSustain";
    constexpr auto modRelease  = "modRelease";

    // LFO
    constexpr auto lfoShape        = "lfoShape";
    constexpr auto lfoRate         = "lfoRate";
    constexpr auto lfoDepth        = "lfoDepth";
    constexpr auto lfoDestination  = "lfoDestination";

    // Master
    constexpr auto masterDrive = "masterDrive";
    constexpr auto masterGain  = "masterGain";

    // Effects
    constexpr auto chorusEnable   = "chorusEnable";
    constexpr auto chorusRate     = "chorusRate";
    constexpr auto chorusDepth    = "chorusDepth";
    constexpr auto chorusFeedback = "chorusFeedback";
    constexpr auto chorusMix      = "chorusMix";

    constexpr auto delayEnable   = "delayEnable";
    constexpr auto delayTimeMs   = "delayTimeMs";
    constexpr auto delayFeedback = "delayFeedback";
    constexpr auto delayMix      = "delayMix";

    constexpr auto reverbEnable = "reverbEnable";
    constexpr auto reverbRoom   = "reverbRoom";
    constexpr auto reverbDamp   = "reverbDamp";
    constexpr auto reverbWet    = "reverbWet";
    constexpr auto reverbWidth  = "reverbWidth";

    // Voicing
    constexpr auto polyphonyMode = "polyphonyMode"; // 0 = Poly, 1 = Mono
    constexpr auto glideTimeMs   = "glideTimeMs";
    constexpr auto legato        = "legato";

    inline juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout()
    {
        std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

        auto floatParam = [&params](const juce::String& id, const juce::String& name,
                                     juce::NormalisableRange<float> range, float def,
                                     const juce::String& unit = {})
        {
            params.push_back(std::make_unique<juce::AudioParameterFloat>(
                juce::ParameterID(id, 1), name, range, def,
                juce::AudioParameterFloatAttributes().withLabel(unit)));
        };

        auto choiceParam = [&params](const juce::String& id, const juce::String& name,
                                      const juce::StringArray& choices, int defIndex)
        {
            params.push_back(std::make_unique<juce::AudioParameterChoice>(
                juce::ParameterID(id, 1), name, choices, defIndex));
        };

        auto boolParam = [&params](const juce::String& id, const juce::String& name, bool def)
        {
            params.push_back(std::make_unique<juce::AudioParameterBool>(
                juce::ParameterID(id, 1), name, def));
        };

        // --- Oscillator ---
        choiceParam(oscModel, "Osc Model", { "Analog", "Wavetable", "FM" }, 0);
        choiceParam(oscWaveA, "Osc A Wave", { "Saw", "Square", "Triangle", "Sine" }, 0);
        choiceParam(oscWaveB, "Osc B Wave", { "Saw", "Square", "Triangle", "Sine", "Noise" }, 1);
        floatParam(oscBGain, "Osc B Gain", { 0.0f, 1.0f, 0.001f }, 0.5f);
        floatParam(oscBPitch, "Osc B Pitch", { -12.0f, 12.0f, 0.01f }, 0.0f, "st");
        boolParam(hardSync, "Hard Sync", false);
        floatParam(pulseWidth, "Pulse Width", { 0.05f, 0.95f, 0.001f }, 0.5f);
        floatParam(oscOctave, "Octave", { -3.0f, 3.0f, 1.0f }, 0.0f);
        floatParam(oscSemitone, "Semitone", { -12.0f, 12.0f, 1.0f }, 0.0f, "st");
        floatParam(oscFineCents, "Fine Tune", { -100.0f, 100.0f, 0.1f }, 0.0f, "ct");
        floatParam(unisonVoices, "Unison Voices", { 1.0f, 8.0f, 1.0f }, 1.0f);
        floatParam(unisonDetune, "Unison Detune", { 0.0f, 50.0f, 0.01f }, 10.0f, "ct");
        floatParam(unisonSpread, "Unison Spread", { 0.0f, 1.0f, 0.001f }, 0.5f);

        floatParam(wtPosition, "WT Position", { 0.0f, 1.0f, 0.001f }, 0.0f);
        floatParam(wtWarp, "WT Warp", { 0.0f, 1.0f, 0.001f }, 0.5f);

        // FM: ratio stepped to musically useful values via fine grain; index in
        // a generous 0..12 range; feedback 0..1.
        floatParam(fmRatio, "FM Ratio", juce::NormalisableRange<float>(0.5f, 12.0f, 0.01f), 1.0f);
        floatParam(fmIndex, "FM Index", juce::NormalisableRange<float>(0.0f, 12.0f, 0.001f, 0.5f), 1.0f);
        floatParam(fmFeedback, "FM Feedback", { 0.0f, 1.0f, 0.001f }, 0.0f);

        // --- AM Injector ---
        floatParam(amAmount, "AM Amount", { 0.0f, 1.0f, 0.001f }, 0.0f);
        choiceParam(amTarget, "AM Target", { "Off", "Character", "Air", "Character+Air" }, 0);

        // --- 3-Band architecture ---
        boolParam(bandsEnable, "3-Band Mode", false);
        floatParam(crossoverLow, "Crossover Low",
                    juce::NormalisableRange<float>(20.0f, 5000.0f, 0.01f, 0.3f), 200.0f, "Hz");
        floatParam(crossoverHigh, "Crossover High",
                    juce::NormalisableRange<float>(100.0f, 20000.0f, 0.01f, 0.3f), 2500.0f, "Hz");

        const juce::StringArray bandModels { "Analog", "Wavetable", "FM" };
        const juce::StringArray bandFilterTypes { "Low Pass", "High Pass", "Band Pass", "Notch" };
        auto cutRange = []() { return juce::NormalisableRange<float>(20.0f, 20000.0f, 0.01f, 0.25f); };

        choiceParam(bodyModel, "Body Model", bandModels, 0);
        floatParam(bodyGain, "Body Gain", { 0.0f, 1.0f, 0.001f }, 1.0f);
        floatParam(bodyPan, "Body Pan", { -1.0f, 1.0f, 0.001f }, 0.0f);
        floatParam(bodyPitch, "Body Pitch", { -24.0f, 24.0f, 1.0f }, 0.0f, "st");
        boolParam(bodyActive, "Body On", true);
        boolParam(bodyFilterOn, "Body Filter On", false);
        choiceParam(bodyFilterType, "Body Filter Type", bandFilterTypes, 0);
        floatParam(bodyFilterCut, "Body Filter Cutoff", cutRange(), 800.0f, "Hz");
        floatParam(bodyFilterRes, "Body Filter Res", { 0.1f, 10.0f, 0.001f }, 0.7f);
        floatParam(bodyDrive, "Body Drive", { 0.0f, 1.0f, 0.001f }, 0.0f);

        choiceParam(charModel, "Character Model", bandModels, 0);
        floatParam(charGain, "Character Gain", { 0.0f, 1.0f, 0.001f }, 1.0f);
        floatParam(charPan, "Character Pan", { -1.0f, 1.0f, 0.001f }, 0.0f);
        floatParam(charPitch, "Character Pitch", { -24.0f, 24.0f, 1.0f }, 0.0f, "st");
        boolParam(charActive, "Character On", true);
        boolParam(charFilterOn, "Character Filter On", false);
        choiceParam(charFilterType, "Character Filter Type", bandFilterTypes, 0);
        floatParam(charFilterCut, "Character Filter Cutoff", cutRange(), 1500.0f, "Hz");
        floatParam(charFilterRes, "Character Filter Res", { 0.1f, 10.0f, 0.001f }, 0.7f);
        floatParam(charDrive, "Character Drive", { 0.0f, 1.0f, 0.001f }, 0.0f);

        choiceParam(airModel, "Air Model", bandModels, 1);
        floatParam(airGain, "Air Gain", { 0.0f, 1.0f, 0.001f }, 1.0f);
        floatParam(airPan, "Air Pan", { -1.0f, 1.0f, 0.001f }, 0.0f);
        floatParam(airPitch, "Air Pitch", { -24.0f, 24.0f, 1.0f }, 0.0f, "st");
        boolParam(airActive, "Air On", true);
        boolParam(airFilterOn, "Air Filter On", false);
        choiceParam(airFilterType, "Air Filter Type", bandFilterTypes, 1);
        floatParam(airFilterCut, "Air Filter Cutoff", cutRange(), 5000.0f, "Hz");
        floatParam(airFilterRes, "Air Filter Res", { 0.1f, 10.0f, 0.001f }, 0.7f);
        floatParam(airDrive, "Air Drive", { 0.0f, 1.0f, 0.001f }, 0.0f);

        // --- Filter ---
        choiceParam(filterType, "Filter Type", { "Low Pass", "High Pass", "Band Pass", "Notch" }, 0);
        floatParam(filterCutoff, "Cutoff",
                    juce::NormalisableRange<float>(20.0f, 20000.0f, 0.01f, 0.25f), 1200.0f, "Hz");
        floatParam(filterResonance, "Resonance", { 0.1f, 10.0f, 0.001f }, 0.7f);
        floatParam(filterDrive, "Filter Drive", { 0.0f, 1.0f, 0.001f }, 0.2f);
        floatParam(filterEnvAmount, "Env Amount", { -1.0f, 1.0f, 0.001f }, 0.0f);
        floatParam(filterKeyTrack, "Key Track", { 0.0f, 1.0f, 0.001f }, 0.0f);

        // --- Amp envelope ---
        floatParam(ampAttack, "Amp Attack", juce::NormalisableRange<float>(0.001f, 5.0f, 0.0001f, 0.3f), 0.01f, "s");
        floatParam(ampDecay, "Amp Decay", juce::NormalisableRange<float>(0.001f, 5.0f, 0.0001f, 0.3f), 0.2f, "s");
        floatParam(ampSustain, "Amp Sustain", { 0.0f, 1.0f, 0.001f }, 0.8f);
        floatParam(ampRelease, "Amp Release", juce::NormalisableRange<float>(0.001f, 5.0f, 0.0001f, 0.3f), 0.3f, "s");

        // --- Mod envelope ---
        floatParam(modAttack, "Mod Attack", juce::NormalisableRange<float>(0.001f, 5.0f, 0.0001f, 0.3f), 0.01f, "s");
        floatParam(modDecay, "Mod Decay", juce::NormalisableRange<float>(0.001f, 5.0f, 0.0001f, 0.3f), 0.25f, "s");
        floatParam(modSustain, "Mod Sustain", { 0.0f, 1.0f, 0.001f }, 0.0f);
        floatParam(modRelease, "Mod Release", juce::NormalisableRange<float>(0.001f, 5.0f, 0.0001f, 0.3f), 0.2f, "s");

        // --- LFO ---
        choiceParam(lfoShape, "LFO Shape", { "Sine", "Triangle", "Square", "Saw", "Sample & Hold" }, 0);
        floatParam(lfoRate, juce::String("LFO Rate"), juce::NormalisableRange<float>(0.01f, 20.0f, 0.001f, 0.4f), 2.0f, "Hz");
        floatParam(lfoDepth, "LFO Depth", { 0.0f, 1.0f, 0.001f }, 0.0f);
        choiceParam(lfoDestination, "LFO Destination", { "Off", "Pitch", "Filter Cutoff", "Pulse Width" }, 0);

        // --- Master ---
        floatParam(masterDrive, "Drive", { 0.0f, 1.0f, 0.001f }, 0.1f);
        floatParam(masterGain, "Output Gain", { -24.0f, 6.0f, 0.01f }, 0.0f, "dB");

        // --- Effects ---
        boolParam(chorusEnable, "Chorus On", false);
        floatParam(chorusRate, "Chorus Rate", { 0.05f, 5.0f, 0.001f }, 0.8f, "Hz");
        floatParam(chorusDepth, "Chorus Depth", { 0.0f, 1.0f, 0.001f }, 0.3f);
        floatParam(chorusFeedback, "Chorus Feedback", { -0.9f, 0.9f, 0.001f }, 0.0f);
        floatParam(chorusMix, "Chorus Mix", { 0.0f, 1.0f, 0.001f }, 0.3f);

        boolParam(delayEnable, "Delay On", false);
        floatParam(delayTimeMs, "Delay Time", juce::NormalisableRange<float>(1.0f, 1000.0f, 0.1f, 0.4f), 250.0f, "ms");
        floatParam(delayFeedback, "Delay Feedback", { 0.0f, 0.95f, 0.001f }, 0.35f);
        floatParam(delayMix, "Delay Mix", { 0.0f, 1.0f, 0.001f }, 0.25f);

        boolParam(reverbEnable, "Reverb On", false);
        floatParam(reverbRoom,  "Room Size",  { 0.0f, 1.0f, 0.001f }, 0.5f);
        floatParam(reverbDamp,  "Damping",    { 0.0f, 1.0f, 0.001f }, 0.5f);
        floatParam(reverbWet,   "Reverb Wet", { 0.0f, 1.0f, 0.001f }, 0.25f);
        floatParam(reverbWidth, "Width",      { 0.0f, 1.0f, 0.001f }, 1.0f);

        // --- Voicing ---
        choiceParam(polyphonyMode, "Polyphony Mode", { "Poly", "Mono" }, 0);
        floatParam(glideTimeMs, "Glide Time", juce::NormalisableRange<float>(0.0f, 500.0f, 0.01f, 0.5f), 0.0f, "ms");
        boolParam(legato, "Legato", false);

        return { params.begin(), params.end() };
    }
}
