#include "FireFighting.h"

//Setting up the analog input
void FireFighting::firefightingSetup(int analogIn)
{
  this->pin = analogIn;
}


int FireFighting::fireScan()
{
	if(oneRound == false)
	{
		for(int i = 0; i <= 180; i++)
		{
			servo.write(i);
			voltage = analogRead(this->pin);
		}
		if(voltage > 700)
		{
			if(lightfilterIndex < 10)
			{
				lightfilterArray[lightfilterIndex] = i;
			}
			lightfilterIndex++;
			fireFound = true;
		}
		delay(15);
		oneRound = true;
	}
	return lightfilterArray[8];
}

void FireFighting::moveServo()
{
  if(fireFound == true){
    servo.write(lightfilterArray[8]);
    delay(200);
  } else {
    servo.write(90);
  }
}

void FireFighting::activateFan()
{
	if(fireFound == true)
	{
		while(fireExists == true){
			digitalWrite(fanPin,HIGH);
			int fire = analogRead(this->pin);
			if(fire < 80)
			{
				fireExists = false;
			}
			delay(100);
		}
	} else if(fireExists == false) {
		servo.write(90);
	}
}
