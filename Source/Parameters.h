#pragma once
#include <JuceHeader.h>

namespace Parameters {
	static const String nameWave1 = "W1";
	static const String nameWave2 = "W2";
	static const String nameAmpAtk = "AA";
	static const String nameAmpDcy = "AD";
	static const String nameAmpSus = "AS";
	static const String nameAmpRel = "AR";
	static const String nameFiltAtk = "FA";
	static const String nameFiltDcy = "FD";
	static const String nameFiltSus = "FS";
	static const String nameFiltRel = "FR";
	static const String nameCutoffFreq = "CF";
	static const String nameResonance = "RE";
	static const String nameEnvAmount = "EA";
	static const String nameOscMix = "OM";
	static const String nameOscDetune = "OD";
	static const String nameMasterGain = "MG";

	static const int defaultWave = 0;
	static const float defaultAmpAtk = 0.000f;
	static const float defaultAmpDcy = 0.025f;
	static const float defaultAmpSus = 1.000f;
	static const float defaultAmpRel = 1.000f;
	static const float defaultFiltAtk = 0.000f;
	static const float defaultFiltDcy = 0.025f;
	static const float defaultFiltSus = 1.000f;
	static const float defaultFiltRel = 1.000f;
	static const float defaultCutoffFreq = 1000.0f;
	static const float defaultResonance = 0.700f;
	static const float defaultEnvAmount = 1.000f;
	static const float defaultOscMix = 0.500f;
	static const float defaultOscDetune = 0.000f;
	static const float defaultMasterGain = 1.000f;

	static const float minCutoffFreq = 20.0f;

	const auto oscChoices = StringArray{ "Sine", "Saw", "Square", "Triangle"};

	static AudioProcessorValueTreeState::ParameterLayout createParameterLayout() {
		std::vector<std::unique_ptr<RangedAudioParameter>> params;

		int id = 1;

		params.push_back(std::make_unique<AudioParameterChoice>(ParameterID(nameWave1, id++), "1st osc waveform", oscChoices, defaultWave));
		params.push_back(std::make_unique<AudioParameterChoice>(ParameterID(nameWave2, id++), "2nd osc waveform", oscChoices, defaultWave));

		params.push_back(std::make_unique<AudioParameterFloat>(ParameterID(nameAmpAtk, id++), "Amp Attack (s)", NormalisableRange<float>(0.0f, 10.0f, 0.001f, 0.3f), defaultAmpAtk));
		params.push_back(std::make_unique<AudioParameterFloat>(ParameterID(nameAmpDcy, id++), "Amp Decay (s)", NormalisableRange<float>(0.0f, 10.0f, 0.001f, 0.3f), defaultAmpDcy));
		params.push_back(std::make_unique<AudioParameterFloat>(ParameterID(nameAmpSus, id++), "Amp Sustain (amp)", NormalisableRange<float>(0.0f, 1.00f, 0.010f, 0.5f), defaultAmpSus));
		params.push_back(std::make_unique<AudioParameterFloat>(ParameterID(nameAmpRel, id++), "Amp Release (s)", NormalisableRange<float>(0.0f, 10.0f, 0.001f, 0.3f), defaultAmpRel));
		params.push_back(std::make_unique<AudioParameterFloat>(ParameterID(nameFiltAtk, id++), "Filter Attack (s)", NormalisableRange<float>(0.0f, 10.0f, 0.001f, 0.3f), defaultFiltAtk));
		params.push_back(std::make_unique<AudioParameterFloat>(ParameterID(nameFiltDcy, id++), "Filter Decay (s)", NormalisableRange<float>(0.0f, 10.0f, 0.001f, 0.3f), defaultFiltDcy));
		params.push_back(std::make_unique<AudioParameterFloat>(ParameterID(nameFiltSus, id++), "Filter Sustain (amp)", NormalisableRange<float>(0.0f, 1.00f, 0.010f, 0.5f), defaultFiltSus));
		params.push_back(std::make_unique<AudioParameterFloat>(ParameterID(nameFiltRel, id++), "Filter Release (s)", NormalisableRange<float>(0.0f, 10.0f, 0.001f, 0.3f), defaultFiltRel));
		params.push_back(std::make_unique<AudioParameterFloat>(ParameterID(nameCutoffFreq, id++), "Cutoff Frequency", NormalisableRange<float>(minCutoffFreq, 20000.0f, 1.0f, 0.3f), defaultCutoffFreq));
		params.push_back(std::make_unique<AudioParameterFloat>(ParameterID(nameResonance, id++), "Filter Resonance", NormalisableRange<float>(0.1f, 10.0f, 0.001f, 0.3f), defaultResonance));
		params.push_back(std::make_unique<AudioParameterFloat>(ParameterID(nameEnvAmount, id++), "Envelope Amount", NormalisableRange<float>(-1.0f, 1.0f, 0.01f, 1.0f), defaultEnvAmount));
		params.push_back(std::make_unique<AudioParameterFloat>(ParameterID(nameOscMix, id++), "Oscillator Mix", NormalisableRange<float>(0.0f, 1.0f, 0.001f, 1.0f), defaultOscMix));
		params.push_back(std::make_unique<AudioParameterFloat>(ParameterID(nameOscDetune, id++), "Oscillator Detune", NormalisableRange<float>(0.0f, 1.0f, 0.001f, 0.3f), defaultOscDetune));
		params.push_back(std::make_unique<AudioParameterFloat>(ParameterID(nameMasterGain, id++), "Volume", NormalisableRange<float>(0.0f, 1.2f, 0.001f, 1.0f), defaultMasterGain));


		return { params.begin(), params.end() };
	}

	static void addListenerToAllParameters(AudioProcessorValueTreeState& valueTreeState, AudioProcessorValueTreeState::Listener* listener) {
		std::unique_ptr<XmlElement> xml(valueTreeState.copyState().createXml());

		for (auto element : xml->getChildWithTagNameIterator("PARAM")) {
			const String& id = element->getStringAttribute("id");
			valueTreeState.addParameterListener(id, listener);
		}
	}
}
