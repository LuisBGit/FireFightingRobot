


//Latest Version for Code Demo Week 7
//Also Testing ATOM Integration and Clean up



#include "Variables.h"
#include "debug.h"
#include "math.h"
#include "FireFighting.h" //Including FireFighting.h
#define pin A12

FireFighting fireFightingSystem;

float conv = PI/180;
float dodgeRef = 0;
distanceCalcV2 xDistance;
distanceCalcV2 yDistance;
//************************************************************

int numberCorners = 0;

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

  fireFightingSystem.firefightingSetup(pin, 45, 3, servo); //Initialising the fireFightingSystem
  
  movement.setupMovement();

  sensors.setupSensors();

  sensors.readIRs();

  LEDSetup();

  delay(100);
  xDistance.restartDistance(sensors.getUltra());
  yDistance.restartDistance(sensors.getRightFront());
  return RUNNING;
}


STATE runCycle() {

  currentTime = millis();


  systemTiming();

  decisionMaking();
  movement.runCurrentState(sensors.getFrontRight(), sensors.getFrontLeft(), sensors.getRightFront(), sensors.getRightBack(), sensors.getYaw(), numberCorners);



  if (batterySafetyTrigger == true) {
    return STOPPED;
  }
  if(numberCorners == 13){
    return STOPPED;
  }
  return RUNNING;

}


void systemTiming() {
    if (currentTime  - pingTiming >= 100) {
     pingTiming = millis();
     sensors.readUltra();
     SerialCom->println(sensors.getUltra());
     //SerialCom->println(xDistance.getDistance(sensors.getUltra()));
    }

    //IR Timing
    if (currentTime - irTiming >= 10) {
      irTiming = millis();
      sensors.readIRs();
     
    }



    //Orientation Reading
    if (currentTime- mpuTimer >= 21) {
      unsigned long dt = millis() - mpuTimer;
      mpuTimer = millis();
      sensors.readYaw(dt);

    }
}


///BATTERY SAFETY SECTION
STATE stopping() {
  static byte counter_lipo_voltage_ok;
  static unsigned long previous_millis;

  movement.disableMovement();
//  slow_flash_LED_builtin();
  if (millis() - previous_millis > 500) { //print massage every 500ms
    previous_millis = millis();
    if (is_battery_voltage_OK()) {
      counter_lipo_voltage_ok++;
      if (counter_lipo_voltage_ok > 10) { //Making sure lipo voltage is stable
        counter_lipo_voltage_ok = 0;
        //movement.enableMovement();
        return RUNNING;
      }
    } else counter_lipo_voltage_ok = 0;
  }
  return STOPPED;
}



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


void decisionMaking() {

  switch(movement.getState()) {
    case(NormalMove):
      //digitalWrite(red, HIGH);
      //digitalWrite(green, LOW);
      //digitalWrite(blue, LOW);
      if (sensors.getUltra() <= (15+13*(numberCorners/4))) {
            //digitalWrite(red, HIGH);
            //digitalWrite(green, LOW);
            //digitalWrite(blue, HIGH);
        boolean corner = false;

        movement.stopMovement();
        corner = sweep();

        numberCorners++;
        sensors.recalibrateYaw();
        delay(20);
        if (corner == true) {
          movement.changeState((int)Cornering);
          servo.write(90);
        } else {
          distanceToMove = ((sensors.getRightFront() + sensors.getRightBack())/ 2) + 12;
          movement.changeState((int)Firefight);
        }
      }
      break;
    case(Cornering):
      //digitalWrite(red, LOW);
      //digitalWrite(green, HIGH);
      //digitalWrite(blue, LOW);
      if(sensors.getYaw() > 90) {
        movement.changeState((int)NormalMove);
      }
      break;
    case(Dodge):
       //digitalWrite(red, LOW);
       //digitalWrite(green, LOW);
       //digitalWrite(blue, HIGH);
       switch (dodgeFSM) {
         case (START):
            /*if (sensors.getRightBack() < 20 || sensors.getRightFront() < 20) {
              SerialCom->println("Change Direction");
              dodgeDirection = (int)LEFT;
              movement.changeDodgeMode(1);
            } else {
              dodgeDirection =(int)RIGHT;
              movement.changeDodgeMode(0);
            }*/
            dodgeDirection =(int)LEFT;
            movement.changeDodgeMode(1);
            prevL = sensors.getFrontLeft();
            prevR = sensors.getFrontRight();
            dodgeFSM = SIDEWAYS;
            yDistance.restartDistance(sensors.getRightBack());
            movement.changeState((int)NormalMove);
            break;

         case (SIDEWAYS):
            /*if (dodgeDirection == (int)RIGHT) {
              if (sensors.getFrontLeft() - prevL > 10) {
                movement.changeDodgeMode(2);
                dodgeFSM = FORWARD;
              }
            } else {
               if (sensors.getFrontRight() - prevR > 10) {
                movement.changeDodgeMode(2);
                dodgeFSM = FORWARD;
              }
            }*/

            if (sensors.getFrontRight() - prevR > 10) {
              movement.changeDodgeMode(2);
              dodgeFSM = FORWARD;
              prevB = sensors.getRightBack();
              //SerialCom->println("Done Sideways");
              xDistance.restartDistance(sensors.getUltra());
              dodgeRef = yDistance.getDistance(sensors.getRightBack());
              
            }
            break;
         case (FORWARD):
         if(fabs(xDistance.getDistance(sensors.getUltra()))>30)
         {
           dodgeFSM = RETURN;
           movement.changeDodgeMode(0);
           //SerialCom->println("Done Forward");
           //SerialCom->println(sensors.getRightBack());
           //SerialCom->println(prevB);
           yDistance.restartDistance(sensors.getRightBack());
         }
         //SerialCom->println(xDistance.getDistance(sensors.getUltra()));
            /*if (sensors.getRightBack() - prevB > 0) {

              if (dodgeDirection == 0) {
                movement.changeDodgeMode(1);
              } else {
                movement.changeDodgeMode(0);
              }
              dodgeFSM = RETURN;
            }*/


            break;
         case (RETURN):
           if(fabs(yDistance.getDistance(sensors.getRightBack()))>dodgeRef-1){
               movement.changeState((int)Stop);
               yDistance.restartDistance(sensors.getRightBack());
               
           }


            break;
       }
      prevR = sensors.getFrontRight();
      prevL = sensors.getFrontLeft();
      prevB = sensors.getRightBack();
      break;
    case(Firefight):
      digitalWrite(red, LOW);
      digitalWrite(green, LOW);
      digitalWrite(blue, HIGH);
      fireFightingSystem.fireFight(); //main fireFight function
      movement.changeState((int)Dodge);
      break;
    case(Stop):
      break;

  }
}


boolean sweep() {
  float ref = sensors.getUltra();
  float lowest = ref;
  float highest = ref;

  for (int i = 45; i <= 135; i+=5) {
    servo.write(i);
    sensors.readUltra();
    float point;
    if (i != 90) {
      point = sensors.getUltra() * cos((i - 90) * conv);
    } else {
      point = sensors.getUltra();
    }

    if (point > highest) {
      highest = point;
    } else if (point < lowest) {
      lowest = point;
    }

    delay(200);

  }
  SerialCom->print(ref - lowest);
  SerialCom->print(",");
  SerialCom->println(highest - ref);
  if (((ref - lowest) > 15) || ((highest - ref) > 15)) {
    SerialCom->println("Obstacle");
    return false;
  } else {
    SerialCom->println("Wall");
    return true;
  }
  servo.write(90);
}
