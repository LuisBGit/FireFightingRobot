

#include "FireFighting.h"
#include "Servo.h"

//Setting up the analog input
void FireFighting::firefightingSetup (int pPin, int fPin, int sPin, Servo &attachedServo)
{
  this->pin = pPin;
  fanPin = fPin;
  servoPin = sPin;
  this->servo = attachedServo;
  
  pinMode(fanPin, OUTPUT);
  pinMode(pin, INPUT);
}

bool FireFighting::fireScan(){
  //Complete reset of variables, just in case
  fireStarted = false;
  viableCount = 0;
  reading = 0;
  firstReading = 0;
  
  //Bring the servo to the zero position before it starts reading anything
  servo.write(35);
  delay(200); //wait before taking readings
  
  //Sweep Through from 35 to 145 degrees
  for (int i=10; i<170; i++) {
    servo.write(i); //move the servo to that particular position
        
    for(int j = 0; j < 10; j++){
      firstReading += analogRead(pin);
    }
    
    reading = firstReading/10; //average out the readings
    firstReading = 0; //reset

    Serial1.println(reading);
    //Logging down the first position of the fire
    if((reading > 700) && (fireStarted == false))
    {
      fireStart = i;
      //this variable is used as the primary variable that indicates the presence of a fire
      fireStarted = true;
    }
  
    //Viable Count Log
    if(reading > 700){ 
      viableCount++;
      //Serial.println(viableCount);
    }
      
    if((fireStarted == true) && (reading < 700) && (viableCount >= 10)) 
    {
      fireEnd = i;
      fireDetected = true;    
    }   
    //Delay for smoother motor movement
    delay(40);
  } 
  
    Serial1.println(viableCount);
  if(fireDetected == true){
    firePos = (fireEnd + fireStart)/2;
    //Complete reset
    fireDetected = false;
    fireStarted = false;
    viableCount = 0;
    return true;
  } else {
    servo.write(90);
    //Complete reset
    fireStarted = false;
    fireDetected = false;
    viableCount = 0;
    return false;
  }
}

void FireFighting::activateFan()
{
  servo.write(firePos);
  delay(200);
  int pointRead = analogRead(pin);
  while(pointRead > 200)
  {
    digitalWrite(fanPin,HIGH);   
    pointRead = analogRead(pin);
  }  
  digitalWrite(fanPin,LOW);
  //Slowly return back to 35 degrees origin point
  if(firePos > 35){
    for(int i = firePos; i > 34; i--){
      servo.write(i);
      delay(40);
    }
  } else if(firePos < 35){
    for(int i = firePos; i < 36; i++){
      servo.write(i);
      delay(40);
  }
  }
}


void FireFighting::fireFight()
{
  Serial1.println("Entered Fire Fighting");
  bool fireStat = fireScan();
  while(fireStat == true && !(TimesRun > 1)){
    activateFan();
    delay(200);
    fireStat = fireScan();
    TimesRun++;
  }
  TimesRun = 0;
  servo.write(90);
}

