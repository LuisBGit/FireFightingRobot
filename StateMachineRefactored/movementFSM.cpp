#include "movementFSM.h"



void movementFSM::setupMovement() {
  handler.setupHandler(left_front, left_rear, right_rear, right_front);
}

void movementFSM::changeState(int movement) {
  currentState = (state)movement;
}

void movementFSM::runCurrentState(float frontRight, float frontLeft, float rightFront, float rightBack, float yawn, int numberCorners) {
  switch (currentState) {
    case(NormalMove):
      normalMove(rightFront, rightBack, numberCorners);
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

void movementFSM::normalMove(float rightFront, float rightBack, int numberCorners) {
  handler.moveHandler(0, 4, 0,  rightFront, rightBack, 0, ((numberCorners-1)/4)*15);
}

void movementFSM::cornering() {
  handler.moveHandler(0, 0, 20, 0 , 0, 3,0);
}

void movementFSM::dodge() {
  handler.stopMotor();
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
