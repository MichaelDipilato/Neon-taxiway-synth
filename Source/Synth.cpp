#include "Synth.h"

bool SimpleSynthVoice::canPlaySound(SynthesiserSound* sound) {
    return dynamic_cast<MySynthSound*>(sound) != nullptr;
}

void SimpleSynthVoice::startNote(int midiNoteNumber, float velocity, SynthesiserSound* sound, int currentPitchWheelPosition) {

	// Cambio frequenza all'oscillatore (il secondo argomento a true indica di NON usare smoothed value)
	Oscillators[0].setFrequency(MidiMessage::getMidiNoteInHertz(midiNoteNumber), true);
	Oscillators[1].setFrequency(MidiMessage::getMidiNoteInHertz(midiNoteNumber), true);

	// Triggero l'ADSR
	ampAdsr.noteOn();
	filterAdsr.noteOn();

	// Mi salvo la velocity da usare come volume della nota suonata
	velocityLevel = velocity;
}

void SimpleSynthVoice::stopNote(float velocity, bool allowTailOff) {

	// Triggero la fase di release dell'ADSR
	ampAdsr.noteOff();
	filterAdsr.noteOff();

	// Se mi chiede di non generare code (o altri casi, tipo se l'ADSR ha già finito la fase di release)
	// allora segnalo alla classe Synthesiser che questa voce è libera per poter riprodurre altri suoni
	if (!allowTailOff || (!ampAdsr.isActive() && !filterAdsr.isActive()))
		clearCurrentNote();
}

void SimpleSynthVoice::renderNextBlock(AudioBuffer<float>& outputBuffer, int startSample, int numSamples) {

	// Se la voce non è attiva ci si può fermare qui.
	// In caso si voglia comunque "far trascorrere il tempo" per alcune componenti (ad esempio
	// far avanzare gli LFO anche mentre la voce non sta suonando), queste operazioni vanno
	// fatte prima di questo controllo.
	if (!isVoiceActive())
		return;

	// Pulizia del buffer di lavoro
	// (userò solo i primi "numSamples" del buffer, da sommare poi nel
	// buffer di output, da startSample, per numSamples campioni)
	oscillatorBuffer.clear(0, numSamples);

	// Preparazione del ProcessContext per le classi DSP
	// auto voiceData = oscillatorBuffer.getArrayOfWritePointers();
	// dsp::AudioBlock<float> audioBlock{ voiceData, 1, (size_t)numSamples };
	// dsp::ProcessContextReplacing<float> context{ audioBlock };

	// [Solitamente qui ci stanno cose tipo mixer degli oscillatori, filtro e saturazione]
	juce::AudioBuffer<float> tempBuffer1(1, numSamples);
	juce::AudioBuffer<float> tempBuffer2(1, numSamples);

	tempBuffer1.clear(0, numSamples);
	tempBuffer2.clear(0, numSamples);

	dsp::AudioBlock<float> audioBlock1{ tempBuffer1 };
	dsp::AudioBlock<float> audioBlock2{ tempBuffer2 };

	dsp::ProcessContextReplacing<float> context1{ audioBlock1 };
	dsp::ProcessContextReplacing<float> context2{ audioBlock2 };

	Oscillators[0].process(context1);
	Oscillators[1].process(context2);

	// Somma i due buffer nel buffer principale
	oscillatorBuffer.addFrom(0, 0, tempBuffer1, 0, 0, numSamples, 0.25f);
	oscillatorBuffer.addFrom(0, 0, tempBuffer2, 0, 0, numSamples, 0.25f);

	filter.processBlock(oscillatorBuffer, filterAdsr);

	normalizeSignal(oscillatorBuffer, numSamples);

	// La modulo in ampiezza con l'ADSR
	ampAdsr.applyEnvelopeToBuffer(oscillatorBuffer, 0, numSamples);

	// Volume proporzionale alla velocity
	oscillatorBuffer.applyGain(0, numSamples, velocityLevel);

	// Copio il segnale generato nel buffer di output considerando la porzione di competenza
	outputBuffer.addFrom(0, startSample, oscillatorBuffer, 0, 0, numSamples);

	// Se gli ADSR hanno finito la fase di decay (o se ho altri motivi per farlo)
	// segno la voce come libera per suonare altre note
	if (!(ampAdsr.isActive() || filterAdsr.isActive()))
		clearCurrentNote();
}

void SimpleSynthVoice::prepareToPlay(double sampleRate, int samplesPerBlock) {

	// Resetto l'ADSR
	ampAdsr.setSampleRate(sampleRate);
	filterAdsr.setSampleRate(sampleRate);
	ampAdsr.setParameters(ampAdsrParams);
	filterAdsr.setParameters(filterAdsrParams);

	filter.prepareToPlay(sampleRate);

	// Preparo le ProcessSpecs per l'oscillatore ed eventuali altre classi DSP
	dsp::ProcessSpec spec;
	spec.maximumBlockSize = samplesPerBlock;
	spec.sampleRate = sampleRate;
	spec.numChannels = 1;

	Oscillators[0].initialise([](float x) { return std::sin(x); });
	Oscillators[1].initialise([](float x) { return std::sin(x); });

	Oscillators[0].prepare(spec);
	Oscillators[1].prepare(spec);

	// Se non ho intenzione di generare un segnale intrinsecamente
	// stereo è inutile calcolare più di un canale. Ne calcolo 1 e
	// poi nel PluginProcessor lo copio su tutti i canali in uscita
	oscillatorBuffer.setSize(1, samplesPerBlock);
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

void SimpleSynthVoice::normalizeSignal(AudioBuffer<float>& oscillatorBuffer, float numSamples) {
	auto RMS = oscillatorBuffer.getRMSLevel(0,0,numSamples);
	auto targetRMS = 0.2f/RMS;
	auto* bufferData = oscillatorBuffer.getWritePointer(0);

	if (RMS < 0.01f) {
        return; // oppure imposta un valore di default, o lascia il buffer invariato
    }

	for (int i = 0; i < numSamples; ++i)
		bufferData[i] *= targetRMS;
}

void SimpleSynthVoice::setFilterFrequency(double newValue) {
	filter.setFrequency(newValue);
}

void SimpleSynthVoice::setFilterResonance(double newValue) {
	filter.setResonance(newValue);
}
