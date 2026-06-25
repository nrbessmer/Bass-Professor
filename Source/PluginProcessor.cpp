#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "DSP/Utils.h"
#include "Presets.h"

UndertowAudioProcessor::UndertowAudioProcessor()
    : AudioProcessor(BusesProperties()
                          .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      apvts(*this, nullptr, "PARAMETERS", Params::createParameterLayout()),
      monoVoice(apvts)
{
    for (int i = 0; i < numVoices; ++i)
        synth.addVoice(new UndertowVoice(apvts));

    synth.addSound(new UndertowSound());
}

void UndertowAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    synth.setCurrentPlaybackSampleRate(sampleRate);

    for (int i = 0; i < synth.getNumVoices(); ++i)
        if (auto* voice = dynamic_cast<UndertowVoice*>(synth.getVoice(i)))
            voice->prepare(sampleRate);

    monoVoice.prepare(sampleRate);
    heldNotes.clear();

    juce::dsp::ProcessSpec spec { sampleRate, (juce::uint32) samplesPerBlock, 2 };
    chorus.prepare(spec);
    delay.prepare(sampleRate, samplesPerBlock);
    reverb.prepare(spec);
    reverb.reset();
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool UndertowAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    return layouts.getMainOutputChannelSet() == juce::AudioChannelSet::stereo()
        || layouts.getMainOutputChannelSet() == juce::AudioChannelSet::mono();
}
#endif

void UndertowAudioProcessor::applyMonoMidiEvent(const juce::MidiMessage& msg)
{
    const float glideTimeSeconds = apvts.getRawParameterValue(Params::glideTimeMs)->load() / 1000.0f;
    const bool legatoOn = apvts.getRawParameterValue(Params::legato)->load() > 0.5f;

    if (msg.isNoteOn())
    {
        const int note = msg.getNoteNumber();
        const float vel = msg.getFloatVelocity();
        const bool wasEmpty = heldNotes.empty();

        // Re-pressing an already-held note (e.g. retrigger) just moves
        // it to the top of the priority stack.
        heldNotes.erase(std::remove_if(heldNotes.begin(), heldNotes.end(),
                                        [note](const auto& n) { return n.first == note; }),
                         heldNotes.end());
        heldNotes.emplace_back(note, vel);

        if (wasEmpty)
            monoVoice.startNote(note, vel, nullptr, 8192);
        else
            monoVoice.glideToNote(note, vel, glideTimeSeconds, !legatoOn);
    }
    else if (msg.isNoteOff())
    {
        const int note = msg.getNoteNumber();
        heldNotes.erase(std::remove_if(heldNotes.begin(), heldNotes.end(),
                                        [note](const auto& n) { return n.first == note; }),
                         heldNotes.end());

        if (heldNotes.empty())
        {
            monoVoice.stopNote(0.0f, true);
        }
        else
        {
            // Last-note priority: fall back to whichever held note was
            // most recently pressed.
            const auto& fallback = heldNotes.back();
            monoVoice.glideToNote(fallback.first, fallback.second, glideTimeSeconds, !legatoOn);
        }
    }
    else if (msg.isPitchWheel())
    {
        monoVoice.pitchWheelMoved(msg.getPitchWheelValue());
    }
    else if (msg.isController())
    {
        monoVoice.controllerMoved(msg.getControllerNumber(), msg.getControllerValue());
    }
}

void UndertowAudioProcessor::renderMonoBlock(juce::AudioBuffer<float>& buffer,
                                              const juce::MidiBuffer& midiMessages,
                                              int startSample, int numSamples)
{
    // Process MIDI events at their sample-accurate positions within the block,
    // splitting the render into sub-blocks between events — the same approach
    // the JUCE Synthesiser uses for poly voices. Without this, a NoteOn and
    // NoteOff that land in the same audio buffer both get processed before any
    // rendering happens, causing the voice to start in release mode and skip
    // the sustain phase entirely (sounds "very short").
    int blockStart = startSample;

    for (const auto metadata : midiMessages)
    {
        const int eventPos = juce::jlimit(startSample, startSample + numSamples - 1,
                                           metadata.samplePosition);

        // Render the sub-block leading up to this event.
        if (eventPos > blockStart)
        {
            monoVoice.renderNextBlock(buffer, blockStart, eventPos - blockStart);
            blockStart = eventPos;
        }

        applyMonoMidiEvent(metadata.getMessage());
    }

    // Render the remainder of the block after the last event.
    const int remaining = (startSample + numSamples) - blockStart;
    if (remaining > 0)
        monoVoice.renderNextBlock(buffer, blockStart, remaining);
}

void UndertowAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    buffer.clear();

    const bool monoMode = (int) apvts.getRawParameterValue(Params::polyphonyMode)->load() == 1;

    if (monoMode)
    {
        renderMonoBlock(buffer, midiMessages, 0, buffer.getNumSamples());
    }
    else
    {
        synth.renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());
        // If the user switches mode mid-note, let any still-sounding mono
        // voice finish its release naturally rather than leaving it stuck.
        monoVoice.renderNextBlock(buffer, 0, buffer.getNumSamples());
    }

    const float drive = apvts.getRawParameterValue(Params::masterDrive)->load();
    const float gainDb = apvts.getRawParameterValue(Params::masterGain)->load();
    const float gain = DspUtils::dbToGain(gainDb);

    // Pre-effects drive/saturation stage.
    for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
    {
        auto* data = buffer.getWritePointer(ch);
        for (int s = 0; s < buffer.getNumSamples(); ++s)
        {
            float x = data[s];
            if (drive > 0.0f)
            {
                const float amount = 1.0f + drive * 4.0f;
                x = DspUtils::softClip(x * amount) / std::tanh(amount);
            }
            data[s] = x;
        }
    }

    // Chorus
    if (apvts.getRawParameterValue(Params::chorusEnable)->load() > 0.5f)
    {
        chorus.setRate(apvts.getRawParameterValue(Params::chorusRate)->load());
        chorus.setDepth(apvts.getRawParameterValue(Params::chorusDepth)->load());
        chorus.setFeedback(apvts.getRawParameterValue(Params::chorusFeedback)->load());
        chorus.setMix(apvts.getRawParameterValue(Params::chorusMix)->load());
        chorus.setCentreDelay(7.0f);

        juce::dsp::AudioBlock<float> block(buffer);
        juce::dsp::ProcessContextReplacing<float> context(block);
        chorus.process(context);
    }

    // Delay
    if (buffer.getNumChannels() >= 2 && apvts.getRawParameterValue(Params::delayEnable)->load() > 0.5f)
    {
        delay.setParameters(apvts.getRawParameterValue(Params::delayTimeMs)->load(),
                             apvts.getRawParameterValue(Params::delayFeedback)->load(),
                             apvts.getRawParameterValue(Params::delayMix)->load());

        auto* left = buffer.getWritePointer(0);
        auto* right = buffer.getWritePointer(1);
        delay.processBlock(left, right, buffer.getNumSamples());
    }

    // Reverb (Freeverb-based — room, shimmer/air, and width)
    if (apvts.getRawParameterValue(Params::reverbEnable)->load() > 0.5f)
    {
        juce::dsp::Reverb::Parameters rp;
        rp.roomSize   = apvts.getRawParameterValue(Params::reverbRoom)->load();
        rp.damping    = apvts.getRawParameterValue(Params::reverbDamp)->load();
        const float wet = apvts.getRawParameterValue(Params::reverbWet)->load();
        rp.wetLevel   = wet;
        rp.dryLevel   = 1.0f - wet;
        rp.width      = apvts.getRawParameterValue(Params::reverbWidth)->load();
        rp.freezeMode = 0.0f;
        reverb.setParameters(rp);

        juce::dsp::AudioBlock<float> block(buffer);
        juce::dsp::ProcessContextReplacing<float> ctx(block);
        reverb.process(ctx);
    }

    // Final output gain.
    for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
    {
        auto* data = buffer.getWritePointer(ch);
        for (int s = 0; s < buffer.getNumSamples(); ++s)
            data[s] *= gain;
    }
}

juce::AudioProcessorEditor* UndertowAudioProcessor::createEditor()
{
    return new UndertowAudioProcessorEditor(*this);
}

void UndertowAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    if (auto state = apvts.copyState(); state.isValid())
    {
        // Stash the current preset index alongside the parameter state so the
        // selector shows the right name when the project reopens.
        state.setProperty("currentPreset", currentPreset, nullptr);
        if (auto xml = state.createXml())
            copyXmlToBinary(*xml, destData);
    }
}

void UndertowAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    if (auto xml = getXmlFromBinary(data, sizeInBytes))
    {
        if (xml->hasTagName(apvts.state.getType()))
        {
            auto tree = juce::ValueTree::fromXml(*xml);
            if (tree.hasProperty("currentPreset"))
                currentPreset = (int) tree.getProperty("currentPreset");
            apvts.replaceState(tree);
        }
    }
}

// --- Presets ---

int UndertowAudioProcessor::getNumBassPresets() const
{
    return (int) BPPresets::getAll().size();
}

juce::StringArray UndertowAudioProcessor::getBassPresetNames() const
{
    juce::StringArray names;
    for (const auto& p : BPPresets::getAll())
        names.add(juce::String(p.name));
    return names;
}

juce::String UndertowAudioProcessor::getBassPresetCategory(int index) const
{
    const auto& all = BPPresets::getAll();
    if (index < 0 || index >= (int) all.size())
        return {};
    return juce::String(all[(size_t) index].category);
}

juce::String UndertowAudioProcessor::getBassPresetDescription(int index) const
{
    const auto& all = BPPresets::getAll();
    if (index < 0 || index >= (int) all.size())
        return {};
    return juce::String(all[(size_t) index].description);
}

void UndertowAudioProcessor::applyBassPreset(int index)
{
    const auto& all = BPPresets::getAll();
    if (index < 0 || index >= (int) all.size())
        return;

    // Reset every parameter to its default first, so presets are deterministic
    // regardless of what was dialled in before.
    for (auto* p : getParameters())
        p->setValueNotifyingHost(p->getDefaultValue());

    // Apply this preset's overrides (real values converted to normalized).
    for (const auto& [id, value] : all[(size_t) index].params)
        if (auto* rp = apvts.getParameter(juce::String(id)))
            rp->setValueNotifyingHost(rp->convertTo0to1(value));

    currentPreset = index;
}

// This creates new instances of the plugin
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new UndertowAudioProcessor();
}
