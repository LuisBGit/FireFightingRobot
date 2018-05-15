
#include "motionHandler.h"
#include <math.h>


void motionHandler::setupHandler(byte p1, byte p2, byte p3, byte p4) {
  this->pidX.setGains(0.25 , 0, 0);
  this->pidY.setGains(0, 0, 0);
  this->pidZ.setGains(0.5, 0, 0);
  this->p1 = p1; this->p2 = p2; this->p3 = p3; this->p4 = p4;
  this->topLeft.attach(p1); this->botLeft.attach(p2); this->botRight.attach(p3); this->topRight.attach(p4);


}

void motionHandler::setGains(float p, float i, float d) {
    this->pidX.setGains(p, i, d);
}


void motionHandler::moveHandler(float x, float y, float z, float xFeedBack, float yFeedBack, float zFeedBack, bool on) {
  float vx = 0;
  float vy = 0;
  float wz = 0;

  /*if (on == true) {
    vx = pidX.applyController(x - xFeedBack);
    vy = pidY.applyController(y- yFeedBack);
    wz = pidZ.applyController(z - zFeedBack);
  } else {
    vx = x;
    vy = y;
    wz = z;
  }*/

  vx = pidX.applyController(x- xFeedBack);
  vy = y;
  wz = pidZ.applyController(z - zFeedBack);
  
  
  theta1 = ((-1/rw)* (vx + vy +((L+l) *wz))) + 1500;
  theta2 =  ((-1/rw)* (vx - vy + ((L+l) * wz)) + 1500);
  theta3 = ((1/rw)* (vx - vy - ((L+l) * wz))) + 1500;
  theta4 = ((1/rw)* (vx + vy - ((L+l) * wz))) + 1500;
  /*Serial1.println("RAW");
  Serial1.print(theta2);
  Serial1.print(",");
  Serial1.print(theta4);
  Serial1.print(",");
  Serial1.print(theta3);
  Serial1.print(",");
  Serial1.println(theta1);*/


  int topLeftWrite = theta2; 
  int botLeftWrite = theta4;
  int botRightWrite = theta3;
  int topRightWrite = theta1;
  topLeftWrite = constrain(this->theta2,  700, 2100);
  botLeftWrite = constrain(this->theta4, 700, 2100);
  botRightWrite = constrain(this->theta3, 700, 2100);
  topRightWrite = constrain(this->theta1, 700, 2100);
  /*Serial1.println("Constrained");
  Serial1.print(topLeftWrite);
  Serial1.print(",");
  Serial1.print(botLeftWrite);
  Serial1.print(",");
  Serial1.print(botRightWrite);
  Serial1.print(",");
  Serial1.println(topRightWrite);*/


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
