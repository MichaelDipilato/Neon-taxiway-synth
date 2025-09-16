#include "Synth.h"

bool SimpleSynthVoice::canPlaySound(SynthesiserSound* sound) {
    return dynamic_cast<MySynthSound*>(sound) != nullptr;
}

void SimpleSynthVoice::startNote(int midiNoteNumber, float velocity, SynthesiserSound* sound, int currentPitchWheelPosition) {

	auto baseFrequency = MidiMessage::getMidiNoteInHertz(midiNoteNumber);
	auto detuneAmt = oscDetune * 20.0f;

	Oscillators[0].setFrequency(baseFrequency * std::pow(2.0f, detuneAmt / 1200.0f), true);
	Oscillators[1].setFrequency(baseFrequency * std::pow(2.0f, -detuneAmt / 1200.0f), true);

	ampAdsr.noteOn();
	filterAdsr.noteOn();

	velocityLevel = velocity;
}

void SimpleSynthVoice::stopNote(float velocity, bool allowTailOff) {

	ampAdsr.noteOff();
	filterAdsr.noteOff();

	if (!allowTailOff || (!ampAdsr.isActive() && !filterAdsr.isActive()))
		clearCurrentNote();
}

void SimpleSynthVoice::renderNextBlock(AudioBuffer<float>& outputBuffer, int startSample, int numSamples) {

	if (!isVoiceActive())
		return;

	oscillatorBuffer.clear(0, numSamples);
	oscBuffer1.clear(0, numSamples);
	oscBuffer2.clear(0, numSamples);

	auto voiceData1 = oscBuffer1.getArrayOfWritePointers();
	auto voiceData2 = oscBuffer2.getArrayOfWritePointers();
	dsp::AudioBlock<float> audioBlock1{ voiceData1, 1, (size_t)numSamples };
	dsp::AudioBlock<float> audioBlock2{ voiceData2, 1, (size_t)numSamples };

	dsp::ProcessContextReplacing<float> context1{ audioBlock1 };
	dsp::ProcessContextReplacing<float> context2{ audioBlock2 };

	Oscillators[0].process(context1);
	Oscillators[1].process(context2);

	oscillatorBuffer.addFrom(0, 0, oscBuffer1, 0, 0, numSamples, 0.5f * (1 - oscMix));
	oscillatorBuffer.addFrom(0, 0, oscBuffer2, 0, 0, numSamples, 0.5f * (oscMix));

	filter.processBlock(oscillatorBuffer, filterAdsr, numSamples);

	ampAdsr.applyEnvelopeToBuffer(oscillatorBuffer, 0, numSamples);

	oscillatorBuffer.applyGain(0, numSamples, velocityLevel * gain);

	outputBuffer.addFrom(0, startSample, oscillatorBuffer, 0, 0, numSamples);

	if (!(ampAdsr.isActive() || filterAdsr.isActive()))
		clearCurrentNote();
}

void SimpleSynthVoice::prepareToPlay(double sampleRate, int samplesPerBlock) {

	ampAdsr.setSampleRate(sampleRate);
	filterAdsr.setSampleRate(sampleRate);
	ampAdsr.setParameters(ampAdsrParams);
	filterAdsr.setParameters(filterAdsrParams);

	dsp::ProcessSpec spec;
	spec.maximumBlockSize = samplesPerBlock;
	spec.sampleRate = sampleRate;
	spec.numChannels = 1;

	filter.prepareToPlay(sampleRate, spec);

	if(!(Oscillators[0].isInitialised() && Oscillators[1].isInitialised())) {
		setWaveform(0, 0);
		setWaveform(1, 0);
	}

	Oscillators[0].prepare(spec);
	Oscillators[1].prepare(spec);
		
	oscillatorBuffer.setSize(1, samplesPerBlock);
	oscBuffer1.setSize(1, samplesPerBlock);
	oscBuffer2.setSize(1, samplesPerBlock);
}

void SimpleSynthVoice::setAttack(const float newValue, bool isAmp) {
	if(isAmp) {
		ampAdsrParams.attack = newValue;
		ampAdsr.setParameters(ampAdsrParams);
	} else {
		filterAdsrParams.attack = newValue;
		filterAdsr.setParameters(filterAdsrParams);
	}
}

void SimpleSynthVoice::setDecay(const float newValue, bool isAmp) {
	if(isAmp) {
		ampAdsrParams.decay = newValue;
		ampAdsr.setParameters(ampAdsrParams);
	} else {
		filterAdsrParams.decay = newValue;
		filterAdsr.setParameters(filterAdsrParams);
	}
}

void SimpleSynthVoice::setSustain(const float newValue, bool isAmp) {
	if(isAmp) {
		ampAdsrParams.sustain = newValue;
		ampAdsr.setParameters(ampAdsrParams);
	} else {
		filterAdsrParams.sustain = newValue;
		filterAdsr.setParameters(filterAdsrParams);
	}
}

void SimpleSynthVoice::setRelease(const float newValue, bool isAmp) {
	if(isAmp) {
		ampAdsrParams.release = newValue;
		ampAdsr.setParameters(ampAdsrParams);
	} else {
		filterAdsrParams.release = newValue;
		filterAdsr.setParameters(filterAdsrParams);
	}
}

void SimpleSynthVoice::setWaveform(const int i, const int newValue) {
	switch (newValue) {
	case 0: // Sine
		Oscillators[i].initialise([](float x) { return std::sin(x); });
		break;
	case 1: // Saw
		Oscillators[i].initialise([](float x) { return x / MathConstants<float>::pi; });
		break;
	case 2: // Square
		Oscillators[i].initialise([](float x) { return x < 0.0f ? -1.0f : 1.0f; });
		break;
	case 3: // Triangle
		Oscillators[i].initialise([](float x) { return -2.0f * std::abs(x / juce::MathConstants<float>::pi) + 1.0f; });
		break;
	}
}

void SimpleSynthVoice::setOscMix(const float newValue) {
	oscMix = newValue;
}

void SimpleSynthVoice::setOscDetune(const float newValue) {
	oscDetune = newValue;
}

void SimpleSynthVoice::setGain(const float newValue) {
	gain = newValue;
}

void SimpleSynthVoice::setFilterFrequency(double newValue) {
	filter.setFrequency(newValue);
}

void SimpleSynthVoice::setFilterResonance(double newValue) {
	filter.setResonance(newValue);
}

void SimpleSynthVoice::setFilterEnvAmount(double newValue) {
	filter.setEnvAmount(newValue);
}
