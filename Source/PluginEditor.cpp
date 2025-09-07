#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "Parameters.h"

NeonTaxiwayAudioProcessorEditor::NeonTaxiwayAudioProcessorEditor (NeonTaxiwayAudioProcessor& p, AudioProcessorValueTreeState& vts)
    : AudioProcessorEditor (&p), audioProcessor (p), valueTreeState(vts) {
    setupDiscreteSlider(osc1Slider, 40, 140, 80, 80, 3);
    setupDiscreteSlider(osc2Slider, 140, 140, 80, 80, 3);
    setupSlider(detuneSlider, 40, 260, 80, 80);
    setupSlider(oscMixSlider, 140, 260, 80, 80);
    setupSlider(filterAtkSlider, 280, 140, 80, 80);
    setupSlider(filterDcySlider, 380, 140, 80, 80);
    setupSlider(filterSusSlider, 280, 260, 80, 80);
    setupSlider(filterRelSlider, 380, 260, 80, 80);
    setupSlider(filterEnvAmtSlider, 580, 200, 80, 80);
    setupSlider(resonanceSlider, 480, 260, 80, 80);
    setupSlider(cutoffSlider, 480, 140, 80, 80);
    setupSlider(ampAtkSlider, 720, 140, 80, 80);
    setupSlider(ampDcySlider, 820, 140, 80, 80);
    setupSlider(ampSusSlider, 720, 260, 80, 80);
    setupSlider(ampRelSlider, 820, 260, 80, 80);
    setupSlider(volumeSlider, 820, 20, 80, 80);

    osc1Attachment.reset(new SliderAttachment(valueTreeState, Parameters::nameWave1, osc1Slider));
    osc2Attachment.reset(new SliderAttachment(valueTreeState, Parameters::nameWave2, osc2Slider));
    detuneAttachment.reset(new SliderAttachment(valueTreeState, Parameters::nameOscDetune, detuneSlider));
    mixAttachment.reset(new SliderAttachment(valueTreeState, Parameters::nameOscMix, oscMixSlider));
    filterAtkAttachment.reset(new SliderAttachment(valueTreeState, Parameters::nameFiltAtk, filterAtkSlider));
    filterDcyAttachment.reset(new SliderAttachment(valueTreeState, Parameters::nameFiltDcy, filterDcySlider));
    filterSusAttachment.reset(new SliderAttachment(valueTreeState, Parameters::nameFiltSus, filterSusSlider));
    filterRelAttachment.reset(new SliderAttachment(valueTreeState, Parameters::nameFiltRel, filterRelSlider));
    filterEnvAmtAttachment.reset(new SliderAttachment(valueTreeState, Parameters::nameEnvAmount, filterEnvAmtSlider));
    filterResoAttachment.reset(new SliderAttachment(valueTreeState, Parameters::nameResonance, resonanceSlider));
    filterCutoffAttachment.reset(new SliderAttachment(valueTreeState, Parameters::nameCutoffFreq, cutoffSlider));
    ampAtkAttachment.reset(new SliderAttachment(valueTreeState, Parameters::nameAmpAtk, ampAtkSlider));
    ampDcyAttachment.reset(new SliderAttachment(valueTreeState, Parameters::nameAmpDcy, ampDcySlider));
    ampSusAttachment.reset(new SliderAttachment(valueTreeState, Parameters::nameAmpSus, ampSusSlider));
    ampRelAttachment.reset(new SliderAttachment(valueTreeState, Parameters::nameAmpRel, ampRelSlider));
    volumeAttachment.reset(new SliderAttachment(valueTreeState, Parameters::nameMasterGain, volumeSlider));

	myThemeOsc.setNumThicks(4);
    
    this->setLookAndFeel(&myTheme);
	osc1Slider.setLookAndFeel(&myThemeOsc);
	osc2Slider.setLookAndFeel(&myThemeOsc);
    setSize (945, 400);
}

NeonTaxiwayAudioProcessorEditor::~NeonTaxiwayAudioProcessorEditor() {
    this->setLookAndFeel(nullptr);
    osc1Slider.setLookAndFeel(nullptr);
    osc2Slider.setLookAndFeel(nullptr);
}

void NeonTaxiwayAudioProcessorEditor::paint (juce::Graphics& g) {
    
    g.setGradientFill(juce::ColourGradient(juce::Colour(90, 40, 80), 0, 0, juce::Colour(40, 40, 60), getWidth(), getHeight(), false));
    g.fillAll ();

    drawTileAndSubtitle(g);

    drawGrid(g);

    drawKnobSectionBoxes(g);

    drawKnobLabels(g);
}

void NeonTaxiwayAudioProcessorEditor::resized() {
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
}

void NeonTaxiwayAudioProcessorEditor::setupSlider(Slider& slider, int x, int y, int w, int h) {
    slider.setSliderStyle(Slider::SliderStyle::RotaryVerticalDrag);
    slider.setTextBoxStyle(Slider::TextBoxBelow, false, 80, 20);
    addAndMakeVisible(&slider);
    slider.setBounds(x, y, w, h);
}

void NeonTaxiwayAudioProcessorEditor::setupDiscreteSlider(Slider& slider, int x, int y, int w, int h, int numSteps) {
    slider.setSliderStyle(Slider::SliderStyle::RotaryVerticalDrag);
    slider.setTextBoxStyle(Slider::TextBoxBelow, false, 80, 20);
    slider.setRange(0, numSteps - 1, 1.0);
    slider.setVelocityBasedMode(false);
    addAndMakeVisible(&slider);
    slider.setBounds(x, y, w, h);
}

