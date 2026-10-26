#include "PluginEditor.h"

namespace
{
    const juce::Colour bg       { 0xff101218 };
    const juce::Colour panel    { 0xff191d26 };
    const juce::Colour panel2   { 0xff202633 };
    const juce::Colour accent   { 0xffb7ff4a };
    const juce::Colour muted    { 0xff8d97a8 };
    const juce::Colour white    { 0xfff2f5f8 };
}

SecretSauceAudioProcessorEditor::SecretSauceAudioProcessorEditor (SecretSauceAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    setSize (760, 560);
    setupLabel (title, "271BLOCK  /  SECRET SAUCE", 25, white);
    title.setFont (juce::Font (juce::FontOptions (25.0f, juce::Font::bold)));
    setupLabel (subtitle, "STEREO VOCAL WIDTH  •  MICRO-DELAY  •  TONE", 11, muted);

    setupLabel (leftLabel, "LEFT CHANNEL", 13, accent);
    setupLabel (rightLabel, "RIGHT CHANNEL", 13, accent);
    setupLabel (eqLabel, "TONE SHAPER", 13, accent);
    setupLabel (blendLabel, "BLEND", 11, muted);

    setupSlider (delayL, "DELAY", " ms");
    setupSlider (delayR, "DELAY", " ms");
    setupSlider (levelL, "LEVEL", " dB");
    setupSlider (levelR, "LEVEL", " dB");
    setupSlider (low, "LOW", " dB");
    setupSlider (mid, "MID", " dB");
    setupSlider (high, "HIGH", " dB");
    setupSlider (mix, "DRY  ↔  SAUCE", " %");

    delayLAttach = std::make_unique<SliderAttachment> (p.parameters, "delayL", delayL);
    delayRAttach = std::make_unique<SliderAttachment> (p.parameters, "delayR", delayR);
    levelLAttach = std::make_unique<SliderAttachment> (p.parameters, "levelL", levelL);
    levelRAttach = std::make_unique<SliderAttachment> (p.parameters, "levelR", levelR);
    lowAttach = std::make_unique<SliderAttachment> (p.parameters, "low", low);
    midAttach = std::make_unique<SliderAttachment> (p.parameters, "mid", mid);
    highAttach = std::make_unique<SliderAttachment> (p.parameters, "high", high);
    mixAttach = std::make_unique<SliderAttachment> (p.parameters, "mix", mix);
}

void SecretSauceAudioProcessorEditor::setupLabel (juce::Label& label, const juce::String& text,
                                                  int size, juce::Colour colour)
{
    label.setText (text, juce::dontSendNotification);
    label.setColour (juce::Label::textColourId, colour);
    label.setFont (juce::Font (juce::FontOptions ((float) size, juce::Font::bold)));
    label.setJustificationType (juce::Justification::centredLeft);
    addAndMakeVisible (label);
}

void SecretSauceAudioProcessorEditor::setupSlider (juce::Slider& slider, const juce::String& name,
                                                   const juce::String& suffix)
{
    slider.setName (name);
    slider.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
    slider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 88, 22);
    slider.setTextValueSuffix (suffix);
    slider.setColour (juce::Slider::rotarySliderFillColourId, accent);
    slider.setColour (juce::Slider::rotarySliderOutlineColourId, panel2);
    slider.setColour (juce::Slider::thumbColourId, accent);
    slider.setColour (juce::Slider::textBoxTextColourId, white);
    slider.setColour (juce::Slider::textBoxBackgroundColourId, panel);
    slider.setColour (juce::Slider::textBoxOutlineColourId, panel2);
    addAndMakeVisible (slider);
}

void SecretSauceAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (bg);
    g.setColour (accent);
    g.fillRoundedRectangle (24.0f, 24.0f, 5.0f, 43.0f, 2.0f);

    g.setColour (panel);
    g.fillRoundedRectangle (24.0f, 100.0f, 344.0f, 270.0f, 14.0f);
    g.fillRoundedRectangle (392.0f, 100.0f, 344.0f, 270.0f, 14.0f);
    g.fillRoundedRectangle (24.0f, 390.0f, 712.0f, 145.0f, 14.0f);

    g.setColour (panel2);
    g.drawRoundedRectangle (24.0f, 100.0f, 344.0f, 270.0f, 14.0f, 1.0f);
    g.drawRoundedRectangle (392.0f, 100.0f, 344.0f, 270.0f, 14.0f, 1.0f);
    g.drawRoundedRectangle (24.0f, 390.0f, 712.0f, 145.0f, 14.0f, 1.0f);

    g.setColour (muted);
    g.setFont (juce::Font (juce::FontOptions (10.0f, juce::Font::bold)));
    g.drawText ("INDEPENDENT TIMING & LEVEL", 24, 78, 360, 15, juce::Justification::centredLeft);
    g.drawText ("SHAPE THE CHARACTER", 24, 372, 360, 15, juce::Justification::centredLeft);

    g.setColour (accent.withAlpha (0.10f));
    g.fillEllipse (666.0f, 30.0f, 50.0f, 50.0f);
    g.setColour (accent);
    g.setFont (juce::Font (juce::FontOptions (13.0f, juce::Font::bold)));
    g.drawText ("271", 666, 44, 50, 22, juce::Justification::centred);
}

void SecretSauceAudioProcessorEditor::resized()
{
    title.setBounds (40, 22, 560, 34);
    subtitle.setBounds (40, 57, 560, 18);

    leftLabel.setBounds (42, 113, 260, 22);
    delayL.setBounds (38, 142, 154, 142);
    levelL.setBounds (205, 142, 145, 142);

    rightLabel.setBounds (410, 113, 260, 22);
    delayR.setBounds (406, 142, 154, 142);
    levelR.setBounds (573, 142, 145, 142);

    eqLabel.setBounds (42, 402, 260, 22);
    low.setBounds (40, 424, 150, 102);
    mid.setBounds (202, 424, 150, 102);
    high.setBounds (364, 424, 150, 102);

    blendLabel.setBounds (544, 407, 150, 18);
    mix.setBounds (532, 420, 180, 108);
}
