#include <AltSoftSerial.h>

AltSoftSerial ucSerial;

void setup() {
  Serial.begin(9600);
  ucSerial.begin(9600);
}

void loop() {
  uint8_t c;
  if(Serial.available()){
    c = Serial.read();
    ucSerial.write(c);
  }
  if(ucSerial.available()){
    c = ucSerial.read();
    switch(c) {
      case '\r':
        Serial.print("<CR>");
        break;
      case '\n':
        Serial.println("<LF>");
        break;
      default:
        if(!isPrintable(c)) {
          Serial.print("<");
          if( c < 0x10 ) Serial.print("0");
          Serial.print(c,HEX);
          Serial.print(">");
        }
        else Serial.write(c);
        break;
    }
  }
}
