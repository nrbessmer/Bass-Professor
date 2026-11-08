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

    static constexpr int headerHeight = 74;
    int requiredHeight = 1000;

    // Preset selector (lives in the header bar).
    juce::ComboBox presetBox;
    juce::TextButton prevButton { "<" };
    juce::TextButton nextButton { ">" };
    juce::Label presetDescription;
    void populatePresetBox();
    void syncPresetBox();
    void applyPresetByBoxId(int comboId);
    void stepPreset(int delta);
    void updatePresetDescription();

    // Tab navigation — one tab per row of sections.
    static constexpr int tabStripH = 34;
    static constexpr int numTabs   = 4;
    int activeTab = 0;
    juce::TextButton tabButtons[numTabs];
    void setActiveTab(int index);
    void updateControlVisibility();

    // Logo image (loaded from BinaryData if available, blank otherwise).
    juce::Image logoImage, tullyImage;
    neon::BackdropCache backdrop;

    // Clickable email link in the header — opens default mail client via mailto:.
    juce::HyperlinkButton emailLink { "info@tullyedmvibe.com",
                                      juce::URL ("mailto:info@tullyedmvibe.com") };

    // Row zone bounds computed in resized(), used in paint() for row backgrounds.
    std::vector<juce::Rectangle<int>> rowZones;
    static const juce::String rowLabels[4];

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

    Knob& addKnob(Section& section, const juce::String& paramID, const juce::String& labelText, const juce::String& tip = {});
    Combo& addCombo(Section& section, const juce::String& paramID, const juce::String& labelText, const juce::String& tip = {});
    Toggle& addToggle(Section& section, const juce::String& paramID, const juce::String& labelText, const juce::String& tip = {});
    Section& addSection(const juce::String& title);

    juce::TooltipWindow tooltipWindow { this, 450 };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(UndertowAudioProcessorEditor)
};
