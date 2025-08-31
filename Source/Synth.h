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
	SimpleSynthVoice(float defaultAmpAtk = 0.0f, float defaultAmpDcy = 0.025f, float defaultAmpSus = 0.6f, float defaultAmpRel = 1.0f,
					 float defaultFiltAtk = 0.0f, float defaultFiltDcy = 0.025f, float defaultFiltSus = 0.6f, float defaultFiltRel = 1.0f)
		: ampAdsrParams(defaultAmpAtk, defaultAmpDcy, defaultAmpSus, defaultAmpRel), filterAdsrParams(defaultFiltAtk, defaultFiltDcy, defaultFiltSus, defaultFiltRel) {
	};

	~SimpleSynthVoice() {};

	bool canPlaySound(SynthesiserSound* sound) override;

	void startNote(int midiNoteNumber, float velocity, SynthesiserSound* sound, int currentPitchWheelPosition) override;

	// Metodo chiamato dalla classe Synthesiser per ogni Note-off assegnato a questa voce
	void stopNote(float velocity, bool allowTailOff) override;

	// Metodo chiamato dalla classe Synthesiser per ogni Control Change ricevuto
	void controllerMoved(int controllerNumber, int newControllerValue) override {}

	// Metodo chiamato dalla classe Synthesiser in caso venga mossa la pitch-wheel
	void pitchWheelMoved(int newPitchWheelValue) override {}

	// Metodo chiamato da Synthesiser per chiedere alla voce di riempire l'outputBuffer.
	// Noterete che non c'è il buffer MIDI, questo perchè in caso di eventi MIDI all'interno di
	// un buffer di lunghezza normale, la classe Synthsiser spezza il buffer in più blocchi:
	// MIDI events:   !.......!....!...!.........!.....
	// AudioBuffers:  |.....|.|....|...|.....|...|.....
	// In questo modo si può renderizzare l'audio senza preoccuparsi degli eventi MIDI (che
	// sono gestiti attraverso gli altri metodi precedenti).
	// ATTENZIONE: Questo è realizzato non istanziando diversi mini-buffer, ma passando il buffer
	// intero insieme a due interi: startSample e numSamples, tenete sempre conto di questi due valori
	// quando andate a riempire l'output buffer!
	// Si noti che lo steso buffer viene passato a tutte le voci di polifonia, non va quindi riempito
	// sovrascrivendo il contenuto, ma gli va sommato sopra quanto generato nella SynthVoice.
	//
	void renderNextBlock(AudioBuffer<float>& outputBuffer, int startSample, int numSamples) override;

	// ==== Metodi personali ====

	// Prepare to play della singola voce di polifonia. Non è prevista dalla classe astratta
	// (c'è un metodo setSampleFrequency o qualcosa del genere che svolge una funzione simile)
	// ma è comodo averla per poter usare il sint come un normale Processor
	void prepareToPlay(double sampleRate, int samplesPerBlock);

	void setAttack(const float newValue, bool isAmp);

	void setDecay(const float newValue, bool isAmp);

	void setSustain(const float newValue, bool isAmp);

	void setRelease(const float newValue, bool isAmp);

	void setWaveform(const int i, const int newValue);

	void setOscMix(const float newValue);

	void setOscDetune(const float newValue);

	void setFilterFrequency(double newValue);

	void setFilterResonance(double newValue);

	void setFilterEnvAmount(double newValue);

private:
	void normalizeSignal(AudioBuffer<float>& oscillatorBuffer, float numSamples);
	// La classe dsp::Oscillator può essere inizializzata con una lambda da usare come forma d'onda
	// (x va da -pi a + pi) e con un intero facoltativo che (se presente e diverso da 0) indica alla
	// classe di usare una lookup table di quelle dimensioni.
	// Se si vuole usare un oscillatore dalla forma d'onda variabile o con più parametri è meglio
	// implementarne uno come quello visto a lezione.
	std::array<dsp::Oscillator<float>,2> Oscillators;

	// La classe di JUCE ADSR mette a disposizione un generatore di inviluppo a 4
	// sezioni (Attack, Decay, Sustain, Release) e una struttura dati che permette
	// di cambiare i 4 parametri.
	// Onestamente non mi piace molto l'ADSR di JUCE per la modulazione
	// dell'inviluppo d'ampiezza, perchè le fasi di A, D e R sono di tipo lineare,
	// mentre suonerebbe più naturale averle di tipo esponenziale...
	ADSR ampAdsr;
	ADSR filterAdsr;
	ADSR::Parameters ampAdsrParams;
	ADSR::Parameters filterAdsrParams;

	AudioBuffer<float> oscillatorBuffer;
	float velocityLevel = 1.0;
	float oscMix = 0.5f;
	float oscDetune = 0.0f;
	LowPass filter;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SimpleSynthVoice)
};
