#include "locomotion.h"

void locomotion::setupLocomotion(byte p1, byte p2, byte p3, byte p4) {
  this->p1 = p1; this->p2 = p2; this->p3 = p3; this->p4 = p4;
  this->m1.attach(p1); this->m2.attach(p2); this->m3.attach(p3); this->m4.attach(p4);
}


void locomotion::STOP() {
  this->m1.writeMicroseconds(1500);
  this->m2.writeMicroseconds(1500);
  this->m3.writeMicroseconds(1500);
  this->m4.writeMicroseconds(1500);
}

void locomotion::IK(float vx, float vy, float wz, HardwareSerial *SerialCom) {

  this->theta1 = ((-1/rw)* (vx + vy +((L+l) *wz)))  + 1500;
  this->theta2 =  ((-1/rw)* (vx - vy + ((L+l) * wz))) + 1500;
  this->theta3 = ((1/rw)* (vx - vy - ((L+l) * wz))) + 1500;
  this->theta4 = ((1/rw)* (vx + vy - ((L+l) * wz))) + 1500;

  

  //Write
  if (this->theta2 < 700) {
    this->theta2 = 700;
  }
   if (this->theta2 > 2300) {
    this->theta2 = 2300;
  }
    if (this->theta1 < 700) {
    this->theta1 = 700;
  }
   if (this->theta1 > 2300) {
    this->theta1 = 2300;
  }
    if (this->theta3 < 700) {
    this->theta3 = 700;
  }
   if (this->theta3 > 2300) {
    this->theta3 = 2300;
  }
    if (this->theta4 < 700) {
    this->theta4 = 700;
  }
   if (this->theta4 > 2300) {
    this->theta4 = 2300;
  }

  SerialCom->println(" ");
  SerialCom->println(" ");
  SerialCom->println(" ");
  SerialCom->println(" ");
  SerialCom->println(" ");
  SerialCom->print(" Theta1: ");
  SerialCom->print(this->theta1 - 1500);
  SerialCom->print(" Theta2: ");
  SerialCom->print(this->theta2 - 1500);
  SerialCom->print(" Theta3: ");
  SerialCom->print(this->theta3 - 1500);
  SerialCom->print(" Theta4: ");
  SerialCom->println(this->theta4 - 1500);

    this->m1.writeMicroseconds(this->theta2);
    this->m4.writeMicroseconds(this->theta1);
    this->m3.writeMicroseconds(this->theta3);
    this->m2.writeMicroseconds(this->theta4);

}

void locomotion::disable_motors() {
  this->m1.detach();
  this->m2.detach();
  this->m3.detach();
  this->m4.detach();

  pinMode(this->p1, INPUT);
  pinMode(this->p2, INPUT);
  pinMode(this->p3, INPUT);
  pinMode(this->p4, INPUT);

  
}

void locomotion::enable_motors() {
  this->m1.attach(this->p1); this->m2.attach(this->p2); this->m3.attach(this->p3); this->m4.attach(this->p4);
}

