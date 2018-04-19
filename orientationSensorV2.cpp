#include "orientationSensorV2.h"
#include <math.h>
#include <FaBo9Axis_MPU9250.h>


void orientationSensor::setupOrientation() {
  fabo.begin();
  out=0;
  prevZ=0;
  currentZ=0;

  prev_time=0;
  dt=0;

  integralSum=0;
}


float orientationSensor::readOrientation(unsigned long dt) {
  fabo.readGyroXYZ(&gx, &gy, &gz);
  prevZ = currentZ;
  currentZ = gz - zBias;
  
  out = (((currentZ +  prevZ)/2)* dt/1000) + integralSum;
  integralSum = out;
  return out;
}

float orientationSensor::readYaw() {
  return out;
}

void orientationSensor::recalibrate() {
  float zSum = 0;
  for (int i = 0; i < 100; i++) {
      fabo.readGyroXYZ(&gx, &gy, &gz);
      zSum += gz;
      delay(20);
  }
  zBias = zSum / 200.0;
  integralSum = 0;
  out = 0;
}

