#include "UC20_TCP.h"

UC20_TCP::UC20_TCP() {
	
}

bool UC20_TCP::openSocket(int8_t contextid, int8_t connectid, String service_type, String ipaddr, unsigned int dest_port, unsigned int src_port, int8_t access_mode) {
	// AT+QIOPEN=<ctxid>,<cntid>,"<service>","<ipaddr>",<dest_port>,<src_port>,<access_mode>
	gsm.print(F("AT+QIOPEN="));
	gsm.print(String(contextid));
	gsm.print(F(","));
	gsm.print(String(connectid));
	gsm.print(F(",\""));
	gsm.print(service_type);
	gsm.print(F("\",\""));
	gsm.print(ipaddr);
	gsm.print(F("\","));
	gsm.print(dest_port, DEC);
	gsm.print(F(","));
	gsm.print(src_port, DEC);
	gsm.print(F(","));
	gsm.println(access_mode, DEC);
	gsm.waitStr(F("OK"));
	
	String resp = gsm.waitStr(F("+QIOPEN:"),10000);
	if(resp != "") {
		int8_t index = resp.indexOf(F(",")) + 1;
		int err_code = resp.substring(index).toInt();
		if(err_code == 0) 
			return true;
		else 
			return false;
	}
	else 
		return false;
}

bool UC20_TCP::openSocket(String service, String ipaddr, unsigned int port, int8_t connectid) {
	if((service != TCP) && (service != UDP)) 
		return false;
	return openSocket(1, connectid, service, ipaddr, port, 0, 0);
}

bool UC20_TCP::closeSocket(int8_t connectid) {
	gsm.print(F("AT+QICLOSE="));
	gsm.println(connectid);
	return (gsm.waitStr(F("OK"),12000) != "");
}

bool UC20_TCP::socketReady(int8_t connectid) {
	gsm.print(F("AT+QISTATE=1,"));
	gsm.println(connectid);
	String resp = gsm.waitStr(F("+QISTATE:"));
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

bool UC20_TCP::beginSend(int8_t connectid, int len) {
	gsm.print(F("AT+QISEND="));
	gsm.print(connectid);
	if(len != -1) {
		gsm.print(F(","));
		gsm.print(len);
	}
	gsm.println();
	if(gsm.waitStr(F(">")) != "") 
		return true;
	else
		return false;	
}

void UC20_TCP::send(String data) {
	gsm.print(data);
}

void UC20_TCP::send(uint8_t val) {
	gsm.write(val);
}

void UC20_TCP::send(uint8_t buf[], unsigned int len) {
	gsm.write(buf, len);
}

void UC20_TCP::send(const char* buf, unsigned int len) {
	gsm.write(buf, len);
}

bool UC20_TCP::endSend() {
	gsm.write(0x1A);
	return (gsm.waitStr(F("SEND OK")) != "");
}

int8_t UC20_TCP::waitRecv(unsigned long t_out) {
	String resp = gsm.waitStr(F("+QIURC: \"recv\""), t_out);
	if(resp != "") {
		int8_t index = resp.indexOf(F(",")) + 1;
		return resp.substring(index).toInt();
	}
	return -1;
}

long UC20_TCP::remainUnread(int8_t connectid) {
	gsm.print(F("AT+QIRD="));
	gsm.print(connectid);
	gsm.println(F(",0"));
	
	String resp = gsm.waitStr(F("+QIRD:"));
	if(resp != "") {
		int8_t index = resp.lastIndexOf(F(",")) + 1;
		gsm.waitStr(F("OK"));
		return resp.substring(index).toInt();
	}
	return -1;
}

String UC20_TCP::readRecv(int8_t connectid, bool toPrint) {
	gsm.print(F("AT+QIRD="));
	gsm.print(connectid);
	if(!toPrint) 
		gsm.println(F(",100"));
	else 
		gsm.println(F(",1500"));
	
	int data_size = 0;
	String resp = gsm.waitStr(F("+QIRD:"));
	if(resp != "") {
		String msg = "";
		uint8_t data_read;
		int8_t index = resp.indexOf(F(" ")) + 1;
		data_size = resp.substring(index).toInt();
		
		unsigned long prev_t;
		for(int i=0; i<data_size; i++) {
			prev_t = millis();
			while(!gsm.available() > 0) {
				if(millis() - prev_t > 1200) {
					gsm.debug->println(F("\nERROR: data loss"));	// ##### Serial #####
					return "";
				}
			}
			data_read = gsm.read();
			if(!toPrint) 
				msg += char(data_read);
			else 
				gsm.debug->write(data_read);						// ##### Serial #####
		}
		gsm.waitStr(F("OK"));
		if(!toPrint) 
			return msg;
		else 
			return "OK";
	}
	return "";
}









