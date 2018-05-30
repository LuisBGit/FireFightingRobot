#include "IRLongRange.h"


void IRLong::setupIR(int AnalogIn) {
    this->pin = AnalogIn;
}

float IRLong::readSensor() {
   float result = analogRead(this->pin);
   this->filterStore[0] = result;
   shiftDown();
   float mean = (((this->filterStore[0])* 1) + (1*(this->filterStore[1])) + (1*(this->filterStore[2])) + (1*(this->filterStore[3])) + (1*(this->filterStore[4]))) /(5) ;
   return multiMap(mean, this->in, this->out, 31);
   
}

void IRLong::shiftDown() {
  for (int i = 0; i < 4; i++) {
    filterStore[i +1] = filterStore[i];
    
  }
}

float IRLong::multiMap(int val, float* _in, float* _out, uint8_t size)
{
  // take care the value is within range
  // val = constrain(val, _in[0], _in[size-1]);
  if (val > _in[0]) return 999;
  if (val <= _in[size-1]) return _out[size-1];

  // search right interval
  uint8_t pos = 1;  // _in[0] allready tested
  while(val < _in[pos]) pos++;

  // this will handle all exact "points" in the _in array
  if (val == _in[pos]) return _out[pos];

  // interpolate in the right segment for the rest
  return (val - _in[pos-1]) * (_out[pos] - _out[pos-1]) / (_in[pos] - _in[pos-1]) + _out[pos-1];
}
