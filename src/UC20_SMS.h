#ifndef UC20_SMS_H
#define UC20_SMS_H

#include "Arduino.h"
#include "UC20_lib.h"

// +CMTI: "XX",x

class UC20_SMS {
	public:
		UC20_SMS();
		void setInit();
		// bool beginsend(String phone_num);
		void send(String msg);
		// bool endsend();
		bool readAt(int index);
		bool deleteAt(int index);
		
		byte strToHex(String str, unsigned int start_i = 0);
		String ucodeToUtf8_TH(String ucode);
		String utf8ToUcode_TH(String utf_8);
		
	private:
		
};

#endif
