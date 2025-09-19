#pragma once

#include <JuceHeader.h>
#include "LowPass.h"

class MySynthSound : public SynthesiserSound {
public:
	MySynthSound() {}
	~MySynthSound() {}

	bool appliesToNote(int midiNoteNumber) override { return true; }
	bool appliesToChannel(int midiChannel) override { return true; }

private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MySynthSound)
};

class SimpleSynthVoice : public SynthesiserVoice {
public:
	SimpleSynthVoice(float defaultAmpAtk = 0.0f, float defaultAmpDcy = 0.025f, float defaultAmpSus = 1.0f, float defaultAmpRel = 1.0f,
					 float defaultFiltAtk = 0.0f, float defaultFiltDcy = 0.025f, float defaultFiltSus = 1.0f, float defaultFiltRel = 1.0f)
		: ampAdsrParams(defaultAmpAtk, defaultAmpDcy, defaultAmpSus, defaultAmpRel), filterAdsrParams(defaultFiltAtk, defaultFiltDcy, defaultFiltSus, defaultFiltRel) {
	};

	~SimpleSynthVoice() {};

	bool canPlaySound(SynthesiserSound* sound) override;

	void startNote(int midiNoteNumber, float velocity, SynthesiserSound* sound, int currentPitchWheelPosition) override;

	void stopNote(float velocity, bool allowTailOff) override;

	void controllerMoved(int controllerNumber, int newControllerValue) override {}

	void pitchWheelMoved(int newPitchWheelValue) override {}

	void renderNextBlock(AudioBuffer<float>& outputBuffer, int startSample, int numSamples) override;

	void prepareToPlay(double sampleRate, int samplesPerBlock);

	void setAttack(const float newValue, bool isAmp);

	void setDecay(const float newValue, bool isAmp);

	void setSustain(const float newValue, bool isAmp);

	void setRelease(const float newValue, bool isAmp);

	void setWaveform(const int i, const int newValue);

	void setOscMix(const float newValue);

	void setOscDetune(const float newValue);

	void setGain(const float newValue);

	void setFilterFrequency(double newValue);

	void setFilterResonance(double newValue);

	void setFilterEnvAmount(double newValue);

private:
	std::array<dsp::Oscillator<float>,2> Oscillators;

	ADSR ampAdsr;
	ADSR filterAdsr;
	ADSR::Parameters ampAdsrParams;
	ADSR::Parameters filterAdsrParams;

	AudioBuffer<float> oscillatorBuffer;
	AudioBuffer<float> oscBuffer1;
	AudioBuffer<float> oscBuffer2;
	float velocityLevel = 1.0;
	float oscMix = 0.5f;
	float oscDetune = 0.0f;
	float gain = 1.0f;
	LowPass filter;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SimpleSynthVoice)
};
