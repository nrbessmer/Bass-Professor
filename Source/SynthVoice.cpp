#include "SynthVoice.h"

namespace
{
    DualOscillator::Waveform toWaveformA(int index)
    {
        switch (index)
        {
            case 0: return DualOscillator::Waveform::Saw;
            case 1: return DualOscillator::Waveform::Square;
            case 2: return DualOscillator::Waveform::Triangle;
            case 3: default: return DualOscillator::Waveform::Sine;
        }
    }

    DualOscillator::Waveform toWaveformB(int index)
    {
        switch (index)
        {
            case 0: return DualOscillator::Waveform::Saw;
            case 1: return DualOscillator::Waveform::Square;
            case 2: return DualOscillator::Waveform::Triangle;
            case 3: return DualOscillator::Waveform::Sine;
            case 4: default: return DualOscillator::Waveform::Noise;
        }
    }

    MultiModeFilter::Type toFilterType(int index)
    {
        switch (index)
        {
            case 0: return MultiModeFilter::Type::LowPass;
            case 1: return MultiModeFilter::Type::HighPass;
            case 2: return MultiModeFilter::Type::BandPass;
            case 3: default: return MultiModeFilter::Type::Notch;
        }
    }

    ModLFO::Shape toLfoShape(int index)
    {
        switch (index)
        {
            case 0: return ModLFO::Shape::Sine;
            case 1: return ModLFO::Shape::Triangle;
            case 2: return ModLFO::Shape::Square;
            case 3: return ModLFO::Shape::Saw;
            case 4: default: return ModLFO::Shape::SampleHold;
        }
    }

    // 0 = Off, 1 = Pitch, 2 = Filter Cutoff, 3 = Pulse Width
    enum class LfoDest { Off, Pitch, FilterCutoff, PulseWidth };
}

