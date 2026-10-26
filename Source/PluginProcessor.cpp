#include "PluginProcessor.h"
#include "PluginEditor.h"

SecretSauceAudioProcessor::SecretSauceAudioProcessor()
    : AudioProcessor (BusesProperties()
        .withInput ("Input", juce::AudioChannelSet::stereo(), true)
        .withOutput ("Output", juce::AudioChannelSet::stereo(), true)),
      parameters (*this, nullptr, "PARAMETERS", createParameterLayout())
{
}

SecretSauceAudioProcessor::APVTS::ParameterLayout SecretSauceAudioProcessor::createParameterLayout()
{
    APVTS::ParameterLayout layout;
    auto addFloat = [&layout](const char* id, const char* name, float min, float max, float def,
                              const juce::String& suffix = juce::String(), float step = 0.01f)
    {
        layout.add (std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID { id, 1 }, name,
            juce::NormalisableRange<float> (min, max, step), def, suffix));
    };

    addFloat ("delayL", "Left Delay", 0.0f, 1000.0f, 18.0f, " ms", 1.0f);
    addFloat ("delayR", "Right Delay", 0.0f, 1000.0f, 27.0f, " ms", 1.0f);
    addFloat ("levelL", "Left Level", -24.0f, 6.0f, -3.0f, " dB", 0.1f);
    addFloat ("levelR", "Right Level", -24.0f, 6.0f, -3.0f, " dB", 0.1f);
    addFloat ("mix", "Blend", 0.0f, 100.0f, 35.0f, " %", 1.0f);
    addFloat ("low", "Low", -12.0f, 12.0f, 0.0f, " dB", 0.1f);
    addFloat ("mid", "Mid", -12.0f, 12.0f, 0.0f, " dB", 0.1f);
    addFloat ("high", "High", -12.0f, 12.0f, 0.0f, " dB", 0.1f);
    return layout;
}

void SecretSauceAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    currentSampleRate = sampleRate;
    delayBuffer.setSize (2, juce::jmax (2, (int) std::ceil (sampleRate * 1.01) + samplesPerBlock));
    delayBuffer.clear();
    writePosition = 0;

    juce::dsp::ProcessSpec spec { sampleRate, (juce::uint32) samplesPerBlock, 1 };
    lowL.prepare (spec); lowR.prepare (spec);
    midL.prepare (spec); midR.prepare (spec);
    highL.prepare (spec); highR.prepare (spec);
    lowL.reset(); lowR.reset(); midL.reset(); midR.reset();
    highL.reset(); highR.reset();
    updateFilters();
}

void SecretSauceAudioProcessor::releaseResources()
{
    delayBuffer.setSize (0, 0);
}

bool SecretSauceAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    return layouts.getMainInputChannelSet() == juce::AudioChannelSet::stereo()
        && layouts.getMainOutputChannelSet() == juce::AudioChannelSet::stereo();
}

void SecretSauceAudioProcessor::updateFilters()
{
    const auto lowGain = juce::Decibels::decibelsToGain (parameters.getRawParameterValue ("low")->load());
    const auto midGain = juce::Decibels::decibelsToGain (parameters.getRawParameterValue ("mid")->load());
    const auto highGain = juce::Decibels::decibelsToGain (parameters.getRawParameterValue ("high")->load());

    lowCoeffs = juce::dsp::IIR::Coefficients<float>::makeLowShelf (currentSampleRate, 180.0, 0.707, lowGain);
    midCoeffs = juce::dsp::IIR::Coefficients<float>::makePeakFilter (currentSampleRate, 1000.0, 0.8, midGain);
    highCoeffs = juce::dsp::IIR::Coefficients<float>::makeHighShelf (currentSampleRate, 6500.0, 0.707, highGain);

    *lowL.coefficients = *lowCoeffs; *lowR.coefficients = *lowCoeffs;
    *midL.coefficients = *midCoeffs; *midR.coefficients = *midCoeffs;
    *highL.coefficients = *highCoeffs; *highR.coefficients = *highCoeffs;
}

void SecretSauceAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;
    const int numSamples = buffer.getNumSamples();
    if (buffer.getNumChannels() < 2 || delayBuffer.getNumSamples() < 2)
        return;

    // Update tone filters from current control positions.
    updateFilters();

    const float delayMsL = parameters.getRawParameterValue ("delayL")->load();
    const float delayMsR = parameters.getRawParameterValue ("delayR")->load();
    const float gainL = juce::Decibels::decibelsToGain (parameters.getRawParameterValue ("levelL")->load());
    const float gainR = juce::Decibels::decibelsToGain (parameters.getRawParameterValue ("levelR")->load());
    const float mix = parameters.getRawParameterValue ("mix")->load() / 100.0f;
    const int ringSize = delayBuffer.getNumSamples();

    auto* left = buffer.getWritePointer (0);
    auto* right = buffer.getWritePointer (1);

    for (int i = 0; i < numSamples; ++i)
    {
        const float inL = left[i];
        const float inR = right[i];

        delayBuffer.setSample (0, writePosition, inL);
        delayBuffer.setSample (1, writePosition, inR);

        const int delaySamplesL = juce::jlimit (0, ringSize - 1, (int) std::round (delayMsL * 0.001 * currentSampleRate));
        const int delaySamplesR = juce::jlimit (0, ringSize - 1, (int) std::round (delayMsR * 0.001 * currentSampleRate));
        const int readL = (writePosition - delaySamplesL + ringSize) % ringSize;
        const int readR = (writePosition - delaySamplesR + ringSize) % ringSize;

        const float delayedL = delayBuffer.getSample (0, readL) * gainL;
        const float delayedR = delayBuffer.getSample (1, readR) * gainR;

        // Blend the delayed stereo layer with the original signal.
        float outL = inL * (1.0f - mix) + delayedL * mix;
        float outR = inR * (1.0f - mix) + delayedR * mix;

        outL = highL.processSample (midL.processSample (lowL.processSample (outL)));
        outR = highR.processSample (midR.processSample (lowR.processSample (outR)));

        left[i] = outL;
        right[i] = outR;
        writePosition = (writePosition + 1) % ringSize;
    }
}

juce::AudioProcessorEditor* SecretSauceAudioProcessor::createEditor()
{
    return new SecretSauceAudioProcessorEditor (*this);
}

void SecretSauceAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    auto state = parameters.copyState();
    std::unique_ptr<juce::XmlElement> xml (state.createXml());
    copyXmlToBinary (*xml, destData);
}

void SecretSauceAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xml (getXmlFromBinary (data, sizeInBytes));
    if (xml != nullptr && xml->hasTagName (parameters.state.getType()))
        parameters.replaceState (juce::ValueTree::fromXml (*xml));
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new SecretSauceAudioProcessor();
}
