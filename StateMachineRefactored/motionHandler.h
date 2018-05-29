#include "arduino.h"
#include "Servo.h"

#include "PID.h"


class motionHandler {
  public:
    void setupHandler(byte p1, byte p2, byte p3, byte p4);
    void moveHandler(int vx, int vy, int wz, float frontReading,float backReading , int motion, float desiredDistance, float yawReading);
    void stopMotor();
    void disableHandler();
    void setGains(float p, float i, float d);
    void enableHandler();
    void moveForward(int speed_val);
    void moveLeft(int speed_val);
    void moveRight(int speed_val);
    void rotateCW(int speed_val);
    void rotateCCW(int speed_val);
    void realignWall(float frontReading, float backReading);
    void rotatePosition(float yawReading, int input);
  private:
    PID pidX, pidY, pidZ;
    unsigned int theta1, theta2, theta3, theta4;
    Servo topLeft, botLeft, botRight, topRight;
    byte p1, p2, p3, p4;
    const float rw  = 0.027;
    const float L = 0.078;
    const float l = 0.080;

    enum correctionType {
      minor,
      rotate,
      wall,
      

    };

    enum state {
      Correction,
      noCorrection
    };

    correctionType currentType = minor;
    state currentState = Correction;

};
