# Lecture 4: Hardware Control and Audio Codec Configuration

The two main goals of this lecture are:
*

is to get a basic understanding of how audio codecs work and how they can be configured using the i2c protocol. By the end of it, you should be able to write a simple audio codec driver.

TODO: double check above in function of what you will say below

## Electronic Basics

While the goal of this class is not to teach electronics nor to make projects involving complicated circuitry, some basic circuits do need to be implemented in order to control the various parameters of the DSP algorithms studied in class using hardware controllers such as buttons and potentiometers. Hence, in case you feel like your electronics skills are a bit rusty, feel free to review the following page: <https://ccrma.stanford.edu/wiki/Introduction_to_Electronics_(condensed)>.

## Teensy 4.0 Pinout

The Teensy pins map can be seen in the following figure (directly taken from the [PJRC website](https://www.pjrc.com/teensy/pinout.html)). Most pins can be used as digital I/Os. Some pins noted "A(N)" can be used as analog inputs. Please, also note that 3.3v power can be retrieved from the top right corner pin and the ground from the top left pin. Make sure to never connect the 5.5v Vin pin to any other pin of the Teensy: that would probably fry it (the Cortex M7 inside the Teensy operates at 3.3v)!

<figure>
<img src="img/pinout0.png" class="mx-auto d-block" width="70%">
<img src="img/pinout1.png" class="mx-auto d-block" width="70%">
<figcaption><center>Teensy pinout.</center></figcaption>
</figure>

The Teensy audio shield uses a bunch of pins on the Teensy for i2c and i2s communication:

<figure>
<img src="img/teensy4_audio_pins.jpg" class="mx-auto d-block" width="50%">
<figcaption><center>Teensy audio shield pins.</center></figcaption>
</figure>

This means that these pins (besides GND and 3.3v, of course) cannot be used for something else (i.e., connecting external sensors).

## Bringing Power to Your Breadboard

The first step in making your first circuit with the Teensy is to bring power to the breadboard included in your kit using jumper wires:

<figure>
<img src="img/teensy.jpg" class="mx-auto d-block" width="80%">
<figcaption><center>Teensy connected to the breadboard.</center></figcaption>
</figure>

Basically connect the 3.3v pin to the red strip and the GND pin to the back strip of the breadboard.

**WARNING: Do not connect the 5.5v pin to the breadboard!**

## Adding a Rotary Potentiometer to the Circuit

Your kit should come with a couple of rotary potentiometers:

<figure>
<img src="img/pot.jpg" class="mx-auto d-block" width="80%">
<figcaption><center>Rotary potentiometer mounted on the breadboard.</center></figcaption>
</figure>

Place it on the breadboard, and connect its leftmost pin to power and its rightmost pin to the ground. Finally, connect its center pin to the A0 pin of the Teensy using a jumper wire. Please, note that we're using the pin since it is not used by the audio shield (see previous section).

## Testing the Potentiometer

In the current configuration, the potentiometer will deliver a 3.3v current at its center pin if it is fully turned to the right side, and 0v if it is fully turned to the left side. The following Teensy program:

```
void setup() {
  Serial.begin(9600);
}

void loop() {
  int sensorValue = analogRead(A0);
  Serial.println(sensorValue);
  delay(100);
}
```

displays the values measured at the A0 pin on the Teensy in the serial debugger. Values should be between 0 and 1023 (10 bits values). Make sure that the values you're getting are consistent with the position of the potentiometer.

## Controlling DSP Parameters With the Potentiometer

Now that you know how to retrieve potentiometer values in the Teensy, plugging it to your audio DSP should be pretty straightforward. Hence, we can reuse the `crazy-sine` example and do:

```
#include <Audio.h>
#include "MyDsp.h"

MyDsp myDsp;
AudioOutputI2S out;
AudioControlSGTL5000 audioShield;
AudioConnection patchCord0(myDsp,0,out,0);
AudioConnection patchCord1(myDsp,0,out,1);

void setup() {
  AudioMemory(2);
  audioShield.enable();
  audioShield.volume(0.5);
}

void loop() {
  int sensorValue = analogRead(A0);
  float freq = sensorValue + 100;
  myDsp.setFreq(freq);
}
```

Note that `sensorValue` needs to be turned into a frequency in hertz so we just add 100 to it to get a frequency between 100 and 1123 hertz.

Now take some time to have fun ;)!

## Using a Button

Using a button with the Teensy is slightly more involving since the use of a pulldown resistor is required (alternatively, a pullup resistor could be used, of course). This is due to the fact that buttons are "just" circuit breakers: they don't have a dedicated output pin like potentiometers. The pulldown resistor is used to suck potential floating currents out of the output pin of the button in order to get a stable signal to be measured on the Teensy. Hence, the following circuit must be implemented:

<figure>
<img src="img/circuit.jpg" class="mx-auto d-block" width="80%">
<figcaption><center>Circuit to connect a button to the Teensy.</center></figcaption>
</figure>

There's no need to use an analog pin on the Teensy to measure the voltage at the output of the button since we're looking at discrete values here (0 or 1). Hence, the button shall be connected to a digital pin (number 0, for example). Expanding on the previous example, we could write:

```
#include <Audio.h>
#include "MyDsp.h"

MyDsp myDsp;
AudioOutputI2S out;
AudioControlSGTL5000 audioShield;
AudioConnection patchCord0(myDsp,0,out,0);
AudioConnection patchCord1(myDsp,0,out,1);

void setup() {
  pinMode(0, INPUT); // configuring digital pin 0 as an input
  AudioMemory(2);
  audioShield.enable();
  audioShield.volume(0.5);
}

void loop() {
  if (digitalRead(0)) { // button is pressed
    myDsp.setGain(1);
  }
  else {
    myDsp.setGain(0);
  }
  int sensorValue = analogRead(A0);
  float freq = sensorValue + 100;
  myDsp.setFreq(freq);
}
```

(Assuming that a `setGain` method has been implemented, which is not the case in the previous example. It shouldn't be too hard though ;) )

## Exercise: Looping Between Notes by Pressing a Button

Expand the "note looper" that [you implemented as part of lecture 3](lecture3.md/#looping-through-a-small-tune) so that new notes are triggered when a button is pressed (as opposed to be triggered automatically). Every time the button is pressed, a new note is produced. This means that you'll have to turn your push button into a switch using software techniques... Finally, make sure that gain is controllable using a rotary potentiometer.

<!--
## Audio Codec

An audio codec is a hardware component providing an ADC and a DAC for audio purposes. Hence, it typically has analog audio inputs and outputs (stereo, in general) and digital audio inputs and outputs. Most audio codecs support standard audio sampling rate (i.e., 44.1, 48kHz, etc.) and bit depth (i.e., 16, 24 bits, etc.). Some high-end audio codecs also support higher sampling rates (e.g., 96, 192 kHz, etc.) and bit depth (32 bits, mostly) as well as more than a stereo interface (e.g., 4x4, 8x8, etc.). The price range of audio codecs can vary significantly impacting the quality of the components, i.e., audio is usually extremely sensitive to the quality of the hardware components of a system. 

Audio codecs usually use two different communication channels to connect to the processor unit (whether it's a CPU, a microcontroller, etc.):

* an [i2c](https://en.wikipedia.org/wiki/I%C2%B2C) bus is used to configure the codec,
* an [i2s](https://en.wikipedia.org/wiki/I%C2%B2S) bus is used to transmit digital audio data.

i2c (pronounced I-squared-C) is a serial communication protocol heavily used in the field of microelectronics. Most digital elements in an electronic circuit communicate using i2c. 

i2s (pronounced I-squared-S) is also a serial communication protocol but targeting specifically audio applications. We'll see that they're very close to each others in practice later in this class.

The LyraT hosts an audio codec (an Everest ES8388) which is connected to the ESP32 using the following model: 

<figure>
<img src="img/hardware.jpg" class="mx-auto d-block" width="80%">
<figcaption><center>Interfacing of a microcontroller with an audio codec.</center></figcaption>
</figure>

Before audio data can be streamed to the audio codec through i2s, it needs to be configured with an audio driver which basically just sends a set of instructions from the microcontroller to the codec using i2c. The goal of this lecture is to get a basic understanding of how audio drivers work in the context of embedded systems.

## Quick Tour of the ES8388

The Everest ES8388 is a low-power 24-bit, 8 kHz to 96 kHz audio codec. Its data sheet can be found on the [course repository](https://github.com/grame-cncm/embaudio20/raw/master/pdfs/ES8388-EverestSemiconductor.pdf) (feel free to download it now because you'll extensively need it later in this lecture). On page 4 of the data sheet, you will find a (not so clear) block diagram indicating the different components of the codec:

<figure>
<img src="img/codec-diagram.jpg" class="mx-auto d-block" width="80%">
<figcaption><center>Block diagram of the ES8388</center></figcaption>
</figure>

and on the following page a pin map:

<figure>
<img src="img/codec-pinout.jpg" class="mx-auto d-block" width="50%">
<figcaption><center>Pin map of the ES8388</center></figcaption>
</figure>

As you can see, the ES8388 hosts a stereo ADC and DAC. The codec has 4 inputs (LIN1, LIN2, RIN1, and RIN2) which can be routed to the left or right channel of the ADC. A similar pattern is used for the outputs and the DAC. Various mixers (*mux* on the diagram) can be used to route the signals. 

The codec also hosts 2 preamps which are required to adjust the gain of potential microphones. For instance, the LyraT board hosts 2 electret mics which are directly connected to the LIN1 and LIN2 inputs of the codec (as shown in the Analog System section of the [LyraT schematics](https://github.com/grame-cncm/embaudio20/raw/master/pdfs/esp32-lyrat-v4.3-schematic.pdf)). The output level of electret mic is extremely low compared to the "line in level" expected by the inputs of the ADC. The role of the preamps is to raise the level of the mics so that it matches that of the ADC inputs (we call this impedance matching).

Finally, the codec also has a digital interface (at the bottom of the block diagram) which is used for i2c and i2s communication. 

## Configuring an Audio Codec

All audio codecs work the same way and are configured through their i2c bus. A system of register/value is used for that. A register corresponds to a set of parameters and an 8 bits value can be provided to configure them. 

A list of all available registers of the ES8388 can be seen on page 11-13 of the [data sheet](https://github.com/grame-cncm/embaudio20/raw/master/pdfs/ES8388-EverestSemiconductor.pdf). Have a quick look at it! For example, register 9 (which is documented on page 16 and 17) allows us to configure the level of the left and right channel preamps. If register 9 is set to 0010 0010 then the level of both the left and right preamps will be set to +6dB. 

## Audio Codec Driver

Writing an audio codec driver consists of sending the right sequence of register/value through i2c to the codec. This will set the signal routing, the i2s format, the sampling rate, the bit depth, etc. 

[`ES8388.cpp`](https://github.com/grame-cncm/embaudio20/blob/master/examples/lib/ES8388.cpp) implements a simple driver for the ES8388 codec. The `initI2C` method configures i2c by providing the correct pin numbers, etc. The `writeReg` method sets a register and its corresponding 8-bit value. Finally, the `init` method configures the audio codec by setting various registers in a mode that will work for most applications that we will study in this class. 

Note that [`ES8388.h`](https://github.com/grame-cncm/embaudio20/blob/master/examples/lib/ES8388.h) contains a set pre-formatted macros for each register. 

In most cases, calling:

```
ES8388 es8388;
es8388.init();
```

in `main.cpp` will be sufficient to get the audio codec going and send it i2s data. Have a look at the [`crazy-sine`](https://github.com/grame-cncm/embaudio20/tree/master/examples/crazy-sine) example to get an idea of how this works.

Once `es8388.init();` has been called, `es8388.writeReg` can be called to override existing register values or set new ones. For example, the bit depth of the DAC could be set to 24 bits (instead of 16 now) by running:

```
writeReg(ES8388_DACCONTROL1,0);
``` 

which corresponds to 0000 0000 in binary format (see the documentation of register 23 on page 22 of the codec data sheet). The value of `ES8388_DACCONTROL1` in `init` is:

```
writeReg(ES8388_DACCONTROL1,24);
```

and corresponds to 16 bits. 24 in binary format is: 0001 1000. If you don't know how to convert decimal numbers to binary and vice versa, [have a look at this page](https://www.binary-code.org/binary/6bit/011110/). 

## Exercises

### Deactivating Power on the ADC

In the current configuration, both the ADC and the DAC are turned on. However, the `crazy-sine` example only uses the DAC. Hence, it might be smart to turn off the ADC to save some power. Turn off of the ADC by setting the right register. You'll have to look at the ES8388 data sheet for that, of course.

**Solution:**

```
writeReg(ES8388_ADCPOWER,255);
```

### Changing the Gain of the Mic Preamp

Set the gain of the left microphone preamp to 21 dB and to 12 dB for the right microphone preamp.

**Solution:**

```
writeReg(ES8388_ADCCONTROL1,116);
```

### Use Line In Instead of Mic

In the current configuration, the two microphones of the LyraT are used as the inputs to the codec. In some cases, we might want to use the line in instead (e.g., to implement a guitar pedal effect). Switch the input of the ADC to the line input.

**Hint:** You might have to look at the [LyraT schematics](https://github.com/grame-cncm/embaudio20/raw/master/pdfs/esp32-lyrat-v4.3-schematic.pdf) for that.

**Solution:**

```
writeReg(ES8388_ADCCONTROL2,80); 
```

### Changing the Gain of the Output

Set the output level of the right channel to -96 dB. 

**Hint:** There might be multiple answers to this question. Pick up the one you prefer.

**Solution:**

```
writeReg(ES8388_DACCONTROL5,0); or 
writeReg(ES8388_DACCONTROL25,0); (-30dB in that case)
```

### Changing the Sampling Rate

That one is slightly harder ;), change the sampling rate of the `crazy-sine` example to 32kHz.

**Solution:**

```
writeReg(ES8388_ADCCONTROL5,3);
writeReg(ES8388_DACCONTROL2,3);
AudioDsp audioDsp(32000,16);
```
-->
