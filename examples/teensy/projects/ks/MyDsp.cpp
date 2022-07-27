#include "MyDsp.h"

#define AUDIO_OUTPUTS 1

#define MULT_16 2147483647
#define DIV_16 4.6566129e-10

MyDsp::MyDsp() : 
AudioStream(AUDIO_OUTPUTS, new audio_block_t*[AUDIO_OUTPUTS]),
ks(AUDIO_SAMPLE_RATE_EXACT)
{
}

MyDsp::~MyDsp(){}

void MyDsp::setFreq(float freq){
  ks.setFreq(freq);
}

void MyDsp::trigger(){
  ks.trigger();
}

void MyDsp::update(void) {
  for (int channel = 0; channel < AUDIO_OUTPUTS; channel++) {
    audio_block_t* outBlock[AUDIO_OUTPUTS];
    outBlock[channel] = allocate();
    if (outBlock[channel]) {
      for (int i = 0; i < AUDIO_BLOCK_SAMPLES; i++) {
        float currentSample = ks.tick();
        int32_t val = currentSample*MULT_16;
        outBlock[channel]->data[i] = val >> 16;
      }
      transmit(outBlock[channel], channel);
      release(outBlock[channel]);
    }
  }
}
