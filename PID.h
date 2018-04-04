#include "arduino.h"

class PID {
  public:
    void setGains(float kp, float ki, float kd);
    int applyController(float error);
  private:
    float kp, ki, kd;
    float lastError;
    unsigned long prevTime;
    unsigned errorSum;
    float lastIntError = 0 ;
    
};

