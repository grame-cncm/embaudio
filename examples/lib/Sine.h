#ifndef SINE_H_INCLUDED
#define SINE_H_INCLUDED

class Sine{
public:
  Sine();
  
  void init(int SR);
  void setFrequency(float f);
  void setGain(float g);
  float tick();
private:
  float angleDelta, currentAngle, gain, samplingRate;
};

#endif  // SINE_H_INCLUDED