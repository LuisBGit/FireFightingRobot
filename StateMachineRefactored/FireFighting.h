#include "Arduino.h"
#include "Servo.h"

class FireFighting
{
	public:
		void firefightingSetup(int pPin, int fPin, int sPin);
    bool fireScan();
		void activateFan();
    void fireFight();
	
	private:
    int fanPin;
    int servoPin;
    unsigned long fireStartTime, currentTime;

    float reading = 0;
    float firstReading = 0;

    bool fireStarted = false;
    bool fireDetected = false;
    //set to the minimum to try and avoid actually writing to 0
    int fireStart;
    int fireEnd;
    int firePos;

    int pin;
    float voltage;
    int viableCount = 0;

    int TimesRun = 0;
       
    Servo servo;
};
