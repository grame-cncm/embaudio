#ifndef ECHO_H_INCLUDED
#define ECHO_H_INCLUDED

class Echo{
public:
  Echo(int maxDel);
  ~Echo();
  
  void init(int SR);
  void setFeedback(float f);
  void setDel(int d);
  float tick(float input);
private:
  float* delBuffer;
  float feedback, samplingRate;
  int readIndex, writeIndex, del;
};

#endif  // SINE_H_INCLUDED