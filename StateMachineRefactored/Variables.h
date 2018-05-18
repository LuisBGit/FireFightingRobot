#include "battery.h"
#include "Sensors.h"
#include "movementFSM.h"
#include "distanceCalcV2.h"


//Variables used in the main arduino loop


//****************************************************************PINS*************************************************************


//Motor Pins
movementFSM movement;


//Servo Pins maybe unneeded when firefighting is integrated
int servoPin = 3;

//LED State pins
int red = 25;
int green = 24;
int blue = 23;

//****************************************************************PINS*************************************************************

//***********************************************************GLOBAL FUNCTIONS******************************************************
boolean is_battery_voltage_OK();

//***********************************************************GLOBAL FUNCTIONS******************************************************

//***********************************************************Global Variables******************************************************
HardwareSerial *SerialCom;
//motionHandler handler;

sensorManager sensors;


Servo servo;

float prevL = 9999;
float prevR = 9999;
float prevB = 9999;
float distanceToMove = 0;
int dodgeDirection = 0; //0 equals left

int stateDisplay=  0;

float disp1 = 0;
float disp2 = 0;
float disp3 = 0;

int changeCounter = 0;

//********************************************************Timing Variables****************************************************//////////

unsigned long currentTime = 0;

long irTiming = 0;
long pingTiming = 0;
long integralTiming = 0;
long cornerTiming = 0;
long stateUpdateTiming = 0;
long displayTiming = 0;
long batteryTiming = 0;
long mpuTimer = 0;
unsigned long sendTime = 0;


unsigned long decisionTiming = 0;
unsigned long motorTiming = 0;

//***********************************************************Global Variables******************************************************


//*************************************************************ENUMERATION*********************************************************
enum STATE {
  INITIALISING,
  RUNNING,
  STOPPED
};

enum dodgeSequence {
  START,
  SIDEWAYS,
  FORWARD,
  RETURN
};

enum state{
  NormalMove = 0,
  Cornering = 1,
  Dodge = 2,
  Firefight = 3,
  Stop = 4
};

enum dodgeDirection {
  RIGHT,
  LEFT
};

dodgeSequence dodgeFSM = START;

//*************************************************************ENUMERATION*********************************************************
