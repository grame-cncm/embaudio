# Lecture 4: TODO Title

## Audio Codec

An audio codec is a hardware component providing an ADC and a DAC for audio purposes. Hence, it typically has analog audio inputs and outputs (stereo, in general) and digital audio inputs and outputs. Most audio codecs support standard audio sampling rate (i.e., 44.1, 48kHz, etc.) and bit depth (i.e., 16, 24 bits, etc.). Some high-end audio codecs also support higher sampling rates (e.g., 96, 192 kHz, etc.) and bit depth (32 bits, mostly) as well as more than a stereo interface (e.g., 4x4, 8x8, etc.). The price range of audio codecs can vary significantly impacting the quality of the components. Indeed, audio is usually extremely sensitive to the quality of the hardware components of a system. 

Audio codecs usually use two different communication channels to connect it to the processor unit (whether it's a CPU, a microcontroller, etc.):

* an [i2c](https://en.wikipedia.org/wiki/I%C2%B2C) bus is used to configure the codec,
* an [i2s](https://en.wikipedia.org/wiki/I%C2%B2S) bus is used to transmit digital audio data.

i2c (pronounced I-squared-C) is a serial communication protocol heavily used in the field of microelectronics. Most digital elements in an electronic circuit communicate using i2c. 

i2s (pronounced I-squared-S) is also a serial communication protocol but targeting specifically audio applications. We'll see that they're very close to each others in practice later in this class (see TODO).

The LyraT hosts an audio codec (an Everest ES8388) which is connected to the ESP32 using the following model: 

<figure>
<img src="img/hardware.jpg" class="mx-auto d-block" width="80%">
<figcaption><center>Interfacing of a microcontroller with an audio codec</center></figcaption>
</figure>

Before audio data can be streamed to the audio codec through i2s, it needs to be configured with an audio driver which basically just sends a set of instructions from the microcontroller to the codec using i2c. The goal of this lecture is to get a basic understanding of how audio drivers work in the context of embedded systems.

## Quick Tour of the ES8388

Difference between preamp gain and main volume.

## Exercises

### Deactivate power of the ADC

writeReg(ES8388_ADCPOWER,255);

### Set the gain of the preamp to 21dB for L and 12dB for ref

writeReg(ES8388_ADCCONTROL1,116);

### Use line in instead of mic

writeReg(ES8388_ADCCONTROL2,80);

### Set Gain of Output (R) to -96dB

writeReg(ES8388_DACCONTROL5,0);

or

writeReg(ES8388_DACCONTROL25,0); (-30dB in that case)

### Change SR to 32k

writeReg(ES8388_ADCCONTROL5,3);
writeReg(ES8388_DACCONTROL2,3);

AudioDsp audioDsp(32000,16);

