#ifndef SINE_H_INCLUDED
#define SINE_H_INCLUDED

class Sine{
public:
  Sine(int SR);
  ~Sine();
  
  void setFrequency(float f);
  void setGain(float g);
  float tick();
private:
  float phasorDelta, phasor, gain, samplingRate;
  float *sineTable;
};

#endif  // SINE_H_INCLUDED