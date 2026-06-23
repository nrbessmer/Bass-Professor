#include "PluginEditor.h"
#include "Params.h"

UndertowAudioProcessorEditor::UndertowAudioProcessorEditor(UndertowAudioProcessor& p)
    : AudioProcessorEditor(&p), processor(p)
{
    setLookAndFeel(&lnf);

    auto& voicing = addSection("VOICING");
    addCombo(voicing, Params::polyphonyMode, "Mode");
    addKnob(voicing, Params::glideTimeMs, "Glide");
    addToggle(voicing, Params::legato, "Legato");

    auto& osc = addSection("OSCILLATOR");
    addCombo(osc, Params::oscModel, "Model");
    addCombo(osc, Params::oscWaveA, "Wave A");
    addCombo(osc, Params::oscWaveB, "Wave B");
    addKnob(osc, Params::oscBGain, "B Gain");
    addKnob(osc, Params::oscBPitch, "B Pitch");
    addToggle(osc, Params::hardSync, "Hard Sync");
    addKnob(osc, Params::pulseWidth, "Pulse Width");
    addKnob(osc, Params::wtPosition, "WT Pos");
    addKnob(osc, Params::wtWarp, "WT Warp");
    addKnob(osc, Params::fmRatio, "FM Ratio");
    addKnob(osc, Params::fmIndex, "FM Index");
    addKnob(osc, Params::fmFeedback, "FM Fdbk");
    addKnob(osc, Params::oscOctave, "Octave");
    addKnob(osc, Params::oscSemitone, "Semitone");
    addKnob(osc, Params::oscFineCents, "Fine");
    addKnob(osc, Params::unisonVoices, "Unison");
    addKnob(osc, Params::unisonDetune, "Detune");
    addKnob(osc, Params::unisonSpread, "Spread");

    auto& bands = addSection("3-BAND");
    addToggle(bands, Params::bandsEnable, "3-Band");
    addKnob(bands, Params::crossoverLow, "X-Low");
    addKnob(bands, Params::crossoverHigh, "X-High");

    auto& am = addSection("AM INJECTOR");
    addKnob(am, Params::amAmount, "Amount");
    addCombo(am, Params::amTarget, "Target");

    auto& body = addSection("BODY");
    addCombo(body, Params::bodyModel, "Model");
    addKnob(body, Params::bodyGain, "Gain");
    addKnob(body, Params::bodyPan, "Pan");
    addKnob(body, Params::bodyPitch, "Pitch");
    addToggle(body, Params::bodyActive, "On");
    addKnob(body, Params::bodyDrive, "Drive");
    addToggle(body, Params::bodyFilterOn, "Filter");
    addCombo(body, Params::bodyFilterType, "Flt Type");
    addKnob(body, Params::bodyFilterCut, "Flt Cut");
    addKnob(body, Params::bodyFilterRes, "Flt Res");

    auto& character = addSection("CHARACTER");
    addCombo(character, Params::charModel, "Model");
    addKnob(character, Params::charGain, "Gain");
    addKnob(character, Params::charPan, "Pan");
    addKnob(character, Params::charPitch, "Pitch");
    addToggle(character, Params::charActive, "On");
    addKnob(character, Params::charDrive, "Drive");
    addToggle(character, Params::charFilterOn, "Filter");
    addCombo(character, Params::charFilterType, "Flt Type");
    addKnob(character, Params::charFilterCut, "Flt Cut");
    addKnob(character, Params::charFilterRes, "Flt Res");

    auto& air = addSection("AIR");
    addCombo(air, Params::airModel, "Model");
    addKnob(air, Params::airGain, "Gain");
    addKnob(air, Params::airPan, "Pan");
    addKnob(air, Params::airPitch, "Pitch");
    addToggle(air, Params::airActive, "On");
    addKnob(air, Params::airDrive, "Drive");
    addToggle(air, Params::airFilterOn, "Filter");
    addCombo(air, Params::airFilterType, "Flt Type");
    addKnob(air, Params::airFilterCut, "Flt Cut");
    addKnob(air, Params::airFilterRes, "Flt Res");

    auto& filt = addSection("FILTER");
    addCombo(filt, Params::filterType, "Type");
    addKnob(filt, Params::filterCutoff, "Cutoff");
    addKnob(filt, Params::filterResonance, "Resonance");
    addKnob(filt, Params::filterDrive, "Drive");
    addKnob(filt, Params::filterEnvAmount, "Env Amount");
    addKnob(filt, Params::filterKeyTrack, "Key Track");

    auto& ampEnv = addSection("AMP ENV");
    addKnob(ampEnv, Params::ampAttack, "Attack");
    addKnob(ampEnv, Params::ampDecay, "Decay");
    addKnob(ampEnv, Params::ampSustain, "Sustain");
    addKnob(ampEnv, Params::ampRelease, "Release");

    auto& modEnv = addSection("MOD ENV -> FILTER");
    addKnob(modEnv, Params::modAttack, "Attack");
    addKnob(modEnv, Params::modDecay, "Decay");
    addKnob(modEnv, Params::modSustain, "Sustain");
    addKnob(modEnv, Params::modRelease, "Release");

    auto& lfoSection = addSection("LFO");
    addCombo(lfoSection, Params::lfoShape, "Shape");
    addKnob(lfoSection, Params::lfoRate, "Rate");
    addKnob(lfoSection, Params::lfoDepth, "Depth");
    addCombo(lfoSection, Params::lfoDestination, "Destination");

    auto& output = addSection("OUTPUT");
    addKnob(output, Params::masterDrive, "Drive");
    addKnob(output, Params::masterGain, "Gain");

    auto& fx = addSection("EFFECTS");
    addToggle(fx, Params::chorusEnable, "Chorus");
    addKnob(fx, Params::chorusRate, "Rate");
    addKnob(fx, Params::chorusDepth, "Depth");
    addKnob(fx, Params::chorusFeedback, "Feedback");
    addKnob(fx, Params::chorusMix, "Mix");
    addToggle(fx, Params::delayEnable, "Delay");
    addKnob(fx, Params::delayTimeMs, "Time");
    addKnob(fx, Params::delayFeedback, "Feedback");
    addKnob(fx, Params::delayMix, "Mix");

    setResizable(true, true);
    setResizable(true, true);
    setResizeLimits(900, 600, 2000, 1500);
    setSize(1200, 1050);
    // First layout populates requiredHeight; snap to it so nothing clips.
    resized();
    setSize(1200, juce::jlimit(600, 1500, requiredHeight));
}

