#include "movementFSM.h"



void movementFSM::setupMovement() {
  handler.setupHandler(left_front, left_rear, right_rear, right_front);
}

void movementFSM::changeState(int movement) {
  currentState = (state)movement;
}

void movementFSM::runCurrentState(float x, float y, float z, float xFeedBack, float yFeedBack, float zFeedBack, bool on, float yaw, int numberCorners) {
  switch (currentState) {
    case(NormalMove):
      normalMove(x, y, z, xFeedBack, yFeedBack, zFeedBack, true);
      break;
    case (Cornering):
      normalMove(x, y, z, 0, 0, zFeedBack, true);
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

void movementFSM::normalMove(float x, float y, float z, float xFeedBack, float yFeedBack, float zFeedBack, bool on) {
  handler.moveHandler(x, y, z, xFeedBack, yFeedBack, zFeedBack, on);

}

void movementFSM::cornering(float yawReading) {
  //handler.moveHandler(0, 0, 20, 0 , 0, 5,0, yawReading);
  //handler.moveHandler(0, 0, 20, xFeedBack, yFeedBack, yawReading, on);
}

void movementFSM::dodge() {
  switch (mod) {
    case (0):
      handler.moveHandler(-5, 0, 0, 0 , 0, 0, false);//-5
      break;
    case (1):
      handler.moveHandler(5, 0, 0, 0 , 0, 0, false);//5
      break;
    case (2):
      handler.moveHandler(0, 3, 0, 0 , 0, 0, false);//3 second one
      break;
    case (3):
      handler.stopMotor();
      break;

  }


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
