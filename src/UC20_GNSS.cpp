#include "UC20_GNSS.h"

UC20_GNSS::UC20_GNSS() {
	
}

bool UC20_GNSS::Start() {
	gsm.println(F("AT+QGPS=1"));
	return (gsm.waitStr(F("OK")) != "");
}

bool UC20_GNSS::Stop() {
	gsm.println(F("AT+QGPSEND"));
	return (gsm.waitStr(F("OK")) != "");
}

gpsInfo UC20_GNSS::GetPosition(uint8_t mode) {
	gsm.print(F("AT+QGPSLOC="));
	gsm.println(mode);
	
	gpsInfo gps_resp;
	String resp = gsm.waitStr(F("+QGPSLOC:"),12000);
	if(resp != "") {
		gsm.waitStr(F("OK"));
		
		int8_t index1 = resp.indexOf(F(" ")) + 1;
		int8_t index2 = resp.indexOf(F(","));
		gps_resp.utc = resp.substring(index1,index2);
		
		index1 = resp.indexOf(F(","),index2+1);
		gps_resp.latitude = resp.substring(index2+1,index1);
		index2 = resp.indexOf(F(","),index1+1);
		gps_resp.longtitude = resp.substring(index1+1,index2);
		
		index1 = resp.indexOf(F(","),index2+1);
		gps_resp.hdop = resp.substring(index2+1,index1);
		index2 = resp.indexOf(F(","),index1+1);
		gps_resp.altitude = resp.substring(index1+1,index2);
		
		index1 = resp.indexOf(F(","),index2+1);
		gps_resp.fix = resp.substring(index2+1,index1);
		index2 = resp.indexOf(F(","),index1+1);
		gps_resp.cog = resp.substring(index1+1,index2);
		
		index1 = resp.indexOf(F(","),index2+1);
		gps_resp.spkm = resp.substring(index2+1,index1);
		index2 = resp.indexOf(F(","),index1+1);
		gps_resp.spkn = resp.substring(index1+1,index2);
		
		index1 = resp.indexOf(F(","),index2+1);
		gps_resp.date = resp.substring(index2+1,index1);
		gps_resp.nsat = resp.substring(index1+1);
	}
	else {
		gps_resp.utc        = "0";
		gps_resp.latitude   = "0";
		gps_resp.longtitude = "0";
		gps_resp.hdop       = "0";
		gps_resp.altitude   = "0";
		gps_resp.fix        = "0";
		gps_resp.cog        = "0";
		gps_resp.spkm       = "0";
		gps_resp.spkn       = "0";
		gps_resp.date       = "0";
		gps_resp.nsat       = "0";
	}
	return gps_resp;
}

bool UC20_GNSS::SetNMEA(bool enable) {
	gsm.print(F("AT+QGPSCFG=\"nmeasrc\","));
	gsm.println(enable, DEC);
	return (gsm.waitStr(F("OK")) != "");
}

String UC20_GNSS::GetNMEA(String nmea) {
	gsm.print(F("AT+QGPSGNMEA=\""));
	gsm.print(nmea);
	gsm.println(F("\""));
	
	String ret = "";
	String resp = "";
	do {
		resp = gsm.waitStr(F("+QGPSGNMEA:"));
		if(resp != "") {
			if(ret != "") {
				ret += "\r\n";
			}
			int index = resp.indexOf(F(" ")) + 1;
			ret += resp.substring(index);
		}
	}
	while(resp != "");
	return ret;
}









