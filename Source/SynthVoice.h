#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include "DSP/DualOscillator.h"
#include "DSP/WavetableOscillator.h"
#include "DSP/FmOscillator.h"
#include "DSP/MultiModeFilter.h"
#include "DSP/ModLFO.h"
#include "DSP/SynthBand.h"
#include "DSP/LinkwitzRiley.h"
#include "Params.h"

class UndertowVoice : public juce::SynthesiserVoice
{
public:
    explicit UndertowVoice(juce::AudioProcessorValueTreeState& stateToUse);

    bool canPlaySound(juce::SynthesiserSound*) override { return true; }

    void prepare(double sampleRate);

    void startNote(int midiNoteNumber, float velocity,
                    juce::SynthesiserSound*, int currentPitchWheelPosition) override;
    void stopNote(float velocity, bool allowTailOff) override;

    // Used by the processor's hand-rolled mono voice routing: moves to a new
    // note while a previous one is still sounding. Pitch glides smoothly
    // over glideTimeSeconds; the envelopes only retrigger if retriggerEnvelope
    // is true (false = legato, envelopes continue from their current state).
    void glideToNote(int midiNoteNumber, float velocity, float glideTimeSeconds, bool retriggerEnvelope);

    void pitchWheelMoved(int newPitchWheelValue) override;
    void controllerMoved(int controllerNumber, int newControllerValue) override;

    void renderNextBlock(juce::AudioBuffer<float>& outputBuffer,
                          int startSample, int numSamples) override;

private:
    juce::AudioProcessorValueTreeState& apvts;

    DualOscillator oscillator;
    WavetableOscillator wtOscillator;
    FmOscillator fmOscillator;
    MultiModeFilter filterL, filterR;
    juce::ADSR ampEnv;
    juce::ADSR modEnv;
    ModLFO lfo;

    // 3-band engine: three independent oscillator lanes, each band-limited by
    // a Linkwitz-Riley crossover before merging. Crossover filters are per
    // channel (L/R) and per split point.
    SynthBand bands[3]; // 0 = Body, 1 = Character, 2 = Air
    LinkwitzRiley xLowL,  xLowR;   // low crossover  (Body / Character)
    LinkwitzRiley xHighL, xHighR;  // high crossover (Character / Air)
    // For the mid band we need a second pass of HP(low) then LP(high); these
    // hold the mid-band's own filter state so it doesn't share with the others.
    LinkwitzRiley midHPL, midHPR, midLPL, midLPR;

    double sampleRate = 44100.0;
    int currentMidiNote = -1;
    float currentVelocity = 1.0f;
    float pitchWheelSemitones = 0.0f;

    // Glide (portamento) state: currentNoteValue chases targetNoteValue
    // linearly in semitone space, advanced one sample at a time so the
    // ramp is audio-rate smooth regardless of host block size.
    float currentNoteValue = 60.0f;
    float targetNoteValue = 60.0f;
    float noteValueIncrement = 0.0f;
    int glideSamplesRemaining = 0;

    float computeNoteValue(int midiNoteNumber) const;
    void setGlideTarget(float newTargetNoteValue, float glideTimeSeconds);

    void configureBandsFromState();

    // Cached pointers into the host-automatable parameter tree.
    std::atomic<float>* pOscModel = nullptr;
    std::atomic<float>* pOscWaveA = nullptr;
    std::atomic<float>* pOscWaveB = nullptr;
    std::atomic<float>* pOscBGain = nullptr;
    std::atomic<float>* pOscBPitch = nullptr;
    std::atomic<float>* pHardSync = nullptr;
    std::atomic<float>* pPulseWidth = nullptr;
    std::atomic<float>* pOscOctave = nullptr;
    std::atomic<float>* pOscSemitone = nullptr;
    std::atomic<float>* pOscFineCents = nullptr;
    std::atomic<float>* pUnisonVoices = nullptr;
    std::atomic<float>* pUnisonDetune = nullptr;
    std::atomic<float>* pUnisonSpread = nullptr;
    std::atomic<float>* pWtPosition = nullptr;
    std::atomic<float>* pWtWarp = nullptr;
    std::atomic<float>* pFmRatio = nullptr;
    std::atomic<float>* pFmIndex = nullptr;
    std::atomic<float>* pFmFeedback = nullptr;

    std::atomic<float>* pBandsEnable = nullptr;
    std::atomic<float>* pAmAmount = nullptr;
    std::atomic<float>* pAmTarget = nullptr;
    std::atomic<float>* pCrossoverLow = nullptr;
    std::atomic<float>* pCrossoverHigh = nullptr;
    std::atomic<float>* pBodyModel = nullptr;
    std::atomic<float>* pBodyGain = nullptr;
    std::atomic<float>* pBodyPan = nullptr;
    std::atomic<float>* pBodyPitch = nullptr;
    std::atomic<float>* pBodyActive = nullptr;
    std::atomic<float>* pBodyFilterOn = nullptr;
    std::atomic<float>* pBodyFilterType = nullptr;
    std::atomic<float>* pBodyFilterCut = nullptr;
    std::atomic<float>* pBodyFilterRes = nullptr;
    std::atomic<float>* pBodyDrive = nullptr;
    std::atomic<float>* pCharModel = nullptr;
    std::atomic<float>* pCharGain = nullptr;
    std::atomic<float>* pCharPan = nullptr;
    std::atomic<float>* pCharPitch = nullptr;
    std::atomic<float>* pCharActive = nullptr;
    std::atomic<float>* pCharFilterOn = nullptr;
    std::atomic<float>* pCharFilterType = nullptr;
    std::atomic<float>* pCharFilterCut = nullptr;
    std::atomic<float>* pCharFilterRes = nullptr;
    std::atomic<float>* pCharDrive = nullptr;
    std::atomic<float>* pAirModel = nullptr;
    std::atomic<float>* pAirGain = nullptr;
    std::atomic<float>* pAirPan = nullptr;
    std::atomic<float>* pAirPitch = nullptr;
    std::atomic<float>* pAirActive = nullptr;
    std::atomic<float>* pAirFilterOn = nullptr;
    std::atomic<float>* pAirFilterType = nullptr;
    std::atomic<float>* pAirFilterCut = nullptr;
    std::atomic<float>* pAirFilterRes = nullptr;
    std::atomic<float>* pAirDrive = nullptr;

    std::atomic<float>* pFilterType = nullptr;
    std::atomic<float>* pFilterCutoff = nullptr;
    std::atomic<float>* pFilterResonance = nullptr;
    std::atomic<float>* pFilterDrive = nullptr;
    std::atomic<float>* pFilterEnvAmount = nullptr;
    std::atomic<float>* pFilterKeyTrack = nullptr;

    std::atomic<float>* pAmpAttack = nullptr;
    std::atomic<float>* pAmpDecay = nullptr;
    std::atomic<float>* pAmpSustain = nullptr;
    std::atomic<float>* pAmpRelease = nullptr;

    std::atomic<float>* pModAttack = nullptr;
    std::atomic<float>* pModDecay = nullptr;
    std::atomic<float>* pModSustain = nullptr;
    std::atomic<float>* pModRelease = nullptr;

    std::atomic<float>* pLfoShape = nullptr;
    std::atomic<float>* pLfoRate = nullptr;
    std::atomic<float>* pLfoDepth = nullptr;
    std::atomic<float>* pLfoDestination = nullptr;

    void updateParametersFromState();
};
