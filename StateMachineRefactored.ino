


//Latest Version for Code Demo Week 7
//Also Testing ATOM Integration and Clean up



#include "Variables.h"
#include "debug.h"
#include "math.h"
#include "FireFighting.h" //Including FireFighting.h

#define pin A12 //Defining the A12 for the phototransistors

float conv = PI/180;
FireFighting fireFightingSystem; //Creating instance

enum state{
  NormalMove = 0,
  Cornering = 1,
  Dodge = 2,
  Firefight = 3,
  Stop = 4
};

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

  fireFightingSystem.firefightingSetup(pin, 45, 3); //Initialising the fireFightingSystem

  sensors.readIRs();

  LEDSetup();

  delay(100);

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
    //semsor reading
    //SerialCom->println( sensors.getRightFront());
    //SerialCom->println( sensors.getRightBack());


    //Ultrasonic Timing
    if (currentTime  - pingTiming >= 150) {
     pingTiming = millis();
     sensors.readUltra();
     //SerialCom->println(sensors.getUltra());
    }
    
    //IR Timing
    if (currentTime - irTiming >= 15) {
      irTiming = millis();
      sensors.readIRs();
    }



    //Orientation Reading
    if (currentTime- mpuTimer >= 21) {
      unsigned long dt = millis() - mpuTimer;
      mpuTimer = millis();
      sensors.readYaw(dt);

    }
    //PID Tuner
   /* if (currentTime - sendTime >= 25) {
      sendTime = millis();
      getInput();
    }*/
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
      digitalWrite(red, HIGH);
      digitalWrite(green, LOW);
      digitalWrite(blue, LOW);
      if (sensors.getUltra() <= (15+13*(numberCorners/4))) {
            digitalWrite(red, HIGH);
    digitalWrite(green, LOW);
    digitalWrite(blue, HIGH);
        boolean corner = false;

        movement.stopMovement();
        //corner = sweep();

        numberCorners++;
        sensors.recalibrateYaw();
        delay(20);
        //if (corner == true) {
          movement.changeState((int)Cornering);
         // servo.write(90);
        //} else {
          //movement.changeState((int)Dodge);
       // }
      }
      break;
    case(Cornering):
    digitalWrite(red, LOW);
    digitalWrite(green, HIGH);
    digitalWrite(blue, LOW);
      if(sensors.getYaw() > 90) {
        movement.changeState((int)NormalMove);
      }
      break;
    case(Dodge):
        digitalWrite(red, LOW);
       digitalWrite(green, LOW);
       digitalWrite(blue, HIGH);
      break;
    case(Firefight):
      fireFightingSystem.fireFight(); //main fireFight function
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

}
