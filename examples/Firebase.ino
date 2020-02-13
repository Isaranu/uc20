#include "UC20_Firebase.h"
#include <AltSoftSerial.h>

AltSoftSerial ucSerial;
UC20_Firebase fb;

// HOST-NAME must start with "http://" or "https://"
#define HOST "<YOUR_FIREBASE_SERVER>"
#define AUTH "<YOUR_FIREBASE_SECRET>"
unsigned long interval = 20000;           // time delay for each packet

void setup() {
  Serial.begin(9600);
  ucSerial.begin(9600);
  gsm.initSerial(ucSerial);

  Serial.println("########## UC20 Firebase ##########");
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
  
  if(!fb.begin(HOST,AUTH)) {                        // set your HOST and SECRET
    Serial.println("HOST name is invalid");
    while(1);
  }
}

void loop() {
  static unsigned long prev_t = 0;
  unsigned long curr_t = millis();
  static unsigned long count = 1;

  if(curr_t - prev_t > interval) {
    prev_t = curr_t;
    
    // push data to firebase
    String pathA = "ex_push";
    String a = String(count++);
    String b = "\"myText\"";
    String jsObj = "{\"dataA\":" + a + ",\"dataB\":" + b + "}";       // a group of data in json format
    String addpth = fb.push(pathA,jsObj);                             // push a group of data
    Serial.println("push : /" + pathA + "/" + addpth);

    // set and get data on firebase
    String pathB = "ex_Set";
    String c = fb.getString(pathB);                                   // get previous data
    if(c == "OFF") 
      fb.setString(pathB,"ON");                                       // set new data (case1)
    else 
      fb.setString(pathB,"OFF");                                      // set new data (case2)
    Serial.println("set : /" + pathB + "/");
    
    c = fb.getString(pathB);                                          // get new update data
    Serial.print("get : /" + pathB + " = ");
    Serial.println(c);
    Serial.println("<------------------------------------------->");
  }
}
