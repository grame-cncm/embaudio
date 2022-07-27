#include "MyDsp.h"

#define AUDIO_OUTPUTS 1

#define MULT_16 2147483647
#define DIV_16 4.6566129e-10

MyDsp::MyDsp() : 
AudioStream(AUDIO_OUTPUTS, new audio_block_t*[AUDIO_OUTPUTS]),
fm(AUDIO_SAMPLE_RATE_EXACT)
{
}

MyDsp::~MyDsp(){}

void MyDsp::setCFreq(float freq){
  fm.setCFreq(freq);
}

void MyDsp::setMFreq(float freq){
  fm.setMFreq(freq);
}

void MyDsp::setIndex(float freq){
  fm.setIndex(freq);
}

// set sine wave gain
void MyDsp::setGain(float gain){
  fm.setGain(gain);
}

void MyDsp::update(void) {
  for (int channel = 0; channel < AUDIO_OUTPUTS; channel++) {
    audio_block_t* outBlock[AUDIO_OUTPUTS];
    outBlock[channel] = allocate();
    if (outBlock[channel]) {
      for (int i = 0; i < AUDIO_BLOCK_SAMPLES; i++) {
        float currentSample = fm.tick()*0.9;
        int32_t val = currentSample*MULT_16;
        outBlock[channel]->data[i] = val >> 16;
      }
      transmit(outBlock[channel], channel);
      release(outBlock[channel]);
    }
  }
}
