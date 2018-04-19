
#include "battery.h"
#include "motionHandler.h"
#include <FaBo9Axis_MPU9250.h> 
#include "MadgwickAHRS.h"
#include "IRShortRange.h"
#include "IRLongRange.h"
#include "math.h"
#include <NewPing.h>
#include <Servo.h>
#include "orientationSensorV2.h"

//****************************************************************PINS*************************************************************

const byte left_front = 46;
const byte left_rear = 47;
const byte right_rear = 50;
const byte right_front = 51;

int trig = 48;
int echo = 49;

int servoPin = 3;

int red = 25;
int green = 24;
int blue = 23;

//****************************************************************PINS*************************************************************

//***********************************************************GLOBAL FUNCTIONS******************************************************
boolean is_battery_voltage_OK();

//***********************************************************GLOBAL FUNCTIONS******************************************************

//***********************************************************Global Variables******************************************************
HardwareSerial *SerialCom;
motionHandler handler;
//FaBo9Axis fabo;
Madgwick filter;
IRShort fRight;
IRShort fLeft;
IRLong rFront;
IRLong rBack; 
NewPing sonar(trig, echo, 400);


float resultR;
float resultL;
float resultRFront;
float resultRBack;
float sonarReading;

Servo servo;

unsigned int tickCounter = 0;

orientationSensor yawSensor;
float currentHeading = 0;
float referenceHeading = 0;
float offSetHeading = 0;

float angle = 0;
float upperLimit, lowerLimit;

boolean cornerFlag;

int stateDisplay=  0;

float disp1 = 0;
float disp2 = 0;
float disp3 = 0;

int changeCounter = 0;

//********************************************************Timing Variables****************************************************//////////

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

enum traversalState{
  NormalMove,
  Dodge,
  Cornering
};

traversalState currentTraversal;
//*************************************************************ENUMERATION*********************************************************

/*
ISR(TIMER3_OVF_vect) {
 tickCounter++;
 //Detect Corner and Spin

 
 if (sonarReading <= 23 && resultR <= 25 && resultL <= 25 && currentTraversal != Cornering) {
  
    SerialCom->print(resultR);
    SerialCom->print(",");
    SerialCom->print(resultL);
    SerialCom->print(",");
    SerialCom->print(sonarReading);



    cornerFlag = true;
 }



}*/



//************************************************************


void setup() {
  SerialCom = &Serial1;  
  SerialCom->begin(115200);
  //SerialCom->println("Starting......");
  servo.attach(servoPin);
  servo.write(90);
  delay(1000);
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

  handler.setupHandler(left_front, left_rear, right_rear, right_front);

  
  fRight.setupIR(4);
  fLeft.setupIR(5);
  rFront.setupIR(6);
  rBack.setupIR(7);
  readIR();

  pinMode(red, OUTPUT);
  pinMode(green, OUTPUT);
  pinMode(blue, OUTPUT);

  digitalWrite(red, LOW);
  digitalWrite(green, LOW);
  digitalWrite(blue, LOW);
  
  //setupPins();
  yawSensor.setupOrientation();
  yawSensor.recalibrate();
  delay(100);
  currentTraversal = NormalMove;
  //sample();
  sei();
  return RUNNING;
  
}


STATE runCycle() {
  float startTest = millis();
  static unsigned long previous_millis; 


  if (sonarReading <= 23 && resultR <= 25 && resultL <= 25 && currentTraversal != Cornering) {
    currentTraversal = Cornering;
    handler.stopMotor();
    
    delay(300);
    angle = yawSensor.readYaw();
    yawSensor.recalibrate();
    
    //setYawReference();
    
  }

  
  if (millis() - irTiming >= 10) {
    irTiming = millis();
    readIR();

  }

  if (millis()  - pingTiming >= 15) {
   pingTiming = millis();
   sonarReading = sonar.ping_cm();
  }

  if (millis() - mpuTimer >= 20) {
    unsigned long dt = millis() - mpuTimer;
    mpuTimer = millis();
    //if (currentTraversal == Cornering) {
      
      //angle = yawSensor.readOrientation(dt);
      yawSensor.readOrientation(dt);  
   // }
  }

  switch (currentTraversal) {
     case NormalMove:
          digitalWrite(red, HIGH);
          digitalWrite(green, LOW);
          digitalWrite(blue, LOW);
          stateDisplay = 0;
          handler.moveHandler(0, 4, 0, resultRFront,resultRBack, 0);
          break;
     case Cornering:
          digitalWrite(red, LOW);
          digitalWrite(green, HIGH);
          digitalWrite(blue, LOW);
          stateDisplay = 1;

          handler.moveHandler(0, 0, 20, 0 , 0, 3);

          if (80<yawSensor.readYaw() ) {

            handler.stopMotor();
            currentTraversal = NormalMove;
          }

          break;
     case Dodge:

          break;
    
  }


  if (millis() - displayTiming >= 150) {
    displayTiming = millis();
    displayMessage("PID ", changeCounter, resultRFront - resultRBack, 0);
  }

  if (millis() - batteryTiming >=  400) {
    batteryTiming = millis();
    if (is_battery_voltage_OK() == false) {
      return STOPPED;
    }
  }

  if (millis() - sendTime >= 25) {
    sendTime = millis();
    getInput();
  }

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

/*
void setupPins() {
  
  TCCR3A |= (1<<COM3A1);
  TCCR3B |= (1<<CS31)|(1<<CS30);
  TIMSK3 |= (1<<TOIE3);

  
}*/


void readIR() {
    resultR = fRight.readSensor();
    resultL = fLeft.readSensor();
    resultRFront = rFront.readSensor();
    resultRBack = rBack.readSensor();
}



void setYawReference() {
  //referenceHeading = yawSensor.readOrientation();
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


void displayMessage(String message, float d1, float d2, float d3) {
  //SerialCom->print(message);
  SerialCom->print(d1);
  SerialCom->print(",");
  SerialCom->print(d2);
  SerialCom->print(",");
  SerialCom->println(d3);
}

void getInput() {
  if (SerialCom->available() > 0) {
    String first  = SerialCom->readStringUntil(',');
    SerialCom->read(); //next character is comma, so skip it using this
    String second = SerialCom->readStringUntil(',');
    SerialCom->read();
    String third  = SerialCom->readStringUntil('\n');
    //parse your data here. example:
    //double x = Double.parseDouble(first);
    disp1 = first.toInt();
    disp2 = second.toInt();
    disp3 = third.toInt();
    handler.setGains(disp1, disp2, disp3);
    changeCounter++;
  }
}
