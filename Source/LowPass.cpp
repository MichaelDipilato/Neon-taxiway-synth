#include "LowPass.h"

void LowPass::prepareToPlay(double sr) {
	sampleRate = sr;
	svfFilters.clear();

	for (int i = 0; i < MAX_NUM_CH; ++i) {
        auto filter = std::make_unique<dsp::StateVariableTPTFilter<float>>();
			filter->setType(dsp::StateVariableTPTFilterType::lowpass);
			filter->setCutoffFrequency(frequency);
			filter->setResonance(resonance);

			svfFilters.add(filter.release());
    }
}

void LowPass::processBlock(AudioBuffer<float>& buffer, ADSR& envelope) {
    const int numCh = buffer.getNumChannels();
    const int numSamples = buffer.getNumSamples();

    AudioBuffer<float> modulatedFreqBuffer(1, numSamples);
    auto* freqData = modulatedFreqBuffer.getWritePointer(0);

    for (int i = 0; i < numSamples; ++i) {
        float envValue = (amount >= 0.0f)
            ? 1.0f - envelope.getNextSample()
            : envelope.getNextSample();

        double freqToSubtract = jmap(static_cast<double>(envValue), 0.0, 1.0, 500.0, frequency);
        double lowerLimit = std::max(0.0, frequency - 500.0);
        double modulatedFreq = frequency - std::abs(amount) * jlimit(0.0, lowerLimit, freqToSubtract);

        modulatedFreq = jlimit(20.0, sampleRate * 0.499, modulatedFreq);
        freqData[i] = static_cast<float>(modulatedFreq);
    }

    for (int ch = 0; ch < numCh; ++ch) {
        auto* channelData = buffer.getWritePointer(ch);

        for (int i = 0; i < numSamples; ++i) {
            svfFilters.getUnchecked(ch)->setCutoffFrequency(freqData[i]);
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

void LowPass::reset() {
    for (int i = 0; i < MAX_NUM_CH; ++i)
        svfFilters.getUnchecked(i)->reset();
}