UndertowAudioProcessorEditor::Section& UndertowAudioProcessorEditor::addSection(const juce::String& title)
{
    sections.push_back(std::make_unique<Section>());
    auto& section = *sections.back();
    section.title = title;
    return section;
}

UndertowAudioProcessorEditor::Knob& UndertowAudioProcessorEditor::addKnob(
    Section& section, const juce::String& paramID, const juce::String& labelText)
{
    allKnobs.push_back(std::make_unique<Knob>());
    auto& knob = *allKnobs.back();

    knob.slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 64, 18);
    addAndMakeVisible(knob.slider);

    knob.label.setText(labelText, juce::dontSendNotification);
    knob.label.setJustificationType(juce::Justification::centred);
    knob.label.setFont(juce::Font(13.0f));
    addAndMakeVisible(knob.label);

    knob.attachment = std::make_unique<APVTS::SliderAttachment>(processor.apvts, paramID, knob.slider);

    section.knobs.push_back(&knob);
    return knob;
}

UndertowAudioProcessorEditor::Combo& UndertowAudioProcessorEditor::addCombo(
    Section& section, const juce::String& paramID, const juce::String& labelText)
{
    allCombos.push_back(std::make_unique<Combo>());
    auto& combo = *allCombos.back();

    if (auto* param = dynamic_cast<juce::AudioParameterChoice*>(processor.apvts.getParameter(paramID)))
        combo.box.addItemList(param->choices, 1);

    addAndMakeVisible(combo.box);

    combo.label.setText(labelText, juce::dontSendNotification);
    combo.label.setJustificationType(juce::Justification::centred);
    combo.label.setFont(juce::Font(13.0f));
    addAndMakeVisible(combo.label);

    combo.attachment = std::make_unique<APVTS::ComboBoxAttachment>(processor.apvts, paramID, combo.box);

    section.combos.push_back(&combo);
    return combo;
}

UndertowAudioProcessorEditor::Toggle& UndertowAudioProcessorEditor::addToggle(
    Section& section, const juce::String& paramID, const juce::String& labelText)
{
    allToggles.push_back(std::make_unique<Toggle>());
    auto& toggle = *allToggles.back();

    toggle.button.setButtonText(labelText);
    addAndMakeVisible(toggle.button);

    toggle.attachment = std::make_unique<APVTS::ButtonAttachment>(processor.apvts, paramID, toggle.button);

    section.toggles.push_back(&toggle);
    return toggle;
}

UndertowAudioProcessorEditor::~UndertowAudioProcessorEditor()
{
    setLookAndFeel(nullptr);
}

