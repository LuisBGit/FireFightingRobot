#include "movementFSM.h"



void movementFSM::setupMovement() {
  handler.setupHandler(left_front, left_rear, right_rear, right_front);
}

void movementFSM::changeState(int movement) {
  currentState = (state)movement;
}

void movementFSM::runCurrentState(float frontRight, float frontLeft, float rightFront, float rightBack, float yawReading, int numberCorners, int yawInput) {
  switch (currentState) {
    case(NormalMove):
      normalMove(rightFront, rightBack, numberCorners);
      break;
    case (Cornering):
      cornering(rightFront, rightBack, yawReading,yawInput);
      break;
    case (Dodge):
      dodge(frontRight, frontLeft);
      break;
    case (Firefight):
      firefight();
      break;
    case (Stop):
      stopMovement();
      break;
  }
}

void movementFSM::normalMove(float rightFront, float rightBack, int numberCorners) {
  if (numberCorners > 10) {
    handler.moveHandler(0, 5 , 0,  rightFront, rightBack, 3, (int((numberCorners-1)/4))*23,0);
  } else {
    handler.moveHandler(0, 5 , 0,  rightFront, rightBack, 0, (int((numberCorners-1)/4))*23,0);
  }
}

//Used for slowly spinning around for the start-up mode
void movementFSM::slowSpin(float moveSpeed){
  handler.moveHandler(0, 0, moveSpeed, 0, 0, 5, 0, 0);
}

void movementFSM::cornering(float frontReading, float backReading, float yawReading, int yawInput) {
  switch(cornerState){
    case(0):
      handler.rotateCCW(150);
      break;
    case(1):
      handler.rotateCW(150);
      break;
    case(2):
      handler.moveForward(200);
      break;
    case(3):
      handler.stopMotor();
      break;
    case(4):
      handler.realignWall(frontReading, backReading);
      break;
    case(5):
      handler.moveRight(100);
      break;
    case(6):
      handler.moveLeft(150);
      break;

  }
}

void movementFSM::dodge(float frontRight, float frontLeft) {
  switch (mod) {
    case (0):
    //LEft
      handler.moveRight(200);//-5
      break;
    case (1):
    //Forward
      handler.moveLeft(200);//5
      break;
    case (2):
    //Right
      handler.moveForward(200);//3 second one
      break;
    case (3):
      handler.stopMotor();
      break;

  }


}

void movementFSM::changeCornerMode(int mode) {
  cornerState= mode;
}


void movementFSM::startupStraight(float moveSpeed) {
  handler.moveHandler(0, moveSpeed, 0, 0 , 0, 3,0,0);
}

void movementFSM::strafeStart(float moveSpeed) {
  handler.moveHandler(-1*moveSpeed, 0, 0, 0 , 0, 3,0,0);//-5
}

void movementFSM::changeDodgeMode(int mode) {
  mod = mode;
}

int movementFSM::getDodgeMode() {
  return mod;
}

void movementFSM::firefight() {

}

void movementFSM::stopMovement() {
  handler.stopMotor();
}

void movementFSM::disableMovement() {
  handler.disableHandler();
}

int movementFSM::getState() {
  return int(currentState);
}
