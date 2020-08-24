#include <cmath>

#include "Echo.h"

#define PI 3.141592653589

Echo::Echo(int maxDel) : 
feedback(0.0),
samplingRate(0.0),
readIndex(1),
writeIndex(0),
del(0)
{
  delBuffer = new float[maxDel];
  for(int i=0; i<maxDel; i++){
    delBuffer[i] = 0.0;
  }
}

Echo::~Echo()
{
  delete[] delBuffer;
}

void Echo::init(int SR){
  samplingRate = SR;
}

void Echo::setFeedback(float f){
  feedback = f;
}
    
void Echo::setDel(int d){
  del = d;
}
    
float Echo::tick(float input){
  float output = input + delBuffer[readIndex]*feedback;
  delBuffer[writeIndex] = output;
  readIndex = (readIndex+1)%del;
  writeIndex = (writeIndex+1)%del;
  return output;
}
