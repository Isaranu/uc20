#include "UC20_SMS.h"

UC20_SMS::UC20_SMS() {
	
}

void UC20_SMS::setInit() {
	gsm.println(F("AT+CMGF=1"));
	gsm.waitStr(F("OK"),1000);
	gsm.println(F("AT+CSCS=\"UCS2\""));
	gsm.waitStr(F("OK"),1000);
	gsm.println(F("AT+CSMP=17,167,0,0"));
	gsm.waitStr(F("OK"),1000);
}
/*
bool UC20_SMS::beginsend(String phone_num) {
	gsm.println(F("AT+CMGF=1"));
	gsm.waitStr(F("OK"));
	gsm.println(F("AT+CSCS=\"UCS2\""));
	gsm.waitStr(F("OK"));
	
	gsm.print(F("AT+CMGS=\""));
	gsm.print(phone_num);
	gsm.println(F("\""));
	
	if(gsm.waitStr(F(">")) == "") 
		return false;
	else 
		return true;
	
	// while(msg.length() > 160) {
		// gsm.print(utf8ToUcode_TH(msg.substring(0,160));
		// msg.remove(0,160);
	// }
	// gsm.write(0x1A);
	
	// String resp = gsm.waitStr(F("+CMGS:"));
	// if(resp != "") {
		// gsm.waitStr(F("OK"));
		// return true;
	// }
	// else 
		// return false;
}
*/
void UC20_SMS::send(String msg) {
	Serial.println(msg.length());
	Serial.println(msg);					//delete
	if(msg.length() == 0 || msg == "") {
		Serial.print(0x1b,HEX);				//delete
		// gsm.write(0x1b);
	}
	// gsm.print(utf8ToUcode_TH(msg));
	Serial.println("ok");					//delete
}
/*
bool UC20_SMS::endsend() {
	gsm.write(0x1A);
	String resp = gsm.waitStr(F("+CMGS:"));
	if(resp != "") {
		gsm.waitStr(F("OK"));
		return true;
	}
	else 
		return false;
}
*/
bool UC20_SMS::readAt(int index) {
	gsm.print(F("AT+CMGR="));
	gsm.println(index);
	
	String resp = gsm.waitStr(F("+CMGR:"));
	if(resp != "") {
		int8_t index1 = resp.indexOf(F(",")) + 2;
		int8_t index2 = resp.indexOf(F("\""),index1);
		gsm.debug->print(F("From: "));
		gsm.debug->println(ucodeToUtf8_TH(resp.substring(index1,index2)));
		index1 = resp.indexOf(F("\""),index2+1) + 1;
		index2 = resp.lastIndexOf(F("\""));
		gsm.debug->print(F("Time :"));
		gsm.debug->println(resp.substring(index1,index2));
	}
	else {
		return false;
	}
	String msg = gsm.readStringUntil('\n');
	int end_index = msg.lastIndexOf(F("\r"));
	if(end_index != -1) {
		msg.remove(end_index);
		while(msg.length() > 160) {
			gsm.debug->print(ucodeToUtf8_TH(msg.substring(0,160)));
			msg.remove(0,160);
		}
		gsm.debug->println(ucodeToUtf8_TH(msg));
		gsm.waitStr(F("OK"));
		return true;
	}
	else {
		return false;		
	}
}

bool UC20_SMS::deleteAt(int index) {
	gsm.print(F("AT+CMGD="));
	gsm.println(index);
	return (gsm.waitStr(F("OK")) != "");
}
		
byte UC20_SMS::strToHex(String str, unsigned int start_i) {
	const char hex_chr[16] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
	const byte hex_val[16] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};
	str.toUpperCase();
	byte val = 0;
	for(byte i=0; i<2; i++) {
		for(byte j=0; j<sizeof(hex_chr); j++) {
			if(str[start_i] == hex_chr[j]) {
				val = (val<<4) | hex_val[j];
			}
		}
		start_i++;
	}
	return val;
}

String UC20_SMS::ucodeToUtf8_TH(String ucode) {
	if(ucode.length()%2 != 0) {
		return "";
	}
	String utf_8 = "";
	int i = 0;
	while(i < ucode.length()) {
		if(strToHex(ucode,i) == 0x0E) {			// read 1st byte : TH
			utf_8 += char(0xE0);
			i = i+2;
			char temp = strToHex(ucode,i);		// read 2nd byte
			if(temp < 0x40) {
				utf_8 += char(0xB8);
				utf_8 += char(temp+0x80);
			}
			else {
				utf_8 += char(0xB9);
				utf_8 += char(temp+0x40);
			}
		}
		else if(strToHex(ucode,i) == 0x00) {	// read 1st byte : ENG
			i = i+2;
			utf_8 += char(strToHex(ucode,i));	// read 2nd byte
		}
		else {
			return "";
		}
		i = i+2;
	}
	return utf_8;
}

String UC20_SMS::utf8ToUcode_TH(String utf_8) {
	String ucode = "";
    int i = 0;
	while(i < utf_8.length()) {
		if(byte(utf_8[i]) == 0xE0) {        		// read 1st byte : not ENG
			ucode += "0E";							// assign unicode 1st half
			i++;
			if(byte(utf_8[i]) == 0xB8) {			// read 2nd byte : case 1
				i++;
				if(byte(utf_8[i])-0x80 < 0x10) {	// read 3rd byte : check if convert value has 1 digit?
					ucode += "0";
				}
				String temp = String(byte(utf_8[i])-0x80, HEX);
				temp.toUpperCase();
				ucode += temp;						// assign unicode 2nd half
			}
			else if(byte(utf_8[i]) == 0xB9) {		// read 2nd byte : case 2
				i++;
				String temp = String(byte(utf_8[i])-0x40, HEX);
				temp.toUpperCase();
				ucode += temp;						// assign unicode 2nd half
			}
			else {
				return "" ;
			}
		}
		else {										// read 1st byte : ENG
			ucode += "00";							// assign unicode 1st half
			if(byte(utf_8[i]) < 0x10) {				// check if convert value has 1 digit?
				ucode += "0";
			}
			ucode += String(byte(utf_8[i]), HEX);	// assign unicode 2nd half
		}
		i++;
	}
    return ucode;
}









