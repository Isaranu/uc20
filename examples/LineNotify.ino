#include "UC20_LineNotify.h"
#include <AltSoftSerial.h>

AltSoftSerial ucSerial;
UC20_LineNotify line;

#define TOKEN "<YOUR_LINE_TOKEN>"

void setup() {
  Serial.begin(9600);
  ucSerial.begin(9600);
  gsm.initSerial(ucSerial);

  Serial.println("######## UC20 Line Notify ########");
  Serial.print("Power on module...");
  while(!gsm.powerOn()) 
    Serial.print(".");
  Serial.println(" READY");
  Serial.print  ("Network registration...");
  Serial.println(gsm.network_regis_state()? " READY":" NOT READY");
  Serial.println("IMEI       : " + gsm.get_IMEI());
  Serial.println("IMSI       : " + gsm.get_IMSI());
  Serial.println("ICCID      : " + gsm.get_ICCID());
  Serial.println("Operator   : " + gsm.get_operator());
  Serial.print  ("RSSI       : ");
  Serial.println(gsm.get_signal_quality());
  if(!gsm.connected()) 
    gsm.connect_internet();
  Serial.println("IP Address : " + gsm.getIPaddress());
  Serial.println();
  
  line.setInit();
  line.setToken(TOKEN);

  // send message
  Serial.println("- Send");
  if(line.sendMessage("LINE NOTIFY TEST")) {
    Serial.print(line.readResp());
  }
}

void loop() {
  if(Serial.available() > 0) {
    gsm.write(Serial.read());
  }
  if(gsm.available() > 0) {
    Serial.write(gsm.read());
  }
}
