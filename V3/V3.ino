
#include "battery.h"
#include "motionHandler.h"
#include <FaBo9Axis_MPU9250.h> 
#include "MadgwickAHRS.h"
#include "IRShortRange.h"
#include "IRLongRange.h"
#include "math.h"
#include <NewPing.h>
#include <Servo.h>
#include "orientationSensor.h"

//****************************************************************PINS*************************************************************

const byte left_front = 46;
const byte left_rear = 47;
const byte right_rear = 50;
const byte right_front = 51;

int trig = 48;
int echo = 49;

int servoPin = 4;

//****************************************************************PINS*************************************************************

//***********************************************************GLOBAL FUNCTIONS******************************************************
boolean is_battery_voltage_OK();

//***********************************************************GLOBAL FUNCTIONS******************************************************

//***********************************************************Global Variables******************************************************
HardwareSerial *SerialCom;
motionHandler handler;
FaBo9Axis fabo;
Madgwick filter;
IRShort fRight;
IRShort fLeft;
IRLong rFront;
IRLong rBack; 
NewPing sonar(trig, echo, 400);

float ax, ay, az;
float gx, gy, gz;
float mx, my, mz;

float resultR;
float resultL;
float resultRFront;
float resultRBack;
float sonarReading;

int tick = 0;
float gzSum, gySum, gxSum, gzOffset, gyOffset, gxOffset;
float azSum, aySum, axSum, azOffset, ayOffset, axOffset;

Servo servo;

unsigned int tickCounter = 0;

orientationSensor yawSensor;
float currentHeading = 0;
float referenceHeading = 0;
float offSetHeading = 0;

float yawError;
float upperLimit, lowerLimit;

boolean cornerFlag;

int stateDisplay=  0;
//***********************************************************Global Variables******************************************************


//*************************************************************ENUMERATION*********************************************************
enum STATE {
  INITIALISING,
  RUNNING,
  STOPPED
};

enum traversalState{
  NormalMove,
  Dodge,
  Cornering
};

traversalState currentTraversal;
//*************************************************************ENUMERATION*********************************************************

ISR(TIMER3_OVF_vect) {
 tickCounter++;
 //Detect Corner and Spin

 /*
 if (sonarReading <= 23 && resultR <= 25 && resultL <= 25 && currentTraversal != Cornering) {
  
    SerialCom->print(resultR);
    SerialCom->print(",");
    SerialCom->print(resultL);
    SerialCom->print(",");
    SerialCom->print(sonarReading);



    cornerFlag = true;
 }*/



}



//************************************************************


void setup() {
  SerialCom = &Serial1;  
  SerialCom->begin(115200);
  //SerialCom->println("Starting......");
  servo.attach(servoPin);
  servo.write(90);
  delay(2000);
}

void loop() {
  static STATE machine_state = INITIALISING;
  
  //FSM

  switch (machine_state) {
     case INITIALISING:
          machine_state=initialise();
          break;
     case RUNNING:
           machine_state=runCycle();
          break;
     case STOPPED:
          machine_state=stopping();
          break;
    
  };
}


STATE initialise() {
  SerialCom->println("START");
  SerialCom->println("Setting up locomotion");
  handler.setupHandler(left_front, left_rear, right_rear, right_front);
  //SerialCom->println("MPU");
  fRight.setupIR(4);
  fLeft.setupIR(5);
  rFront.setupIR(6);
  rBack.setupIR(7);
  readIR();
  setupPins();
  yawSensor.setupOrientation();
  delay(200);
  for (int i = 0; i < 11; i++) {
      referenceHeading = yawSensor.readOrientation();
  }
  currentTraversal = NormalMove;
  //sample();
  sei();
  return RUNNING;
  
}


