#include <AltSoftSerial.h>

AltSoftSerial ucSerial;

void setup() {
  Serial.begin(9600);
  ucSerial.begin(9600);
}

void loop() {
  uint8_t c;
  if(Serial.available() > 0){
    c = Serial.read();
    ucSerial.write(c);
  }
  if(ucSerial.available() > 0){
    c = ucSerial.read();
    Serial.write(c);
  }
}
