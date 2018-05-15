#include "arduino.h"

class PID {
  public:
    void setGains(float kp, float ki, float kd);
    float applyController(float error);
  private:
    float kp, ki, kd;
    float lastError;
    unsigned long prevTime;
    unsigned errorSum;
    float lastIntError = 0 ;

};
