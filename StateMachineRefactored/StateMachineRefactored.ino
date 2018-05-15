


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

  if (currentTime - decisionTimer >= 180) {
      decisionTimer = currentTime;
      decisionMaking();
  }
  


  FeedBack();
  if (currentTime - controlTimer >= 200) {
    controlTimer = currentTime;
    movement.runCurrentState(x, y, z, xFeedBack, yFeedBack, zFeedBack, true, sensors.getYaw(), numberCorners);
  }//movement.runCurrentState(x, y, z, xFeedBack, yFeedBack, zFeedBack, true, sensors.getYaw(), numberCorners)



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
     SerialCom->print(sensors.getUltra());
     SerialCom->print(",");
     SerialCom->print(xDistanceSpiral.getDistance(sensors.getUltra()));
     SerialCom->print(",");
     SerialCom->println(xDistanceSpiral.getReference());
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
          if (within(fabs(xDistanceSpiral.getDistance(sensors.getUltra())), (120 - (15+13*(numberCorners/4))), 5)) {
            digitalWrite(red, HIGH);
            digitalWrite(green, LOW);
            digitalWrite(blue, HIGH);
            boolean obstacle = true;
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
      if(sensors.getYaw() > 90) {
          movement.changeState(4);
          delay(100);
          movement.changeState((int)NormalMove);
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
            dodgeDirection =(int)LEFT;
            movement.changeDodgeMode(1);
            prevL = sensors.getFrontLeft();
            prevR = sensors.getFrontRight();
            dodgeFSM = SIDEWAYS;
            yDistance.restartDistance(sensors.getRightBack());

            break;

         case (SIDEWAYS):
            if (sensors.getFrontRight() - prevR > 10) {
              movement.changeDodgeMode(2);
              dodgeFSM = FORWARD;
              prevB = sensors.getRightBack();
              //SerialCom->println("Done Sideways");
              sensors.readUltra();
              xDistance.restartDistance(sensors.getUltra());
              dodgeRef = yDistance.getDistance(sensors.getRightBack());
            }
            break;
         case (FORWARD):
         if(fabs(xDistance.getDistance(sensors.getUltra()))>30)
         {
           dodgeFSM = RETURN;
           movement.changeDodgeMode(0);
           yDistance.restartDistance(sensors.getRightBack());
         }
            break;
         case (RETURN):
           if(fabs(yDistance.getDistance(sensors.getRightBack()))>dodgeRef-1){
               movement.changeState((int)NormalMove);
               yDistance.restartDistance(sensors.getRightBack());
               //xDistanceSpiral.setReferenceDistance(sensors.getUltra());

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

void FeedBack() {
  
  if (movement.getState() == (int)NormalMove) {
  
    x = 17 + (13*(numberCorners/4)) ;
    y = 5;
    z = 0;
    xFeedBack = (sensors.getRightBack() + sensors.getRightFront())/2;
    //yFeedBack = fabs(xDistanceSpiral.getDistance(sensors.getUltra()));
    zFeedBack = atan2(sensors.getRightFront() - sensors.getRightBack(), 12) * (180/ PI);
  } else if (movement.getState() == (int)Cornering) {
    x = 0;
    y = 0;
    z = 90;
    xFeedBack = (sensors.getRightBack() + sensors.getRightFront())/2;
    yFeedBack = fabs(xDistanceSpiral.getDistance(sensors.getUltra()));
    zFeedBack = sensors.getYaw();
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
  boolean ultra = within(sensors.getUltra(), 15, 5);
  boolean left = within(sensors.getFrontLeft(), 12, 5) /*&& within(sensors.getFrontLeft(), sensors.getUltra(), 5)*/;
  boolean right = within(sensors.getFrontRight(), 12, 5) /*&& within(sensors.getFrontRight(), sensors.getUltra(), 5)*/;
      /*SerialCom->print(ultra);
      SerialCom->print(", ");
      SerialCom->print(left);
      SerialCom->print(", ");
      SerialCom->println(right);*/
  if ((ultra || left || right)&&((!ultra) || (!left) || (!right))) {
    //Wall
    return true;
  } else {
    //
    return false;
  }
}

