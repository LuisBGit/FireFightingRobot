
#include "battery.h"
#include "motionHandler.h"
#include <FaBo9Axis_MPU9250.h> 
#include "MadgwickAHRS.h"
#include "IRShortRange.h"
#include "IRLongRange.h"
#include <NewPing.h>

//****************************************************************PINS*************************************************************

const byte left_front = 46;
const byte left_rear = 47;
const byte right_rear = 50;
const byte right_front = 51;

int trig = 48;
int echo = 49;

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

 if (sonarReading <= 15 && resultR <= 15 && resultL <= 15 && currentTraversal != Cornering) {
    currentTraversal = Cornering;
 }



}



void setup() {
  SerialCom = &Serial1;  
  SerialCom->begin(115200);
  //SerialCom->println("Starting......");
  
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
  SerialCom->println("START");
  SerialCom->println("Setting up locomotion");
  handler.setupHandler(left_front, left_rear, right_rear, right_front);
  //SerialCom->println("MPU");
  fabo.begin();
  fRight.setupIR(4);
  fLeft.setupIR(5);
  rFront.setupIR(6);
  rBack.setupIR(7);
  setupPins();
  currentTraversal = NormalMove;
  //sample();
  return RUNNING;
  
}


STATE runCycle() {
  float startTest = millis();
  static unsigned long previous_millis; 
  handler.serialReceive(SerialCom);

  
  
  if (millis() % 3 == 0) {

    resultR = fRight.readSensor();
    resultL = fLeft.readSensor();
    resultRFront = rFront.readSensor();
    resultRBack = rBack.readSensor();


  }

  if (millis() % 2 == 0) {
   SerialCom->println(sonar.ping_cm());
   sonar.ping_cm();

  }

  switch (currentTraversal) {
     case NormalMove:
          handler.moveHandler(0, 5, 0);
          break;
     case Cornering:
          handler.moveHandler(0, 0, 30);
          if (resultRFront <= 15 && resultRBack <= 15 && resultR >= 15 && resultL >= 15) {
            currentTraversal = NormalMove;
          }
          break;
     case Dodge:

          break;
    
  }
  

  if (millis() % 500 == 0) {
    if (is_battery_voltage_OK() == false) {
      return STOPPED;
    }
  }
  tick++;

  SerialCom->println(millis() - startTest);
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
  sei();
  
}

void magnetometerTest() {

  float roll, pitch, heading;
  fabo.readAccelXYZ(&ax, &ay, &az);
  fabo.readGyroXYZ(&gx, &gy, &gz);
  fabo.readGyroXYZ(&mx, &my, &mz);
  filter.updateIMU(gx - gxOffset, gy - gyOffset, gz - gzOffset, ax - axOffset, ay - ayOffset, az - azOffset);
  roll = filter.getRoll();
  pitch = filter.getPitch();
  heading = filter.getYaw();
  SerialCom->print(mx);
  SerialCom->print(",");
  SerialCom->print(my);
  SerialCom->print(",");
  SerialCom->print(mz);
  SerialCom->print(";");

}