void NeonTaxiwayAudioProcessorEditor::drawGrid(juce::Graphics& g) {
    int horizonY = 100;
    int gridBottom = getHeight();
    int vanishingX = getWidth() / 2;

    g.setColour(juce::Colour(0xffFF00FF).withAlpha(0.3f));

    int numHorizontalLines = 12;
    float spacingY = (gridBottom - horizonY) / (float)numHorizontalLines;

    for (int i = 0; i <= numHorizontalLines; ++i) {
        float y = horizonY + i * spacingY;
        g.drawLine(0.0f, y, (float)getWidth(), y, 1.0f);
    }

    int numVerticalLines = 30;
    float spread = getWidth() * 2.0f / (float)(numVerticalLines - 1);

    for (int i = 0; i < numVerticalLines; ++i) {
        float x = (i * spread) - 472.5f;

        float offset = (x - vanishingX) * 0.45f;
        float targetX = vanishingX + offset;

        g.drawLine(x, (float)gridBottom, targetX, (float)horizonY, 1.0f);
    }
}

void NeonTaxiwayAudioProcessorEditor::drawTileAndSubtitle(juce::Graphics& g) {
    g.setColour(juce::Colour(0xff8000FF));

    g.fillRoundedRectangle(-50.0f, 15.0f, 580.0f, 68.0f, 30.0f);

    juce::FontOptions titleOptions("Liberation Mono", 45.0f, juce::Font::bold | juce::Font::italic);
    juce::Font titleFont(titleOptions);
    g.setFont(titleFont);
    g.setColour(juce::Colours::red.withAlpha(0.6f));
    g.drawText("NEON TAXIWAY", 22, 17, 540, 50, juce::Justification::topLeft, true);

    g.setColour(juce::Colours::yellow);
    g.drawText("NEON TAXIWAY", 20, 15, 540, 50, juce::Justification::topLeft, true);

    juce::FontOptions subtitleOptions("Liberation Mono", 20.0f, juce::Font::italic);
    juce::Font subtitleFont(subtitleOptions);
    g.setFont(subtitleFont);
    g.setColour(juce::Colours::red.withAlpha(0.6f));
    g.drawText("A synth not yet cleared for takeoff...", 22, 60, 740, 20, juce::Justification::left, true);

    g.setColour(juce::Colours::yellow);
    g.drawText("A synth not yet cleared for takeoff...", 20, 58, 740, 20, juce::Justification::left, true);
}

void NeonTaxiwayAudioProcessorEditor::drawKnobSectionBoxes(juce::Graphics& g) {
    g.setColour(juce::Colours::yellow);

    g.drawRoundedRectangle(25, 110, 215, 260, 10.0f, 7.0f);
    g.drawRoundedRectangle(265, 110, 415, 260, 10.0f, 7.0f);
    g.drawRoundedRectangle(705, 110, 215, 260, 10.0f, 7.0f);

    g.setFont(juce::FontOptions("Liberation Mono", 20.0f, juce::Font::bold | juce::Font::italic));

    g.setColour(juce::Colour(0xff8000FF));
    g.fillRoundedRectangle(67, 355, 130, 30, 6.0f);
    g.setColour(juce::Colours::red.withAlpha(0.6f));
    g.drawText("Oscillators", 74, 358, 120, 30, juce::Justification::centred);
    g.setColour(juce::Colours::yellow);
    g.drawText("Oscillators", 72, 356, 120, 30, juce::Justification::centred);

    g.setColour(juce::Colour(0xff8000FF));
    g.fillRoundedRectangle(427, 355, 90, 30, 6.0f);
    g.setColour(juce::Colours::red.withAlpha(0.6f));
    g.drawText("Filter", 429, 358, 90, 30, juce::Justification::centred);
    g.setColour(juce::Colours::yellow);
    g.drawText("Filter", 427, 356, 90, 30, juce::Justification::centred);

    g.setColour(juce::Colour(0xff8000FF));
    g.fillRoundedRectangle(758, 355, 110, 30, 6.0f);
    g.setColour(juce::Colours::red.withAlpha(0.6f));
    g.drawText("Amplifier", 765, 358, 100, 30, juce::Justification::centred);
    g.setColour(juce::Colours::yellow);
    g.drawText("Amplifier", 763, 356, 100, 30, juce::Justification::centred);
}

void NeonTaxiwayAudioProcessorEditor::drawKnobLabels(juce::Graphics& g) {
    g.setColour (juce::Colours::white);
    g.setFont (juce::FontOptions ("Liberation Mono", 15.0f, juce::Font::bold | juce::Font::italic));

	g.drawText("1st Wave", 35, 120, 90, 20, juce::Justification::centred);
    g.drawText("2nd Wave", 135, 120, 90, 20, juce::Justification::centred);
    g.drawText("Detune", 40, 240, 80, 20, juce::Justification::centred);
    g.drawText("Osc Mix", 140, 240, 80, 20, juce::Justification::centred);
    g.drawText("Attack", 280, 120, 80, 20, juce::Justification::centred);
    g.drawText("Decay", 380, 120, 80, 20, juce::Justification::centred);
    g.drawText("Sustain", 280, 240, 80, 20, juce::Justification::centred);
    g.drawText("Release", 380, 240, 80, 20, juce::Justification::centred);
    g.drawText("Cutoff", 480, 120, 80, 20, juce::Justification::centred);
    g.drawText("Resonance", 480, 240, 80, 20, juce::Justification::centred);
    g.drawText("Amount", 580, 180, 80, 20, juce::Justification::centred);
    g.drawText("Attack", 720, 120, 80, 20, juce::Justification::centred);
    g.drawText("Decay", 820, 120, 80, 20, juce::Justification::centred);
    g.drawText("Sustain", 720, 240, 80, 20, juce::Justification::centred);
    g.drawText("Release", 820, 240, 80, 20, juce::Justification::centred);
    g.drawText("Volume", 820, 0, 80, 20, juce::Justification::centred);
}
