#ifndef UC20_GNSS_H
#define UC20_GNSS_H

#include "Arduino.h"
#include "UC20_lib.h"

#define GGA   "GGA"
#define RMC   "RMC"
#define GSV   "GSV"
#define GSA   "GSA"
#define VTG   "VTG"
#define GNS   "GNS"

typedef struct GpsResp {
	String utc;
	String latitude;
	String longtitude;
	String hdop;
	String altitude;
	String fix;
	String cog;
	String spkm;
	String spkn;
	String date;
	String nsat;
} gpsInfo;

class UC20_GNSS {
	public:
		UC20_GNSS();
		bool Start();
		bool Stop();
		gpsInfo GetPosition(uint8_t mode = 2);
		bool SetNMEA(bool enable);
		String GetNMEA(String nmea);
		
	private:
		
};

#endif

/*
AT+QGPS=1					// turn on gnss engine
AT+QGPSLOC?					// obtain position
AT+QGPSLOC=0				// ddmm.mmmmN/S,dddmm,mmmmE/W
           1				// ddmm.mmmmmm,N/S,dddmm.mmmmmm,E/W
		   2				// (-)dd.ddddd,(-)ddd,ddddd
AT+QGPSEND					// turn off

//////////////////////////////////////////////////////////////////////

AT+QGPSCFG="nmeasrc",1		// enable nmeasrc
AT+QGPSGNMEA="GGA"			// obtain GGA
AT+QGPSCFG="nmeasrc",0		// disable
AT+QGPSGNMEA="GGA"			// ERROR

//////////////////////////////////////////////////////////////////////

AT+QGPSXTRA=1				// enable xtra

AT+QGPSXTRADATA?			// check if xtra is valid?
							// if invalid go to download
*/