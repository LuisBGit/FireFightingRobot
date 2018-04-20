#include "SystemStateMachine.h"




void SystemState() {
  static STATE machine_state = INITIALISING;
  switch (machine_state) {
     case INITIALISING:
          machine_state=initialise();
          break;
     case RUNNING:
           machine_state=runCycle();
          break;
     case STOPPED:
          machine_state=stopping();
          break;
  }
}

STATE initialise() {
  currentTime = 0;

  irTiming = 0;
  pingTiming = 0;
  integralTiming = 0;
  cornerTiming = 0;
  stateUpdateTiming = 0;
  displayTiming = 0;
  batteryTiming = 0;
  mpuTimer = 0;
  sendTime = 0;
  
  sensors.setupSensors();
  delay(100);
  return RUNNING;
}

STATE runCycle() {
  currentTime = millis();
  systemTiming();
  
  return STOPPED;

}

STATE stopping() {
  return STOPPED;
}

