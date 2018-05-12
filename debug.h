

void displayMessage(String message, float d1, float d2, float d3) {
  //SerialCom->print(message);
  SerialCom->print(d1);
  SerialCom->print(",");
  SerialCom->print(d2);
  SerialCom->print(",");
  SerialCom->println(d3);
}

void getInput() {
  if (SerialCom->available() > 0) {
    String first  = SerialCom->readStringUntil(',');
    SerialCom->read(); //next character is comma, so skip it using this
    String second = SerialCom->readStringUntil(',');
    SerialCom->read();
    String third  = SerialCom->readStringUntil('\n');
    //parse your data here. example:
    //double x = Double.parseDouble(first);
    disp1 = first.toInt();
    disp2 = second.toInt();
    disp3 = third.toInt();
    //handler.setGains(disp1, disp2, disp3);

    
    changeCounter++;
  }
}
