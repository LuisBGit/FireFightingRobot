


//Latest Version for Code Demo Week 7
//Also Testing ATOM Integration and Clean up



#include "Variables.h"
#include "FireFighting.h"
#include "math.h"
#define pin A8

FireFighting fireFightingSystem;

float prevY = 0;
float conv = PI/180;
float dodgeRef = 0;
int dodgeBuffer = 0;
distanceCalcV2 xDistance;
distanceCalcV2 yDistance;
distanceCalcV2 xDistanceSpiral;
int normalMoveState = 0;
int sidePass = 0;
bool forceKill = false;
bool freshStart = true;
int dodgeTime=0;
int corneringState = 0;
bool cornerSpace = 0;
int cornerTime = 0;
//************************************************************

int numberCorners = 0;

void setup() {
  SerialCom = &Serial1;
  SerialCom->begin(115200);
  //servo.attach(servoPin);
  //servo.write(45);
  //SerialCom->println("setup");
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

  fireFightingSystem.firefightingSetup(pin, 45, 2); //Initialising the fireFightingSystem


  movement.setupMovement();

  sensors.setupSensors();

  sensors.readIRs();
  prevY = sensors.getUltra();
  //servo.write(88);
  delay(100);
  xDistanceSpiral.restartDistance(sensors.getUltra());
  xDistance.restartDistance(sensors.getUltra());
  yDistance.restartDistance(sensors.getRightFront());
  startUp();
  return RUNNING;
}

void startUp() {



  boolean startUp = true;
  while (startUp) {
      currentTime = millis();
      systemTiming();
    
      startRoamCheck();
      safetySwitchCheck();

      Serial1.print(sensors.getRightBack());
      Serial1.print(",");
      Serial1.print(sensors.getRightFront());
      Serial1.print(",");
      Serial1.print(sensors.getFrontRight());
      Serial1.print(",");
      Serial1.println(sensors.getFrontLeft());
     
     switch(startSeq) {
        case spin:
          Serial1.println("SPIN");
          movement.slowSpin(55);
          break;
        case straight:
          Serial1.println("STRAIGHT");
          movement.startupStraight(6);
          break;
        case align:
          Serial1.println("ALIGN");
          movement.slowSpin(25);
          if ((sensors.getRightFront() != 999) && (sensors.getRightBack() != 999) && within(sensors.getRightFront(), sensors.getRightBack(), 5)) {
            movement.stopMovement();
            //startUp = false;
            startSeq = strafeIn;
          }
          break;
        case strafeIn:
        Serial1.println("STRAFEIN");
          movement.strafeStart(3);
          if ((sensors.getRightFront() != 999) && (sensors.getRightBack() != 999) &&  within((sensors.getRightBack() + sensors.getRightFront()) /2, 13, 5)) {
            movement.stopMovement();
            startUp = false;
            break;
          }
          break;
          
        case strafeOut:
        Serial1.println("STRAFEOUT");
          movement.strafeStart(-7);
          delay(200);
          startSeq = straight;
          break;
     }
      
  }
}

void safetySwitchCheck() {
  //Checks if too close to wall
  
  float sideAverage = (sensors.getRightBack() + sensors.getRightFront()) / 2;
  if ((sideAverage < 12) && (startSeq != align) && (startSeq != strafeIn)) {
    startSeq = strafeOut;
  }
}

void startRoamCheck() {
    //Checks if object is a wall or an obstacle
    if (obCheck() && (startSeq != align) && (startSeq != strafeIn)&& (startSeq != strafeOut)) {
     if (wallCheck()) {
        movement.slowSpin(55);
        delay(1000);
        startSeq = align;
      } else {
        startSeq = spin;
      }
   } else if ((startSeq != align) && (startSeq != strafeIn)&& (startSeq != strafeOut)){
     startSeq = straight;
   }
}

boolean obCheck() {
  if ((sensors.getUltra() < 15) || (sensors.getFrontRight() < 12) || (sensors.getFrontLeft() < 12)) {
    return true;
  } else {
    return false;
  }
}
 


