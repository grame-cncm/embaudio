# Lecture 5: Sound Synthesis Basics

## History of Sound Synthesis

TODO

Explain that what we're presenting here is not exhaustive.

## Wave Shape Synthesis

Wave Shape synthesis is one of the most basic sound synthesis technique. It consists of using oscillators producing waveforms of different shapes to generate sound. The most standard wave shapes are:

* [sine wave](https://en.wikipedia.org/wiki/Square_wave#/media/File:Waveforms.svg),
* [square wave](https://en.wikipedia.org/wiki/Square_wave#/media/File:Waveforms.svg),
* [triangle wave](https://en.wikipedia.org/wiki/Square_wave#/media/File:Waveforms.svg),
* [sawtooth wave](https://en.wikipedia.org/wiki/Square_wave#/media/File:Waveforms.svg).

The [`sine-control`](https://github.com/grame-cncm/embaudio20/tree/master/examples/sine-control) and [`crazy-sine`](https://github.com/grame-cncm/embaudio20/tree/master/examples/crazy-sine) examples can be considered as "wave shape synthesis" in that regard. 

The [`crazy-saw`](https://github.com/grame-cncm/embaudio20/tree/master/examples/crazy-saw) example is very similar to `crazy-sine`, but it's based on a sawtooth wave instead. The sawtooth wave is created by using a `phasor` object. Just as a reminder, a phasor produces a signals tamping from 0 to 1 at a given frequency, it can therefore be seen as a sawtooth wave. Since the range of oscillators must be bounded between -1 and 1, we adjusts the output of the phasor such that:

```
float currentSample = sawtooth.tick()*2 - 1;
``` 

Feel free to try the crazy-saw example at this point.

## Amplitude Modulation (AM) Synthesis

Amplitude modulation synthesis consists of modulating the amplitude of a signal with another one. Sine waves are typically used for that:

<figure>
<img src="img/am.png" class="mx-auto d-block" width="90%">
<figcaption><center>Amplitude Modulation (Source: <a href="https://en.wikipedia.org/wiki/Amplitude_modulation#/media/File:Illustration_of_Amplitude_Modulation.png">Wikipedia</a>)</center></figcaption>
</figure>

When the frequency of the modulator is low (bellow 20Hz), our ear is able to distinguish each independent "beat", creating a tremolo effect. However, above 20Hz two side bands (if sine waves are used) start appearing following this rule:

<figure>
<img src="img/am-spectrum.svg" class="mx-auto d-block" width="80%">
<figcaption><center>Amplitude Modulation Spectrum (Source: <a href="https://en.wikipedia.org/wiki/Amplitude_modulation#/media/File:AM_spectrum.svg">Wikipedia</a>)</center></figcaption>
</figure> 

The mathematical proof of this can be found on [Julius Smith's website](https://ccrma.stanford.edu/~jos/mdft/Sinusoidal_Amplitude_Modulation_AM.html).

[`Am.cpp`](https://github.com/grame-cncm/embaudio20/tree/master/examples/lib) implements a sinusoidal amplitude modulation synthesizer:

```
float Am::tick(){
  int cIndex = cPhasor.tick()*SINE_TABLE_SIZE;
  int mIndex = mPhasor.tick()*SINE_TABLE_SIZE;
  float posMod = sineTable.tick(mIndex)*0.5 + 0.5;
  return sineTable.tick(cIndex)*(1 - posMod*modIndex)*gain;
}
```

Note that phasors are used instead of "complete" sine wave oscillators to save the memory of an extra sine wave table. The range of the modulating oscillator is adjusted to be {0,1} instead of {-1,1}.

The amplitude parameter of the modulating oscillator is called the *index of modulation* and its frequency, the *frequency of modulation*.

In practice, the same result could be achieved using additive synthesis and three sine wave oscillators but AM allows us to save one oscillator. Also, AM is usually used an audio effect and modulation is applied to an input signal in that case instead of a sine wave. Sidebands will then be produced for each harmonic of the processed sound.

The [`am` example](https://github.com/grame-cncm/embaudio20/tree/master/examples/am) demonstrates a use case of an AM synthesizer. Use the `Rec` and `Mode` button to cycle through the parameters of the synth and change their value.

## Frequency Modulation (FM) Synthesis

Frequency modulation synthesis consists in modulating the frequency of an oscillator with another one:

<figure>
<img src="img/fm.svg" class="mx-auto d-block" width="90%">
<figcaption><center>Frequency Modulation (Source: <a href="https://en.wikipedia.org/wiki/Frequency_modulation_synthesis">Wikipedia</a>)</center></figcaption>
</figure>

which mathematically can be expressed as:

\[x(t) = A_c\sin[\omega_ct + \phi_c + A_m\sin(\omega_mt + \phi_m)]\]

where \(c\) denotes the carrier and \(m\), the modulator.

As for AM, the frequency of the modulating oscillator is called the *frequency of modulation* and the amplitude of the modulating oscillator, the *index of modulation*. Unlike AM, the value of the index of modulation can exceed 1 which will increase the number of sidebands. FM is not limited to two sidebands and can have an infinite number of sidebands depending on the value of the index. The mathematical rational behind this can be found on [Julius Smith's website](https://ccrma.stanford.edu/~jos/mdft/FM_Spectra.html).

[`fm.cpp`](https://github.com/grame-cncm/embaudio20/blob/master/examples/lib/Fm.cpp) provides a simple example of how an FM synthesizer can be implemented:

```
float Fm::tick(){
  int mIndex = mPhasor.tick()*SINE_TABLE_SIZE;
  float modulator = sineTable.tick(mIndex);
  cPhasor.setFrequency(cFreq + modulator*modIndex);
  int cIndex = cPhasor.tick()*SINE_TABLE_SIZE;
  return sineTable.tick(cIndex)*gain;
}
```

Note that as for the AM example, we're saving an extra sine wave table by using the same one for both oscillators.

The examples folder of the course repository hosts [a simple LyraT program](https://github.com/grame-cncm/embaudio20/tree/master/examples/fm) illustrating the use of FM. Use the `Rec` and `Mode` button to cycle through the parameters of the synth and change their value.

FM synthesis was discovered in the late 1960s by John Chowning at Stanford University in California. He's now considered as one of the funding fathers of music technology and computer music. FM completely revolutionized the world of music in the 1980s by allowing Yamaha to produce the first commercial digital synthesizers: the [DX7](https://en.wikipedia.org/wiki/Yamaha_DX7) which met a huge success. FM synthesis is the second most profitable patent that Stanford ever had.

## Physical Modeling: the Simple Case of the Karplus Strong

Physical modeling is one of the most advanced sound synthesis technique and a very active field of research. It consists of using physics/mathematical models of musical instruments or vibrating structures to synthesize sound. Various physical modeling techniques are used in the field of audio synthesis:

* Mass/Interaction (MI),
* Finite Difference Scheme (FDS),
* Signal models (e.g., waveguides, modal systems, etc.). 

While MI and FDS model the vibrational behavior of a system (i.e., using partial differential equation in the case of FDS), signal models model an object as a combination of signal processors. In this section, we will only look at this type of model the other ones being out of the scope of this class.

A extremely primitive string model can be implemented using a delay line and a loop. The delay line models the time it takes for vibration in the string to go from one extremity to the other, and the loop models the reflections at the boundaries of the string. In other words, we can literally just reuse [the echo algorithm](TODO) for this. This primitive string model is called the "Karplus-Strong" algorithm:

<figure>
<img src="img/karplus.svg" class="mx-auto d-block" width="60%">
<figcaption><center>Karplus-Strong Algorithm (Source: <a href="https://en.wikipedia.org/wiki/Karplus%E2%80%93Strong_string_synthesis#/media/File:Karplus-strong-schematic.svg">Wikipedia</a>)</center></figcaption>
</figure>

The Karplus-Strong algorithm is typically implemented as:

\[y(n) = x(n) + \alpha\frac{y(n-L) + y(n-L-1)}{2}\]

where:

* \(x(n)\) is the input signal (typically an dirac or a noise burst),
* \(\alpha\) is the feedback coefficient (or dispersion coefficient, in that case),
* \(L\) is the length of the delay and hence, the length of the string.

\(\frac{y(n-L) + y(n-L-1)}{2}\) can be seen as a one zero filter implementing a lowpass. It models the fact that high frequencies are absorbed faster than low frequencies at the extremities of a string. 

The length of the delay \(L\) can be controlled as a frequency using the following formula: \(L = fs/f\) where \(f\) is the desired frequency.

At the very least, the system must be excited by a dirac (i.e., a simple impulse going from 1 to 0). The quality of the generated sound can be significantly improved if a noise impulse is used though. 

[`KS.cpp`](https://github.com/grame-cncm/embaudio20/blob/master/examples/lib/KS.cpp) implements a basic Karplus-Strong algorithm:

```
float KS::tick(){
  float excitation;
  if(trig){
    excitation = 1.0;
    trig = false;
  }
  else{
    excitation = 0.0;
  }
  float output = excitation + oneZero(delBuffer[readIndex])*feedback;
  delBuffer[writeIndex] = output;
  readIndex = (readIndex+1)%del;
  writeIndex = (writeIndex+1)%del;
  return output;
}
``` 

with:

```
float KS::oneZero(float x){
  float output = (x + zeroDel)*0.5; 
  zeroDel = output;
  return output;
}
```

The examples folder of the course repository hosts [a simple LyraT program](https://github.com/grame-cncm/embaudio20/tree/master/examples/ks) illustrating the use of `KS.cpp`.

Note that this algorithm could be improved in many ways. In particular, the fact that the delay length is currently expressed as an integer can result in frequency mismatches at high frequencies. In other words, our current string is out of tune. This could be fixed by using [fractional delay](https://ccrma.stanford.edu/~jos/Interpolation/Welcome.html).

In practice, the Karplus-Strong algorithm is not a physical model per se and is just a simplification of the [ideal string wave equation](https://ccrma.stanford.edu/~jos/pasp/Digital_Waveguide_Models.html). More advanced signal models can be implemented using waveguides.

Waveguide physical modeling has been extensively used in modern synthesizers to synthesize the sound of acoustic instruments. Julius O. Smith (Stanford professor) is the father of waveguide physical modeling. 
