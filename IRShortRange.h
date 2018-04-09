#include "Arduino.h"


class IRShort {

  public:

    void setupIR(int analogIn);
    float readSensor();

  private:
    float val;
    float pin;
    float out[14] = {4, 6, 8, 10 ,12, 14,16,18,20,22,24,26,28,30};
    float in[14] = {466,356,285,233,198,176,156,141,127,113,101,91,78,67}; 
    float filterStore[5] = {0,0,0,0,0};
    float multiMap(int val, float* _in, float* _out, uint8_t size);
    int arrayPos;
    
    


  
};

