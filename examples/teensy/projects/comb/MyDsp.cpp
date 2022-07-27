#include "MyDsp.h"

#define AUDIO_OUTPUTS 2

#define MULT_16 2147483647
#define DIV_16 4.6566129e-10

MyDsp::MyDsp() : 
AudioStream(AUDIO_OUTPUTS, new audio_block_t*[AUDIO_OUTPUTS]),
combL(AUDIO_SAMPLE_RATE_EXACT,200),
combR(AUDIO_SAMPLE_RATE_EXACT,200)
{
  // setting up DSP objects
  combL.setDel(100);
  combL.setFeedback(0.99);
  combR.setDel(100);
  combR.setFeedback(0.99);
}

MyDsp::~MyDsp(){}

void MyDsp::setDel(int del){
  combL.setDel(del);
  combR.setDel(del);
}

void MyDsp::update(void) {
  audio_block_t* inBlock[AUDIO_OUTPUTS];
  for (int channel = 0; channel < AUDIO_OUTPUTS; channel++) {
    inBlock[channel] = receiveReadOnly(channel);
    audio_block_t* outBlock[AUDIO_OUTPUTS];
    outBlock[channel] = allocate();
    if (outBlock[channel]) {
      for (int i = 0; i < AUDIO_BLOCK_SAMPLES; i++) {
        float currentSample = (inBlock[channel]->data[i] << 16)*DIV_16;
        if(channel == 0) currentSample = combL.tick(currentSample);
        else if(channel == 1) currentSample = combR.tick(currentSample);
        int32_t val = currentSample*MULT_16;
        outBlock[channel]->data[i] = val >> 16;
      }
      transmit(outBlock[channel], channel);
      release(outBlock[channel]);
    }
  }
}
