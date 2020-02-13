#include "UC20_SSL.h"
#include <AltSoftSerial.h>

AltSoftSerial ucSerial;
UC20_SSL ssl;

#define HOST "<YOUR_SERVER>"  // server's domain/IP
#define PORT 443

int8_t clientid = 0;    // Available range (0-11)
int8_t sctxid   = 1;    // Available range (0-5)

void setup() {
  Serial.begin(9600);
  ucSerial.begin(9600);
  gsm.initSerial(ucSerial);

  Serial.println("############ UC20 SSL ############");
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

  ssl.sslversion(sctxid, 1);        // version 1 = TLS1.0
  ssl.ciphersuite(sctxid, 0xffff);  // support all type available in module
  ssl.seclevel(sctxid, 0);          // level 0 = No Authentication

  // send process
  if(ssl.openSocket(1,sctxid,clientid,HOST,PORT,0)) {
    Serial.println("- socket is open");
  }
  if(ssl.socketReady(clientid)) {
    Serial.println("- socket ready");
    ssl.beginSend(clientid);
    ssl.send("MY_MESSAGE");           // your data
    ssl.endSend();
    Serial.println("- send");
    if(ssl.waitRecv() == clientid) {
      while(!ssl.recvEmpty())
        ssl.readRecv(clientid, true);     // print 1500 chars (max) per instruction
    }
  }
  ssl.closeSocket(clientid);
  gsm.disconnect_internet();
}

void loop() {
  if(Serial.available() > 0) {
    gsm.write(Serial.read());
  }
  if(gsm.available() > 0) {
    Serial.write(gsm.read());
  }
}
