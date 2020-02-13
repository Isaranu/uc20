#include "UC20_GNSS.h"
#include <AltSoftSerial.h>

AltSoftSerial ucSerial;
UC20_GNSS gnss;

void setup() {
  Serial.begin(9600);
  ucSerial.begin(9600);
  gsm.initSerial(ucSerial);

  Serial.println("############ UC20 GNSS ############");
  Serial.print("Power on module...");
  while(!gsm.powerOn()) 
    Serial.print(".");
  Serial.println(" READY");
  
  Serial.print("Start GPS Session...");
  Serial.println(gnss.Start()? " DONE":" FAIL");

  Serial.print("Wait for signal");
  delay(5000);
  gpsInfo pos;
  do {
    Serial.print(".");
    pos = gnss.GetPosition();
    if(pos.utc == "0") {
      delay(2500);
    }
  }
  while(pos.utc == "0");
  Serial.println(" READY");
  Serial.println();

  Serial.println("# Obtain position");
  Serial.println("UTC       : " + pos.utc);
  Serial.println("latitude  : " + pos.latitude);
  Serial.println("longtitude: " + pos.longtitude);
  Serial.println("hdop      : " + pos.hdop);
  Serial.println("altitude  : " + pos.altitude);
  Serial.println("fix       : " + pos.fix);
  Serial.println("cog       : " + pos.cog);
  Serial.println("spkm      : " + pos.spkm);
  Serial.println("spkn      : " + pos.spkn);
  Serial.println("date      : " + pos.date);
  Serial.println("nsat      : " + pos.nsat);
  Serial.println();

  gnss.SetNMEA(true);
  delay(5000);
  Serial.println("# Obtain NMEA Sentences");
  Serial.println("- GGA: ");
  Serial.println(gnss.GetNMEA(GGA));
  Serial.println("- RMC: ");
  Serial.println(gnss.GetNMEA(RMC));
  Serial.println("- GSV: ");
  Serial.println(gnss.GetNMEA(GSV));
  Serial.println("- GSA: ");
  Serial.println(gnss.GetNMEA(GSA));
  Serial.println("- VTG: ");
  Serial.println(gnss.GetNMEA(VTG));
  Serial.println("- GNS: ");
  Serial.println(gnss.GetNMEA(GNS));

  Serial.println();
  Serial.println("Close Session");
  gnss.Stop();
}

void loop() {
  if(Serial.available() > 0) {
    gsm.write(Serial.read());
  }
  if(gsm.available() > 0) {
    Serial.write(gsm.read());
  }
}
