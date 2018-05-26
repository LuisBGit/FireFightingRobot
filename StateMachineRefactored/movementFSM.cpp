#include "movementFSM.h"



void movementFSM::setupMovement() {
  handler.setupHandler(left_front, left_rear, right_rear, right_front);
}

void movementFSM::changeState(int movement) {
  currentState = (state)movement;
}

void movementFSM::runCurrentState(float frontRight, float frontLeft, float rightFront, float rightBack, float yaw, int numberCorners) {
  switch (currentState) {
    case(NormalMove):
      normalMove(rightFront, rightBack, numberCorners);
      break;
    case (Cornering):
      cornering(yaw);
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
    handler.moveHandler(0, 5 , 0,  rightFront, rightBack, 3, ((numberCorners-1)/4)*14,0);
  } else {
  handler.moveHandler(0, 5 , 0,  rightFront, rightBack, 0, ((numberCorners-1)/4)*14,0);
  }
}

//Used for slowly spinning around for the start-up mode
void movementFSM::slowSpin(float moveSpeed){
  handler.moveHandler(0, 0, moveSpeed, 0, 0, 3, 0, 0);
}

void movementFSM::cornering(float yawReading) {
  handler.moveHandler(0, 0, 20, 0 , 0, 5,0, yawReading);
}

void movementFSM::dodge(float frontRight, float frontLeft) {
  switch (mod) {
    case (0):
      handler.moveHandler(-5, 0, 0, 0 , 0, 3,0,0);//-5
      break;
    case (1):
      handler.moveHandler(5, 0, 0, 0 , 0, 3,0,0);//5
      break;
    case (2):
      handler.moveHandler(0, 3, 0, 0 , 0, 3,0,0);//3 second one
      break;
    case (3):
      handler.stopMotor();
      break;
  }
}

void movementFSM::startupStraight() {
  handler.moveHandler(0, 3, 0, 0 , 0, 3,0,0);
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
