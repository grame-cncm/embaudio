# Lecture 4: Audio Codec Configuration

The goal of this lecture is to get a basic understanding of how audio codecs work and how they can be configured using the i2c protocol. By the end of it, you should be able to write a simple audio codec driver.

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

<!-- Answer: writeReg(ES8388_ADCPOWER,255); -->

### Changing the Gain of the Mic Preamp

Set the gain of the left microphone preamp to 21 dB and to 12 dB for the right microphone preamp.

<!-- Answer: writeReg(ES8388_ADCCONTROL1,116); -->

### Use Line In Instead of Mic

In the current configuration, the two microphones of the LyraT are used as the inputs to the codec. In some cases, we might want to use the line in instead (e.g., to implement a guitar pedal effect). Switch the input of the ADC to the line input.

**Hint:** You might have to look at the [LyraT schematics](https://github.com/grame-cncm/embaudio20/raw/master/pdfs/esp32-lyrat-v4.3-schematic.pdf) for that.

<!-- writeReg(ES8388_ADCCONTROL2,80); -->

### Changing the Gain of the Output

Set the output level of the right channel to -96 dB. 

**Hint:** There might be multiple answers to this question. Pick up the one you prefer.

<!-- Answer: writeReg(ES8388_DACCONTROL5,0); or writeReg(ES8388_DACCONTROL25,0); (-30dB in that case) -->

### Changing the Sampling Rate

That one is slightly harder ;), change the sampling rate of the `crazy-sine` example to 32kHz.

<!--
Answer:
writeReg(ES8388_ADCCONTROL5,3);
writeReg(ES8388_DACCONTROL2,3);
AudioDsp audioDsp(32000,16);
-->
