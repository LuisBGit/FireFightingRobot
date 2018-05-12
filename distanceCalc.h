#include "arduino.h"

class distanceCalc {
  public:
    float getDistance(float ultraRading);
	   void restartDistance(float ultraReading);
	   void setReferenceDistance(float ultraReading);
	   float updateDistance(float ultraReading);
     float getDistance();
  private:
    float distanceTravelled=0;
    float currentUltraReading=0;
    float previousUltraReading=0;


};
