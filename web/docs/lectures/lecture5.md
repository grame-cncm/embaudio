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

## White Noise

White noise is a specific kind of signal in which there's an infinite number of harmonics all having the same level. In other words, the spectrum of white noise looks completely flat. 

White noise is produced by generating random numbers between -1 and 1. [`Noise.cpp`](https://github.com/grame-cncm/embaudio20/blob/master/examples/lib/Noise.cpp) demonstrates how this can be done in C++ using the `rand()` function:

```
Noise::Noise() :
randDiv(1.0/RAND_MAX){}
    
float Noise::tick(){
  return rand()*randDiv*2 - 1;
}
```

The [Simple Filter: One Zero](#simple-filter-one-zero) section presents a use example of white noise.

## Simple Filter: One Zero

Filters are heavily used in the field of audio processing. In fact, designing filters is a whole field by itself. They are at the basis of many audio effects such as Wah guitar pedals, etc. From an algorithmic standpoint, the most basic filter is what we called a "one zero" filter which means that its transfer function only has numerators and no denominators. The differential equation of a one zero filter can be expressed as:

\[y(n) = b_0x(n) + b_1x(n-1)\]

where \(b_1\) is "the zero" of the filter (also called *feed forward coefficient*), \(b_0\) can be discarded as it is equal to 1 in most cases.

One zero filters can either be used as a lowpass if the value of \(b_1\) is positive or as a highpass if \(b_1\) is negative. The frequency response of the filter which is be obtained with \(H(e^{j \omega T}) = b_0 + b_1e^{-j \omega T}\) can be visualized on [Julius Smith's website](https://ccrma.stanford.edu/~jos/filters/One_Zero.html).

Note that the gain of the signal is amplified on the second half of the spectrum which needs to be taken into account if this filter is used to process audio (once again, the output signal must be bounded within {-1,1}). 

[`OneZero.cpp`](https://github.com/grame-cncm/embaudio20/blob/master/examples/lib/OneZero.cpp) implements a one zero filter:

```
float OneZero::tick(float input){
  float output = input + del*b1;
  del = input;
  return output*0.5;
}
```

Note that we multiply the output by 0.5 to normalize the output gain.

The [`filtered-noise`](https://github.com/grame-cncm/embaudio20/tree/master/examples/filtered-noise)) example program for the LyraT demonstrates the use of `OneZero.cpp` by feeding [white noise](#white-noise) in it. The value of \(b_1\) can be changed by pressing the "Mode" button on the board, give it a try!

## Harmonic Distortion: Rock On!

Distortion is one of the most common electric guitar effect. It consists of over driving a signal by increasing its gain to "square" the extremities of its waveform. This results in the creation of lots of harmonics, producing very "rich" sounds. Overdrive is easily achievable with an analog electronic circuit and "sharp edges" in the waveform are rounded thanks to the tolerance of the electronic components. In the digital world, things are slightly more complicated since clipping will happen resulting in a very dirty sound with potentially lots of aliasing. One way to solve this problem is to use a "cubic function" which will round the edges of the signal above a certain amplitude:

\[
f(x) = 
\begin{cases}
\frac{-2}{3}, \; \; x \leq -1\\
x - \frac{x^3}{3}, \; \; -1 < x < 1\\
\frac{2}{3}, \; \; x \geq -1
\end{cases}
\] 

[`Distortion.cpp`](https://github.com/grame-cncm/embaudio20/blob/master/examples/lib/Distortion.cpp) implements a cubic distortion as:

```
float Distortion::cubic(float x){
  return x - x*x*x/3;
}
    
float Distortion::tick(float input){
  float output = input*pow(10.0,2*drive) + offset;
  output = fmax(-1,fmin(1,output));
  output = cubic(output);
  return output*gain;
}
```

The range of `drive` is {0;1} which means that the value of `input` can be multiplied by a number as great as 100 here. `offset` is a common parameter which just adds a positive or negative DC offset to the signal. If this parameter is used, it is recommended to add a DC blocking filter after the distortion. 

Distortion is created here by clipping the signal using the `fmin` and `fmax` functions. Finally, the cubic polynomial is used to round the edges of the waveform of the signal as explained above. 

The [`distortion`](https://github.com/grame-cncm/embaudio20/tree/master/examples/distortion)) example program for the LyraT demonstrates the use of `Distortion.cpp`.

Distortion is a very trendy field of research in audio technology these days especially using "virtual analog" algorithms which consists of modeling the electronic circuit of distortion on a computer.

## Echo

An echo is a very common audio effect which is used a lot to add some density and depth to a sound. It is based on a feedback loop and a delay and can be expressed as:

\[y(n) = x(n) + g.y(n - M)\]

where \(g\) is the feedback between 0 and 1 and \(M\) the delay as a number of samples.  

It can be seen as a simple physical model of what happens in the real world when echo is produced: the delay represents the time it takes for an acoustical wave to go from point A to point B at the speed of sound and \(g\) can control the amount of absorption created by the air and the reflecting material.

[`Echo.cpp`](https://github.com/grame-cncm/embaudio20/blob/master/examples/lib/Echo.cpp) implements an echo as: 

```
float Echo::tick(float input){
  float output = input + delBuffer[readIndex]*feedback;
  delBuffer[writeIndex] = output;
  readIndex = (readIndex+1)%del;
  writeIndex = (writeIndex+1)%del;
  return output;
}
```

Here, `delBuffer` is used as a "ring buffer": incoming samples are stored and the read and write indices loop around to buffer to write incoming samples and read previous ones. Note that memory is allocated in the constructor of the class for `delBuffer` based on the value of `maxDel`, the maximum size of the delay.

The [`echo`](https://github.com/grame-cncm/embaudio20/tree/master/examples/echo)) example program for the LyraT demonstrates the use of `Echo.cpp`. 

## Comb

A comb filter is a filter whose frequency response looks like a "comb". Comb filters can be implemented with feed-forward filters (Finite Impulse Response -- FIR) or feedback filters (Infinite Impulse Response -- IIR). In fact, the [Echo](#echo) algorithm can be used as a comb filter if the delay is very short:

\[y(n) = x(n)-gy(n-M)\]

where \(M\) is the length of the delay and \(g\) feedback coefficient. 

[Julius Smith's website](https://ccrma.stanford.edu/~jos/pasp/Feedback_Comb_Filter_Amplitude.html) presents the frequency response of such filter and the mathematical rational behind it. 

From an acoustical standpoint, a feedback comb filter will introduce resonances at specific point in the spectrum of the sound. The position and the spacing of these resonances is determined by the value of \(M\). \(g\), on the other hand, will determine the amplitude and sharpness of these resonances. 

The [`comb`](https://github.com/grame-cncm/embaudio20/tree/master/examples/comb)) example program for the LyraT demonstrates the use of `Echo.cpp` as a comb filter. The "Mode" button can be used to change the value of the delay. 

<!--
## Flanger

Flanger could be a bonus if we manage to fix it...

-->
