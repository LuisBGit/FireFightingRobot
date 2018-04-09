#include "ultrasonic.h"


void ultrasonic::setupUltrasonic(int trig, int echo) {
  this->trig = trig;
  this->echo = echo;
  pinMode(trig, OUTPUT);
  pinMode(echo, INPUT);
}

float ultrasonic::readUltrasonic() {
  if(this->arrayPos == 5){
    this->arrayPos = 0;
  }
  digitalWrite(this->trig, LOW);
  delayMicroseconds(5);
  digitalWrite(this->trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(this->trig, LOW);

  duration = pulseIn(echo, HIGH);
  ultraArray[arrayPos] = duration;
  meanDuration = (ultraArray[0] + ultraArray[1] + ultraArray[2] + ultraArray[3] + ultraArray[4]) /5;

  this->arrayPos++;
  return (this->meanDuration/2) / 29.1;
}