UndertowVoice::UndertowVoice(juce::AudioProcessorValueTreeState& stateToUse)
    : apvts(stateToUse)
{
    pOscModel       = apvts.getRawParameterValue(Params::oscModel);
    pOscWaveA       = apvts.getRawParameterValue(Params::oscWaveA);
    pOscWaveB       = apvts.getRawParameterValue(Params::oscWaveB);
    pOscBGain       = apvts.getRawParameterValue(Params::oscBGain);
    pOscBPitch      = apvts.getRawParameterValue(Params::oscBPitch);
    pHardSync       = apvts.getRawParameterValue(Params::hardSync);
    pPulseWidth     = apvts.getRawParameterValue(Params::pulseWidth);
    pOscOctave      = apvts.getRawParameterValue(Params::oscOctave);
    pOscSemitone    = apvts.getRawParameterValue(Params::oscSemitone);
    pOscFineCents   = apvts.getRawParameterValue(Params::oscFineCents);
    pUnisonVoices   = apvts.getRawParameterValue(Params::unisonVoices);
    pUnisonDetune   = apvts.getRawParameterValue(Params::unisonDetune);
    pUnisonSpread   = apvts.getRawParameterValue(Params::unisonSpread);
    pWtPosition     = apvts.getRawParameterValue(Params::wtPosition);
    pWtWarp         = apvts.getRawParameterValue(Params::wtWarp);
    pFmRatio        = apvts.getRawParameterValue(Params::fmRatio);
    pFmIndex        = apvts.getRawParameterValue(Params::fmIndex);
    pFmFeedback     = apvts.getRawParameterValue(Params::fmFeedback);

    pBandsEnable    = apvts.getRawParameterValue(Params::bandsEnable);
    pAmAmount       = apvts.getRawParameterValue(Params::amAmount);
    pAmTarget       = apvts.getRawParameterValue(Params::amTarget);
    pCrossoverLow   = apvts.getRawParameterValue(Params::crossoverLow);
    pCrossoverHigh  = apvts.getRawParameterValue(Params::crossoverHigh);
    pBodyModel      = apvts.getRawParameterValue(Params::bodyModel);
    pBodyGain       = apvts.getRawParameterValue(Params::bodyGain);
    pBodyPan        = apvts.getRawParameterValue(Params::bodyPan);
    pBodyPitch      = apvts.getRawParameterValue(Params::bodyPitch);
    pBodyActive     = apvts.getRawParameterValue(Params::bodyActive);
    pBodyFilterOn   = apvts.getRawParameterValue(Params::bodyFilterOn);
    pBodyFilterType = apvts.getRawParameterValue(Params::bodyFilterType);
    pBodyFilterCut  = apvts.getRawParameterValue(Params::bodyFilterCut);
    pBodyFilterRes  = apvts.getRawParameterValue(Params::bodyFilterRes);
    pBodyDrive      = apvts.getRawParameterValue(Params::bodyDrive);
    pCharModel      = apvts.getRawParameterValue(Params::charModel);
    pCharGain       = apvts.getRawParameterValue(Params::charGain);
    pCharPan        = apvts.getRawParameterValue(Params::charPan);
    pCharPitch      = apvts.getRawParameterValue(Params::charPitch);
    pCharActive     = apvts.getRawParameterValue(Params::charActive);
    pCharFilterOn   = apvts.getRawParameterValue(Params::charFilterOn);
    pCharFilterType = apvts.getRawParameterValue(Params::charFilterType);
    pCharFilterCut  = apvts.getRawParameterValue(Params::charFilterCut);
    pCharFilterRes  = apvts.getRawParameterValue(Params::charFilterRes);
    pCharDrive      = apvts.getRawParameterValue(Params::charDrive);
    pAirModel       = apvts.getRawParameterValue(Params::airModel);
    pAirGain        = apvts.getRawParameterValue(Params::airGain);
    pAirPan         = apvts.getRawParameterValue(Params::airPan);
    pAirPitch       = apvts.getRawParameterValue(Params::airPitch);
    pAirActive      = apvts.getRawParameterValue(Params::airActive);
    pAirFilterOn    = apvts.getRawParameterValue(Params::airFilterOn);
    pAirFilterType  = apvts.getRawParameterValue(Params::airFilterType);
    pAirFilterCut   = apvts.getRawParameterValue(Params::airFilterCut);
    pAirFilterRes   = apvts.getRawParameterValue(Params::airFilterRes);
    pAirDrive       = apvts.getRawParameterValue(Params::airDrive);

    pFilterType       = apvts.getRawParameterValue(Params::filterType);
    pFilterCutoff     = apvts.getRawParameterValue(Params::filterCutoff);
    pFilterResonance  = apvts.getRawParameterValue(Params::filterResonance);
    pFilterDrive      = apvts.getRawParameterValue(Params::filterDrive);
    pFilterEnvAmount  = apvts.getRawParameterValue(Params::filterEnvAmount);
    pFilterKeyTrack   = apvts.getRawParameterValue(Params::filterKeyTrack);

    pAmpAttack  = apvts.getRawParameterValue(Params::ampAttack);
    pAmpDecay   = apvts.getRawParameterValue(Params::ampDecay);
    pAmpSustain = apvts.getRawParameterValue(Params::ampSustain);
    pAmpRelease = apvts.getRawParameterValue(Params::ampRelease);

    pModAttack  = apvts.getRawParameterValue(Params::modAttack);
    pModDecay   = apvts.getRawParameterValue(Params::modDecay);
    pModSustain = apvts.getRawParameterValue(Params::modSustain);
    pModRelease = apvts.getRawParameterValue(Params::modRelease);

    pLfoShape       = apvts.getRawParameterValue(Params::lfoShape);
    pLfoRate        = apvts.getRawParameterValue(Params::lfoRate);
    pLfoDepth       = apvts.getRawParameterValue(Params::lfoDepth);
    pLfoDestination = apvts.getRawParameterValue(Params::lfoDestination);
}

