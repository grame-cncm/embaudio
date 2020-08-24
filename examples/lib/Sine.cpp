#include <cmath>

#include "Sine.h"

#define PI 3.141592653589

Sine::Sine() : 
angleDelta(0.0),
currentAngle(0.0),
gain(1.0),
samplingRate(0.0){}

void Sine::init(int SR){
  samplingRate = SR;
  setFrequency(440);
}

void Sine::setFrequency(float f){
  angleDelta = f/samplingRate;
}
    
void Sine::setGain(float g){
  gain = g;
}
    
float Sine::tick(){
  float currentSample = std::sin(currentAngle*2.0*PI)*gain;
  currentAngle += angleDelta;
  currentAngle = currentAngle - std::floor(currentAngle);
  return currentSample;
}
