#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include "TullyNeon.h"

// Bass Professor visual identity. A deep blue-black panel palette with a warm
// amber accent — chosen to read as a "studio hardware" bass instrument rather
// than a default JUCE grey. All custom drawing lives here so the editor stays
// layout-only.
namespace BPColours
{
    const juce::Colour bgTop        = neon::Pal::night1;
    const juce::Colour bgBottom     = neon::Pal::night0;
    const juce::Colour panel        = neon::Pal::glass;
    const juce::Colour panelEdge    = neon::Pal::edge;
    const juce::Colour accent       { 0xfff0a830 }; // warm amber
    const juce::Colour accentDim    { 0x66f0a830 };
    const juce::Colour knobBody     = neon::Pal::glassHi;
    const juce::Colour knobRim      = neon::Pal::edge;
    const juce::Colour track        { 0xff32383f };
    const juce::Colour textBright   = neon::Pal::text;
    const juce::Colour textDim      = neon::Pal::dim;
    const juce::Colour neonGreen    { 0xff39ff14 }; // neon green for preset name + description
}

class BassProfessorLookAndFeel : public neon::LookAndFeel
{
public:
    BassProfessorLookAndFeel() : neon::LookAndFeel (BPColours::accent)
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



    juce::Font getLabelFont(juce::Label&) override { return juce::Font(12.0f); }
    juce::Font getComboBoxFont(juce::ComboBox&) override { return juce::Font(12.0f); }
};
