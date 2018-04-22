#include "movementFSM.h"



void movementFSM::setupMovement() {
  handler.setupHandler(left_front, left_rear, right_rear, right_front);
}

void movementFSM::changeState(int movement) {
  currentState = (state)movement;
}

void movementFSM::runCurrentState(float frontRight, float frontLeft, float rightFront, float rightBack, float yaw) {
  switch (currentState) {
    case(NormalMove):
      normalMove(rightFront, rightBack);
      break;
    case (Cornering):
      cornering();
      break;
    case (Dodge):
      dodge();
      break;
    case (Firefight):
      firefight();
      break;
    case (Stop):
      stopMovement();
      break;
  }
}

void movementFSM::normalMove(float rightFront, float rightBack) {
  handler.moveHandler(0, 4, 0,  rightFront, rightBack, 0);
}

void movementFSM::cornering() {
  handler.moveHandler(0, 0, 20, 0 , 0, 3);
}

void movementFSM::dodge() {

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
