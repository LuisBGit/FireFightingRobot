


//Latest Version for Code Demo Week 7
//Also Testing ATOM Integration and Clean up



#include "Variables.h"
#include "debug.h"
#include "math.h"



float conv = PI/180;
float dodgeRef = 0;
distanceCalcV2 xDistance;
distanceCalcV2 yDistance;
distanceCalcV2 xDistanceSpiral;
int normalMoveState = 0;
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

  movement.setupMovement();

  sensors.setupSensors();

  sensors.readIRs();

  LEDSetup();

  delay(100);
  xDistanceSpiral.restartDistance(sensors.getUltra());
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
    if (currentTime  - pingTiming >= 150) {
     pingTiming = millis();
     sensors.readUltra();
     /*
     SerialCom->print(xDistanceSpiral.getDistance(sensors.getUltra()));
     SerialCom->print(", ");
     SerialCom->print(xDistanceSpiral.getReference() - 15);
     SerialCom->print(", ");
     SerialCom->println(sensors.getUltra()); */
     //SerialCom->println(sensors.getUltra());
     //SerialCom->println(xDistance.getDistance(sensors.getUltra()));
    }

    //IR Timing
    if (currentTime - irTiming >= 5) {
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
    digitalWrite(red, HIGH);
    digitalWrite(green, LOW);
    digitalWrite(blue, LOW);
    case(NormalMove):
          if (ObstacleDetection() == true && !finishedLength()) {
              distanceToMove = ((sensors.getRightFront() + sensors.getRightBack())/ 2) + 12;
              movement.changeState((int)Dodge);
        
          }else if(finishedLength()) {
            digitalWrite(red, HIGH);
            digitalWrite(green, LOW);
            digitalWrite(blue, HIGH);
            boolean corner = true;
            movement.stopMovement();
            numberCorners++;
            sensors.recalibrateYaw();
            delay(20);
            movement.changeState((int)Cornering);
          }

      break;
    case(Cornering):
      digitalWrite(red, LOW);
      digitalWrite(green, HIGH);
      digitalWrite(blue, LOW);
      //SerialCom->print(sensors.getYaw());
      //SerialCom->print("' ");
      //SerialCom->println(sensors.getGyro());
      if(sensors.getYaw() > 90) {
          movement.changeState(4);
          delay(100);
          movement.changeState((int)NormalMove);
          //numberCorners++;
          xDistanceSpiral.restartDistance(sensors.getUltra());
          if(numberCorners >4){
            //normalMoveState = 1;
            xDistanceSpiral.restartDistance(sensors.getUltra());
          }
      }
      break;
    case(Dodge):
       digitalWrite(red, LOW);
       digitalWrite(green, LOW);
       digitalWrite(blue, HIGH);
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
               movement.changeState((int)NormalMove);
               yDistance.restartDistance(sensors.getRightBack());

           }


            break;
       }
      prevR = sensors.getFrontRight();
      prevL = sensors.getFrontLeft();
      prevB = sensors.getRightBack();
      break;
    case(Firefight):
      break;
    case(Stop):
      break;

  }
}

boolean within(float value, float compare, float percent) {
  if (value >= (compare - (percent * compare/100)) && value <= (compare + (percent * compare/100))) {
    return true;
  } else {
    return false;
  }
}
boolean ObstacleDetection() {
  digitalWrite(red, LOW);
  digitalWrite(green, LOW);
  digitalWrite(blue, LOW); 
  //Obtain sensor readings from the ultrasonic, left and right IRs
  float ultra = sensors.getUltra();
  float left = sensors.getFrontLeft(); 
  float right = sensors.getFrontRight(); 

  SerialCom->print(sensors.getFrontLeft());
  SerialCom->print(", ");
  SerialCom->print(sensors.getUltra());
  SerialCom->print(", ");
  SerialCom->println(sensors.getFrontRight());
    
  //Object Detection Scenarios
  //Obstacle-left scenarios - if the leftmost IR sensor obtains a reading that is either:
  //(a) Both less than the ultrasonic (which is front-facing) and the right IR 
  //(b) Close to the ultrasonic but very different to the right IR
  //Then an object is around the left region
  boolean obstacleFarLeft = (left < ultra) && (left < right); //the object will ONLY touch the left IR
  boolean obstacleMidLeft = (left < right) && (ultra < right); //the object is interfering with the left IR and centre ultrasonic, a threshold is introduced for the ultrasonic
  //Obstacle-right scenarios - if the rightmost IR sensor obtains a reading that is either:
  //(a) Both less than the ultrasonic (which is front-facing) and the left IR 
  //(b) Close to the ultrasonic but very different to the left IR
  //Then an object is around the right region  
  boolean obstacleFarRight = (right < ultra) && (right < left); //the object will ONLY touch the left IR
  boolean obstacleMidRight = (right < left) && (ultra < left); //the object is interfering with the left IR and centre ultrasonic, a threshold is introduced for the ultrasonic
  //Wall Detection Scenarios - Uses a tolerance to check whether the readings indicate a wall or not
  boolean wallDetectedLeftCheck = ((left > 0.9*ultra) && (left < 1.1*ultra)) && ((left > 0.9*right) && (left < 1.1*right));
  boolean wallDetectedRightCheck = ((right > 0.9*ultra) && (right < 1.1*ultra)) && ((right > 0.9*left) && (right < 1.1*left));
  boolean wallDetectedUltraCheck = ((ultra > 0.9*left) && (ultra < 1.1*left)) && ((ultra > 0.9*right) && (ultra < 1.1*right));
  if (obstacleFarRight || obstacleMidRight || obstacleFarLeft || obstacleMidLeft){
    //Right Region
    // BLUE
    digitalWrite(red, LOW);
    digitalWrite(green, LOW);
    digitalWrite(blue, HIGH);  
    return true;
  } else if (wallDetectedLeftCheck && wallDetectedRightCheck && wallDetectedUltraCheck){
    //RED & BLUE
    digitalWrite(red, LOW);
    digitalWrite(green, LOW);
    digitalWrite(blue, HIGH);  
    return false;
  } else {
    digitalWrite(red, HIGH);
    digitalWrite(green, LOW);
    digitalWrite(blue, HIGH); 
    return false;
  }
}

boolean finishedLength() {
  if(within(fabs(xDistanceSpiral.getDistance(sensors.getUltra())), (120 - (15+13*(numberCorners/4))), 5)) {
    return true;
  } else {
    return false;
  }
}

