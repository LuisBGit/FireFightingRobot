//Updated

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
  servo.write(10);
  delay(200); //wait before taking readings
  
  //Sweep Through from 35 to 145 degrees
  for (int i=20; i<160; i++) {
    servo.write(i); //move the servo to that particular position
        
    for(int j = 0; j < 10; j++){
      firstReading += analogRead(pin);
    }
    
    reading = firstReading/10; //average out the readings
    firstReading = 0; //reset

    Serial1.println(reading);
    //Logging down the first position of the fire
    if((reading > 200) && (fireStarted == false))
    {
      fireStart = i;
      //this variable is used as the primary variable that indicates the presence of a fire
      fireStarted = true;
    }
  
    //Viable Count Log
    if(reading > 200){ 
      viableCount++;
    }
      
    if((fireStarted == true) && (reading < 200)) 
    {
      fireEnd = i;
      fireDetected = true; 
      break;
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
  fireStartTime = millis(); //get the starting time for the fire
  Serial1.println(pointRead);
  while((pointRead > 100) && (currentTime - fireStartTime >= 20000))
  {
    currentTime = millis();
    digitalWrite(fanPin,HIGH); 
    Serial1.println("Fan On"); 
    delay(200); 
    pointRead = analogRead(pin);
  }  
  Serial1.println("Fan Off");
  digitalWrite(fanPin,LOW);
  //Slowly return back to 35 degrees origin point
  if(firePos > 25){
    for(int i = firePos; i > 24; i--){
      servo.write(i);
      delay(40);
    }
  } else if(firePos < 35){
    for(int i = firePos; i < 26; i++){
      servo.write(i);
      delay(40);
  }
  }
}

void FireFighting::fireFight()
{
  bool fireStat = fireScan();
  if(fireStat == true){
    activateFan();
    /*
    servo.write(firePos);
    Serial1.println("FireStart = ");
    Serial1.println(fireStart);
    Serial1.println("FireEnd = ");
    Serial1.println(fireEnd);
    Serial1.println("FirePos = ");
    Serial1.println(firePos);
    delay(1000);
    */
  }
  
  
  servo.write(90);
}

