# Lecture 6: Audio Processing Basics II

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

The [`distortion`](https://github.com/grame-cncm/embaudio20/tree/master/examples/distortion) example program for the LyraT demonstrates the use of `Distortion.cpp`.

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

The [`echo`](https://github.com/grame-cncm/embaudio20/tree/master/examples/echo) example program for the LyraT demonstrates the use of `Echo.cpp`. 

## Comb

A comb filter is a filter whose frequency response looks like a "comb." Comb filters can be implemented with feed-forward filters (Finite Impulse Response -- FIR) or feedback filters (Infinite Impulse Response -- IIR). In fact, the [Echo](#echo) algorithm can be used as a comb filter if the delay is very short:

\[y(n) = x(n)-g.y(n-M)\]

where \(M\) is the length of the delay and \(g\) feedback coefficient. 

[Julius Smith's website](https://ccrma.stanford.edu/~jos/pasp/Feedback_Comb_Filter_Amplitude.html) presents the frequency response of such filter and the mathematical rationals behind it. 

From an acoustical standpoint, a feedback comb filter will introduce resonances at specific point in the spectrum of the sound. The position and the spacing of these resonances is determined by the value of \(M\). \(g\), on the other hand, will determine the amplitude and sharpness of these resonances. 

The [`comb`](https://github.com/grame-cncm/embaudio20/tree/master/examples/comb) example program for the LyraT demonstrates the use of `Echo.cpp` as a comb filter. The "Mode" button can be used to change the value of the delay. 

## Physical Modeling: the Simple Case of the Karplus Strong

Physical modeling is one of the most advanced sound synthesis technique and a very active field of research. It consists of using physics/mathematical models of musical instruments or vibrating structures to synthesize sound. Various physical modeling techniques are used in the field of audio synthesis:

* Mass/Interaction (MI),
* Finite Difference Scheme (FDS),
* Signal models (e.g., waveguides, modal systems, etc.). 

While MI and FDS model the vibrational behavior of a system (i.e., using partial differential equation in the case of FDS), signal models model an object as a combination of signal processors. In this section, we will only look at this type of model the other ones being out of the scope of this class.

An extremely primitive string model can be implemented using a delay line and a loop. The delay line models the time it takes for vibration in the string to go from one extremity to the other, and the loop models the reflections at the boundaries of the string. In other words, we can literally just reuse [the echo algorithm](TODO) for this. This primitive string model is called the "Karplus-Strong" algorithm:

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

Note that this algorithm could be improved in many ways. In particular, the fact that the delay length is currently expressed as an integer can result in frequency mismatches at high frequencies. In other words, our current string is out of tune. This could be fixed using [fractional delay](https://ccrma.stanford.edu/~jos/Interpolation/Welcome.html).

In practice, the Karplus-Strong algorithm is not a physical model per se and is just a simplification of the [ideal string wave equation](https://ccrma.stanford.edu/~jos/pasp/Digital_Waveguide_Models.html). More advanced signal models can be implemented using waveguides.

Waveguide physical modeling has been extensively used in modern synthesizers to synthesize the sound of acoustic instruments. Julius O. Smith (Stanford professor) is the father of waveguide physical modeling. 

<!--
## Flanger

Flanger could be a bonus if we manage to fix it...

-->

## Exercise

### Smoothing

In most cases, DSP parameters are executed at control rate. Moreover, the resolution of the value used to configure parameters is much lower than that of audio samples since it might come from a Graphical User Interface (GUI), a low resolution sensor ADC (e.g., arduino), etc. For all these reasons, changing the value of a DSP parameter will often result in a "click"/discontinuity. A common way to prevent this from happening is to interpolate between the values of the parameter using a "leaky integrator." In signal processing, this can be easily implemented using a normalized one pole lowpass filter: 

\[y(n) = (1-s)x(n) + sy(n-1)\]

where \(s\) is the value of the pole and is typically set to 0.999 for optimal results.

Modify the [crazy-saw](https://github.com/grame-cncm/embaudio20/tree/master/examples/crazy-saw) example by "smoothing" the value of the frequency parameter by implementing the filter above with \(s=0.999\). Then slow down the rate at which frequency is being changed so that only two new values are generated per second. The result should sound quite funny :).  

<!--
**Solution:**

In addition to `Smooth.cpp` and `Smooth.h`, in `Phasor.h`:

```
  int samplingRate;
  Smooth smooth;
};
```

and `Phasor.cpp`:

```
float Phasor::tick(){
  float currentSample = phasor;
  phasor += smooth.tick(phasorDelta);
  phasor = phasor - std::floor(phasor);
  return currentSample;
}
```
-->
