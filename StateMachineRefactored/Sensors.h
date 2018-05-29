#include "arduino.h"
#include <FaBo9Axis_MPU9250.h>
#include "IRShortRange.h"
#include "IRLongRange.h"
#include "math.h"
#include <NewPing.h>
#include <Servo.h>
#include "orientationSensorV2.h"


class sensorManager {

  public:
    void setupSensors();

    void readIRs();
    void readUltra();
    void readYaw(unsigned long dt);

    void recalibrateYaw();

    float getYaw();
    float getFrontRight();
    float getFrontLeft();
    float getRightFront();
    float getRightBack();
    float getUltra();
    float ultrasonic();
    float getGyro();
    float laserYaw();


  private:
    int count = 0; 
    float filterArray[5] = {0,0,0,0,0};
    int ultraTrig = 48;
    int ultraEcho = 49;
    const unsigned int MAX_DIST = 23200;
    int irFrRiPin = 4;
    int irFrLePin = 5;
    
    float resultFrontRight = 0;
    float resultFrontLeft = 0;
    float resultRightFront = 0;
    float resultRightBack = 0;

    float ultraReading = 0;

    float yaw = 0;
    
    IRShort irFrontRight;
    IRShort irFrontLeft;
    IRLong irRightFront;
    IRLong irRightBack;


    orientationSensor yawSensor;

};
