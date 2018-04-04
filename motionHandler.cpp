#include "motionHandler.h"


void motionHandler::setupHandler(byte p1, byte p2, byte p3, byte p4) {
  this->pidX.setGains(200, 0, 0);
  this->pidY.setGains(1, 0, 0);
  this->pidZ.setGains(1, 0, 0);
  this->p1 = p1; this->p2 = p2; this->p3 = p3; this->p4 = p4;
  this->m1.attach(p1); this->m2.attach(p2); this->m3.attach(p3); this->m4.attach(p4);
  
  
}


void motionHandler::moveHandler(int vx, int vy, int wz, float error, HardwareSerial *SerialCom) {


  int m1Write;
  int m2Write;
  int m3Write;
  int m4Write;
  int modifier;

  
  theta1 = ((-1/rw)* (vx + vy +((L+l) *wz)))  + 1500;
  theta2 =  ((-1/rw)* (vx - vy + ((L+l) * wz))) + 1500;
  theta3 = ((1/rw)* (vx - vy - ((L+l) * wz))) + 1500;
  theta4 = ((1/rw)* (vx + vy - ((L+l) * wz))) + 1500;


  if (error != 0 ) {
    modifier = pidX.applyController(error);
    m1Write = constrain(this->theta2 + modifier, 1500, 2100);
    m2Write = constrain(this->theta4 - modifier, 1500, 2100);
    m3Write = constrain(this->theta3 + modifier, 700, 1500);
    m4Write = constrain(this->theta1 - modifier, 700, 1500);

  } else {
     modifier = 0;
     m1Write = theta2;
     m2Write = theta4;
     m3Write = theta3;
     m4Write = theta1;
  }
  //SerialCom->print("Controller Modifier: ");
  //SerialCom->println(modifier);


/*
  this->m1.writeMicroseconds(this->theta2 + modifier);
  this->m4.writeMicroseconds(this->theta1 - modifier);
  this->m3.writeMicroseconds(this->theta3 + modifier);
  this->m2.writeMicroseconds(this->theta4 - modifier);*/

  this->m1.writeMicroseconds(m1Write);
  this->m4.writeMicroseconds(m4Write);
  this->m3.writeMicroseconds(m3Write);
  this->m2.writeMicroseconds(m2Write);



}



void motionHandler::stopMotor() {
  this->m1.writeMicroseconds(1500);
  this->m2.writeMicroseconds(1500);
  this->m3.writeMicroseconds(1500);
  this->m4.writeMicroseconds(1500);
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
  this->m1.detach();
  this->m2.detach();
  this->m3.detach();
  this->m4.detach();

  pinMode(this->p1, INPUT);
  pinMode(this->p2, INPUT);
  pinMode(this->p3, INPUT);
  pinMode(this->p4, INPUT);
}

void motionHandler::enableHandler() {
  this->m1.attach(this->p1); this->m2.attach(this->p2); this->m3.attach(this->p3); this->m4.attach(this->p4);
}

