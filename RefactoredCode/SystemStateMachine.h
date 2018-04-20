#include "arduino.h"
#include "Sensors.h"

extern sensorManager sensors;
extern unsigned long currentTime;

extern unsigned long irTiming;
extern unsigned long pingTiming;
extern unsigned long integralTiming;
extern unsigned long cornerTiming;
extern unsigned long stateUpdateTiming;
extern unsigned long displayTiming;
extern unsigned long batteryTiming;
extern unsigned long mpuTimer;
extern unsigned long sendTime;

enum STATE {
  INITIALISING,
  RUNNING,
  STOPPED
};

void SystemState();

STATE initialise();

STATE runCycle();

STATE stopping();

void systemTiming();
