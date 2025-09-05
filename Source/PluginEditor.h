#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "NeonTaxiwayTheme.h"

typedef AudioProcessorValueTreeState::SliderAttachment SliderAttachment;

class NeonTaxiwayAudioProcessorEditor  : public juce::AudioProcessorEditor {
public:
    NeonTaxiwayAudioProcessorEditor (NeonTaxiwayAudioProcessor&, AudioProcessorValueTreeState&);
    ~NeonTaxiwayAudioProcessorEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    void setupSlider(Slider& slider, int x, int y, int w, int h);

    void setupDiscreteSlider(Slider& slider, int x, int y, int w, int h, int numSteps);

    NeonTaxiwayAudioProcessor& audioProcessor;

    AudioProcessorValueTreeState& valueTreeState;

    Slider volumeSlider;

    Slider detuneSlider;
    Slider oscMixSlider;
    
    Slider ampAtkSlider;
    Slider ampDcySlider;
    Slider ampSusSlider;
    Slider ampRelSlider;

    Slider filterAtkSlider;
    Slider filterDcySlider;
    Slider filterSusSlider;
    Slider filterRelSlider;
    Slider filterEnvAmtSlider;
    Slider resonanceSlider;
    Slider cutoffSlider;

    Slider osc1Slider;
    Slider osc2Slider;
 
    NeonTaxiwayLookAndFeel myTheme;
    NeonTaxiwayLookAndFeel myThemeOsc;

    std::unique_ptr<SliderAttachment> volumeAttachment;

    std::unique_ptr<SliderAttachment> detuneAttachment;
    std::unique_ptr<SliderAttachment> mixAttachment;

    std::unique_ptr<SliderAttachment> ampAtkAttachment;
    std::unique_ptr<SliderAttachment> ampDcyAttachment;
    std::unique_ptr<SliderAttachment> ampSusAttachment;
    std::unique_ptr<SliderAttachment> ampRelAttachment; 

    std::unique_ptr<SliderAttachment> filterAtkAttachment;
    std::unique_ptr<SliderAttachment> filterDcyAttachment;
    std::unique_ptr<SliderAttachment> filterSusAttachment;
    std::unique_ptr<SliderAttachment> filterRelAttachment;
    std::unique_ptr<SliderAttachment> filterEnvAmtAttachment;
    std::unique_ptr<SliderAttachment> filterResoAttachment;
    std::unique_ptr<SliderAttachment> filterCutoffAttachment;

    std::unique_ptr<SliderAttachment> osc1Attachment;
    std::unique_ptr<SliderAttachment> osc2Attachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (NeonTaxiwayAudioProcessorEditor)
};
