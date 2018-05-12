#include "arduino.h"

class distanceCalcV2 {
  public:
    float getDistance(float sensorReading);
	   void restartDistance(float sensorReading);
	   void setReferenceDistance(float sensorReading);
  private:
    float distanceTravelled=0;
    float referenceReading=0;


};
