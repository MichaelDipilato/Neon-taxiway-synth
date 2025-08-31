#pragma once

#include <JuceHeader.h>
#include "Parameters.h"

#define MAX_NUM_CH 2

class LowPass {
public:
	LowPass(double defaultFrequency = 1000.0, double defaultResonance = 0.7)
		: frequency(defaultFrequency), resonance(defaultResonance) {
		for (int i = 0; i < MAX_NUM_CH; ++i) {
			svfFilters.add(std::make_unique<dsp::StateVariableTPTFilter<float>>());
		}
	}

	~LowPass() {}

	void prepareToPlay(double sr);

	void processBlock(AudioBuffer<float>& buffer, ADSR& envelope);

	void setFrequency(double newValue);

	void setResonance(double newValue);

	void setEnvAmount(double newValue);

	void reset();

private:
	void updateCoefficients();

	double frequency;
	double resonance;
	double amount;
	double sampleRate = 44100.0;

	OwnedArray< juce::dsp::StateVariableTPTFilter<float>> svfFilters;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LowPass)
};
