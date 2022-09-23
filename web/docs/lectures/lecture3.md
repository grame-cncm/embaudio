# Lecture 3: Digital Audio Systems Architectures and Audio Callback

By the end of this lecture, you should be able to produce sound with your Teensy and have a basic understanding of the software and hardware architecture of embedded audio systems.

## Basic Architecture of a Digital Audio System

All digital audio systems have an architecture involving at least an ADC and/or a DAC. Audio samples are processed on a computer (i.e., CPU, microcontroller, DSP, etc.) typically in an audio callback and are transmitted to the DAC and/or received from the ADC:

<figure>
<img src="img/audio-arch.jpg" class="mx-auto d-block" width="70%">
</figure>

The format of audio samples depends on the hardware configuration of the system. 

## Architecture of Embedded Audio Systems Such as the Teensy

TODO: add some text here

<!--

REWRITE THE FOLLOWING PARAGRAPH

The audio codec can be seen as an audio interface providing audio inputs and outputs. It is connected to the Teensy board through an i2s bus. Additional information on how this kind of system works will be provided in [Lecture 4](lecture4.md). 
-->

<figure>
<img src="img/teensy-diagram.jpg" class="mx-auto d-block" width="70%">
<figcaption><center>Teensy and Audio Shield Overview</center></figcaption>
</figure>

## Concept of Audio Blocks (Buffers), Audio Rate, and Control Rate

A large number of audio samples must be processed and transmitted every second. For example, if the sampling rate of the system is 48 kHz, 48000 samples will be processed in one second. Digital audio is extremely demanding and if one sample is missed, the result on the produced sound will be very audible. Most processors cannot process and transmit samples one by one which is why buffers need to be used. Hence, most digital audio systems will process audio as "blocks." The smallest size of a block will be determined by the performance of the system. On a modern computer running an operating system such as Windows, MacOS or Linux, the standard block size is usually 256 samples. In that case, the audio callback will process and then transmit to the DAC 256 samples all at once. 

An audio callback function typically takes the following form: 

```
void audioCallback(float *inputs, float *outputs){
  // control rate portion
  int gain = mainVolume;
  for(int i=0; i<blockSize; i++){
    // audio rate portion
    outputs[i] = inputs[i]*gain;
  }
}
``` 

`audioCallback` is called every time a new buffer is needed by the audio interface (ADC/DAC). For example, if the sampling rate is 48 kHz and the block size is 256 samples, `audioCallback` will be called 187.5 (48000/256) per seconds. Here, the `for` loop parses the input buffer and copy it to the output by modifying its gain. Note that `gain` is set outside of the `for` loop. That's a very common thing to do in the field of real-time audio processing: what happens outside of the `for` loop is called the **control rate** and what happens inside the `for` loop is called the **audio rate**. The parameters of an audio program are typically processed at control rate since user interface elements usually run at a much lower rate than audio.

Block size is directly linked to the audio latency of the system by the following formula: \(latency = BS/fs\) where \(latency\) is in seconds. Hence, the greater the block size, the larger the latency. For example, a block size of 256 samples at a sampling rate of 48 kHz will induce a latency of approximately 5ms. If the system has an audio input, this value has to be doubled, of course. A latency of 10ms might not seem like a lot but if the system is used for music purposes, this might be perceived by the performer. 

Embedded systems such as the Teensy can achieve much lower latencies than regular computers because of their lightness. Hence, the block size of your Teensy can be as small as 8!

## First Audio Program on the Teensy: `crazy-sine`

