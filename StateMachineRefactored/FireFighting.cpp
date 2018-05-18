

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
  //Serial.begin(115200);
  //servo.attach(sPin);
  //servo.write(90);
}

bool FireFighting::fireScan(){
  //Bring the servo to the zero position before it starts reading anything
  Serial1.println("Entering firescan");
  servo.write(35);
  delay(200);

  //Sweep Through 
  for (int i=35; i<145; i++) {
    servo.write(i);
    float firstReading, reading;
    
    for(int j = 0; j < 10; j++){
      firstReading += analogRead(pin);
    }
  
    reading = firstReading/10;
    firstReading = 0;

    //Logging down the first position of the fire
    if((reading > 700) && (fireStarted == false) && (i > 40))
    {
      fireStart = i;
      //this variable is used as the primary variable that indicates the presence of a fire
      fireStarted = true;
    }
  
    //Viable Count Log
    if((reading > 700) && (i > 45) && (i < 140)){ //i < 140 & i > 45 "blinds" the robot to potential fires along its peripheries, this is used to prevent the error bug from appearing
      viableCount++;
      Serial.println(viableCount);
    }
      
    if((fireStarted == true) && (reading < 700) && (fireDetected == false) && (viableCount >= 10) && (i < 140)) 
    {
      fireEnd = i;
      fireDetected = true;    
    }
    delay(40);
  } 

  if(fireDetected == true){
    firePos = (fireEnd + fireStart)/2;
    fireDetected = false;
    fireStarted = false;
    viableCount = 0;
    return true;
  } else {
    servo.write(90);
    fireStarted = false;
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
  Serial1.println("fire check");
  bool fireStat = fireScan();
  while(fireStat == true){
    
    activateFan();
    delay(200);
    fireStat = fireScan();
  }
  servo.write(90);
}
