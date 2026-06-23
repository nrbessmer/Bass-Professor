#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "DSP/Utils.h"

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
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool UndertowAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    return layouts.getMainOutputChannelSet() == juce::AudioChannelSet::stereo()
        || layouts.getMainOutputChannelSet() == juce::AudioChannelSet::mono();
}
#endif

void UndertowAudioProcessor::handleMonoMidi(const juce::MidiBuffer& midiMessages)
{
    const float glideTimeSeconds = apvts.getRawParameterValue(Params::glideTimeMs)->load() / 1000.0f;
    const bool legatoOn = apvts.getRawParameterValue(Params::legato)->load() > 0.5f;

    for (const auto metadata : midiMessages)
    {
        const auto msg = metadata.getMessage();

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
}

void UndertowAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    buffer.clear();

    const bool monoMode = (int) apvts.getRawParameterValue(Params::polyphonyMode)->load() == 1;

    if (monoMode)
    {
        handleMonoMidi(midiMessages);
        monoVoice.renderNextBlock(buffer, 0, buffer.getNumSamples());
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
        if (auto xml = state.createXml())
            copyXmlToBinary(*xml, destData);
    }
}

void UndertowAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    if (auto xml = getXmlFromBinary(data, sizeInBytes))
        if (xml->hasTagName(apvts.state.getType()))
            apvts.replaceState(juce::ValueTree::fromXml(*xml));
}

// This creates new instances of the plugin
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new UndertowAudioProcessor();
}
