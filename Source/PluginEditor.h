#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

class NeonTaxiwayAudioProcessorEditor  : public juce::AudioProcessorEditor {
public:
    NeonTaxiwayAudioProcessorEditor (NeonTaxiwayAudioProcessor&);
    ~NeonTaxiwayAudioProcessorEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    NeonTaxiwayAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (NeonTaxiwayAudioProcessorEditor)
};
