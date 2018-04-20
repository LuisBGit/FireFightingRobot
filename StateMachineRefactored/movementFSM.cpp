#include "movementFSM.h"



void movementFSM::setupMovement() {
  handler.setupHandler(left_front, left_rear, right_rear, right_front);
}

void movementFSM::changeState(int move) {
  state = move;
}

void movementFSM::runCurrentState(float frontRight, float frontLeft, float rightFront, float rightBack) {
  switch (currentState) {
    case(NormalMove):
      normalMove(rightFront, rightBack);
      break;
    case (cornering):
      cornering();
      break;
    case (Dodge):
      dodge();
      break;
    case (Firefight):
      firefight();
      break;
    case (Stop):
      stop();
      break;
  }
}

void movementFSM::normalMove(float rightFront, float rightBack) {
  handler.moveHandler(0, 4, 0,  rightFront, rightBack, 0);
}
