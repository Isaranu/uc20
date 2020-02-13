#include "UC20_FTP.h"
#include <AltSoftSerial.h>

AltSoftSerial ucSerial;
UC20_FTP ftp;

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

  ftp.setContext(1);          // set ftp contextID
  Serial.print  ("> ");
}

void loop() {
  if(Serial.available() > 0) {
    String req = Serial.readStringUntil('\n');
    Serial.println(req);
    int cr = req.lastIndexOf("\r");
    if(cr != -1) {
      req.remove(cr,1);
    }
    actionTrig(req);
    Serial.print("> ");
  }
}

void actionTrig(String str) {
  if(str == "help") 
    showHelp();
  else if(str.startsWith("open ")) {
    int8_t index1 = str.indexOf(" ") + 1;
    int8_t index2 = str.indexOf(" ",index1);
    if((index1 != 0)&&(index2 != -1)) {
      String host = str.substring(index1,index2);
      int port = str.substring(index2+1).toInt();
      getUserPass();
      if(!ftp.login(host,port)) 
        Serial.println("Error: cannot connect to FTP server");
    }
    else
      Serial.println("Error: \"open\" need 2 params");
  }
  else if(str.startsWith("cd ")) {
    int8_t index = str.indexOf(" ") + 1;
    if(index != 0) {
      String dir = str.substring(index);
      if(!ftp.setPath(dir)) 
        Serial.println("Error: command is not accomplished");
    }
  }
  else if(str == "pwd") {
    Serial.println(ftp.getPath());
  }
  else if(str == "ls") {
    ftp.list();
    Serial.println();
  }
  else if(str.startsWith("get ")) {
    int8_t index = str.indexOf(" ") + 1;
    if(index != 0) {
      String filename = str.substring(index);
      if(!ftp.getFile(filename, filename, RAM)) 
        Serial.println("Error: cannot get the specific file or time out");
    }
  }
  else if(str.startsWith("put ")) {
    int8_t index = str.indexOf(" ") + 1;
    if(index != 0) {
      String filename = str.substring(index);
      ftp.putFile(filename,filename);
      if(!ftp.waitPut()) 
        Serial.println("Error: cannot put the specific file or time out");
    }
  }
  else if(str.startsWith("delete ")) {
    int8_t index = str.indexOf(" ") + 1;
    if(index != 0) {
      String filename = str.substring(index);
      if(!ftp.deleteFile(filename)) 
        Serial.println("Error: cannot execute command");
    }
  }
  else if(str.startsWith("rename ")) {
    int8_t index1 = str.indexOf(" ") + 1;
    int8_t index2 = str.indexOf(" ",index1);
    if((index1 != 0)&&(index2 != -1)) {
      String oldN = str.substring(index1,index2);
      String newN = str.substring(index2+1);
      if(!ftp.rename(oldN,newN)) 
        Serial.println("Error: cannot rename");
    }
    else 
      Serial.println("Error: \"rename\" need 2 params");
  }
  else if(str.startsWith("mkdir ")) {
    int8_t index = str.indexOf(" ") + 1;
    if(index != 0) {
      String dir = str.substring(index);
      if(!ftp.makeDir(dir)) 
        Serial.println("Error: cannot make directory");
    }
  }
  else if(str.startsWith("rmdir ")) {
    int8_t index = str.indexOf(" ") + 1;
    if(index != 0) {
      String dir = str.substring(index);
      if(!ftp.deleteDir(dir)) 
        Serial.println("Error: cannot execute command");
    }
  }
  else if((str=="bye")||(str=="quit")) {
    ftp.logout();
  }
  else if(str.startsWith("_ls")) {
    int8_t index = str.indexOf(" ") + 1;
    if(index != 0) {
      String storage = str.substring(index);
      gsm.listFile("*",storage);
    }
    else 
      gsm.listFile();
  }
  else if(str.startsWith("_view ")) {
    int8_t index = str.indexOf(" ") + 1;
    if(index != 0) {
      String filename = str.substring(index);
      gsm.loadFile(filename);
    }
    Serial.println();
  }
  else {
    Serial.println("Error: no such commands. Type \"help\" to get a list of commands");
  }
}

void showHelp() {
  Serial.println(F("### ftp commands"));
  Serial.println(F("open [host] [port] : Connect to FTP host"));
  Serial.println(F("cd [directory]     : Navigate to directory"));
  Serial.println(F("pwd                : Query current directory"));
  Serial.println(F("ls                 : Display a list of files and folders in current directory"));
  Serial.println(F("get [filename.ext] : Copy file from remote to local"));
  Serial.println(F("put [filename.ext] : Copy file from local to remote"));
  Serial.println(F("delete [filename.ext] : Delete a file"));
  Serial.println(F("rename [old.ext] [new.ext] : Rename a file or directory"));
  Serial.println(F("mkdir [name]       : Create a directory"));
  Serial.println(F("rmdir [name]       : Remove a directory"));
  Serial.println(F("bye (or quit)      : Disconnect to FTP server"));
  Serial.println(F("### UC20 local commands"));
  Serial.println(F("_ls [RAM,UFS}      : Display a list of files in local storage"));
  Serial.println(F("_view [filename]   : Display contents of file in local storage"));
  Serial.println(F("### etc"));
  Serial.println(F("help               : List available commands"));
}

void getUserPass() {
  Serial.print("Enter username: ");
  while(Serial.available() < 2);
  String user = Serial.readStringUntil('\n');
  int8_t cr = user.indexOf('\r');
  if(cr != -1) 
    user.remove(cr);
  Serial.println(user);

  Serial.print("Enter password: ");
  while(Serial.available() < 2);
  String pass = Serial.readStringUntil('\n');
  cr = pass.indexOf('\r');
  if(cr != -1)
    pass.remove(cr);
  for(uint8_t i=0; i<pass.length(); i++) 
    Serial.print("*");
  Serial.println();

  ftp.setUserPassword(user,pass);
}
