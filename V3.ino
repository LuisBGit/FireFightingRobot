
#include "battery.h"
#include "motionHandler.h"
#include <FaBo9Axis_MPU9250.h> 
#include "MadgwickAHRS.h"
#include "IRShortRange.h"

//****************************************************************PINS*************************************************************

const byte left_front = 46;
const byte left_rear = 47;
const byte right_rear = 50;
const byte right_front = 51;

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

float ax, ay, az;
float gx, gy, gz;
float mx, my, mz;


float gzSum, gySum, gxSum, gzOffset, gyOffset, gxOffset;
float azSum, aySum, axSum, azOffset, ayOffset, axOffset;
//***********************************************************Global Variables******************************************************


//*************************************************************ENUMERATION*********************************************************
enum STATE {
  INITIALISING,
  RUNNING,
  STOPPED
};


//*************************************************************ENUMERATION*********************************************************

ISR(TIMER1_OVF_vect) {
 // SerialCom->print("Result: ");
 // float result = fRight.readSensor();
 // SerialCom->println(result);
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
  //SerialCom->println("START");
  //SerialCom->println("Setting up locomotion");
  handler.setupHandler(left_front, left_rear, right_rear, right_front);
  //SerialCom->println("MPU");
  fabo.begin();
  fRight.setupIR(4);
  fLeft.setupIR(5);
  setupPins();
  //sample();
  return RUNNING;
  
}


STATE runCycle() {
  static unsigned long previous_millis; 
  handler.serialReceive(SerialCom);
  if (millis() - previous_millis > 500) {
    
    previous_millis = millis();

     ///magnetometerTest();
     float resultR, resultL;
     resultR = fRight.readSensor();
     resultL = fLeft.readSensor();
     SerialCom->print(resultL);
     SerialCom->print("   ");
     SerialCom->println(resultR);
     
    if (is_battery_voltage_OK() == false) {
      return STOPPED;
    }
  }
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
      //SerialCom->print("Lipo OK Counter:");
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
  TCCR1A |= (1<<COM1A1);
  TCCR1B |= (1<<CS11)|(1<<CS10);
 // TIMSK |= (1<<TOEI1);
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

/*
  SerialCom->print(heading*360);
  SerialCom->print(",");
  SerialCom->print(pitch);
  SerialCom->print(",");
  SerialCom->print(roll); 
  SerialCom->println(",");
  */
/*
  SerialCom->println("Offset value thing");
  SerialCom->print(gx - gxOffset);
  SerialCom->println(",");
  SerialCom->print(gy - gyOffset);
  SerialCom->println(",");
  SerialCom->print(gz - gzOffset);
  SerialCom->println(",");
  SerialCom->print(ax - axOffset);
  SerialCom->println(",");
  SerialCom->print(ay - ayOffset);
  SerialCom->println(",");
  SerialCom->print(az - azOffset);
*/
  
}

/*
void sample() {
  for (int i = 0; i <1000; i++) {
    fabo.readAccelXYZ(&ax, &ay, &az);
    fabo.readGyroXYZ(&gx, &gy, &gz);

    axSum += ax;
    aySum += ay;
    azSum += az;

    gxSum += gx;
    gySum += gy;
    gzSum += gz;

    delay(20);
    
  }

  axOffset = axSum/1000;
  ayOffset = aySum/1000;
  azOffset = azSum/1000;

  gxOffset = gxSum/1000;
  gyOffset = gySum/1000;
  gzOffset = gzSum/1000;


  
}*/