void UndertowVoice::prepare(double newSampleRate)
{
    sampleRate = newSampleRate;
    oscillator.prepare(sampleRate);
    wtOscillator.prepare(sampleRate);
    fmOscillator.prepare(sampleRate);
    filterL.prepare(sampleRate);
    filterR.prepare(sampleRate);
    lfo.prepare(sampleRate);
    ampEnv.setSampleRate(sampleRate);
    modEnv.setSampleRate(sampleRate);

    for (auto& b : bands)
        b.prepare(sampleRate);

    xLowL.setType(LinkwitzRiley::Type::LowPass);   xLowL.prepare(sampleRate);
    xLowR.setType(LinkwitzRiley::Type::LowPass);   xLowR.prepare(sampleRate);
    xHighL.setType(LinkwitzRiley::Type::HighPass); xHighL.prepare(sampleRate);
    xHighR.setType(LinkwitzRiley::Type::HighPass); xHighR.prepare(sampleRate);
    midHPL.setType(LinkwitzRiley::Type::HighPass); midHPL.prepare(sampleRate);
    midHPR.setType(LinkwitzRiley::Type::HighPass); midHPR.prepare(sampleRate);
    midLPL.setType(LinkwitzRiley::Type::LowPass);  midLPL.prepare(sampleRate);
    midLPR.setType(LinkwitzRiley::Type::LowPass);  midLPR.prepare(sampleRate);
}

float UndertowVoice::computeNoteValue(int midiNoteNumber) const
{
    return (float) midiNoteNumber;
}

void UndertowVoice::setGlideTarget(float newTargetNoteValue, float glideTimeSeconds)
{
    targetNoteValue = newTargetNoteValue;

    if (glideTimeSeconds <= 0.0005f)
    {
        currentNoteValue = newTargetNoteValue;
        noteValueIncrement = 0.0f;
        glideSamplesRemaining = 0;
    }
    else
    {
        const int totalSamples = juce::jmax(1, (int) (glideTimeSeconds * sampleRate));
        noteValueIncrement = (targetNoteValue - currentNoteValue) / (float) totalSamples;
        glideSamplesRemaining = totalSamples;
    }
}

void UndertowVoice::startNote(int midiNoteNumber, float velocity,
                               juce::SynthesiserSound*, int currentPitchWheelPosition)
{
    // If the voice is currently audible (deClickGain > 0 means it's producing
    // output) we must NOT zero deClickGain immediately — that hard-cuts the
    // signal mid-sample and creates an audible click. Instead, store the
    // incoming note parameters and let the declick ramp fade to silence first.
    // renderNextBlock()'s retrigger handler will apply everything once quiet.
    if (deClickGain > 0.0f && currentMidiNote >= 0)
    {
        pendingMidiNote   = midiNoteNumber;
        pendingVelocity   = velocity;
        pendingPitchWheel = currentPitchWheelPosition;
        deClickTarget     = 0.0f;
        deClickPendingClear = false;
        deClickRetrigger  = true;
        return;
    }

    // Voice is already silent — start immediately.
    currentMidiNote = midiNoteNumber;
    currentVelocity = velocity;
    pitchWheelSemitones = ((float) currentPitchWheelPosition - 8192.0f) / 8192.0f * 2.0f;

    updateParametersFromState();

    // Fresh note: no glide, full retrigger.
    const float noteValue = computeNoteValue(midiNoteNumber);
    currentNoteValue = noteValue;
    setGlideTarget(noteValue, 0.0f);

    oscillator.resetPhases();
    wtOscillator.resetPhases();
    fmOscillator.resetPhases();
    for (auto& b : bands)
        b.resetPhases();
    xLowL.reset(); xLowR.reset(); xHighL.reset(); xHighR.reset();
    midHPL.reset(); midHPR.reset(); midLPL.reset(); midLPR.reset();
    lfo.resetPhase();
    filterL.reset();
    filterR.reset();

    // Ramp from silence to prevent onset click regardless of attack time.
    deClickGain = 0.0f;
    deClickTarget = 1.0f;
    deClickPendingClear = false;
    deClickRetrigger = false;
    pendingMidiNote = -1;

    // Reset output smoother so it doesn't carry over state from a previous note.
    outSmoothL = 0.0f;
    outSmoothR = 0.0f;

    ampEnv.reset();
    ampEnv.noteOn();
    modEnv.reset();
    modEnv.noteOn();
}

