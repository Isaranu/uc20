#include "UC20_MQTT.h"
#include <AltSoftSerial.h>

UC20_MQTT mqtt;
AltSoftSerial ucSerial;

#define HOST        "<YOUR_BROKER>"        // Server
#define PORT        <YOUR_PORT>            // port (SSL)
#define USER        "<YOUR_USERNAME>"      // User
#define PASS        "<YOUR_PASSWORD>"      // Password
#define CLIENT_ID   "UC20_NODE"
#define TOPIC_SEND  "count"
#define SUBSCRIBE   "count"

unsigned long prev_time_send = 0;
unsigned long interval = 20000;
unsigned int pack_no = 0;

void setup() {
  Serial.begin(9600);
  ucSerial.begin(9600);
  gsm.initSerial(ucSerial);

  Serial.println("############ UC20 MQTT ############");
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
  Serial.println();

  mqtt.setSSL(true);  // *** set true if connect via SSL port
  checkConnection();  // connect to server & subscribe topic
}

void loop() {
  unsigned long curr_time = millis();
  
  // publish packet
  if(curr_time-prev_time_send > interval) {
    prev_time_send = curr_time;
    String topic = TOPIC_SEND;
    String message = String(pack_no++);
    checkConnection();  // check if reconnect is needed
    mqtt.publish(topic,message);
  }

  // receive packet (subscribed topic)
  if(gsm.available() > 0) {
    if(mqtt.recvPublish()) {
      Serial.println("<---- Received message ---->");
      Serial.println("topic   : " + mqtt.getTopic());
      Serial.println("message : " + mqtt.getMessage());
      Serial.println("<-------------------------->");
    }
  }
}

void checkConnection() {
  if(!mqtt.socketReady()) {
    if(!gsm.connected()) {
      Serial.print("connect to internet... ");
      if(gsm.connect_internet()) {
        Serial.println("OK");
      }
    }
    mqtt.closeSocket();
    connect2server();
  }
}

void connect2server() {
  Serial.print("connect to server... ");
  if(mqtt.connectServer(HOST, PORT)) {
    Serial.println("OK");
    Serial.print  ("mqtt connect... ");
    Serial.println(mqtt.connect(CLIENT_ID, USER, PASS)? "OK":"FAIL");
  }
  else
    Serial.println("FAIL");
    
  Serial.print("subscribe... ");
  Serial.println(mqtt.subscribe(SUBSCRIBE)? "OK":"FAIL");
}
