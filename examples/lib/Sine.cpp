#include <cmath>

#include "Sine.h"

#define SINE_TABLE_SIZE 16384
#define PI 3.141592653589

Sine::Sine(int SR) : 
phasorDelta(0.0),
phasor(0.0),
gain(1.0),
samplingRate(0.0){
  sineTable = new float[SINE_TABLE_SIZE];
  for(int i=0; i<SINE_TABLE_SIZE; i++){
    sineTable[i] = std::sin(i*2.0*PI/SINE_TABLE_SIZE);
  }
  samplingRate = SR;
  setFrequency(440);
}

Sine::~Sine()
{
  delete[] sineTable;
}

void Sine::setFrequency(float f){
  phasorDelta = f/samplingRate;
}
    
void Sine::setGain(float g){
  gain = g;
}
    
float Sine::tick(){
  int index = phasor*SINE_TABLE_SIZE;
  float currentSample = sineTable[index]*gain;
  phasor += phasorDelta;
  phasor = phasor - std::floor(phasor);
  return currentSample;
}