void UndertowVoice::glideToNote(int midiNoteNumber, float velocity, float glideTimeSeconds, bool retriggerEnvelope)
{
    currentMidiNote = midiNoteNumber;
    currentVelocity = velocity;

    updateParametersFromState();

    const float noteValue = computeNoteValue(midiNoteNumber);
    setGlideTarget(noteValue, glideTimeSeconds);

    // Oscillator phase and filter state are left running — that's what
    // makes this sound like a slide rather than a re-pluck.
    if (retriggerEnvelope)
    {
        // Fade out first so ampEnv.noteOn() restarts from 0 without a
        // mid-envelope discontinuity click. renderNextBlock() completes
        // the retrigger once the ramp reaches silence.
        deClickTarget    = 0.0f;
        deClickRetrigger = true;
        pendingNoteOff   = false;
        // A previous startNote() may have stored a pending note while the
        // voice was still audible. That pending note is now superseded by
        // this glide; clear it so the retrigger handler doesn't re-apply it.
        pendingMidiNote  = -1;
    }
}

void UndertowVoice::stopNote(float, bool allowTailOff)
{
    if (allowTailOff)
    {
        if (deClickRetrigger)
        {
            // A retrigger fade is in progress — the pending noteOn hasn't fired yet.
            // Flag the note-off so the retrigger handler can issue it immediately
            // after noteOn(), giving the note its proper attack+release shape rather
            // than leaving the voice stuck open indefinitely.
            pendingNoteOff = true;
            return;
        }

        // Normal note-off: trigger ADSR release.
        ampEnv.noteOff();
        modEnv.noteOff();

        // If the onset declick ramp hasn't finished yet (very quick/light keypress),
        // reverse it back toward 0. Without this, deClickGain keeps climbing while
        // the ADSR is releasing — the filter accumulates signal into a dying note
        // and cuts off abruptly when the voice clears, causing an audible click.
        if (deClickGain < 1.0f)
            deClickTarget = 0.0f;
    }
    else
    {
        // Hard stop (e.g. voice steal). DO NOT reset the envelope here — that
        // immediately zeros ampLevel, making the de-click ramp multiply by 0
        // and do nothing. Instead, leave the envelope running at its current
        // level and ramp *output* gain to 0 via deClickGain. The envelope
        // reset happens in renderNextBlock once the ramp completes.
        deClickTarget = 0.0f;
        deClickPendingClear = true;
    }
}

void UndertowVoice::pitchWheelMoved(int newPitchWheelValue)
{
    pitchWheelSemitones = ((float) newPitchWheelValue - 8192.0f) / 8192.0f * 2.0f;
}

void UndertowVoice::controllerMoved(int, int) {}

