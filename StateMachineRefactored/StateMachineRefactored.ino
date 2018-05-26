


//Latest Version for Code Demo Week 7
//Also Testing ATOM Integration and Clean up



#include "Variables.h"
#include "FireFighting.h"
#include "math.h"
#define pin A8

FireFighting fireFightingSystem;


float conv = PI/180;
float dodgeRef = 0;
int dodgeBuffer = 0;
distanceCalcV2 xDistance;
distanceCalcV2 yDistance;
distanceCalcV2 xDistanceSpiral;
int normalMoveState = 0;
int sidePass = 0;
bool forceKill = false;
//************************************************************

int numberCorners = 0;

void setup() {
  SerialCom = &Serial1;
  SerialCom->begin(115200);
  servo.attach(servoPin);
  servo.write(45);
  SerialCom->println("setup");
  pinMode(45, OUTPUT);
  digitalWrite(45, LOW);
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
  servo.write(88);
  delay(100);
  xDistanceSpiral.restartDistance(sensors.getUltra());
  xDistance.restartDistance(sensors.getUltra());
  yDistance.restartDistance(sensors.getRightFront());
  startUp();
  return RUNNING;
}

void startUp() {

  //Function variables all initialised here
  boolean wallReached = false;
  boolean ultraMatching = false;
  boolean leftMatching = false;
  boolean rightMatching =false;
  boolean spin = true; //initialised to true to begin with
  boolean driveLeft = false;
  boolean driveRight = false;
  boolean driveCentre = false; 
  unsigned long startTime = 0;
   
  while(spin == true){
     //Spin a lower speed rate than the standard speed of a normal speed
     movement.slowSpin(15);
     //Read the sensors
     sensors.readUltra();
     sensors.readIRs();

     //Sensor Reading
     float ultra = sensors.getUltra();
     float left = sensors.getFrontLeft();
     float right = sensors.getFrontRight();

     //Variables that whether the robot has driven close enough to the wall or not
     boolean driveLeft = (left > 10) && (spin == false);
     boolean driveRight = (right > 10) && (spin == false);
     boolean driveCentre = (ultra > 10) && (spin == false);
      
     ultraMatching = (ultra >= left - 3) && (ultra <= left + 3) && (ultra >= right - 3) && (ultra <= right + 3);
     leftMatching = (left >= ultra - 3) && (left <= ultra + 3) && (left >= right - 3) && (left <= right + 3);
     rightMatching = (right >= ultra - 3) && (right <= ultra + 3) && (right >= left - 3) && (right <= left + 3);
     
     Serial1.print("Readings: ");
     Serial1.print(left);
     Serial1.print(":");
     Serial1.print(ultra);
     Serial1.print(":");
     Serial1.println(right);
     Serial1.print("Checks: ");
     Serial1.print(driveLeft);
     Serial1.print(":");
     Serial1.print(driveCentre);
     Serial1.print(":");
     Serial1.println(driveRight);

     if(ultraMatching && leftMatching && rightMatching){
        Serial1.println("Found Wall!");
        spin = false; //turn the spin value off to not re-enter the loop
        movement.stopMovement();  
     }
     delay(100);
  }

  if(wallReached == false){
      while((driveLeft && driveRight && driveCentre) && (spin == false)){
        movement.slowForward(15);
      }
      movement.stopMovement();
      wallReached = true;
  } else if(wallReached == true){ //if the wall has been reached, then commence the correction spin
          unsigned long correctionSpin = 0;  
          movement.cornering(0);
          delay(1000);
          while (!(within(sensors.getRightFront(), sensors.getRightBack(), 20))) {
            currentTime = millis();
             if (currentTime- correctionSpin >= 16) {
                unsigned long dt = millis() - correctionSpin;
                correctionSpin = currentTime;
                sensors.readIRs();
              }
          }
          movement.stopMovement();
  }

/*
      //Object detection  
      objectLeft = (left < 10) && (spin == false);
      objectRight = (right < 10) && (spin == false);
      objectCentre = (ultra < 10) && (spin == false);
      
      if(objectLeft||objectRight||objectCentre){
        movement.stopMovement();    
      }
      currentTime = millis();
    
    if(currentTime - startTime >= 6000 && spin == true) {
      Serial1.println(currentTime);
      spin = false;
      startTime = currentTime;
      movement.startupStraight();  
    } else if (currentTime - startTime >= 8000 && spin == true) {
      Serial1.println(currentTime);
      spin = true;
      startTime = currentTime;
      movement.cornering(0);
    }




    if(ultraMatching && leftMatching && rightMatching){
      Serial1.println("Wall Found");
      movement.stopMovement();  
      
      if(wallCheck() == true){        
          unsigned long correctionSpin = 0;  
          movement.cornering(0);
          delay(1000);
          while (!(within(sensors.getRightFront(), sensors.getRightBack(), 20))) {
            currentTime = millis();

             if (currentTime- correctionSpin >= 16) {
                unsigned long dt = millis() - correctionSpin;
                
                correctionSpin = currentTime;
                sensors.readIRs();
              }
          }
          movement.stopMovement();
          break;
      }

      
    } */

   delay(100);
}

