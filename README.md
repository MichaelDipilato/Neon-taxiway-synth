# Neon Taxiway

A synth not yet cleared for takeoff...
![](Screenshot/NeonTaxiwayGUI.png)

## Overview

Neon Taxiway is a simple two-oscillator synthesizer developed as a project for the "Development of Technologies for Music Production" course at the University of Milan.
The synth was built using the [JUCE framework](https://github.com/juce-framework/JUCE) (v 8.0.6). As a starting point, I referenced [this synth](https://github.com/Kuig/DemoSynth).

## Features

- 2 Oscillators
- Low-pass
- Filter ADSR envelope with amount control
- Amplitude ADSR envelope

## Oscillators

The synthesizer features two oscillators, each capable of producing four waveforms:
- Sine
- Sawtooth
- Square
- Triangle

You can detune the oscillators (especially effective when both use the sawtooth waveform) and blend them using the mix control.

## Filter

Neon Taxiway includes a simple low-pass filter with adjustable:
- Cutoff frequency
- Resonance 

The filter is modulated by an ADSR envelope, with a dedicated control for the envelope amount. Additionally, there's a second ADSR envelope for the amplifier.

## OS Compatibility

This synth was developed on Linux, but also works on Windows. In order to install Neon Taxiway, you can clone this repo, open the projucer project and then compile it for yourself using your preferred platform.

## Future improvements

- Remove aliasing from oscillators
- Add and LFO
- Add a "magic" button that applies an effect (chorus maybe?) 
- Publish a release here on GitHub
