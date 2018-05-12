#include "distanceCalc.h"

void distanceCalc::restartDistance(float ultraReading){
	distanceTravelled = 0;
	currentUltraReading =ultraReading;
	previousUltraReading=ultraReading;
}

void distanceCalc::setReferenceDistance(float ultraReading){
	currentUltraReading =ultraReading;
	previousUltraReading=ultraReading;
}

float distanceCalc::updateDistance(float ultraReading){
	previousUltraReading=currentUltraReading;
	currentUltraReading = ultraReading;
	distanceTravelled = distanceTravelled + (currentUltraReading - previousUltraReading);
	return distanceTravelled;
}

float distanceCalc::getDistance(){
	return distanceTravelled;
}
