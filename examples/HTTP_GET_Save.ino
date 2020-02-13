#include "UC20_HTTP.h"
#include <AltSoftSerial.h>

AltSoftSerial ucSerial;
UC20_HTTP http;

// HOST-NAME must start with "http://" or "https://" (SSL)
#define HOST "https://www.google.com/"

void setup() {
  Serial.begin(9600);
  ucSerial.begin(9600);
  gsm.initSerial(ucSerial);

  Serial.println("############ UC20 HTTP ############");
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

  // HTTP GET
  http.setInit();
  http.setResponseHeader(false);    // Set true, if you need HTTP header
  http.enableSSL();
  if(!http.setUrl(HOST)) {
    Serial.println("HOST name is invalid");
    return;
  }
  http.get();
  Serial.print("- GET ");
  Serial.println(HOST);
  bool done = http.readResp_save("RAM:GOOG.html");
  Serial.println(done? "- Save html Done":"- Fail");
  gsm.disconnect_internet();

  // open file
  Serial.println("- print file's content");
  gsm.loadFile("RAM:GOOG.html");
  Serial.println("...");
}

void loop() {
  if(Serial.available() > 0) {
    gsm.write(Serial.read());
  }
  if(gsm.available() > 0) {
    Serial.write(gsm.read());
  }
}
