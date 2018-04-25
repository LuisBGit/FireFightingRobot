#include "motionHandler.h"
#include <math.h>


void motionHandler::setupHandler(byte p1, byte p2, byte p3, byte p4) {
  this->pidX.setGains(150, 0, 0);
  this->pidY.setGains(1, 0, 0);
  this->pidZ.setGains(1, 0, 0);
  this->p1 = p1; this->p2 = p2; this->p3 = p3; this->p4 = p4;
  this->topLeft.attach(p1); this->botLeft.attach(p2); this->botRight.attach(p3); this->topRight.attach(p4);
  
  
}

void motionHandler::setGains(float p, float i, float d) {
    this->pidX.setGains(p, i, d);
}


void motionHandler::moveHandler(int vx, int vy, int wz, float frontReading,float backReading, int motion, float desiredDistance) {
  // motion = 0, 1 => forward and backward
  // motion = 2, 3 => clockWise and anti clock wise
  float error = frontReading - backReading;
  int topLeftWrite;
  int botLeftWrite;
  int botRightWrite;
  int topRightWrite;
  int modifier;

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

  switch (currentState) {
    case (Correction):
      switch (currentType) {
        case (minor):
            //Serial1.println("minor");
            if (frontReading <= (12 + desiredDistance) || backReading <= (12+ desiredDistance) || (frontReading == 999) || (backReading == 999)) {
              currentType = wall;
            }
            else if(fabs(error) >= 3 && desiredDistance<15){
              currentType = rotate; 
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
          //Serial1.println("rotate");
          Serial1.print(frontReading);
          Serial1.print(", ");
          Serial1.println(backReading);
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
          if((frontReading == 999 || backReading == 999) || (frontReading <= 14+ desiredDistance && backReading <= 14+ desiredDistance)){
            topLeftWrite = 1400;
            botLeftWrite = 1600;
            botRightWrite = 1600;
            topRightWrite = 1400;
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



  
/*

  if (motion == 0) {
    modifier = pidX.applyController(error);
    topLeftWrite = constrain(this->theta2 - -modifier,  1450, 2100);
    botLeftWrite = constrain(this->theta4 - -modifier, 1450, 2100);
    botRightWrite = constrain(this->theta3, 700, 1500);
    topRightWrite = constrain(this->theta1, 700, 1500);

  }else if (motion == 2){
    if (error  > 0 ) {
      topLeftWrite = -theta2;
      botLeftWrite = -theta4;
      botRightWrite = -theta3;
      topRightWrite = -theta1;
    } else {
      topLeftWrite = theta2;
      botLeftWrite = theta4;
      botRightWrite = theta3;
      topRightWrite = theta1;
    }


  } else {
     modifier = 0;
     topLeftWrite = theta2;
     botLeftWrite = theta4;
     botRightWrite = theta3;
     topRightWrite = theta1;
  }*/
  //SerialCom->print("Controller Modifier: ");
  //SerialCom->println(modifier);


/*
  this->topLeft.writeMicroseconds(this->theta2 + modifier);
  this->topRight.writeMicroseconds(this->theta1 - modifier);
  this->botRight.writeMicroseconds(this->theta3 + modifier);
  this->botLeft.writeMicroseconds(this->theta4 - modifier);*/

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

