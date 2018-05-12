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
    float out[31] = {10, 12,  14,  16,  18,  20,  22,  24,  26,  28,  30,  32,  34,  36,  38,  40,  42,  44,  46,  48, 50, 52, 54, 56, 58, 60, 62, 64, 66, 68, 70};
    float in[31] = {418, 359, 318, 277, 250, 231, 209, 192, 179, 167, 157, 145, 138, 128, 123, 115, 110, 104, 100, 92, 88, 87, 81, 76, 76, 72, 68, 66, 64, 60, 59};
    float filterStore[5] = {0,0,0,0,0};
    int arrayPos;

    


  
};

