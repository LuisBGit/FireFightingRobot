#include "arduino.h"
#include "Servo.h"
#include "locomotion.h"
#include "PID.h"


class motionHandler {
  public:
    void setupHandler(byte p1, byte p2, byte p3, byte p4);
    void serialReceive(HardwareSerial *SerialCom);
    void disableHandler();
    void enableHandler();
  private:
    locomotion motorSystem;
    PID pidX, pidY, pidZ;
};

