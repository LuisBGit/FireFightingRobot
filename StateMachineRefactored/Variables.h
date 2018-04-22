#include "battery.h"
#include "Sensors.h"
#include "movementFSM.h"



//Variables used in the main arduino loop


//****************************************************************PINS*************************************************************


//Motor Pins
/*
const byte left_front = 46;
const byte left_rear = 47;
const byte right_rear = 50;
const byte right_front = 51;*/
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

//***********************************************************Global Variables******************************************************


//*************************************************************ENUMERATION*********************************************************
enum STATE {
  INITIALISING,
  RUNNING,
  STOPPED
};

//*************************************************************ENUMERATION*********************************************************
