#include "Sensors.h"

void sensorManager::setupSensors() {
  //Setup all the sensors
  irFrontRight.setupIR(3);
  irFrontLeft.setupIR(4);
  irRightFront.setupIR(6);
  irRightBack.setupIR(7);

  pinMode(ultraTrig, OUTPUT);
  pinMode(ultraEcho, INPUT);

  //Read IR to remove startup error
  for (int i =0; i <5; i++) {
    readIRs();
    readUltra();
    delay(200);
  }
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
  if(count == 4){
    count = 0;
  }
  
  float duration;
  float filt;
  float distance;
  digitalWrite(ultraTrig, LOW); //Force to low, just in case
  delayMicroseconds(2); //hold low
  digitalWrite(ultraTrig, HIGH); //turn pin on for 10us to send all required pulsess
  delayMicroseconds(10);
  digitalWrite(ultraTrig, LOW);

  duration = pulseIn(ultraEcho, HIGH); //Waits for pin to go low from HIGH / pulsewidth
  filterArray[count] = duration;
  filt = (filterArray[0] + filterArray[1] + filterArray[2] + filterArray[3] + filterArray[4])/5;
  count++;
  distance = (filt * 0.0221) - 1.39; //Coverts to cm
  return distance;
}

float sensorManager::laserYaw() {
  return atan2(resultRightFront - resultRightBack, 12) * (180/PI);
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


float sensorManager::getGyro() {
  return yawSensor.getGyro();
}
