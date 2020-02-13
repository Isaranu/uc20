#include "UC20_SSL.h"

UC20_SSL::UC20_SSL() {

}

bool UC20_SSL::sslversion(int8_t sctxid, int8_t version) {
	gsm.print(F("AT+QSSLCFG=\"sslversion\","));
	gsm.print(sctxid);
	gsm.print(F(","));
	gsm.println(version);
	
	return (gsm.waitStr(F("OK")) != "")? true:false;
}

bool UC20_SSL::ciphersuite(int8_t sctxid, unsigned int type) {
	unsigned int cipher_type[] = {0x0035,0x002F,0x0005,0x0004,0x000A,0x003D,0xFFFF};
	uint8_t i = 0;
	while(cipher_type[i] != type) {
		i++;
		if(i >= 7)
			return false;
	}
	gsm.print(F("AT+QSSLCFG=\"ciphersuite\","));
	gsm.print(sctxid);
	gsm.print(F(",0X"));
	if(type < 0x100)
		gsm.print(F("00"));
	if(type < 0x10)
		gsm.print(F("0"));
	gsm.println(type,HEX);
	
	return (gsm.waitStr(F("OK")) != "")? true:false;
}

bool UC20_SSL::seclevel(int8_t sctxid, int8_t level) {
	if(level < 0 || 2 < level)
		return false;
	gsm.print(F("AT+QSSLCFG=\"seclevel\","));
	gsm.print(sctxid);
	gsm.print(F(","));
	gsm.println(level);
	
	return (gsm.waitStr(F("OK")) != "")? true:false;
}

bool UC20_SSL::openSocket(int8_t contextid, int8_t sctxid, int8_t clientid, String ipaddr, unsigned int port, int8_t access_mode) {
	gsm.print(F("AT+QSSLOPEN="));
	gsm.print(contextid);
	gsm.print(F(","));
	gsm.print(sctxid);
	gsm.print(F(","));
	gsm.print(clientid);
	gsm.print(F(",\""));
	gsm.print(ipaddr);
	gsm.print(F("\","));
	gsm.print(port);
	gsm.print(F(","));
	gsm.println(access_mode);
	gsm.waitStr(F("OK"));
	
	String resp = gsm.waitStr(F("+QSSLOPEN"));
	if(resp != "") {
		int8_t index = resp.indexOf(F(","));
		int err_code = resp.substring(index).toInt();
		if(err_code == 0)
			return true;
		else
			return false;
	}
	else
		return false;
}

bool UC20_SSL::openSocket(String ipaddr, unsigned int port, int8_t clientid) {
	return openSocket(1, 1, clientid, ipaddr, port, 0);
}

bool UC20_SSL::closeSocket(int8_t clientid) {
	gsm.print(F("AT+QSSLCLOSE="));
	gsm.println(clientid);
	return (gsm.waitStr(F("OK"),12000) != "")? true:false;
}

bool UC20_SSL::socketReady(int8_t clientid) {
	gsm.print(F("AT+QSSLSTATE="));
	gsm.println(clientid);
	String resp = gsm.waitStr(F("+QSSLSTATE:"));
	if(resp != "") {
		int8_t index = 0;
		for(uint8_t i=0; i<5; i++) {
			index = resp.indexOf(F(","), index) + 1;
		}
		int8_t index2 = resp.indexOf(F(","), index);
		int8_t state = resp.substring(index, index2).toInt();
		return (state == 2);
	}
	return false;
}

bool UC20_SSL::beginSend(int8_t clientid, int len) {
	gsm.print(F("AT+QSSLSEND="));
	gsm.print(clientid);
	if(len != -1) {
		gsm.print(F(","));
		gsm.print(len);
	}
	gsm.println();
	return (gsm.waitStr(F(">")) != "");
}

void UC20_SSL::send(String data) {
	gsm.print(data);
}

void UC20_SSL::send(uint8_t val) {
	gsm.write(val);
}

void UC20_SSL::send(uint8_t buf[], unsigned int len) {
	gsm.write(buf, len);
}

void UC20_SSL::send(const char* buf, unsigned int len) {
	gsm.write(buf, len);
}

bool UC20_SSL::endSend() {
	gsm.write(0x1A);
	return (gsm.waitStr(F("SEND OK")) != "")? true:false;
}

int8_t UC20_SSL::waitRecv(unsigned long t_out) {
	String resp = gsm.waitStr(F("+QSSLURC: \"recv\""), t_out);
	if(resp != "") {
		int8_t index = resp.indexOf(F(",")) + 1;
		readbuff_empty = false;
		return resp.substring(index).toInt();
	}
	return -1;
}

String UC20_SSL::readRecv(int8_t clientid, bool toPrint) {
	gsm.print(F("AT+QSSLRECV="));
	gsm.print(clientid);
	if(!toPrint) 
		gsm.println(F(",100"));
	else 
		gsm.println(F(",1500"));
	
	int data_size = 0;
	String resp = gsm.waitStr(F("+QSSLRECV:"));
	if(resp != "") {
		String msg = "";
		uint8_t data_read;
		int8_t index = resp.indexOf(F(" ")) + 1;
		data_size = resp.substring(index).toInt();
		if(data_size == 0) {
			readbuff_empty = true;
			return "";
		}
		unsigned long prev_t;
		for(int i=0; i<data_size; i++) {
			prev_t = millis();
			while(!gsm.available() > 0) {
				if(millis() - prev_t > 1200) {
					gsm.debug->println("\nError: data loss");	// ##### Serial #####
					readbuff_empty = true;
					return "";
				}
			}
			data_read = gsm.read();
			if(!toPrint) 
				msg += char(data_read);
			else
				gsm.debug->write(data_read);					// ##### Serial #####
		}
		gsm.waitStr(F("OK"));
		if(!toPrint) 
			return msg;
		else 
			return "OK";
	}
	return "";
}

bool UC20_SSL::recvEmpty() {
	return readbuff_empty;
}