The [course repository](https://github.com/grame-cncm/embaudio) hosts an example containing a program synthesizing a sine wave on the Teensy and controlling its frequency: [crazy-sine](https://github.com/grame-cncm/embaudio/tree/master/examples/teensy/projects/crazy-sine). This program contains all the building blocks of a real-time audio program including... the audio callback which can be found in [`AudioDsp.cpp`](https://github.com/grame-cncm/embaudio/tree/master/examples/teensy/projects/crazy-sine/MyDsp.cpp)! The audio callback is implemented in this class in the `update` method and take the following shape:

```
void MyDsp::update(void) {
  for (int channel = 0; channel < AUDIO_OUTPUTS; channel++) {
    audio_block_t* outBlock[AUDIO_OUTPUTS];
    outBlock[channel] = allocate();
    if (outBlock[channel]) {
      for (int i = 0; i < AUDIO_BLOCK_SAMPLES; i++) {
        float currentSample = echo.tick(sine.tick())*0.5;
        int32_t val = currentSample*MULT_16;
        outBlock[channel]->data[i] = val >> 16;
      }
      transmit(outBlock[channel], channel);
      release(outBlock[channel]);
    }
  }
}
```

The `update` method is called everytime a new audio buffer is needed by the system. 

<!--
First, a `for` loop is implemented and is repeated every time a new buffer is needed, that's basically the "callback". `samples_data_out` is the output buffer whose size is the buffer size multiplied by the number of outputs of the system. For example, if the system has a stereo output and the buffer size is 256 samples, then the size of `samples_data_out` will be 512. Audio samples are coded here on 16 bits integers which is the data type accepted by the audio codec of the LyraT.

Then, the audio rate `for` loop is implemented and samples are processed and stored in a `float` called `currentSample`. `echo` and `sine` are defined in the [`lib` folder](https://github.com/grame-cncm/embaudio20/tree/master/examples/lib) and implement an echo and a sine wave oscillator, respectively.

Note that `currentSample` is multiplied by 0.5 to control the output gain of the system here. 

### Converting Floats to `int16_t`

Since the type of the output buffer is 16 bits signed integer, the float value of `currentSample` must be converted. For that, we just have to multiply `currentSample` by \(2^{16}/2\) (the range of `currentSample` is {-1;1}). As explained in [Lecture 2](lecture2.md), `float` are used for signal processing for convenience because most algorithms are easier to deal with using decimal numbers.

### Interlacing Output Samples

The LyraT has a stereo output (2 channels). The way parallel channels are transmitted to the audio codec is by interlacing samples in the output buffer (the same is true for the input, of course). This is carried out with the following piece of code:

```
samples_data_out[i*fNumOutputs] = currentSample*MULT_S16;
samples_data_out[i*fNumOutputs+1] = samples_data_out[i*fNumOutputs];
```

Hence, the left channel sample is first written into the buffer, then the right channel sample and so on, etc. Here, the left channel is copied into the right channel since the DSP algorithm only has a single output. Now you should understand why `samples_data_out` was declared as `int16_t samples_data_out[fNumOutputs*fBufferSize];` ;). 

### i2s Transmission

Once `samples_data_out` has been formatted, the buffer is transmitted to the audio codec using the i2s protocol. The `i2s_write` uses a blocking mechanism to hold the thread (task) until a new buffer is needed. 

## C++ Sine Wave Oscillator

Sine wave are at the basis of many algorithms in the field of audio. The sound of a sine wave is what we call a "pure tone" since it only has a single harmonic. One of the consequences of this is that all sounds can be synthesized using a combination of sine waves ([Fourier transform](https://en.wikipedia.org/wiki/Fourier_transform)).

From a mathematical standpoint, a sine oscillator can be implemented with the following differential equation:

\[x(t) = Asin(\omega t + \phi)\]

with:

* \(A\): the peak amplitude
* \(\omega = 2 \pi f\): the radian frequency (rad/sec)
* \(f\): the frequency in Hz
* \(t\): the time seconds
* \(\phi\): the initial phase (radians)

\(x(t)\) could be translated to C++ by writing something like (\(\phi\) is ignored here):

```
float currentSample = A*std::sin(2*PI*f*t);
```

however sine oscillators are rarely implemented as such since calling the `std::sin` function at every sample can be quite computationally expensive. For that reason, it is better to pre-compute the sine wave and store it in a wave table before computation starts. That kind of algorithm is then called a "wave table oscillator."

[Sine.cpp](https://github.com/grame-cncm/embaudio20/blob/master/examples/lib/Sine.cpp), which is used in `crazy-sine` is a good example of that. It uses [SineTable.cpp](https://github.com/grame-cncm/embaudio20/blob/master/examples/lib/Sine.cpp) which pre-computes a sine table:

```
table = new float[size];
for(int i=0; i<size; i++){
  table[i] = std::sin(i*2.0*PI/size);
}
```

and then makes it accessible through the `tick` (compute) method:

```
float SineTable::tick(int index){
  return table[index%tableSize];
}
```

The size of the table plays an important role on the quality of the generated sound. The greater the size, the more accurate/pure the sine wave. A low resolution sine wave will produce more distortion. In `Sine.cpp`, the sine wave table has a size of \(2^{14}\) which presents a good compromise between sound quality and memory.

It is important to keep in mind that when working with embedded systems memory is also an important factor to take into account. 

The sine table is then read with a "phasor." A phasor produces a ramp signal which is reset at a certain frequency. It can also be seen as a sawtooth wave. [Phasor.cpp](https://github.com/grame-cncm/embaudio20/blob/master/examples/lib/Sine.cpp) is used for that purpose and its `tick` method is defined as:

```
float Phasor::tick(){
  float currentSample = phasor;
  phasor += phasorDelta;
  phasor = phasor - std::floor(phasor);
  return currentSample;
}
```

It hence ramps from 0 to 1 at a given frequency. The `phasor` object in `Sine.cpp` is used to read the sine table by adjusting the range of its output:

```
float Sine::tick(){
  int index = phasor.tick()*SINE_TABLE_SIZE;
  return sineTable.tick(index)*gain;
}
```

## Exercises

### Looping Through a Small Tune

In the world of music technology, musical notes are usually represented by [MIDI numbers](https://djip.co/blog/logic-studio-9-midi-note-numbers). In MIDI, each pitch of the chromatic scale has a number between 0 and 127 associated to it: <https://djip.co/blog/logic-studio-9-midi-note-numbers>

As you can see, middle C (Do) corresponds to number 72. MIDI note numbers can be converted to a frequency using the following formula:

\[f=2^{(d-69)/12}.440\]

where \(d\) is the MIDI number.

Write a small tune/song looping through at least 5 notes and play it with the `crazy-sine` program on your LyraT.

**Hint:** For that, you'll probably have to replace the `audioDsp.setFreq(rand()%(2000-50 + 1) + 50);` line of of `main.cpp` by something else.

**Solution:**

In `main.cpp`:

```
extern "C" {
  void app_main(void);
}

float mtof(float note){
  return pow(2.0,(note-69.0)/12.0)*440.0;
}

void app_main(void)
{
  // initialize Audio Codec
  ES8388 es8388;
  es8388.init();
  
  // start audio DSP
  AudioDsp audioDsp(48000,16);
  audioDsp.start();
  
  int tune[] = {62,78,65,67,69};
  int cnt = 0;
  
  // infinite loop
  while(1) {
    // changing frequency randomly every 100ms
    audioDsp.setFreq(mtof(tune[cnt]));
    cnt = (cnt+1)%5;
    vTaskDelay(500 / portTICK_PERIOD_MS);
  }
}
```

### Basic Additive Synthesis

One of the most basic kind of sound synthesis is "additive synthesis." In consists of adding multiple sine wave oscillators together to "sculpt" the timbre of a sound. Both the frequency and the gain of each individual oscillator can then be used to change the properties of the synthesized sound. 

A simple additive synthesizer could be implemented from the `crazy-sine` example by declaring multiple instances of `sine`. E.g.:

```
float currentSample = echo.tick(sine0.tick()*gain0 + sine1.tick()*gain1);
```

but the problem with that option is that memory will be allocated twice for the `sineTable` array which is a terrible idea in the context of our embedded audio system with very little memory. Instead, the additive synthesizer should reuse the same instance of `sineTable` for each oscillator. In the `tick` method of `Sine.cpp`, try to call the `sineTable` a second time after `float currentSample = sineTable[index]*gain;` to add a second oscillator to the generated sample. The value of its index could be something like `index = (int) (index*1.5)%SINE_TABLE_SIZE;` so that the frequency of the second oscillator is one fifth above the main frequency. In other words, the differential equation of the synth should be:

\[x(t) = sin(2 \pi f t) + sin(2 \pi (1.5f) t)\]

**Hint:** Beware of clipping! Adding two sine waves together even though they don't have the same frequency will likely produce a signal whose range exceeds {-1;1}: you should take that into account for your final product. 

**Solution:**

In `Sine.cpp`:

```
float Sine::tick(){
  int index = phasor.tick()*SINE_TABLE_SIZE;
  int index2 = phasor.tick()*1.5*SINE_TABLE_SIZE;
  return (sineTable.tick(index)+sineTable.tick(index2))*gain*0.5;
}
```

### Stereo Echo

Reusing the result of the previous exercise, create a second instance of `echo` (connected to the same instance of `sine`) *with different parameters from the first one* that will be connected to the second channel of the output (i.e., the first instance should be connected to the left channel and the second one to the right channel). The final algorithm should look like this:

```
float sineSample = sine.tick();
float currentSampleL = echo0.tick(sineSample)*0.5;
float currentSampleR = echo1.tick(sineSample)*0.5;
```

**Hint:** Beware of memory allocation again! Make sure that the maxim delay of your echo (on the 2 parameters of the class constructor) doesn't exceed 10000 for now for both instances of the echo. 

**Solution:**

In `AudioDsp.h`:

```
  Sine sine;
  Echo echo0, echo1;
};
```

In `AudioDsp.cpp`:

```
AudioDsp::AudioDsp(int SR, int BS) : 
fSampleRate(SR),
fBufferSize(BS),
fNumOutputs(2),
fHandle(nullptr),
fRunning(false),
sine(SR),
echo0(SR,10000),
echo1(SR,7000)
{
  
...

// setting up DSP objects
echo0.setDel(10000);
echo0.setFeedback(0.5);
echo1.setDel(7000);
echo1.setFeedback(0.4);

...

// processing buffers
for (int i = 0; i < fBufferSize; i++) {
  // DSP
  float sineSample = sine.tick();
  float currentSampleL = echo0.tick(sineSample)*0.5;
  float currentSampleR = echo1.tick(sineSample)*0.5;
  
  // copying to output buffer
  samples_data_out[i*fNumOutputs] = currentSampleL*MULT_S16;
  samples_data_out[i*fNumOutputs+1] = currentSampleR*MULT_S16;
}
```
-->
