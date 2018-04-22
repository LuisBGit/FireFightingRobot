#include "arduino.h"
#include "motionHandler.h"

class movementFSM {
  public:
    void setupMovement();
    void runCurrentState(float frontRight, float frontLeft, float rightFront, float rightBack, float yaw, int numberCorners);
    void changeState(int movement);
    void disableMovement();
    void stopMovement();
    int getState();

  private:

    enum state{
      NormalMove = 0,
      Cornering = 1,
      Dodge = 2,
      Firefight = 3,
      Stop = 4
    };

    state currentState = state(0);

    motionHandler handler;

    const byte left_front = 46;
    const byte left_rear = 47;
    const byte right_rear = 50;
    const byte right_front = 51;

    void normalMove(float rightFront, float rightBack, int numberCorners);
    void cornering();
    void dodge();
    void firefight();



};
