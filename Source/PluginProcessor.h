#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_audio_utils/juce_audio_utils.h>
#include <juce_dsp/juce_dsp.h>
#include "Params.h"
#include "SynthSound.h"
#include "SynthVoice.h"
#include "DSP/StereoDelay.h"

class UndertowAudioProcessor : public juce::AudioProcessor
{
public:
    UndertowAudioProcessor();
    ~UndertowAudioProcessor() override = default;

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override {}

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
   #endif

    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override { return JucePlugin_Name; }

    bool acceptsMidi() const override { return true; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 2.0; }

    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram(int) override {}
    const juce::String getProgramName(int) override { return {}; }
    void changeProgramName(int, const juce::String&) override {}

    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    juce::AudioProcessorValueTreeState apvts;

private:
    static constexpr int numVoices = 8;
    juce::Synthesiser synth;

    // Mono mode bypasses juce::Synthesiser's voice-allocation entirely and
    // drives this single dedicated voice directly, with hand-rolled
    // last-note-priority + glide/legato handling. See handleMonoMidi().
    UndertowVoice monoVoice;
    std::vector<std::pair<int, float>> heldNotes; // (midiNote, velocity), most recent at back

    void handleMonoMidi(const juce::MidiBuffer& midiMessages);

    juce::dsp::Chorus<float> chorus;
    StereoDelay delay;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(UndertowAudioProcessor)
};
