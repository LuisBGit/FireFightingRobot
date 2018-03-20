#include "Arduino.h"
#include "Servo.h"

class locomotion{
  public:
    void setupLocomotion(byte p1, byte p2, byte p3, byte p4);
    void IK(float vx, float vy, float wz, HardwareSerial *SerialCom);
    void STOP();
    void disable_motors();
    void enable_motors();
  private:
    Servo m1, m2, m3, m4;
    byte p1, p2, p3, p4;
    float theta1,theta2,theta3,theta4;
    const float rw  = 0.027;
    const float L = 0.078;
    const float l = 0.080;



    
    
};

