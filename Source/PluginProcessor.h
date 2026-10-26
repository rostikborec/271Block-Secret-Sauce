#pragma once
#include <JuceHeader.h>

class SecretSauceAudioProcessor : public juce::AudioProcessor
{
public:
    SecretSauceAudioProcessor();
    ~SecretSauceAudioProcessor() override = default;

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }
    const juce::String getName() const override { return JucePlugin_Name; }
    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 1.0; }

    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram (int) override {}
    const juce::String getProgramName (int) override { return {}; }
    void changeProgramName (int, const juce::String&) override {}

    void getStateInformation (juce::MemoryBlock&) override;
    void setStateInformation (const void*, int) override;

    using APVTS = juce::AudioProcessorValueTreeState;
    APVTS parameters;

    static APVTS::ParameterLayout createParameterLayout();

private:
    juce::AudioBuffer<float> delayBuffer;
    int writePosition = 0;
    double currentSampleRate = 44100.0;

    juce::dsp::IIR::Filter<float> lowL, lowR, midL, midR, highL, highR;
    juce::dsp::IIR::Coefficients<float>::Ptr lowCoeffs, midCoeffs, highCoeffs;
    void updateFilters();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SecretSauceAudioProcessor)
};