STATE runCycle() {

  currentTime = millis();


  systemTiming();


  if (currentTime - decisionTiming >= 18) {
    decisionTiming = currentTime;
    decisionMaking();
  }



  /*if (currentTime - motorTiming >= 25 && forceKill == false) {
    motorTiming = currentTime;
    movement.runCurrentState(sensors.getFrontRight(), sensors.getFrontLeft(), sensors.getRightFront(), sensors.getRightBack(), sensors.getYaw(), numberCorners);
  }*/


  if (batterySafetyTrigger == true) {
    return STOPPED;
  }
  if(numberCorners == 13){
    return STOPPED;
  }
  return RUNNING;

}


void systemTiming() {
    if (currentTime  - pingTiming >= 115) {
     pingTiming = currentTime;
     sensors.readUltra();
     SerialCom->println(sensors.getUltra());
     
      //SerialCom->println(xDistanceSpiral.getDistance(sensors.getUltra()));
      if(sensors.getUltra() <= 20 && movement.getState() == (int)NormalMove){
      movement.stopMovement();
      }
    }
    //IR Timing
    if (currentTime - irTiming >= 5) {
        irTiming = currentTime;
        sensors.readIRs();
        if((sensors.getFrontLeft() <= 15 || sensors.getFrontRight() <= 15) && movement.getState() == (int)NormalMove){
        movement.stopMovement();
      }
    }
      
    //Orientation Reading
    if (currentTime- mpuTimer >= 16) {
      unsigned long dt = millis() - mpuTimer;
      mpuTimer = currentTime;
      sensors.readYaw(dt);
    }
}



