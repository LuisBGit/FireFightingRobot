#include "distanceCalcV2.h"

void distanceCalcV2::restartDistance(float sensorReading){
	referenceReading = sensorReading ;
	distanceTravelled=0;

}

float distanceCalcV2::getDistance(float sensorReading){

	return distanceTravelled + (sensorReading - referenceReading);
}

void distanceCalcV2::setReferenceDistance(float sensorReading){
  referenceReading = sensorReading;
  distanceTravelled = distanceTravelled + (sensorReading - referenceReading);
}


void distanceCalcV2::setOnlyReference(float sensorReading){
  referenceReading = sensorReading;
}

float distanceCalcV2::getReference(){
  return referenceReading;
}

