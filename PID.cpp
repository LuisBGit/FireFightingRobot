#include "PID.h"



void PID::setGains(float kp, float ki, float kd) {
  this->kp = kp;
  this->ki = ki;
  this->kd = kd;
  this->prevTime = 0;
  this->lastError = 0;
  this->errorSum = 0;
}

int PID::applyController(float error) {

   unsigned long currentTime = millis();
   unsigned long deltaTime = currentTime - this->prevTime;
   this->errorSum += error;
   float pOut = kp*error;
   float dOut = kd*(error - this->lastError) / (deltaTime);
   float iOut = ki*errorSum*deltaTime + lastIntError;
   lastIntError = iOut;
   this->prevTime = currentTime;
   this->lastError = error;
   
   return pOut + dOut + iOut;
  
}