//State Ending Bracket


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

  if (currentTime - msgTime >= 200) {
    msgTime = currentTime;
    //sendToPC(1, (sensors.getRightBack() + sensors.getRightFront())/ 2, fabs(sensors.getUltra() - prevY), numberCorners, (int)movement.getState());
  }


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
     prevY = sensors.getUltra();
     sensors.readUltra();
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
              //servo.write(88);
              if (wallCheck() == false) {
                movement.changeState((int)Firefight);
               } else {
                movement.stopMovement();
                numberCorners++;
                sensors.recalibrateYaw();
                delay(20);
                movement.changeState((int)Cornering);
                movement.changeCornerMode(0);

               }
               forceKill = false;
          }

      break;
    case(Cornering):
      forceKill = false;
      prevY = 0;
      SerialCom->println(corneringState);
      switch(corneringState){
        //when you find the corner and spin 180 to check if there are any obstacles
        case(0):
        //whens its done spinning 180
          if(fabs(sensors.getYaw()) > 170) {
              //buffer to give it time to settle
              dodgeBuffer++;
              movement.changeCornerMode(3);
              if(dodgeBuffer>15){
                dodgeBuffer = 0;
                //move to spin 90 CW state
                corneringState = 1;
                sensors.recalibrateYaw();
                delay(20);
                //determines whether there is space to trafe after turn
                if(sensors.getUltra()>23*((numberCorners-1)/4)&& sensors.getFrontRight()>23*((numberCorners-1)/4) &&sensors.getFrontLeft()>23*((numberCorners-1)/4)){
                    cornerSpace = true;
                    
                }
                movement.changeCornerMode(1);
              }
          }
          break;
          //state for turning back 90
        case(1):
          //SerialCom->println(sensors.getYaw());
          //after its done spinning 90
          if(fabs(sensors.getYaw()) > 85) {
            //buffer to let it settle
            dodgeBuffer++;
            movement.changeCornerMode(3);
            if(dodgeBuffer>15){
              sensors.recalibrateYaw();
              delay(20);
              //chnage to realign state
              movement.changeCornerMode(4);
              dodgeBuffer = 0;
              corneringState = 2;
            }
          }
          break;
        case(2):
        //when the irs are less than 1 cm from each other
          if(fabs(sensors.getRightFront()- sensors.getRightBack())<1){
            //if there is space return back to normal move
            if(cornerSpace){
              movement.changeCornerMode(0);
              movement.changeState((int)NormalMove);
              dodgeBuffer=0;
              corneringState = 0;
              cornerSpace = 0;
            }
            //if there isnt change to move forward state
            else{
              cornerTime = millis();
              corneringState = 3;
              movement.changeCornerMode(2);
            }

          }
          break;
        case(3):
          if(millis()-cornerTime>700){
            dodgeBuffer++;
            movement.changeCornerMode(3);
            if(dodgeBuffer>15){
              movement.changeCornerMode(0);
              movement.changeState((int)NormalMove);
              dodgeBuffer=0;
              corneringState = 0;
              cornerSpace = 0;
              
            }
          }
          break;

      }
      /*
      if(sensors.getYaw() > 90) {
          movement.changeState(4);
          delay(100);
          movement.changeState((int)NormalMove);
          xDistanceSpiral.restartDistance(sensors.getUltra());
          if(numberCorners >4){
            xDistanceSpiral.restartDistance(sensors.getUltra());
          }
      }*/
      
      break;
    case(Dodge):
       switch (dodgeFSM) {
         case (START):
            dodgeDirection =(int)LEFT;
            sidePass = 0;
            movement.changeDodgeMode(3);
            prevL = sensors.getFrontLeft();
            prevR = sensors.getFrontRight();
            yDistance.restartDistance(sensors.getRightBack());
            dodgeBuffer++;
            if(dodgeBuffer>25){
              dodgeFSM = SIDEWAYS;
              movement.changeDodgeMode(1);
              dodgeBuffer = 0;
            }
            break;

         case (SIDEWAYS):
           // SerialCom->println("Sideways");
            if (sensors.getFrontRight() - prevR > 10 || sidePass ==1) {
              sidePass = 1;
              dodgeBuffer++;
              movement.changeDodgeMode(3);
              prevB = sensors.getRightBack();
              //SerialCom->println("Done Sideways");
              
              if(dodgeBuffer>25){
                dodgeFSM = FORWARD;
                dodgeRef = yDistance.getDistance(sensors.getRightBack());
                movement.changeDodgeMode(2);
                dodgeBuffer = 0;
                sidePass=0;
                dodgeTime = millis();
              }

            }
            break;
         case (FORWARD):
         //SerialCom->println("Forward");
         if((sensors.getRightBack()-prevR)>5||millis()>(dodgeTime+1800)|| sidePass ==1)
         {
           movement.changeDodgeMode(3);
           dodgeBuffer++;
           sidePass=1;
           if(dodgeBuffer>25){
              dodgeFSM = RETURN;
              movement.changeDodgeMode(0);
              yDistance.restartDistance(sensors.getRightBack());
              dodgeBuffer=0;
              sidePass=0;
           }
          }/*else if (sensors.getUltra() <= 15) {
                movement.stopMovement();
                forceKill = true;
                numberCorners++;
                sensors.recalibrateYaw();
                delay(20);
                movement.changeDodgeMode(3);
                movement.changeState((int)Cornering);
                dodgeFSM = START;

         }*/
          break;
         case (RETURN):
           if(fabs(yDistance.getDistance(sensors.getRightBack()))>dodgeRef-1 || sidePass==1 ){
               movement.changeState((int)NormalMove);
               yDistance.restartDistance(sensors.getRightBack());
               movement.changeDodgeMode(3);
               dodgeBuffer++;
               sidePass=1;
               if(dodgeBuffer>25){
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
    //Serial1.println("Entering FF");
      fireFightingSystem.fireFight();
    //Serial1.println("Leaving FF");
      dodgeFSM = START;
      movement.changeState((int)Dodge);
      break;
    case(Stop):
      break;
  }
    movement.runCurrentState(sensors.getFrontRight(), sensors.getFrontLeft(), sensors.getRightFront(), sensors.getRightBack(), sensors.getYaw(),numberCorners); //(numberCorners% 2 ==0) ? numberCorners : 0);
}


void sendToPC(int startMSG, float x, float y, int turns, int state) {
  Serial1.print(startMSG);
  Serial1.print(",");
  Serial1.print(x);
  Serial1.print(",");
  Serial1.print(y);
  Serial1.print(",");
  Serial1.print(turns);
  Serial1.print(",");
  Serial1.println(state);
}


boolean finishedLength() {
  if(within(fabs(xDistanceSpiral.getDistance(sensors.getUltra())), (120 - (15+13*(numberCorners/4))), 5)) {
    //SerialCom->println("Length finished");
    return true;
  } else {
    //SerialCom->println("Not Done");
    return false;
  }
}


boolean wallCheck() {
  boolean ultra = sensors.getUltra() <= 20;
  boolean left = within(sensors.getFrontLeft(), sensors.getUltra(), 60);
  boolean right = within(sensors.getFrontRight(),sensors.getUltra(), 60);
     /*SerialCom->print(left);
     SerialCom->print(", ");
     SerialCom->print(ultra);
     SerialCom->print(", ");
     SerialCom->println(right);*/
  if (ultra && left && right) {
    /*SerialCom->println("WALL");
     SerialCom->print(sensors.getFrontLeft());
     SerialCom->print(", ");
     SerialCom->print(sensors.getUltra());
     SerialCom->print(", ");
     SerialCom->println(sensors.getFrontRight());*/
    return true;
  } else {
    /*SerialCom->println("NO WALL");
         SerialCom->print(sensors.getFrontLeft());
     SerialCom->print(", ");
     SerialCom->print(sensors.getUltra());
     SerialCom->print(", ");
     SerialCom->println(sensors.getFrontRight());*/
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
