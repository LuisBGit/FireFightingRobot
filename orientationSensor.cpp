#include "orientationSensor.h"
#include <FaBo9Axis_MPU9250.h>


void orientationSensor::setupOrientation() {
   fabo.begin();  
}

float orientationSensor::readOrientation() {
   fabo.readMagnetXYZ(&mx, &my, &mz);
   float correctX = (xScale) * (mx - xOffset);
   float correctY = (yScale) * (my - yOffset); 
   float heading;
   float result = atan2(correctX, correctY);
   array[index] = result;
   index++;
   if (index > 9) {
     index = 0;
    }
   // result = (array[0] + array[1] + array[2] + array[3] + array[4])/5;
    result = (array[0] + array[1] + array[2] + array[3] + array[4] + array[5] + array[6] + array[7] + array[8] + array[9]) / 10;
    
   heading = (result * 180)/3.141592653;
   if (result < 0) {
      heading = heading + 360;
   }

   return heading;
}


