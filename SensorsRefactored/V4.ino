


//Latest Version for Code Demo Week 7
//Also Testing ATOM Integration and Clean up

#include "Variables.h"
#include "debug.h"


//************************************************************


void setup() {
  SerialCom = &Serial1;
  SerialCom->begin(115200);
  servo.attach(servoPin);
  servo.write(90);
  delay(500);
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

  sensors.setupSensors();

  sensors.readIRs();

  LEDSetup();

  delay(100);
  currentTraversal = NormalMove;
  return RUNNING;

}


STATE runCycle() {

  currentTime = millis();

  if (sensors.getUltra() <= 23 && sensors.getFrontRight() <= 25 && sensors.getFrontLeft() <= 25 && currentTraversal != Cornering) {
    currentTraversal = Cornering;
    handler.stopMotor();

    delay(300);
    sensors.recalibrateYaw();

  }

  systemTiming();


  switch (currentTraversal) {
     case NormalMove:
          digitalWrite(red, HIGH);
          digitalWrite(green, LOW);
          digitalWrite(blue, LOW);
          stateDisplay = 0;
          handler.moveHandler(0, 4, 0,  sensors.getRightFront(),sensors.getRightBack(), 0);
          break;
     case Cornering:
          digitalWrite(red, LOW);
          digitalWrite(green, HIGH);
          digitalWrite(blue, LOW);
          stateDisplay = 1;

          handler.moveHandler(0, 0, 20, 0 , 0, 3);

          if (80<sensors.getYaw() ) {

            handler.stopMotor();
            currentTraversal = NormalMove;
          }

          break;
     case Dodge:

          break;

  }


  if (batterySafetyTrigger == true) {
    return STOPPED;
  }

  return RUNNING;

}


void systemTiming() {

    //IR Timing
    if (currentTime - irTiming >= 10) {
      irTiming = millis();
      sensors.readIRs();

    }

    //Ultrasonic Timing
    if (currentTime  - pingTiming >= 15) {
     pingTiming = millis();
     sensors.readUltra();
    }

    //Orientation Reading
    if (currentTime- mpuTimer >= 20) {
      unsigned long dt = millis() - mpuTimer;
      mpuTimer = millis();
      sensors.readYaw(dt);

    }
/*
    //Display Out
    if (currentTime - displayTiming >= 150) {
      displayTiming = millis();
      displayMessage("PID ", changeCounter, resultRFront - resultRBack, 0);
    }
*/

    //PID Tuner
    if (currentTime - sendTime >= 25) {
      sendTime = millis();
      getInput();
    }
}


///BATTERY SAFETY SECTION
STATE stopping() {
  static byte counter_lipo_voltage_ok;
  static unsigned long previous_millis;

  handler.disableHandler();
//  slow_flash_LED_builtin();
  if (millis() - previous_millis > 500) { //print massage every 500ms
    previous_millis = millis();
    if (is_battery_voltage_OK()) {
      counter_lipo_voltage_ok++;
      if (counter_lipo_voltage_ok > 10) { //Making sure lipo voltage is stable
        counter_lipo_voltage_ok = 0;
        handler.enableHandler();
        return RUNNING;
      }
    } else counter_lipo_voltage_ok = 0;
  }
  return STOPPED;
}


/*
void readIR() {
    resultR = fRight.readSensor();
    resultL = fLeft.readSensor();
    resultRFront = rFront.readSensor();
    resultRBack = rBack.readSensor();
}*/


bool batterySafetyTrigger() {
  if (currentTime - batteryTiming >=  400) {
    batteryTiming = millis();
    if (is_battery_voltage_OK() == false) {
      return true;
    }
  }
}


void LEDSetup() {
  pinMode(red, OUTPUT);
  pinMode(green, OUTPUT);
  pinMode(blue, OUTPUT);

  digitalWrite(red, LOW);
  digitalWrite(green, LOW);
  digitalWrite(blue, LOW);
}
