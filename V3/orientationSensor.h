#include "arduino.h"
#include <math.h>
#include <FaBo9Axis_MPU9250.h>

class orientationSensor{
  public:
    float readOrientation();
    void setupOrientation();

  private:
    //float array[5] = {0, 0, 0, 0, 0};
    float arrayX[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    float arrayY[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    int index = 0;
    float mx, my, mz;
    FaBo9Axis fabo;
    float xOffset = 25.98;
    float yOffset = 63.415;
    float xScale = 0.0107158;
    float yScale = 0.00788457;
    float prevRead;

};

