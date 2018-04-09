#include "Arduino.h"
#include "Servo.h"

class FireFighting
{
	public:
		void firefightingSetup(int analogIn);
		int fireScan();
    void moveServo();
		void activateFan();
	
	private:
    int fanPin = 45;
    int servoPin = 4;
    
    int pin;
    float voltage;
    Servo servo;
		bool fireFound = false;
		bool fireExists = false;
	  //Light reading variables
	  int lightfilterArray[10] = {0,0,0,0,0,0,0,0,0,0};
    int i;
		int lightfilterIndex = 0;
		int maxLightReading = 0;
		int maxLightPos = 0;
		//Boolean variable to prevent the code from running multiple different times
		bool oneRound = false;
};