STATE runCycle() {
  float startTest = millis();
  static unsigned long previous_millis; 
  handler.serialReceive(SerialCom);

/*
  if (tickCounter % 250) {
    currentHeading = yawSensor.readOrientation();
    float headingError = currentHeading - referenceHeading;
    SerialCom->print("Reference Point: ");
    SerialCom->print(referenceHeading);
    SerialCom->print(",");
    SerialCom->print("currentHeading: ");
    SerialCom->print(currentHeading);
    SerialCom->print(",");
    SerialCom->print("Error: ");
    SerialCom->println(headingError);
    
    
  }*/
  if (sonarReading <= 23 && resultR <= 25 && resultL <= 25 && currentTraversal != Cornering) {
    currentTraversal = Cornering;
    handler.stopMotor();

    delay(1500);
    
    setYawReference();
    
  }

  
  if (tickCounter % 31 == 0) {

    readIR();
    //SerialCom->println(resultR);
    //SerialCom->println(resultL);
    //SerialCom->print(resultRFront);
    //SerialCom->print(",");
    //SerialCom->println(resultRBack);


  }

  if (tickCounter % 80 == 0) {
   //SerialCom->println(sonar.ping_cm());
   sonarReading = sonar.ping_cm();
   //SerialCom->println(sonarReading);

  }
  /*
  if (cornerFlag  == true && currentTraversal != Cornering) {
    currentTraversal = Cornering;
    handler.stopMotor();

    delay(2000);
    
    setYawReference();

    cornerFlag = false;
  }*/

  switch (currentTraversal) {
     case NormalMove:
          //SerialCom->println("NormalMove");
          stateDisplay = 0;
          handler.moveHandler(0, 4, 0, (resultRFront - resultRBack), SerialCom, 0);
          break;
     case Cornering:
          //SerialCom->println("Cornering");
          stateDisplay = 1;
          readYaw();
          //SerialCom->println("HELP");

          //yawError = (referenceHeading - 90)- currentHeading;

                
   
          if (referenceHeading < 90 && currentHeading > 180) {
              yawError = ((currentHeading - 360) - (referenceHeading - 90));
          } else {
               yawError = currentHeading - (referenceHeading - 90);
          }

          SerialCom->print("Reference Point: ");
          SerialCom->print(referenceHeading);
          SerialCom->print(",");
          SerialCom->print("currentHeading: ");
          SerialCom->print(currentHeading);
          SerialCom->print(",");
          SerialCom->print("Error: ");
          SerialCom->println(yawError);
    
          //SerialCom->println(yawError);
          handler.moveHandler(0, 0, 20, 0 , SerialCom, 3);
          /*if (fabs(resultRFront - resultRBack) <= 2 && resultRFront <= 30 && resultRBack <= 30 && resultR >= 23 && resultL >= 23) {
            currentTraversal = NormalMove;
          }*/
          if (yawError <=5 ) {
            SerialCom->println("finished turn");
            handler.stopMotor();
            currentTraversal = NormalMove;
          }

          break;
     case Dodge:

          break;
    
  }

  if (tickCounter % 401 == 0) {
    SerialCom->println(stateDisplay);
  }

  if (tickCounter % 400 == 0) {
    if (is_battery_voltage_OK() == false) {
      return STOPPED;
    }
  }
  tick++;

  //SerialCom->println(millis() - startTest);
  return RUNNING;
  
}





///BATTERY SAFETY SECTION
STATE stopping() {
  static byte counter_lipo_voltage_ok;
  static unsigned long previous_millis;

  handler.disableHandler();
//  slow_flash_LED_builtin();

 
  if (millis() - previous_millis > 500) { //print massage every 500ms
    previous_millis = millis();
   // SerialCom->println("Lipo voltage too LOW, any lower and the lipo with be damaged");
    //SerialCom->println("Please Re-charge Lipo");

    //500ms timed if statement to check lipo and output speed settings
    if (is_battery_voltage_OK()) {
      //SerialCom->print("Lipo OK Counter:");-
      //SerialCom->println(counter_lipo_voltage_ok);
      counter_lipo_voltage_ok++;
      if (counter_lipo_voltage_ok > 10) { //Making sure lipo voltage is stable
        counter_lipo_voltage_ok = 0;
        handler.enableHandler();
        //SerialCom->println("Lipo OK returning to RUN STATE");
        return RUNNING;
      }
    } else counter_lipo_voltage_ok = 0;
  }
  return STOPPED;
}


void setupPins() {
  
  TCCR3A |= (1<<COM3A1);
  TCCR3B |= (1<<CS31)|(1<<CS30);
  TIMSK3 |= (1<<TOIE3);

  
}


void readIR() {
    resultR = fRight.readSensor();
    resultL = fLeft.readSensor();
    resultRFront = rFront.readSensor();
    resultRBack = rBack.readSensor();
}

void readYaw() {
    currentHeading = yawSensor.readOrientation();
}

void setYawReference() {
  referenceHeading = yawSensor.readOrientation();
  if(referenceHeading < 90){
    lowerLimit = 360 - 90 - referenceHeading; 
  }
  else{
    lowerLimit = referenceHeading - 90;
  }
  if(referenceHeading > 270){
    upperLimit = referenceHeading +90-360; 
  }
  else{
    upperLimit = referenceHeading + 90;
  }
}

