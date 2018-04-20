#include "SystemStateMachine.h"


void systemTiming() {
  //IR Timing
  if (currentTime - irTiming >= 10) {
    irTiming = millis();

  }

  //Ultrasonic Timing
  if (currentTime  - pingTiming >= 15) {
   pingTiming = millis();

  }

  //Orientation Reading
  if (currentTime- mpuTimer >= 20) {
    unsigned long dt = currentTime - mpuTimer;
    mpuTimer = millis();

  }

}