void decisionMaking() {

  switch(movement.getState()) {
    case(NormalMove):
         if (((sensors.getFrontLeft() <= 12 ) || (sensors.getFrontRight() <= 12) || (sensors.getUltra() <=20)) ){
              movement.stopMovement();

              forceKill = true;
              sensors.readUltra();
              sensors.readIRs();
              servo.write(88);
              if (wallCheck() == false) {
                movement.changeState((int)Firefight);
               } else {
                movement.stopMovement();
                numberCorners++;
                sensors.recalibrateYaw();
                delay(20);
                movement.changeState((int)Cornering);

               }
               forceKill = false;
          }

      break;
    case(Cornering):
      forceKill = false;
      if(sensors.getYaw() > 90) {
          movement.changeState(4);
          delay(100);
          movement.changeState((int)NormalMove);
          xDistanceSpiral.restartDistance(sensors.getUltra());
          if(numberCorners >4){
            xDistanceSpiral.restartDistance(sensors.getUltra());
          }
      }
      break;
    case(Dodge):
       Serial1.println("Dodging Entered");
       switch (dodgeFSM) {
         case (START):
            dodgeDirection =(int)LEFT;
            sidePass = 0;
            movement.changeDodgeMode(3);
            prevL = sensors.getFrontLeft();
            prevR = sensors.getFrontRight();
            yDistance.restartDistance(sensors.getRightBack());
            xDistanceSpiral.setReferenceDistance(sensors.getUltra());
            dodgeBuffer++;
            if(dodgeBuffer>35){
              dodgeFSM = SIDEWAYS;
              movement.changeDodgeMode(1);
              dodgeBuffer = 0;
            }
            break;

         case (SIDEWAYS):
           // SerialCom->println("Sideways");
            if (sensors.getFrontRight() - prevR > 10 || sidePass ==1) {
              //SerialCom->println("check thing");
              sidePass = 1;
              dodgeBuffer++;
              movement.changeDodgeMode(3);
              prevB = sensors.getRightBack();
              //SerialCom->println("Done Sideways");
              xDistance.restartDistance(sensors.getUltra());
              if(dodgeBuffer>35){
                dodgeFSM = FORWARD;
                dodgeRef = yDistance.getDistance(sensors.getRightBack());
                xDistanceSpiral.setOnlyReference(sensors.getUltra());
                movement.changeDodgeMode(2);
                dodgeBuffer = 0;
                sidePass=0;
              }

            }
            break;
         case (FORWARD):
         //SerialCom->println("Forward");
         if(fabs(xDistance.getDistance(sensors.getUltra()))>33.5|| sidePass ==1)
         {
           movement.changeDodgeMode(3);
           dodgeBuffer++;
           sidePass=1;
           if(dodgeBuffer>35){
              dodgeFSM = RETURN;
              movement.changeDodgeMode(0);
              yDistance.restartDistance(sensors.getRightBack());
              xDistanceSpiral.setReferenceDistance(sensors.getUltra());
              dodgeBuffer=0;
              sidePass=0;
           }
          }else if (sensors.getUltra() <= 15) {
              movement.stopMovement();
                forceKill = true;
                numberCorners++;
                sensors.recalibrateYaw();
                delay(20);
                movement.changeDodgeMode(3);
                movement.changeState((int)Cornering);
                dodgeFSM = START;

         }
          break;
         case (RETURN):
           if(fabs(yDistance.getDistance(sensors.getRightBack()))>dodgeRef-1 || sidePass==1){
               movement.changeState((int)NormalMove);
               yDistance.restartDistance(sensors.getRightBack());
               xDistanceSpiral.setOnlyReference(sensors.getUltra());
               movement.changeDodgeMode(3);
               dodgeBuffer++;
               sidePass=1;
               if(dodgeBuffer>35){
                  movement.changeState((int)NormalMove);
                  yDistance.restartDistance(sensors.getRightBack());
                  xDistanceSpiral.setOnlyReference(sensors.getUltra());
                  dodgeBuffer=0;
                  sidePass=0;
                  dodgeFSM = START;
              }
           }
            break;
       }
      prevR = sensors.getFrontRight();
      prevL = sensors.getFrontLeft();
      prevB = sensors.getRightBack();
      break;
    case(Firefight):
    Serial1.println("Entering FF");
      fireFightingSystem.fireFight();
    Serial1.println("Leaving FF");
      dodgeFSM = START;
      movement.changeState((int)Dodge);
      break;
    case(Stop):
      break;
  }
    movement.runCurrentState(sensors.getFrontRight(), sensors.getFrontLeft(), sensors.getRightFront(), sensors.getRightBack(), sensors.getYaw(), (numberCorners% 2 ==0) ? numberCorners : 0);
}



boolean finishedLength() {
  if(within(fabs(xDistanceSpiral.getDistance(sensors.getUltra())), (120 - (15+13*(numberCorners/4))), 5)) {
    SerialCom->println("Length finished");
    return true;
  } else {
    SerialCom->println("Not Done");
    return false;
  }
}


boolean wallCheck() {
  boolean ultra = sensors.getUltra() <= 20;
  boolean left = within(sensors.getFrontLeft(), sensors.getUltra(), 75);
  boolean right = within(sensors.getFrontRight(),sensors.getUltra(), 75);
     SerialCom->print(left);
     SerialCom->print(", ");
     SerialCom->print(ultra);
     SerialCom->print(", ");
     SerialCom->println(right);
  if (ultra && left && right) {
    SerialCom->println("WALL");
     SerialCom->print(sensors.getFrontLeft());
     SerialCom->print(", ");
     SerialCom->print(sensors.getUltra());
     SerialCom->print(", ");
     SerialCom->println(sensors.getFrontRight());
    return true;
  } else {
    SerialCom->println("NO WALL");
         SerialCom->print(sensors.getFrontLeft());
     SerialCom->print(", ");
     SerialCom->print(sensors.getUltra());
     SerialCom->print(", ");
     SerialCom->println(sensors.getFrontRight());
    return false;
  }
}


/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////CODE UNDERNEATH ESSENTIAL BUT NOT LOOKED AT ////////////////////////////////////////////////////////////////////////////////////////////////////////*/
bool batterySafetyTrigger() {
  if (currentTime - batteryTiming >=  400) {
    batteryTiming = millis();
    if (is_battery_voltage_OK() == false) {
      return true;
    }
  }
}


///BATTERY SAFETY SECTION
STATE stopping() {
  static byte counter_lipo_voltage_ok;
  static unsigned long previous_millis;

  //movement.disableMovement();
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

boolean within(float value, float compare, float percent) {
  if (value >= (compare - (percent * compare/100)) && value <= (compare + (percent * compare/100))) {
    return true;
  } else {
    return false;
  }
}