void UndertowAudioProcessorEditor::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds();

    // Background gradient
    juce::ColourGradient bg(BPColours::bgTop, 0.0f, 0.0f,
                             BPColours::bgBottom, 0.0f, (float) getHeight(), false);
    g.setGradientFill(bg);
    g.fillRect(bounds);

    // Header bar
    auto header = bounds.removeFromTop(headerHeight);
    g.setColour(BPColours::panel);
    g.fillRect(header);
    g.setColour(BPColours::accent);
    g.fillRect(header.removeFromBottom(2)); // accent underline

    // Title
    auto titleArea = juce::Rectangle<int>(18, 0, 520, headerHeight);
    g.setColour(BPColours::accent);
    g.setFont(juce::Font(26.0f, juce::Font::bold));
    g.drawText("BASS PROFESSOR", titleArea.removeFromTop(34).withTrimmedTop(8),
                juce::Justification::bottomLeft, false);
    g.setColour(BPColours::textDim);
    g.setFont(juce::Font(11.0f));
    g.drawText("3-BAND BASS SYNTHESIZER", juce::Rectangle<int>(20, 34, 320, 16),
                juce::Justification::topLeft, false);

    // Branding (right side of header)
    g.setColour(BPColours::textDim);
    g.setFont(juce::Font(11.0f));
    auto brand = juce::Rectangle<int>(getWidth() - 360, 8, 344, headerHeight - 12);
    g.drawText(juce::String::fromUTF8("\u00A9\u2117 2026 Tully EDM Vibe"),
                brand.removeFromTop(18), juce::Justification::topRight, false);
    g.drawText("info@tullyedmvibe.com", brand.removeFromTop(16),
                juce::Justification::topRight, false);

    // Section panels
    for (auto& sectionPtr : sections)
    {
        auto& section = *sectionPtr;
        auto r = section.bounds.toFloat();
        g.setColour(BPColours::panel);
        g.fillRoundedRectangle(r, 6.0f);
        g.setColour(BPColours::panelEdge);
        g.drawRoundedRectangle(r, 6.0f, 1.0f);

        // Section title with accent tick
        auto titleBar = r.removeFromTop(20.0f).reduced(8.0f, 2.0f);
        g.setColour(BPColours::accent);
        g.fillRect(titleBar.removeFromLeft(3.0f).withTrimmedTop(2.0f).withTrimmedBottom(2.0f));
        g.setColour(BPColours::textBright);
        g.setFont(juce::Font(12.0f, juce::Font::bold));
        g.drawText(section.title, titleBar.withTrimmedLeft(6.0f).toNearestInt(),
                    juce::Justification::centredLeft, false);
    }
}

void UndertowAudioProcessorEditor::resized()
{
    auto full = getLocalBounds();
    full.removeFromTop(headerHeight); // reserve header
    auto bounds = full.reduced(10);

    const int controlW = 84;
    const int controlH = 84;

    // First pass: compute each section's footprint from its control count.
    std::vector<juce::Rectangle<int>> sectionSizes;
    sectionSizes.reserve(sections.size());

    for (auto& sectionPtr : sections)
    {
        auto& section = *sectionPtr;
        const int controlCount = (int) (section.knobs.size() + section.combos.size() + section.toggles.size());
        const int columns = juce::jmax(2, (int) std::ceil(std::sqrt((double) controlCount)));
        const int rows = (controlCount + columns - 1) / columns;
        const int sectionW = columns * controlW + 20;
        const int sectionH = rows * controlH + 40;
        sectionSizes.push_back({ 0, 0, sectionW, sectionH });
    }

    // Pack sections left-to-right, wrapping to a new row when they don't fit.
    int x = bounds.getX();
    int y = bounds.getY();
    int rowHeight = 0;
    int contentBottom = bounds.getY();

    for (size_t i = 0; i < sections.size(); ++i)
    {
        auto& section = *sections[i];
        auto size = sectionSizes[i];

        if (x + size.getWidth() > bounds.getRight() && x != bounds.getX())
        {
            x = bounds.getX();
            y += rowHeight + 8;
            rowHeight = 0;
        }

        section.bounds = juce::Rectangle<int>(x, y, size.getWidth(), size.getHeight());
        x += size.getWidth() + 8;
        rowHeight = juce::jmax(rowHeight, size.getHeight());
        contentBottom = juce::jmax(contentBottom, y + size.getHeight());
        requiredHeight = contentBottom + 10 + headerHeight;

        // Second pass: place this section's controls inside its now-known bounds.
        auto area = section.bounds.reduced(8);
        area.removeFromTop(16); // space for the section title
        const int rightEdge = area.getRight();
        int cx = area.getX();
        int cy = area.getY();

        auto advance = [&]()
        {
            cx += controlW;
            if (cx + controlW > rightEdge + 1)
            {
                cx = area.getX();
                cy += controlH;
            }
        };

        for (auto* knob : section.knobs)
        {
            knob->slider.setBounds(cx, cy, controlW - 6, controlH - 18);
            knob->label.setBounds(cx, cy + controlH - 18, controlW - 6, 16);
            advance();
        }
        for (auto* combo : section.combos)
        {
            combo->label.setBounds(cx, cy, controlW - 6, 16);
            combo->box.setBounds(cx, cy + 18, controlW - 6, 22);
            advance();
        }
        for (auto* toggle : section.toggles)
        {
            toggle->button.setBounds(cx, cy + (controlH / 2 - 10), controlW + 14, 20);
            advance();
        }
    }
}
