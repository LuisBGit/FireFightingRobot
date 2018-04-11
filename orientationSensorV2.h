#include "arduino.h"
#include <math.h>
#include <FaBo9Axis_MPU9250.h>

class orientationSensor{
  public:
    float readOrientation(unsigned long dt);
    float readYaw();
    void setupOrientation();
    void recalibrate();

  private:

    float arrayX[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    float arrayY[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    int index = 0;
    float gx, gy, gz;
    FaBo9Axis fabo;
    float zBias;

    float out;
    float prevZ;
    float currentZ;

    unsigned long prev_time;
    unsigned long dt;

    float integralSum ;
    
    float prevRead;

};

