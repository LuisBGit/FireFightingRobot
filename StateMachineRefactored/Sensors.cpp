#include "Sensors.h"

void sensorManager::setupSensors() {
  //Setup all the sensors
  irFrontRight.setupIR(4);
  irFrontLeft.setupIR(5);
  irRightFront.setupIR(6);
  irRightBack.setupIR(7);

  pinMode(ultraTrig, OUTPUT);
  pinMode(ultraEcho, INPUT);

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
  ultraReading = ultrasonic();
}

void sensorManager::readYaw(unsigned long dt) {
  //Read the orientation Sensor
  yaw = yawSensor.readOrientation(dt);
}

void sensorManager::recalibrateYaw() {
  yawSensor.recalibrate();
}

float sensorManager::ultrasonic(){
  float duration, distance;
  

  
  digitalWrite(ultraTrig, LOW); //Force to low, just in case
  delayMicroseconds(2); //hold low
  digitalWrite(ultraTrig, HIGH); //turn pin on for 10us to send all required pulsess
  delayMicroseconds(10);
  digitalWrite(ultraTrig, LOW);

  duration = pulseIn(ultraEcho, HIGH); //Waits for pin to go low from HIGH / pulsewidth
  return distance = (duration * 0.0343)/2; //Coverts to cm

}

//Get Functions
float sensorManager::getFrontRight() {
  return resultFrontRight;
}

float sensorManager::getFrontLeft() {
  return resultFrontLeft;
}
float sensorManager::getRightFront() {
  return resultRightFront;
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
