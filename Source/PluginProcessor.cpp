#include "PluginProcessor.h"
#include "PluginEditor.h"

#define NUM_VOICES 8

NeonTaxiwayAudioProcessor::NeonTaxiwayAudioProcessor()
    : AudioProcessor(BusesProperties().withOutput("Output", juce::AudioChannelSet::stereo(), true)),
    parameters(*this, nullptr, "SynthSettings", { Parameters::createParameterLayout() }) {

    mySynth.addSound(new MySynthSound());

    for (int v = 0; v < NUM_VOICES; ++v)
        mySynth.addVoice(new SimpleSynthVoice(Parameters::defaultAmpAtk, Parameters::defaultAmpDcy, Parameters::defaultAmpSus, Parameters::defaultAmpRel,
                                              Parameters::defaultFiltAtk, Parameters::defaultFiltDcy, Parameters::defaultFiltSus, Parameters::defaultFiltRel));

    Parameters::addListenerToAllParameters(parameters, this);
}

NeonTaxiwayAudioProcessor::~NeonTaxiwayAudioProcessor() {
}

void NeonTaxiwayAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock) {
    mySynth.setCurrentPlaybackSampleRate(sampleRate);

    for (int v = 0; v < mySynth.getNumVoices(); ++v)
        if (auto voice = dynamic_cast<SimpleSynthVoice*>(mySynth.getVoice(v)))
            voice->prepareToPlay(sampleRate, samplesPerBlock);
}

void NeonTaxiwayAudioProcessor::releaseResources() {
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool NeonTaxiwayAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const {
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void NeonTaxiwayAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) {
    juce::ScopedNoDenormals noDenormals;
    const auto numSamples = buffer.getNumSamples();

    buffer.clear();

    mySynth.renderNextBlock(buffer, midiMessages, 0, numSamples);

    if (getTotalNumOutputChannels() == 2)
        buffer.copyFrom(1, 0, buffer, 0, 0, numSamples);
}

bool NeonTaxiwayAudioProcessor::hasEditor() const {
    return true;
}

juce::AudioProcessorEditor* NeonTaxiwayAudioProcessor::createEditor() {
    return new NeonTaxiwayAudioProcessorEditor (*this, parameters);
}

void NeonTaxiwayAudioProcessor::getStateInformation (juce::MemoryBlock& destData) {
    auto state = parameters.copyState();
    std::unique_ptr<XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void NeonTaxiwayAudioProcessor::setStateInformation (const void* data, int sizeInBytes) {
    std::unique_ptr<XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));
    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName(parameters.state.getType()))
            parameters.replaceState(ValueTree::fromXml(*xmlState));
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() {
    return new NeonTaxiwayAudioProcessor();
}

void NeonTaxiwayAudioProcessor::parameterChanged(const String& paramID, float newValue) {
    for (int v = 0; v < mySynth.getNumVoices(); ++v)
        if (auto voice = dynamic_cast<SimpleSynthVoice*>(mySynth.getVoice(v))) {
            if (paramID == Parameters::nameAmpAtk)
                voice->setAttack(newValue, true);

            if (paramID == Parameters::nameAmpDcy)
                voice->setDecay(newValue, true);

            if (paramID == Parameters::nameAmpSus)
                voice->setSustain(newValue, true);

            if (paramID == Parameters::nameAmpRel)
                voice->setRelease(newValue, true);

            if (paramID == Parameters::nameFiltAtk)
                voice->setAttack(newValue, false);

            if (paramID == Parameters::nameFiltDcy)
                voice->setDecay(newValue, false);

            if (paramID == Parameters::nameFiltSus)
                voice->setSustain(newValue, false);

            if (paramID == Parameters::nameFiltRel)
                voice->setRelease(newValue, false);

            if (paramID == Parameters::nameWave1)
                voice->setWaveform(0, newValue);

            if (paramID == Parameters::nameWave2)
                voice->setWaveform(1, newValue);

            if (paramID == Parameters::nameOscMix)
                voice->setOscMix(newValue);

            if (paramID == Parameters::nameOscDetune)
                voice->setOscDetune(newValue);

            if (paramID == Parameters::nameMasterGain)
                voice->setGain(newValue);

            if (paramID == Parameters::nameCutoffFreq)
                voice->setFilterFrequency(newValue);

            if (paramID == Parameters::nameResonance)
                voice->setFilterResonance(newValue);

            if (paramID == Parameters::nameEnvAmount)
                voice->setFilterEnvAmount(newValue);
        }

}
