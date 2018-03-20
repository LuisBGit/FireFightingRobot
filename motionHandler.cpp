#include "motionHandler.h"


void motionHandler::setupHandler(byte p1, byte p2, byte p3, byte p4) {
  this->motorSystem.setupLocomotion(p1, p2, p3, p4);
  this->pidX.setGains(1, 0, 0);
  this->pidY.setGains(1, 0, 0);
  this->pidZ.setGains(1, 0, 0);
  
  
  
}

void motionHandler::serialReceive(HardwareSerial *SerialCom) {
  
  if (SerialCom->available()) {
    char val = SerialCom->read();
    SerialCom->print("Speed:");
    SerialCom->print(" ms ");

    //Perform an action depending on the command
    switch (val) {
      case 'w'://Move Forward
      case 'W':
        motorSystem.IK(0,10,0, SerialCom);
        SerialCom->println("Forward");
        break;
      case 's'://Move Backwards
      case 'S':
        motorSystem.IK(0,-10,0 , SerialCom);
        SerialCom->println("Backwards");
        break;
      case 'q'://Turn Left
      case 'Q':        
        motorSystem.IK(10,0,0, SerialCom);
        SerialCom->println("Strafe Left");
        break;
      case 'e'://Turn Right
      case 'E':
        motorSystem.IK(-10,0,0, SerialCom);
        SerialCom->println("Strafe Right");
        break;
      case 'a'://Turn Right
      case 'A':
        motorSystem.IK(0,0,50, SerialCom);
        SerialCom->println("ccw");
        break;
      case 'd'://Turn Right
      case 'D':
        motorSystem.IK(0,0,-50, SerialCom);
        SerialCom->println("cw");
        break;
      case '-'://Turn Right

      default:
        motorSystem.STOP();
        SerialCom->println("stop");
        break;
    }
  }


  
}

void motionHandler::disableHandler() {
  motorSystem.disable_motors();
}

void motionHandler::enableHandler() {
  motorSystem.enable_motors();
}

