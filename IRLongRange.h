#include "Arduino.h"


class IRLong {

  public:

    void setupIR(int analogIn);
    float readSensor();

  private:
    float ranges[];
    float val;
    float pin;
    float multiMap(int val, float* _in, float* _out, uint8_t size);
    float out[31] = {10, 12, 14, 16, 18, 20, 22, 24, 26, 28, 30, 32, 34, 36, 38, 40, 42, 44, 46, 48, 50, 52, 54, 56, 58, 60,62,72,74,76,78};
    float in[31] = {487, 406,352,316,286,260,235,225,206,193,181,170,162,154,151,142,136,130,126,121,113,110,107,104,102,95,93,81,73,66,59};
    float filterStore[5] = {0,0,0,0,0};
    int arrayPos;

    


  
};

