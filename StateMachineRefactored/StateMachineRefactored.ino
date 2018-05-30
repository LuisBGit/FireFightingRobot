


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
unsigned long dodgeTime=0;
int corneringState = 0;
bool cornerSpace = 0;
unsigned long cornerTime = 0;
float spinIn = 0;
int settlingCounter = 0;
int cornerCount = 0;
int dumbBreak = 0;
int yawInput = 0;
//************************************************************
bool cornerStrafe = true;
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

  fireFightingSystem.firefightingSetup(pin, 45, 11); //Initialising the fireFightingSystem


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

  //Serial1.println("STARTUP");

  boolean startUp = true;
  while (startUp) {
      currentTime = millis();
      systemTiming();
    
      startRoamCheck();
      safetySwitchCheck();

      /*Serial1.print(sensors.getRightBack());
      Serial1.print(",");
      Serial1.print(sensors.getRightFront());
      Serial1.print(",");
      Serial1.print(sensors.getFrontRight());
      Serial1.print(",");
      Serial1.println(sensors.getFrontLeft());*/
     
     switch(startSeq) {
        case spin:
          //Serial1.println("SPIN");
          movement.slowSpin(55);
          break;
        case straight:
          //Serial1.println("STRAIGHT");
          movement.startupStraight(6);
          break;
        case align:
          //Serial1.println("ALIGN");
          spinIn = 1.5*(0 - sensors.laserYaw());
          movement.slowSpin(spinIn);
          /*Serial1.print(sensors.laserYaw());
          Serial1.print(",");
          Serial1.println(settlingCounter);*/
          dumbBreak++;
          if ((sensors.getRightFront() != 999) && (sensors.getRightBack() != 999) && within(sensors.laserYaw(),0, 2)) {
             //Serial1.println("Found One");
             
            //movement.stopMovement();
            //startUp = false;
            //startSeq = strafeIn;
            settlingCounter++;
          } else {
            settlingCounter = 0;
          }

          if (dumbBreak > 500) {
            dumbBreak = 0;
            startSeq = straight;
          }
          if (settlingCounter > 100) {
            if (!obCheck()) {
               movement.stopMovement();
               startSeq = strafeIn;
            } else {
              startSeq = straight;
            }

          }
          break;
        case strafeIn:
        //Serial1.println("STRAFEIN");
          movement.strafeStart(3);
          if (((sensors.getRightFront() != 999) && (sensors.getRightBack() != 999) &&  within((sensors.getRightBack() + sensors.getRightFront()) /2, 13, 5)) || dumbTimeout(1600)) {
            movement.stopMovement();
            dumbBreak = 0;
            startUp = false;
            break;
          }
          break;
          
        case strafeOut:
        //Serial1.println("STRAFEOUT");
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
        delay(750);
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

  if (currentTime - msgTime >= 400) {
    msgTime = currentTime;
    sendToPC(1, (sensors.getRightBack() + sensors.getRightFront())/ 2, fabs(sensors.getUltra() - prevY), numberCorners, (int)movement.getState());
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
        /*Serial1.print("RIGHT FRONT: ");
        Serial1.print(sensors.getRightFront());
        Serial1.print(",");
        Serial1.print("RIGHT BACK: ");
        Serial1.println(sensors.getRightBack());*/
        if((sensors.getFrontLeft() <= 15 || sensors.getFrontRight() <= 15) && movement.getState() == (int)NormalMove){
        movement.stopMovement();
      }
    }

    //Orientation Reading
    if (currentTime- mpuTimer >= 16) {
      unsigned long dt = millis() - mpuTimer;
      mpuTimer = currentTime;
      sensors.readYaw(dt);
      //SerialCom->println(sensors.getYaw());
    }
}


bool dumbTimeout(int thresh) {
  dumbBreak++;
  if (dumbBreak > thresh) {
    dumbBreak = 0;
    return true;
  } else {
    return false;
  }
}

void decisionMaking() {

  switch(movement.getState()) {
    case(NormalMove):
        //Serial1.println("Normal");
         if (((sensors.getFrontLeft() <= 9) || (sensors.getFrontRight() <= 9) || (sensors.getUltra() <=17)) ){
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
                cornerStrafe = true;

               }
               forceKill = false;
          }

      break;
    case(Cornering):
      //forceKill = false;
      //Serial1.println("Corner");
      prevY = 0;
      if(numberCorners >4){
        switch(corneringState){

          //when you find the corner and spin 180 to check if there are any obstacles
          case(0): 
          //whens its done spinning 180
            Serial1.println("180 SPIN");
             //180 STATE
            yawInput = 180;

            if(within(sensors.getYaw(), yawInput,5)){
              cornerCount++;
            }
            else{
              cornerCount = 0;
            }
            if(cornerCount>50) {
               //buffer to give it time to settle
                dodgeBuffer++;
                movement.changeCornerMode(3);
                  dodgeBuffer = 0;
                  //move to spin 90 CW state
                  corneringState = 1;
                  sensors.readUltra();
                  delay(20);
                  sensors.recalibrateYaw();
                  //determines whether there is space to trafe after turn
                  if(sensors.getUltra()>23*(int((numberCorners-1)/4))&& sensors.getFrontRight()>23*(int((numberCorners-1)/4)) ){
                      cornerSpace = true;
                  }
                  movement.changeCornerMode(0);
                  yawInput = -90;
                  cornerCount = 0;

            }
            break;
            //state for turning back 90
          case(1):
            Serial1.println("Back to 90");
            //SerialCom->println(sensors.getYaw());
            //after its done spinning 90
            yawInput = -90;
            Serial1.print("Count: ");
            Serial1.print(cornerCount);
            Serial1.print("FeedBack" );
            Serial1.println(sensors.laserYaw());
            if(within(sensors.getYaw(), yawInput,15)){
              cornerCount++;
            }
            else{
              cornerCount = 0;
            }
            if(dumbTimeout(120) | cornerCount > 50) {
              //buffer to let it settle
              dodgeBuffer++;
              movement.stopMovement();
              movement.changeCornerMode(3);
                sensors.recalibrateYaw();
               //chnage to realign state
                movement.changeCornerMode(4);
                dodgeBuffer = 0;
                corneringState = 2;
                cornerCount = 0;
            }
            break;
          case(2):
          //Reallign
          Serial1.println("REALIGN and To Normal");
          //when the irs are less than 1 cm from each other
            if(fabs(sensors.getRightFront()- sensors.getRightBack())<1){
              //if there is space return back to normal move
              if(((sensors.getFrontLeft() <= 9 ) || (sensors.getFrontRight() <= 9) || (sensors.getUltra() <=18))){
                movement.changeCornerMode(0);
                movement.changeState((int)NormalMove);
                dodgeBuffer=0;
                corneringState = 0;
                cornerSpace = 0;
                cornerStrafe = false;
              }
              else if(cornerSpace){
                movement.changeCornerMode(6);
                dodgeBuffer=0;
                corneringState = 4;
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
            Serial1.println("Move Forward");
            //Move forward a bit
            if(millis()-cornerTime>850){
              dodgeBuffer++;
              movement.changeCornerMode(3);
              if (dodgeBuffer > 6) {

                movement.changeCornerMode(6);
                dodgeBuffer=0;
                corneringState = 4;
			  }
            }
            break;
          case(4):
            //Back to normal move
            Serial1.println("Back to NORMAL");
            if(sensors.getRightBack()>23*(int((numberCorners-1)/4)) && sensors.getRightBack()>23*(int((numberCorners-1)/4))){
              dodgeBuffer++;
              movement.changeCornerMode(3);
                movement.changeCornerMode(0);
                movement.changeState((int)NormalMove);
                dodgeBuffer=0;
                corneringState = 0;
                cornerSpace = 0;
            }
            break;

        }
      }
      else{
        switch(corneringState){
          case(0):
            Serial1.println(sensors.getYaw());
            yawInput = 90;
            if(within(sensors.getYaw(), yawInput,2)){
              cornerCount++;
            }
            if(cornerCount>15) {
               //buffer to give it time to settle
                movement.changeCornerMode(3);
                  dodgeBuffer = 0;
                  corneringState = 1;
                  sensors.recalibrateYaw();
                  movement.changeCornerMode(4);
                  cornerCount = 0;
            }
            break;
           case(1):
             if(fabs(sensors.getRightFront()- sensors.getRightBack())<1){
                movement.changeCornerMode(3);
                dodgeBuffer++;

                corneringState = 2;
                movement.changeCornerMode(5);
                dodgeBuffer = 0;

             }
            break;
           case(2):
            if(sensors.getRightBack()<=13 &&sensors.getRightFront()<=13){
              movement.changeCornerMode(0);
              movement.changeState((int)NormalMove);
              dodgeBuffer=0;
              corneringState = 0;
              cornerSpace = 0;
            }
            break;
        }
      }
      
      break;
    case(Dodge):
      //Serial1.println("DODGE");
       switch (dodgeFSM) {
         case (START):
            //Serial1.println("START");
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
           //SerialCom->println("Sideways");
            if (sensors.getFrontRight() - prevR > 10 || sidePass ==1 || dumbTimeout(150)) {
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
                /*Serial1.print("DODGE START: ");
                Serial1.print(dodgeTime);*/
              }

            }
            break;
         case (FORWARD):
         //SerialCom->println("Forward");
         if((millis() - dodgeTime>=  1700 ))
         {
          //Serial1.print("END TIME: ");
          //Serial1.println(millis());
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
          }else if (obCheck()) {
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
    //Serial1.println("FireFIGHT");
    //Serial1.println("Entering FF");
      fireFightingSystem.fireFight();
    //Serial1.println("Leaving FF");
      dodgeFSM = START;
      movement.changeState((int)Dodge);
      break;
    case(Stop):
      break;
  }
    movement.runCurrentState(sensors.getFrontRight(), sensors.getFrontLeft(), sensors.getRightFront(), sensors.getRightBack(), sensors.getYaw(),numberCorners*cornerStrafe,yawInput); //(numberCorners% 2 ==0) ? numberCorners : 0);
}


void sendToPC(int startMSG, float x, float y, int turns, int state) {
 /* Serial1.print(startMSG);
  Serial1.print(",");
  Serial1.print(x);
  Serial1.print(",");
  Serial1.print(y);
  Serial1.print(",");
  Serial1.print(turns);
  Serial1.print(",");
  Serial1.println(state);*/
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
  boolean left = within(sensors.getFrontLeft(), sensors.getUltra(), 40);
  boolean right = within(sensors.getFrontRight(),sensors.getUltra(), 40);
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
  if (compare== 0) {
    if (value >= (compare - (percent * 360/100)) && value <= (compare + (percent * 360/100))) {
        return true;
      } else {
        return false;
      }
  }
  
  if (value >= (compare - (percent * compare/100)) && value <= (compare + (percent * compare/100))) {
    return true;
  } else {
    return false;
  }
}
