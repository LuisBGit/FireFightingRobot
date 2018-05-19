#include "arduino.h"

class distanceCalcV2 {
  public:
    float getDistance(float sensorReading);
	   void restartDistance(float sensorReading);
	   void setReferenceDistance(float sensorReading);
     void setOnlyReference(float sensorReading);
     float getReference();
  private:
    float distanceTravelled=0;
    float referenceReading=0;


};
