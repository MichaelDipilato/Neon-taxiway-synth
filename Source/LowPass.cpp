#include "LowPass.h"

void LowPass::prepareToPlay(double sr, dsp::ProcessSpec spec) {
	sampleRate = sr;
	svfFilters.clear();

	for (int i = 0; i < MAX_NUM_CH; ++i) {
        auto filter = std::make_unique<dsp::StateVariableTPTFilter<float>>();
        filter->setType(dsp::StateVariableTPTFilterType::lowpass);
        filter->setCutoffFrequency(frequency);
        filter->setResonance(resonance);
        filter->prepare(spec);

        svfFilters.add(filter.release());
    }
}

void LowPass::processBlock(AudioBuffer<float>& buffer, ADSR& envelope, int numSamples) {
    const int numCh = buffer.getNumChannels();

    for (int ch = 0; ch < numCh; ++ch) {
        auto* channelData = buffer.getWritePointer(ch);

        for (int i = 0; i < numSamples; ++i) {
            float modFrequency = updateModulatedFrequency(envelope);
            if (i % 8 == 0)
                svfFilters.getUnchecked(ch)->setCutoffFrequency(modFrequency);
            channelData[i] = svfFilters.getUnchecked(ch)->processSample(0, channelData[i]);
        }
    }
}


void LowPass::setFrequency(double newValue) {
	frequency = jmin(newValue, sampleRate * 0.499);

	for (int i = 0; i < MAX_NUM_CH; ++i)
		svfFilters.getUnchecked(i)->setCutoffFrequency(frequency);
}

void LowPass::setResonance(double newValue) {
	resonance = newValue;

	for (int i = 0; i < MAX_NUM_CH; ++i)
		svfFilters.getUnchecked(i)->setResonance(resonance);
}

void LowPass::setEnvAmount(double newValue) {
	amount = newValue;
}

float LowPass::updateModulatedFrequency(ADSR& envelope) {
    float envValue = envelope.getNextSample();

    double modulatedFreq;

    double multFactor = (amount >= 0) ?
        (1.0 + amount * ((20000.0 / frequency) - 1.0)) : (1.0 + std::abs(amount) * ((20.0 / frequency) - 1.0));

    double modulationFactor = jmap(static_cast<double>(envValue), 0.0, 1.0, 1.0, multFactor);
    modulatedFreq = frequency * modulationFactor;

    modulatedFreq = jlimit(20.0, sampleRate * 0.499, modulatedFreq);
    return static_cast<float>(modulatedFreq);
}
