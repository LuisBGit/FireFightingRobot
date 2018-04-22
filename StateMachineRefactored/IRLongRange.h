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
    float out[12] = {15, 20, 25, 30, 35, 40, 45, 50, 55, 60, 65, 70};
    float in[12] = {298, 227, 179, 145, 119, 101, 85, 73, 65, 53, 43, 34};
    float filterStore[5] = {0,0,0,0,0};
    int arrayPos;

    


  
};

