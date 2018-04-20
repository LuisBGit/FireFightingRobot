#include "Sensors.h"

void sensorManager::setupSensors() {
  //Setup all the sensors
  irFrontRight.setupIR(4);
  irFrontLeft.setupIR(5);
  irRightFront.setupIR(6);
  irRightBack.setupIR(7);

  sonar = new NewPing(ultraTrig, ultraEcho, 400);

  //Read IR to remove startup error
  readIRs();

  //Setup orientationSensor
  yawSensor.setupOrientation();
  yawSensor.recalibrate();


}

void sensorManager::readIRs() {
  //Read the IR sensors
  resultFrontRight = irFrontRight.readSensor();
  resultFrontLeft = irFrontLeft.readSensor();
  resultRightFront = irRightFront.readSensor();
  resultRightBack = irRightBack.readSensor();
}

void sensorManager::readUltra() {
  //Read the ultrasonic sensor
  ultraReading = sonar->ping_cm();
}

void sensorManager::readYaw(unsigned long dt) {
  //Read the orientation Sensor
  yaw = yawSensor.readOrientation(dt);
}

void sensorManager::recalibrateYaw() {
  yawSensor.recalibrate();
}


//Get Functions
float sensorManager::getFrontRight() {
  return resultFrontRight;
}

float sensorManager::getFrontLeft() {
  return resultFrontLeft;
}
float sensorManager::getRightFront() {
  return resultFrontRight;
}

float sensorManager::getRightBack() {
  return resultRightBack;
}

float sensorManager::getUltra() {
  return ultraReading;
}

float sensorManager::getYaw() {
  return yawSensor.readYaw();
}
