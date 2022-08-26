#include "MyDsp.h"
#include "FaustSynth.h"

#define AUDIO_OUTPUTS 1

#define MULT_16 2147483647
#define DIV_16 4.6566129e-10

MyDsp::MyDsp() : 
AudioStream(AUDIO_OUTPUTS, new audio_block_t*[AUDIO_OUTPUTS])
{
  fDSP = new mydsp();
  fDSP->init(AUDIO_SAMPLE_RATE_EXACT);
  fUI = new MapUI();
  fDSP->buildUserInterface(fUI);
  outputs = new float*[2];
  for (int channel = 0; channel < 2; ++channel){
    outputs[channel] = new float[AUDIO_BLOCK_SAMPLES];
  }
}

MyDsp::~MyDsp(){
  delete fDSP;
  delete fUI;
  for (int channel = 0; channel < 2; ++channel){
    delete[] outputs[channel];
  }
  delete [] outputs;
}

void MyDsp::setFreq(float freq){
  fUI->setParamValue("freq",freq);
}

void MyDsp::setCutoff(float freq){
  fUI->setParamValue("cutoff",freq);
}

void MyDsp::setGate(int gate){
  fUI->setParamValue("gate",gate);
}

void MyDsp::update(void) {
  fDSP->compute(AUDIO_BLOCK_SAMPLES,NULL,outputs);
  for (int channel = 0; channel < AUDIO_OUTPUTS; channel++) {
    audio_block_t* outBlock[AUDIO_OUTPUTS];
    outBlock[channel] = allocate();
    if (outBlock[channel]) {
      for (int i = 0; i < AUDIO_BLOCK_SAMPLES; i++) {
        int32_t val = outputs[channel][i]*MULT_16;
        outBlock[channel]->data[i] = val >> 16;
      }
      transmit(outBlock[channel], channel);
      release(outBlock[channel]);
    }
  }
}
