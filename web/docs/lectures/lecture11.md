# Lecture 11: Faust on the LyraT and Advanced Control

## Generating and Using a Faust C++ Object

**In order to run the examples in this lecture, you should install the Faust distribution on your system from the [Faust Git Repository](https://github.com/grame-cncm/faust).**

At the most fundamental level, the Faust compiler is a command line tool translating a Faust DSP object into C++ code. For example, assuming that Faust is properly installed on your system, given the following simple Faust program implementing a filtered sawtooth wave oscillator ([`FaustSynth.dsp`](https://github.com/grame-cncm/embaudio20/blob/master/faust/FaustSynth.dsp)):

```
import("stdfaust.lib");
freq = nentry("freq",200,50,1000,0.01);
gain = nentry("gain",0.5,0,1,0.01) : si.smoo;
gate = button("gate") : si.smoo;
cutoff = nentry("cutoff",10000,50,10000,0.01) : si.smoo;
process = os.sawtooth(freq)*gain*gate : fi.lowpass(3,cutoff) <: _,_;
```

running:

```
faust FaustSynth.dsp
```

will output the C++ code corresponding to this file in the terminal. 

Faust comes with a system of C++ wrapper (called architectures in the Faust ecosystem) which can be used to customize the generated C++ code. [`faustMininal.h`](https://github.com/grame-cncm/embaudio20/blob/master/faust/faustMinimal.h) is a minimal architecture file including some C++ objects that can be used to facilitate interactions with the generated DSP:

```
#include <cmath>
#include <cstring>

#include "faust/gui/MapUI.h"
#include "faust/gui/meta.h"
#include "faust/dsp/dsp.h"

// BEGIN-FAUSTDSP

<<includeIntrinsic>>

<<includeclass>>

// END-FAUSTDSP
```

For instance, MapUI allows us to access the parameters of a Faust DSP object using the `setParamValue` method, etc.

To generate a C++ file using this architecture, you can run:

```
faust -i -a faustMinial.h FaustSynth.dsp -o FaustSynth.h
```

which will produce a [`FaustSynth.h`](https://github.com/grame-cncm/embaudio20/blob/master/faust/FaustSynth.h) file (feel free to click on it).

The `-i` inlines all the included C++ `.h` files in the generated file.

The [`faust-synth`](https://github.com/grame-cncm/embaudio20/tree/master/examples/faust-synth) LyraT example project demonstrates how `FaustSynth.h` can be used. First, it is included in [`AudioDsp.cpp`](https://github.com/grame-cncm/embaudio20/tree/master/examples/faust-synth) and the following elements are declared in [the corresponding header file](https://github.com/grame-cncm/embaudio20/blob/master/examples/faust-synth/main/AudioDsp.h):

```
MapUI* fUI;
dsp* fDSP;
float **outputs;
```

`dsp` is the actual Faust DSP, `MapUI` will be used to interact with it, and `outputs` is the multidimensional output buffer. These objects are then allocated in the constructor of `AudioDsp.cpp`:

```
fDSP = new mydsp();
fDSP->init(fSampleRate);
fUI = new MapUI();
fDSP->buildUserInterface(fUI);
outputs = new float*[2];
for (int channel = 0; channel < 2; ++channel){
  outputs[channel] = new float[fBufferSize];
}
```

`buildUserInterface` is used to connect `fUI` to `fDSP` and then memory is allocated for the output buffer. Note that memory should be de-allocated in the destructor after this. In the `audioTask`, we just call the `compute` method of `fDSP` and then reformat the generated samples to transmit them via i2s:

```
fDSP->compute(fBufferSize,NULL,outputs);
    
// processing buffers
for (int channel = 0; channel < 2; ++channel){
  for(int i=0; i<fBufferSize; i++){
    samples_data_out[i*fNumOutputs+channel] = outputs[channel][i]*MULT_S16;
  }
}
    
// transmitting output buffer
size_t bytes_written = 0;
i2s_write((i2s_port_t)0, &samples_data_out, fNumOutputs*sizeof(int16_t)*fBufferSize,
  &bytes_written, portMAX_DELAY);
``` 

Note that `outputs` is used as an intermediate here and the first dimension of the array is the channel number and the second dimension the samples themselves. 

The various parameters of the Faust object can then be changed just by calling the `setParamValue` method. The first argument of the method corresponds to the name of the parameter as specified in the Faust program:

```
void AudioDsp::setFreq(float freq){
  fUI->setParamValue("freq",freq);
}

void AudioDsp::setCutoff(float freq){
  fUI->setParamValue("cutoff",freq);
}
```

## Better Control on the LyraT

The control of the parameters of an audio DSP object on the LyraT is relatively limited with what we have seen in this class. In this brief section, we propose a few solutions to overcome this problem. Since they are beyond the scope of this class, we wont go into too many details.

### MIDI

[MIDI](https://en.wikipedia.org/wiki/MIDI) is THE standard in the world of music to control digital devices. It has been around since 1983 and even though it is very "low tech," it is still heavily used. While MIDI was traditionally transmitted over MIDI ports, USB is used nowadays to send MIDI.

Implementing MIDI USB on the LyraT should be relatively straight forward but it hasn't been done yet. 

### OSC

[Open Sound Control (OSC)](https://en.wikipedia.org/wiki/Open_Sound_Control) is a more modern communication standard used in the field of music technology. It is based UDP which means that information can be transmitted via Ethernet or Wi-Fi. OSC uses a system of address/values to access the different parameters of a system. An OSC message can therefore look like:

```
/synth/freq 440
```

Since the LyraT has Wi-Fi built-in, implementing OSC control should also be relatively straight forward.

### HTTP

Another common way on embedded audio systems to control parameters is to implement a simple web server. A webpage can then present the parameters to control in a user interface, etc. 

### Hardware Control

A simple way to offer a better control on the LyraT is to add physical sensors/elements to it (e.g., buttons, potentiometers, etc.). However, even though the LyraT hosts an ESP32 microcontroller, it doesn't provide any analog inputs to plug sensors to them. This can be solved by adding a sensor DAC to this device such as a MCP3008 (8 channel 10-bit ADC with SPI interface). 

## Exercises

### Faust Triangle Oscillator

The Faust libraries host a triangle wave oscillator:

```
os.triangle
```

Try to replace the sawtooth wave oscillator from the previous example by a triangle wave oscillator in Faust and run it on the LyraT.

### Flanger

The Faust libraries host a [flanger function](https://faustlibraries.grame.fr/libs/phaflangers/#pfflanger_mono):

```
pf.flanger_mono
```

Turn your LyraT into a flanger effect processor!
