#include "arduino.h"

class ultrasonic{

  public:
    float readUltrasonic();
    void setupUltrasonic(int trig, int echo);


 private:
    float ultraArray[5] = {0,0,0,0,0};
    byte trig;
    byte echo;
    int arrayPos = 0;
    int meanDuration;
    long duration;
};

