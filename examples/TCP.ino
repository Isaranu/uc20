#include "UC20_TCP.h"
#include <AltSoftSerial.h>

AltSoftSerial ucSerial;
UC20_TCP tcp;

#define HOST   "<YOUR_SERVER>"  // server's domain/IP
#define PORT   <YOUR_PORT>

int8_t socket = 0;    // Available range is (0-11)
unsigned long prev_time = 0;
unsigned long interval = 20000;

unsigned int pack_no = 0;   // data to send

void setup() {
  Serial.begin(9600);
  ucSerial.begin(9600);
  gsm.initSerial(ucSerial);

  Serial.println("############ UC20 TCP ############");
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
}

void loop() {
  unsigned long curr_time = millis();

  if(!tcp.socketReady(socket)) {
    if(!gsm.connected()) {
      gsm.connect_internet();
      Serial.print("reconnect - IP address : ");
      Serial.println(gsm.getIPaddress());
    }
    tcp.closeSocket(socket);
    tcp.openSocket(TCP, HOST, PORT, socket);
  }
  
  if(curr_time-prev_time > interval) {
    prev_time = curr_time;
    String message = String(pack_no++);
    Serial.println("Send message: " + message);
    tcp.beginSend(socket);
    tcp.send(message);
    tcp.endSend();
    if(tcp.waitRecv() == socket) {
      String response = tcp.readRecv(socket); // get first 100 chars (max)
      Serial.print("Response message: ");
      Serial.println(response);
    }
  }
}
