#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include "PluginProcessor.h"
#include "BassProfessorLookAndFeel.h"

class UndertowAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    explicit UndertowAudioProcessorEditor(UndertowAudioProcessor&);
    ~UndertowAudioProcessorEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    UndertowAudioProcessor& processor;
    BassProfessorLookAndFeel lnf;

    static constexpr int headerHeight = 56;
    int requiredHeight = 1000;

    using APVTS = juce::AudioProcessorValueTreeState;

    struct Knob
    {
        juce::Slider slider { juce::Slider::RotaryHorizontalVerticalDrag, juce::Slider::TextBoxBelow };
        juce::Label label;
        std::unique_ptr<APVTS::SliderAttachment> attachment;
    };

    struct Combo
    {
        juce::ComboBox box;
        juce::Label label;
        std::unique_ptr<APVTS::ComboBoxAttachment> attachment;
    };

    struct Toggle
    {
        juce::ToggleButton button;
        std::unique_ptr<APVTS::ButtonAttachment> attachment;
    };

    struct Section
    {
        juce::String title;
        juce::Rectangle<int> bounds;
        std::vector<Knob*> knobs;
        std::vector<Combo*> combos;
        std::vector<Toggle*> toggles;
    };

    std::vector<std::unique_ptr<Knob>> allKnobs;
    std::vector<std::unique_ptr<Combo>> allCombos;
    std::vector<std::unique_ptr<Toggle>> allToggles;
    std::vector<std::unique_ptr<Section>> sections;

    Knob& addKnob(Section& section, const juce::String& paramID, const juce::String& labelText);
    Combo& addCombo(Section& section, const juce::String& paramID, const juce::String& labelText);
    Toggle& addToggle(Section& section, const juce::String& paramID, const juce::String& labelText);
    Section& addSection(const juce::String& title);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(UndertowAudioProcessorEditor)
};
