#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"

class SecretSauceAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    explicit SecretSauceAudioProcessorEditor (SecretSauceAudioProcessor&);
    ~SecretSauceAudioProcessorEditor() override = default;
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    SecretSauceAudioProcessor& audioProcessor;

    juce::Label title, subtitle, leftLabel, rightLabel, eqLabel, blendLabel;
    juce::Slider delayL, delayR, levelL, levelR, low, mid, high, mix;
    std::unique_ptr<SliderAttachment> delayLAttach, delayRAttach, levelLAttach, levelRAttach,
                                      lowAttach, midAttach, highAttach, mixAttach;

    void setupSlider (juce::Slider&, const juce::String&, const juce::String&);
    void setupLabel (juce::Label&, const juce::String&, int, juce::Colour);
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SecretSauceAudioProcessorEditor)
};