void UndertowVoice::configureBandsFromState()
{
    // Each band inherits the shared waveform/unison/morph settings, then
    // applies its own model selection, gain, pan, pitch offset, and on/off.
    const auto waveA = toWaveformA((int) pOscWaveA->load());
    const auto waveB = toWaveformB((int) pOscWaveB->load());
    const float oscBGain = pOscBGain->load();
    const float oscBPitch = pOscBPitch->load();
    const bool hardSync = pHardSync->load() > 0.5f;
    const float pulseWidth = pPulseWidth->load();
    const int uVoices = (int) pUnisonVoices->load();
    const float uDetune = pUnisonDetune->load();
    const float uSpread = pUnisonSpread->load();
    const float wtPos = pWtPosition->load();
    const float wtWarp = pWtWarp->load();
    const float fmRatio = pFmRatio->load();
    const float fmIndex = pFmIndex->load();
    const float fmFeedback = pFmFeedback->load();

    struct BandParams {
        std::atomic<float>*model; std::atomic<float>*gain; std::atomic<float>*pan;
        std::atomic<float>*pitch; std::atomic<float>*active;
        std::atomic<float>*filterOn; std::atomic<float>*filterType;
        std::atomic<float>*filterCut; std::atomic<float>*filterRes;
        std::atomic<float>*drive;
    };
    const BandParams bp[3] = {
        { pBodyModel, pBodyGain, pBodyPan, pBodyPitch, pBodyActive,
          pBodyFilterOn, pBodyFilterType, pBodyFilterCut, pBodyFilterRes, pBodyDrive },
        { pCharModel, pCharGain, pCharPan, pCharPitch, pCharActive,
          pCharFilterOn, pCharFilterType, pCharFilterCut, pCharFilterRes, pCharDrive },
        { pAirModel,  pAirGain,  pAirPan,  pAirPitch,  pAirActive,
          pAirFilterOn, pAirFilterType, pAirFilterCut, pAirFilterRes, pAirDrive },
    };

    for (int i = 0; i < 3; ++i)
    {
        auto& band = bands[i];
        band.analogOsc().setWaveforms(waveA, waveB);
        band.analogOsc().setOscBParams(oscBGain, oscBPitch);
        band.analogOsc().setHardSync(hardSync);
        band.analogOsc().setPulseWidth(pulseWidth);
        band.analogOsc().setUnison(uVoices, uDetune, uSpread);
        band.wavetableOsc().setMorph(wtPos, wtWarp);
        band.wavetableOsc().setUnison(uVoices, uDetune, uSpread);
        band.fmOsc().setFmParams(fmRatio, fmIndex, fmFeedback);
        band.fmOsc().setUnison(uVoices, uDetune, uSpread);

        band.setModel((int) bp[i].model->load());
        band.setGain(bp[i].gain->load());
        band.setPan(bp[i].pan->load());
        band.setPitchOffsetSemitones(bp[i].pitch->load());
        band.setActive(bp[i].active->load() > 0.5f);
        band.setDrive(bp[i].drive->load());

        band.setFilterEnabled(bp[i].filterOn->load() > 0.5f);
        band.setFilterParams(toFilterType((int) bp[i].filterType->load()),
                             bp[i].filterCut->load(),
                             bp[i].filterRes->load(),
                             0.0f);
    }
}

void UndertowVoice::updateParametersFromState()
{
    oscillator.setWaveforms(toWaveformA((int) pOscWaveA->load()),
                             toWaveformB((int) pOscWaveB->load()));
    oscillator.setOscBParams(pOscBGain->load(), pOscBPitch->load());
    oscillator.setHardSync(pHardSync->load() > 0.5f);
    oscillator.setPulseWidth(pPulseWidth->load());
    oscillator.setUnison((int) pUnisonVoices->load(), pUnisonDetune->load(), pUnisonSpread->load());

    wtOscillator.setMorph(pWtPosition->load(), pWtWarp->load());
    wtOscillator.setUnison((int) pUnisonVoices->load(), pUnisonDetune->load(), pUnisonSpread->load());

    fmOscillator.setFmParams(pFmRatio->load(), pFmIndex->load(), pFmFeedback->load());
    fmOscillator.setUnison((int) pUnisonVoices->load(), pUnisonDetune->load(), pUnisonSpread->load());

    filterL.setType(toFilterType((int) pFilterType->load()));
    filterR.setType(toFilterType((int) pFilterType->load()));
    filterL.setDrive(pFilterDrive->load());
    filterR.setDrive(pFilterDrive->load());

    lfo.setShape(toLfoShape((int) pLfoShape->load()));
    lfo.setRate(pLfoRate->load());

    juce::ADSR::Parameters ampParams;
    ampParams.attack  = pAmpAttack->load();
    ampParams.decay   = pAmpDecay->load();
    ampParams.sustain = pAmpSustain->load();
    // Enforce a 5 ms minimum release — prevents zero-release clicks when a key
    // is released during the attack phase before the de-click ramp completes.
    ampParams.release = juce::jmax(0.005f, pAmpRelease->load());
    ampEnv.setParameters(ampParams);

    juce::ADSR::Parameters modParams;
    modParams.attack  = pModAttack->load();
    modParams.decay   = pModDecay->load();
    modParams.sustain = pModSustain->load();
    modParams.release = pModRelease->load();
    modEnv.setParameters(modParams);
}

