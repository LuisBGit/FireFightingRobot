
#include "motionHandler.h"
#include <math.h>


void motionHandler::setupHandler(byte p1, byte p2, byte p3, byte p4) {
  this->pidX.setGains(150, 0, 0);
  this->pidY.setGains(1, 0, 0);
  this->pidZ.setGains(35, 0, 0);
  this->p1 = p1; this->p2 = p2; this->p3 = p3; this->p4 = p4;
  this->topLeft.attach(p1); this->botLeft.attach(p2); this->botRight.attach(p3); this->topRight.attach(p4);


}

void motionHandler::setGains(float p, float i, float d) {
    this->pidX.setGains(p, i, d);
}


void motionHandler::moveHandler(int vx, int vy, int wz, float frontReading,float backReading, int motion, float desiredDistance, float yawReading) {
  // motion = 0, 1 => forward and backward
  // motion = 2, 3 => clockWise and anti clock wise
  float error = frontReading - backReading;
  int topLeftWrite;
  int botLeftWrite;
  int botRightWrite;
  int topRightWrite;
  int modifier;
/*
  if(motion !=0){
    wz = ((150-yawReading) * wz/100);
  }*/

  theta1 = ((-1/rw)* (vx + vy +((L+l) *wz)))  + 1500;
  theta2 =  ((-1/rw)* (vx - vy + ((L+l) * wz))) + 1500;
  theta3 = ((1/rw)* (vx - vy - ((L+l) * wz))) + 1500;
  theta4 = ((1/rw)* (vx + vy - ((L+l) * wz))) + 1500;

  switch (motion) {
    case(0):
      currentState = Correction;
      break;
    default:
      currentState = noCorrection;
      break;
  }
  if(fabs(error) > 5
  ){ //Default 6
    currentState = noCorrection;
  }
  switch (currentState) {
    case (Correction):
      switch (currentType) {
        case (minor):
            //Serial1.println("minor");
            if (frontReading <= (11 + desiredDistance) || backReading <= (11+ desiredDistance) || (frontReading == 999) || (backReading == 999)) {
              currentType = wall;
            }
            else if(fabs(error) >= 3 && desiredDistance<15){
              //currentType = rotate;
              //error = 0;
            }
            else {
              modifier = pidX.applyController(error);
              topLeftWrite = constrain(this->theta2 - -modifier,  1450, 2100);
              botLeftWrite = constrain(this->theta4 - -modifier, 1450, 2100);
              botRightWrite = constrain(this->theta3, 700, 1500);
              topRightWrite = constrain(this->theta1, 700, 1500);
            }
          break;
        case (rotate):

          if (error > 1) {
              topLeftWrite = 1600;
              botLeftWrite = 1600;
              botRightWrite = 1600;
              topRightWrite = 1600;
          } else if (error < -1) {
              topLeftWrite = 1400;
              botLeftWrite = 1400;
              botRightWrite = 1400;
              topRightWrite = 1400;
          } else {
            currentType = minor;
          }
          break;
        case(wall):
          //Serial1.println("wall");
          if((frontReading == 999 || backReading == 999) || (frontReading <= 11.5+ desiredDistance && backReading <= 11.5+ desiredDistance)){
            topLeftWrite = 1300;
            botLeftWrite = 1700;
            topRightWrite = 1300;
            botRightWrite = 1700;

          }

          else{
            currentType = minor;
          }
          break;

      }
      break;
    case (noCorrection):

       topLeftWrite = theta2;
       botLeftWrite = theta4;
       botRightWrite = theta3;
       topRightWrite = theta1;
      break;
    default:
      break;
  }



  this->topLeft.writeMicroseconds(topLeftWrite);
  this->topRight.writeMicroseconds(topRightWrite);
  this->botRight.writeMicroseconds(botRightWrite);
  this->botLeft.writeMicroseconds(botLeftWrite);



}



void motionHandler::stopMotor() {
  this->topLeft.writeMicroseconds(1500);
  this->botLeft.writeMicroseconds(1500);
  this->botRight.writeMicroseconds(1500);
  this->topRight.writeMicroseconds(1500);
}

void motionHandler::moveForward(int speed_val){
  this->topLeft.writeMicroseconds(1500 + speed_val);
  this->topRight.writeMicroseconds(1500 - speed_val);
  this->botRight.writeMicroseconds(1500 - speed_val);
  this->botLeft.writeMicroseconds(1500 +speed_val);
}
void motionHandler::moveRight(int speed_val){
  this->topLeft.writeMicroseconds(1500+speed_val);
  this->topRight.writeMicroseconds(1500+speed_val);
  this->botRight.writeMicroseconds(1500-speed_val);
  this->botLeft.writeMicroseconds(1500-speed_val);
}
void motionHandler::moveLeft(int speed_val){
  this->topLeft.writeMicroseconds(1500-speed_val);
  this->topRight.writeMicroseconds(1500-speed_val);
  this->botRight.writeMicroseconds(1500+speed_val);
  this->botLeft.writeMicroseconds(1500+speed_val);
}
void motionHandler::rotateCW(int speed_val){

  
  this->topLeft.writeMicroseconds(1500+speed_val);
  this->topRight.writeMicroseconds(1500+speed_val);
  this->botRight.writeMicroseconds(1500+speed_val);
  this->botLeft.writeMicroseconds(1500+speed_val);
}
void motionHandler::rotateCCW(int speed_val){
  this->topLeft.writeMicroseconds(1500-speed_val);
  this->topRight.writeMicroseconds(1500-speed_val);
  this->botRight.writeMicroseconds(1500-speed_val);
  this->botLeft.writeMicroseconds(1500-speed_val);
}

void motionHandler::rotatePosition(float yawReading, int input){
  float controllerInput = input - yawReading;
  float wz=-1*pidZ.applyController(controllerInput);
  if(wz<0){
   wz = constrain(wz,-500,0);
  }
  else{
    wz = constrain(wz,0,500);
  }
  this->topLeft.writeMicroseconds(1500+wz);
  this->topRight.writeMicroseconds(1500+wz);
  this->botRight.writeMicroseconds(1500+wz);
  this->botLeft.writeMicroseconds(1500+wz);
}


void motionHandler::realignWall(float frontReading, float backReading){
  float error =frontReading-backReading;
  if(error>0){
    rotateCW(100);
  }
  else{
    rotateCCW(100);
  }
}

void motionHandler::disableHandler() {
  this->topLeft.detach();
  this->botLeft.detach();
  this->botRight.detach();
  this->topRight.detach();

  pinMode(this->p1, INPUT);
  pinMode(this->p2, INPUT);
  pinMode(this->p3, INPUT);
  pinMode(this->p4, INPUT);
}

void motionHandler::enableHandler() {
  this->topLeft.attach(this->p1); this->botLeft.attach(this->p2); this->botRight.attach(this->p3); this->topRight.attach(this->p4);
}
