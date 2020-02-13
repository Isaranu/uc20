#include "UC20_FTP.h"
#include <AltSoftSerial.h>

AltSoftSerial ucSerial;
UC20_FTP ftp;

#define HOST   "<YOUR_HOST_SERVER>"   // Domain Name or IP Address
#define PORT   21                     // port (default is 21)
#define USER   "anonymous"            // username (or "anonymous")
#define PASS   "<YOUR_PASSWORD>"      // password

String initPath = "/";

void setup() {
  Serial.begin(9600);
  ucSerial.begin(9600);
  gsm.initSerial(ucSerial);

  Serial.println("############ UC20 FTP ############");
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

  int handle = gsm.openFile("RAM:test0.txt",UFS,1);     // create new file (overwrite mode)
  gsm.beginWriteFile(handle);
  gsm.print("Create file for testing FTP by UC20");
  gsm.endWriteFile();
  gsm.closeFile(handle);
  
  ftp.setContext(1);
  ftp.setUserPassword(USER,PASS);
  Serial.print("> Login to FTP Server...");
  while(!ftp.login(HOST,PORT)) {                        // connect to host server
    Serial.print(".");
  }
  Serial.println(" DONE");

  ftp.setPath(initPath);                                // set initial path (REMOTE)
  Serial.println("> Current path: " + ftp.getPath());   // get path (REMOTE)
  Serial.println("> Create directory");
  ftp.makeDir("UCXX_test");                             // create new directory (REMOTE)
  Serial.println("> Set path to new directory");
  ftp.setPath("UCXX_test");                             // set new path (REMOTE)
  Serial.println("> Current path: " + ftp.getPath());   // get path (REMOTE)
  
  Serial.print("> Put file to server... ");
  ftp.putFile("test1.txt","RAM:test0.txt");             // put file to server
  Serial.println(ftp.waitPut()? "DONE":"FAIL");         // wait put response
  
  Serial.println("> List files of current directory");  // list files (REMOTE)
  ftp.list();

  Serial.print("> Get file from server... ");
  Serial.println(ftp.getFile("test1.txt","RAM:test2.txt")? 
                 "DONE":"FAIL");                        // get file from server

  Serial.println("> Disconnect from FTP Server");
  ftp.logout();                                         // terminate connection

  Serial.println("> List local RAM of UC20");
  gsm.listFile("RAM:*");                                // list files (LOCAL)

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
