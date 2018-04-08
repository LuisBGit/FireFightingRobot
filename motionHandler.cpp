#include "motionHandler.h"


void motionHandler::setupHandler(byte p1, byte p2, byte p3, byte p4) {
  this->pidX.setGains(300, 0.0000000001, 0.125);
  this->pidY.setGains(1, 0, 0);
  this->pidZ.setGains(1, 0, 0);
  this->p1 = p1; this->p2 = p2; this->p3 = p3; this->p4 = p4;
  this->topLeft.attach(p1); this->botLeft.attach(p2); this->botRight.attach(p3); this->topRight.attach(p4);
  
  
}


void motionHandler::moveHandler(int vx, int vy, int wz, float error, HardwareSerial *SerialCom, int motion) {
  // motion = 0, 1 => forward and backward
  // motion = 2, 3 => clockWise and anti clock wise

  int topLeftWrite;
  int botLeftWrite;
  int botRightWrite;
  int topRightWrite;
  int modifier;

  
  theta1 = ((-1/rw)* (vx + vy +((L+l) *wz)))  + 1500;
  theta2 =  ((-1/rw)* (vx - vy + ((L+l) * wz))) + 1500;
  theta3 = ((1/rw)* (vx - vy - ((L+l) * wz))) + 1500;
  theta4 = ((1/rw)* (vx + vy - ((L+l) * wz))) + 1500;


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
  }
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



void motionHandler::serialReceive(HardwareSerial *SerialCom) {
  
  if (SerialCom->available()) {
    char val = SerialCom->read();
    SerialCom->print("Speed:");
    SerialCom->print(" ms ");

    /*//Perform an action depending on the command
    switch (val) {
      case 'w'://Move Forward
      case 'W':
        moveHandler(0,10,0, 0);
        SerialCom->println("Forward");
        break;
      case 's'://Move Backwards
      case 'S':
        moveHandler(0,-10,0, 0);
        SerialCom->println("Backwards");
        break;
      case 'q'://Turn Left
      case 'Q':        
        moveHandler(10,0,0,0);
        SerialCom->println("Strafe Left");
        break;
      case 'e'://Turn Right
      case 'E':
        moveHandler(-10,0,0,0);
        SerialCom->println("Strafe Right");
        break;
      case 'a'://Turn Right
      case 'A':
        moveHandler(0,0,50,0);
        SerialCom->println("ccw");
        break;
      case 'd'://Turn Right
      case 'D':
        moveHandler(0,0,-50,0);
        SerialCom->println("cw");
        break;
      case '-'://Turn Right

      default:
        stopMotor();
        SerialCom->println("stop");
        break;
    }*/
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