void UndertowVoice::renderNextBlock(juce::AudioBuffer<float>& outputBuffer,
                                     int startSample, int numSamples)
{
    if (currentMidiNote < 0)
        return;

    updateParametersFromState();

    const auto lfoDest = (LfoDest) (int) pLfoDestination->load();
    const float lfoDepth = pLfoDepth->load();
    const float baseCutoff = pFilterCutoff->load();
    const float resonance = pFilterResonance->load();
    const float envAmount = pFilterEnvAmount->load();
    const float keyTrack = pFilterKeyTrack->load();

    // Tuning offset is read once per block (not sample-accurate) so octave/
    // semitone/fine stay live-automatable without interfering with the
    // per-sample glide ramp, which operates in raw-MIDI-note space.
    const float tuningOffset = pOscOctave->load() * 12.0f + pOscSemitone->load() + pOscFineCents->load() / 100.0f;
    const int oscModel = (int) pOscModel->load(); // 0 Analog, 1 Wavetable, 2 FM

    const bool threeBand = pBandsEnable->load() > 0.5f;
    const float amAmount = pAmAmount->load();
    const int amTarget = (int) pAmTarget->load(); // 0 Off,1 Char,2 Air,3 Both
    if (threeBand)
    {
        configureBandsFromState();

        // Enforce the spec's minimum 1-octave separation between crossovers.
        float fLow = pCrossoverLow->load();
        float fHigh = pCrossoverHigh->load();
        if (fHigh < fLow * 2.0f) fHigh = fLow * 2.0f;

        xLowL.setCutoff(fLow);   xLowR.setCutoff(fLow);
        xHighL.setCutoff(fHigh); xHighR.setCutoff(fHigh);
        midHPL.setCutoff(fLow);  midHPR.setCutoff(fLow);
        midLPL.setCutoff(fHigh); midLPR.setCutoff(fHigh);
    }

    auto* left = outputBuffer.getWritePointer(0, startSample);
    auto* right = outputBuffer.getNumChannels() > 1
                      ? outputBuffer.getWritePointer(1, startSample)
                      : nullptr;

    for (int s = 0; s < numSamples; ++s)
    {
        if (glideSamplesRemaining > 0)
        {
            currentNoteValue += noteValueIncrement;
            if (--glideSamplesRemaining == 0)
                currentNoteValue = targetNoteValue;
        }

        const float fullNoteValue = currentNoteValue + tuningOffset + pitchWheelSemitones;
        const float baseHz = DspUtils::midiToHz(fullNoteValue);
        oscillator.setBaseFrequency(baseHz);
        wtOscillator.setBaseFrequency(baseHz);
        fmOscillator.setBaseFrequency(baseHz);

        const float lfoRaw = lfo.renderSample();
        const float lfoValue = lfoRaw * lfoDepth;

        const float pitchModSemis = (lfoDest == LfoDest::Pitch) ? lfoValue * 12.0f : 0.0f;
        const float pwModOffset = (lfoDest == LfoDest::PulseWidth) ? lfoValue * 0.4f : 0.0f;

        float oscL = 0.0f, oscR = 0.0f;
        if (threeBand)
        {
            // Each band generates its own signal, gets band-limited to its
            // spectral region by the crossover, then all three merge.
            for (auto& b : bands)
                b.setBaseFrequency(baseHz);

            float bL = 0.0f, bR = 0.0f;   // Body
            float cL = 0.0f, cR = 0.0f;   // Character
            float aL = 0.0f, aR = 0.0f;   // Air
            bands[0].renderSample(pitchModSemis, pwModOffset, bL, bR);
            bands[1].renderSample(pitchModSemis, pwModOffset, cL, cR);
            bands[2].renderSample(pitchModSemis, pwModOffset, aL, aR);

            // Body  = low-passed at fLow
            float bodyL = xLowL.processSample(bL);
            float bodyR = xLowR.processSample(bR);
            // Character = high-passed at fLow then low-passed at fHigh (bandpass)
            float charL = midLPL.processSample(midHPL.processSample(cL));
            float charR = midLPR.processSample(midHPR.processSample(cR));
            // Air = high-passed at fHigh
            float airL = xHighL.processSample(aL);
            float airR = xHighR.processSample(aR);

            // Optional per-band filter, applied to each band's isolated signal.
            bands[0].applyFilter(bodyL, bodyR);
            bands[1].applyFilter(charL, charR);
            bands[2].applyFilter(airL, airR);

            // AM Injector: the Body band amplitude-modulates the upper band(s),
            // creating sidebands locked to the low-band content. True AM form
            // carrier * (1 + depth * modulator), so amount=0 is the dry carrier.
            if (amAmount > 0.0f && amTarget != 0)
            {
                const float modL = bodyL;
                const float modR = bodyR;
                if (amTarget == 1 || amTarget == 3) // Character
                {
                    charL *= (1.0f + amAmount * modL);
                    charR *= (1.0f + amAmount * modR);
                }
                if (amTarget == 2 || amTarget == 3) // Air
                {
                    airL *= (1.0f + amAmount * modL);
                    airR *= (1.0f + amAmount * modR);
                }
            }

            oscL = bodyL + charL + airL;
            oscR = bodyR + charR + airR;
        }
        else if (oscModel == 1)
        {
            wtOscillator.renderSample(pitchModSemis, pwModOffset, oscL, oscR);
        }
        else if (oscModel == 2)
        {
            fmOscillator.renderSample(pitchModSemis, pwModOffset, oscL, oscR);
        }
        else
        {
            oscillator.renderSample(pitchModSemis, pwModOffset, oscL, oscR);
        }

        const float ampLevel = ampEnv.getNextSample();
        const float modLevel = modEnv.getNextSample();

        const float keyTrackOctaves = keyTrack * (currentNoteValue - 60.0f) / 12.0f;
        const float lfoCutoffOctaves = (lfoDest == LfoDest::FilterCutoff) ? lfoValue * 2.0f : 0.0f;
        const float totalOctaveShift = envAmount * modLevel * 4.0f + keyTrackOctaves + lfoCutoffOctaves;
        const float cutoffHz = juce::jlimit(20.0f, 20000.0f,
                                             baseCutoff * std::pow(2.0f, totalOctaveShift));

        filterL.setCutoffAndResonance(cutoffHz, resonance);
        filterR.setCutoffAndResonance(cutoffHz, resonance);

        // Advance the de-click ramp BEFORE feeding the filter.
        // If we advanced it after, the filter would accumulate full-amplitude
        // oscillator signal while deClickGain was still 0, then release that
        // stored energy as a burst/click once the ramp completed.
        if (deClickGain < deClickTarget)
            deClickGain = juce::jmin(deClickTarget, deClickGain + kDeClickRate);
        else if (deClickGain > deClickTarget)
            deClickGain = juce::jmax(deClickTarget, deClickGain - kDeClickRate);

        // Gate the oscillator INTO the filter so filter state builds/decays
        // with the ramp rather than accumulating at full amplitude.
        const float filteredL = filterL.processSample(oscL * deClickGain);
        const float filteredR = filterR.processSample(oscR * deClickGain);

        // Also apply deClickGain to the OUTPUT. A resonant filter continues to
        // ring (output signal) even when its input is 0 — without this second
        // gate that ringing plays at full volume = audible crackle on note changes.
        const float gain = ampLevel * currentVelocity * deClickGain;

        // One-pole output smoother: spreads any remaining amplitude discontinuity
        // over ~2 samples. Bass content (<2kHz) is unaffected; ultra-fast clicks are caught.
        const float rawL = filteredL * gain;
        const float rawR = filteredR * gain;
        outSmoothL += kOutSmoothAlpha * (rawL - outSmoothL);
        outSmoothR += kOutSmoothAlpha * (rawR - outSmoothR);

        if (left != nullptr)  left[s]  += outSmoothL;
        if (right != nullptr) right[s] += outSmoothR;

        // Normal ADSR voice-done check. Skip if a retrigger is pending — the
        // retrigger handler will reactivate the envelope; clearing here would
        // swallow the incoming note.
        if (!deClickRetrigger && !ampEnv.isActive() && ampLevel <= 0.0001f)
        {
            clearCurrentNote();
            currentMidiNote = -1;
            break;
        }

        // De-click action when gain reaches 0.
        if (deClickGain <= 0.0f)
        {
            if (deClickRetrigger)
            {
                // If this retrigger was initiated by startNote() while the voice
                // was audible, pendingMidiNote holds the new note's params.
                // Apply them now that we're at silence (safe to reset everything).
                if (pendingMidiNote >= 0)
                {
                    currentMidiNote = pendingMidiNote;
                    currentVelocity = pendingVelocity;
                    pitchWheelSemitones = ((float) pendingPitchWheel - 8192.0f) / 8192.0f * 2.0f;

                    const float noteValue = computeNoteValue(pendingMidiNote);
                    currentNoteValue = noteValue;
                    setGlideTarget(noteValue, 0.0f);

                    // Fresh-start reset: oscillator phases, LFO, envelopes.
                    oscillator.resetPhases();
                    wtOscillator.resetPhases();
                    fmOscillator.resetPhases();
                    for (auto& b : bands)
                        b.resetPhases();
                    lfo.resetPhase();

                    pendingMidiNote = -1;
                }

                // Reset filter state — the resonant filter stores energy from
                // the previous note and would ring into the new note as gain
                // ramps back up, causing the audible crackle. Safe to reset
                // here because output is already 0 (deClickGain == 0).
                filterL.reset();
                filterR.reset();
                xLowL.reset();  xLowR.reset();
                xHighL.reset(); xHighR.reset();
                midHPL.reset(); midHPR.reset();
                midLPL.reset(); midLPR.reset();

                // Always reset envelopes before noteOn so the attack shape is
                // clean regardless of which envelope state the previous note
                // left behind. (Legato never sets deClickRetrigger, so this
                // path only runs for non-legato retriggering.)
                // Also reset the output smoother — deClickGain is 0 here so
                // output is already 0; starting the smoother at 0 prevents
                // any leftover state from decaying into the new note's attack.
                outSmoothL = 0.0f;
                outSmoothR = 0.0f;
                ampEnv.reset();
                modEnv.reset();
                ampEnv.noteOn();
                modEnv.noteOn();

                // If the key was released while the retrigger fade was in
                // progress the noteOff was deferred. Apply it now so the note
                // follows its proper attack → release shape rather than
                // sustaining indefinitely.
                if (pendingNoteOff)
                {
                    ampEnv.noteOff();
                    modEnv.noteOff();
                    pendingNoteOff = false;
                }

                deClickTarget = 1.0f;
                deClickRetrigger = false;
            }
            else if (deClickPendingClear)
            {
                // Hard-stop: envelopes were kept running during the fade;
                // reset them now that gain is at zero.
                ampEnv.reset();
                modEnv.reset();
                clearCurrentNote();
                currentMidiNote = -1;
                deClickPendingClear = false;
                break;
            }
        }
    }
}
