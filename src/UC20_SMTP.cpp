#include "UC20_SMTP.h"

UC20_SMTP::UC20_SMTP() {
	
}

void UC20_SMTP::setInit() {
	gsm.println(F("AT+QSMTPCFG=\"contextid\",1"));
	gsm.waitStr(F("OK"),1000);
	gsm.println(F("AT+QSMTPCFG=\"ssltype\",0"));
	gsm.waitStr(F("OK"),1000);
}

bool UC20_SMTP::setSSL(uint8_t type) {
	if(type > 2) 
		return false;
	
	bool success = true;
	gsm.print(F("AT+QSMTPCFG=\"ssltype\","));
	gsm.println(type);
	success &= gsm.waitStr(F("OK"),1000) != "";
	if(type != 0) {
		gsm.println(F("AT+QSMTPCFG=\"sslctxid\",1"));
		success &= gsm.waitStr(F("OK"),1000) != "";
		gsm.println(F("AT+QSSLCFG=\"ciphersuite\",1,0xffff"));
		success &= gsm.waitStr(F("OK"),1000) != "";
		gsm.println(F("AT+QSSLCFG=\"seclevel\",1,0"));
		success &= gsm.waitStr(F("OK"),1000) != "";
		gsm.println(F("AT+QSSLCFG=\"sslversion\",1,1"));
		success &= gsm.waitStr(F("OK"),1000) != "";
	}
	return success;
}

void UC20_SMTP::setServer(String server, unsigned int port) {
	gsm.print(F("AT+QSMTPCFG=\"smtpserver\",\""));
	gsm.print(server);
	gsm.print(F("\","));
	gsm.println(port);
	gsm.waitStr(F("OK"),1000);
}

void UC20_SMTP::setAccount(String user, String pass) {
	gsm.print(F("AT+QSMTPCFG=\"account\",\""));
	gsm.print(user);
	gsm.print(F("\",\""));
	gsm.print(pass);
	gsm.println(F("\""));
	gsm.waitStr(F("OK"),1000);
}

void UC20_SMTP::setSenderName(String name, String email) {
	gsm.print(F("AT+QSMTPCFG=\"sender\",\""));
	gsm.print(name);
	gsm.print(F("\",\""));
	gsm.print(email);
	gsm.println(F("\""));
	gsm.waitStr(F("OK"),1000);
}
		
void UC20_SMTP::receiver(String email, uint8_t type) {
	gsm.print(F("AT+QSMTPDST=1,"));
	gsm.print(type);
	gsm.print(F(",\""));
	gsm.print(email);
	gsm.println(F("\""));
	gsm.waitStr(F("OK"),1000);
}

void UC20_SMTP::subject(String sub, uint8_t charset) {
	gsm.print(F("AT+QSMTPSUB="));
	gsm.print(charset);
	gsm.print(F(",\""));
	gsm.print(sub);
	gsm.println(F("\""));
	gsm.waitStr(F("OK"));
}

bool UC20_SMTP::body(String content, uint8_t charset) {
	uint8_t len = content.length();
	gsm.print(F("AT+QSMTPBODY="));
	gsm.print(charset);
	gsm.print(F(","));
	gsm.print(len);
	gsm.println(F(",60"));
	
	if(gsm.waitStr(F("CONNECT")) == "") 
		return false;
	
	gsm.print(content);
	if(gsm.waitStr(F("+QSMTPBODY:")) != "") {
		gsm.waitStr(F("OK"));
		return true;
	}
	else 
		return false;
}

bool UC20_SMTP::attachFile(uint8_t index, String filename, String storage) {
	if((index <= 0) && (index > 10)) 
		return false;
	gsm.print(F("AT+QSMTPATT=1,"));
	gsm.print(index);
	gsm.print(F(",\""));
	if(storage == RAM) 
		gsm.print(F("RAM:"));
	gsm.print(filename);
	gsm.println(F("\""));
	return (gsm.waitStr(F("OK")) != "");		
}

bool UC20_SMTP::sendEmail(unsigned int t_out_sec) {
	gsm.print(F("AT+QSMTPPUT="));
	gsm.println(t_out_sec);
	gsm.waitStr(F("OK"));
	
	String resp = gsm.waitStr(F("+QSMTPPUT:"),8000);
	if(resp != "") {
		if(resp.indexOf(F(" 0,0")) != -1) 
			return true;
		else 
			return false;
	}
	else 
		return false;
}

bool UC20_SMTP::clearDraft() {
	// clear content of receiver, subject, body and attachment
	gsm.println(F("AT+QSMTPCLR"));
	return (gsm.waitStr(F("OK")) != "");
}

void UC20_SMTP::viewDraft() {
	String resp = "";
	int8_t index1 = -1;
	int8_t index2 = -1;
	
	gsm.println(F("AT+QSMTPDST?"));
	do {
		resp = gsm.waitStr(F("+QSMTPDST:"),1000);
		if(resp != "") {
			index1 = resp.indexOf(F(" ")) + 1;
			if(resp[index1] == '1') 
				gsm.debug->print("To : ");
			else if(resp[index1] == '2') 
				gsm.debug->print("CC : ");
			else if(resp[index1] == '3') 
				gsm.debug->print("BCC: ");
			else 
				gsm.debug->print("   : ");
			index1 = resp.indexOf(F(",")) + 2;
			index2 = resp.lastIndexOf(F("\""));
			gsm.debug->println(resp.substring(index1,index2));
		}
	}
	while(resp != "");
	
	gsm.println(F("AT+QSMTPSUB?"));
	resp = gsm.waitStr(F("+QSMTPSUB:"));
	if(resp != "") {
		index1 = resp.indexOf(F(",")) + 2;
		index2 = resp.lastIndexOf(F("\""));
		gsm.debug->print(F("subject: "));
		gsm.debug->println(resp.substring(index1,index2));
	}
	
	gsm.println(F("AT+QSMTPATT?"));
	do {
		resp = gsm.waitStr(F("+QSMTPATT:"),1000);
		if(resp != "") {
			index1 = resp.indexOf(F(" ")) + 1;
			index2 = resp.indexOf(F(","));
			gsm.debug->print(F("attach"));
			gsm.debug->print(resp.substring(index1,index2));
			index1 = resp.indexOf(F("\"")) + 1;
			index2 = resp.lastIndexOf(F("\""));
			gsm.debug->print(F(": "));
			gsm.debug->println(resp.substring(index1,index2));
		}
	}
	while(resp != "");
}









