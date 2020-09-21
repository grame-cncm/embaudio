#ifndef PHASOR_H_INCLUDED
#define PHASOR_H_INCLUDED

#include "Smooth.h"

class Phasor{
public:
  Phasor(int SR);
  
  void setFrequency(float f);
  float tick();
private:
  float phasorDelta, phasor;
  int samplingRate;
  Smooth smooth;
};

#endif  // PHASOR_H_INCLUDED