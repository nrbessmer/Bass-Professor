#pragma once
#include <juce_gui_basics/juce_gui_basics.h>

// Bass Professor visual identity. A deep blue-black panel palette with a warm
// amber accent — chosen to read as a "studio hardware" bass instrument rather
// than a default JUCE grey. All custom drawing lives here so the editor stays
// layout-only.
namespace BPColours
{
    const juce::Colour bgTop        { 0xff15171c };
    const juce::Colour bgBottom     { 0xff0e0f13 };
    const juce::Colour panel        { 0xff1d2129 };
    const juce::Colour panelEdge    { 0xff2c323d };
    const juce::Colour accent       { 0xfff0a830 }; // warm amber
    const juce::Colour accentDim    { 0x66f0a830 };
    const juce::Colour knobBody     { 0xff262b34 };
    const juce::Colour knobRim      { 0xff3a414e };
    const juce::Colour track        { 0xff32383f };
    const juce::Colour textBright   { 0xffe8eaed };
    const juce::Colour textDim      { 0xff8b929c };
    const juce::Colour neonGreen    { 0xff39ff14 }; // neon green for preset name + description
}

class BassProfessorLookAndFeel : public juce::LookAndFeel_V4
{
public:
    BassProfessorLookAndFeel()
    {
        setColour(juce::Slider::textBoxTextColourId, BPColours::textDim);
        setColour(juce::Slider::textBoxBackgroundColourId, juce::Colours::transparentBlack);
        setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
        setColour(juce::Label::textColourId, BPColours::textBright);
        setColour(juce::ComboBox::backgroundColourId, BPColours::knobBody);
        setColour(juce::ComboBox::textColourId, BPColours::textBright);
        setColour(juce::ComboBox::outlineColourId, BPColours::knobRim);
        setColour(juce::ComboBox::arrowColourId, BPColours::accent);
        setColour(juce::PopupMenu::backgroundColourId, BPColours::panel);
        setColour(juce::PopupMenu::highlightedBackgroundColourId, BPColours::accent);
        setColour(juce::PopupMenu::highlightedTextColourId, juce::Colours::black);
        setColour(juce::PopupMenu::textColourId, BPColours::textBright);
    }

    void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
                           float sliderPos, float rotaryStartAngle, float rotaryEndAngle,
                           juce::Slider&) override
    {
        const auto bounds = juce::Rectangle<int>(x, y, width, height).toFloat().reduced(4.0f);
        const float radius = juce::jmin(bounds.getWidth(), bounds.getHeight()) / 2.0f;
        const float centreX = bounds.getCentreX();
        const float centreY = bounds.getCentreY();
        const float angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);

        // Track arc
        const float arcRadius = radius - 2.0f;
        juce::Path track;
        track.addCentredArc(centreX, centreY, arcRadius, arcRadius, 0.0f,
                            rotaryStartAngle, rotaryEndAngle, true);
        g.setColour(BPColours::track);
        g.strokePath(track, juce::PathStrokeType(2.5f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

        // Value arc
        juce::Path value;
        value.addCentredArc(centreX, centreY, arcRadius, arcRadius, 0.0f,
                            rotaryStartAngle, angle, true);
        g.setColour(BPColours::accent);
        g.strokePath(value, juce::PathStrokeType(2.5f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

        // Knob body
        const float bodyR = radius - 6.0f;
        juce::ColourGradient grad(BPColours::knobRim, centreX, centreY - bodyR,
                                   BPColours::knobBody, centreX, centreY + bodyR, false);
        g.setGradientFill(grad);
        g.fillEllipse(centreX - bodyR, centreY - bodyR, bodyR * 2.0f, bodyR * 2.0f);
        g.setColour(BPColours::knobRim);
        g.drawEllipse(centreX - bodyR, centreY - bodyR, bodyR * 2.0f, bodyR * 2.0f, 1.0f);

        // Pointer
        juce::Path pointer;
        const float pointerLen = bodyR * 0.7f;
        const float pointerThick = 2.5f;
        pointer.addRoundedRectangle(-pointerThick * 0.5f, -bodyR + 2.0f, pointerThick, pointerLen, 1.0f);
        pointer.applyTransform(juce::AffineTransform::rotation(angle).translated(centreX, centreY));
        g.setColour(BPColours::textBright);
        g.fillPath(pointer);
    }

    void drawToggleButton(juce::Graphics& g, juce::ToggleButton& button,
                           bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override
    {
        juce::ignoreUnused(shouldDrawButtonAsDown);
        auto bounds = button.getLocalBounds().toFloat();
        const float h = juce::jmin(18.0f, bounds.getHeight());
        const bool on = button.getToggleState();

        // Pill track
        g.setColour(on ? BPColours::accentDim : BPColours::track);
        g.fillRoundedRectangle(juce::Rectangle<float>(2.0f, bounds.getY() + 1.0f, 30.0f, h - 2.0f), (h - 2.0f) * 0.5f);

        auto knobArea = juce::Rectangle<float>(on ? 16.0f : 4.0f, bounds.getY() + 3.0f, h - 6.0f, h - 6.0f);
        g.setColour(on ? BPColours::accent : BPColours::textDim);
        g.fillEllipse(knobArea);

        // Label
        g.setColour(shouldDrawButtonAsHighlighted ? BPColours::textBright : BPColours::textDim);
        g.setFont(juce::Font(11.0f));
        g.drawText(button.getButtonText(), 37, 0, button.getWidth() - 37, (int) h,
                    juce::Justification::centredLeft, false);
    }

    juce::Font getLabelFont(juce::Label&) override { return juce::Font(12.0f); }
    juce::Font getComboBoxFont(juce::ComboBox&) override { return juce::Font(12.0f); }
};
