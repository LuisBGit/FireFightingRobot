#include "Arduino.h"
#include "Servo.h"

class FireFighting
{
	public:
		void firefightingSetup(int pPin, int fPin, int sPin, Servo &attachedServo);
    bool fireScan();
		void activateFan();
    void fireFight();
	
	private:
    int fanPin;
    int servoPin;

    float reading;

    bool fireStarted = false;
    bool fireDetected = false;
    //set to the minimum to try and avoid actually writing to 0
    int fireStart =35;
    int fireEnd = 35;
    int firePos = 35;

    int redLED = 25;
    int greenLED = 24;
    int blueLED = 23;

    bool fireFound = false;
    bool fireExists = false;
    int pin;
    float voltage;
    int viableCount = 0;
    
    Servo servo;
	  //Light reading variables
	  int lightfilterArray[15] = {90,90,90,90,90,90,90,90,90,90,90,90,90,90,90};
    int i = 35;
		int lightfilterIndex = 0;
		int maxLightReading = 0;
		int maxLightPos = 0;
    int fireCount = 2;
		//Boolean variable to prevent the code from running multiple different times
		bool oneRound = false;
};
