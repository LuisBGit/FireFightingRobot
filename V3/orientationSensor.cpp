#include "orientationSensor.h"
#include <FaBo9Axis_MPU9250.h>


void orientationSensor::setupOrientation() {
   fabo.begin();  
}

float orientationSensor::readOrientation() {
   fabo.readMagnetXYZ(&mx, &my, &mz);
   float correctX = (xScale) * (mx - xOffset);
   float correctY = (yScale) * (my - yOffset); 


   
   arrayX[index] = correctX;
   arrayY[index] = correctY;
   float resultX = (arrayX[0] + arrayX[1] + arrayX[2] + arrayX[3] + arrayX[4] + arrayX[5] + arrayX[6] + arrayX[7] + arrayX[8] + arrayX[9]) / 10;
   float resultY = (arrayY[0] + arrayY[1] + arrayY[2] + arrayY[3] + arrayY[4] + arrayY[5] + arrayY[6] + arrayY[7] + arrayY[8] + arrayY[9]) / 10;

   index++;
   if (index > 9) {
     index = 0;
    }
   float result = atan2(resultX, resultY);
   float heading;


   // result = (arrayX[0] + arrayX[1] + arrayX[2] + arrayX[3] + arrayX[4])/5;
    //result = (arrayX[0] + arrayX[1] + arrayX[2] + arrayX[3] + arrayX[4] + arrayX[5] + arrayX[6] + arrayX[7] + arrayX[8] + arrayX[9]) / 10;
    
   heading = (result * 180)/3.141592653;
   if (result < 0) {
      heading = heading + 360;
   }

   return heading;
}


