#include "MyDsp.h"

#define AUDIO_OUTPUTS 2

#define MULT_16 2147483647
#define DIV_16 4.6566129e-10

MyDsp::MyDsp() : 
AudioStream(AUDIO_OUTPUTS, new audio_block_t*[AUDIO_OUTPUTS]),
echoL(AUDIO_SAMPLE_RATE_EXACT,10000),
echoR(AUDIO_SAMPLE_RATE_EXACT,10000)
{
  // setting up DSP objects
  echoL.setDel(10000);
  echoL.setFeedback(0.5);
  echoR.setDel(10000);
  echoR.setFeedback(0.5);
}

MyDsp::~MyDsp(){}

void MyDsp::update(void) {
  audio_block_t* inBlock[AUDIO_OUTPUTS];
  for (int channel = 0; channel < AUDIO_OUTPUTS; channel++) {
    inBlock[channel] = receiveReadOnly(channel);
    audio_block_t* outBlock[AUDIO_OUTPUTS];
    outBlock[channel] = allocate();
    if (outBlock[channel]) {
      for (int i = 0; i < AUDIO_BLOCK_SAMPLES; i++) {
        float currentSample = (inBlock[channel]->data[i] << 16)*DIV_16;
        if(channel == 0) currentSample = echoL.tick(currentSample);
        else if(channel == 1) currentSample = echoR.tick(currentSample);
        int32_t val = currentSample*MULT_16;
        outBlock[channel]->data[i] = val >> 16;
      }
      transmit(outBlock[channel], channel);
      release(outBlock[channel]);
    }
  }
}
