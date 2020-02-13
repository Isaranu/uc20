#include "UC20_HTTP.h"

UC20_HTTP::UC20_HTTP() {
	
}

bool UC20_HTTP::setInit() {
	bool success = true;
	gsm.println(F("AT+QHTTPCFG=\"contextid\",1"));
	success &= gsm.waitStr(F("OK")) != "";
	gsm.println(F("AT+QHTTPCFG=\"requestheader\",0"));
	success &= gsm.waitStr(F("OK")) != "";
	gsm.println(F("AT+QHTTPCFG=\"responseheader\",0"));
	success &= gsm.waitStr(F("OK")) != "";
	return success;
}

bool UC20_HTTP::setResponseHeader(bool enable) {
	gsm.print(F("AT+QHTTPCFG=\"responseheader\","));
	gsm.println(enable, DEC);
	return (gsm.waitStr(F("OK")) != "");
}

bool UC20_HTTP::setRequestHeader(bool enable) {
	gsm.print(F("AT+QHTTPCFG=\"requestheader\","));
	gsm.println(enable, DEC);
	return (gsm.waitStr(F("OK")) != "");
}

bool UC20_HTTP::enableSSL() {
	bool success = true;
	gsm.println(F("AT+QHTTPCFG=\"sslctxid\",1"));
	success &= gsm.waitStr(F("OK"),1000) != "";
	gsm.println(F("AT+QSSLCFG=\"ciphersuite\",1,0XFFFF"));
	success &= gsm.waitStr(F("OK"),1000) != "";
	gsm.println(F("AT+QSSLCFG=\"seclevel\",1,0"));
	success &= gsm.waitStr(F("OK"),1000) != "";
	gsm.println(F("AT+QSSLCFG=\"sslversion\",1,1"));
	success &= gsm.waitStr(F("OK"),1000) != "";
	
	return success;
}

bool UC20_HTTP::setUrl(String domain, unsigned int t_out_sec) {
	int len = domain.length();
	gsm.print(F("AT+QHTTPURL="));
	gsm.print(len);
	gsm.print(F(","));
	gsm.println(t_out_sec);
	
	if(gsm.waitStr(F("CONNECT")) == "") 
		return false;
	
	gsm.print(domain);
	return (gsm.waitStr(F("OK")) != "");
}

bool UC20_HTTP::get(unsigned int t_out_sec) {
	gsm.print(F("AT+QHTTPGET="));
	gsm.println(t_out_sec);
	gsm.waitStr(F("OK"));
	
	String resp = gsm.waitStr(F("+QHTTPGET:"),8000);
	if(resp != "") {
		if(resp.indexOf(F(" 0,200")) != -1) 
			return true;
		else 
			return false;
	}
	else 
		return false;
}

bool UC20_HTTP::post(String data) {
	unsigned int len = data.length();
	if(!beginPost(len)) 
		return false;
	gsm.print(data);
	gsm.waitStr(F("OK"));
	
	return waitPost();
}

bool UC20_HTTP::beginPost(unsigned int len) {
	gsm.print(F("AT+QHTTPPOST="));
	gsm.println(len);
	
	if(gsm.waitStr(F("CONNECT"),15000) == "") 
		return false;
	else
		return true;
}

bool UC20_HTTP::waitPost() {
	String resp = gsm.waitStr(F("+QHTTPPOST:"),12000);
	if(resp != "") {
		if(resp.indexOf(F(" 0,200")) != -1) 
			return true;
		else 
			return false;
	}
	else 
		return false;
}

bool UC20_HTTP::postFile(String fn, String storage) {
	if(storage == UFS)
		gsm.print(F("AT+QHTTPPOSTFILE=\""));
	else if(storage == RAM) 
		gsm.print(F("AT+QHTTPPOSTFILE=\"RAM:"));
	else 
		return false;
	gsm.print(fn);
	gsm.println(F("\""));
	gsm.waitStr(F("OK"));
	
	String resp = gsm.waitStr(F("+QHTTPPOSTFILE:"),12000);
	if(resp != "") {
		if(resp.indexOf(F(" 0,200")) != -1) 
			return true;
		else 
			return false;
	}
	else 
		return false;
}

String UC20_HTTP::readResp(bool toPrint, unsigned int t_out_sec) {
	gsm.print(F("AT+QHTTPREAD="));
	gsm.println(t_out_sec);
	
	if(gsm.waitStr(F("CONNECT")) == "") 
		return "";
	
	String ret;
	if(toPrint) {
		ret = gsm.readCharUntil_ser("\r\nOK\r\n");				// ##### Serial #####
		gsm.waitStr(F("+QHTTPREAD: 0"));
	}
	else {
		ret = gsm.readCharUntil_buf("\r\nOK\r\n");
		gsm.waitStr(F("+QHTTPREAD: 0"));
	}
	return ret;
}

bool UC20_HTTP::readResp_save(String fn, String storage, unsigned int t_out_sec) {
	if(storage == UFS) {
		gsm.print(F("AT+QHTTPREADFILE=\""));
	}
	else if(storage == RAM) {
		gsm.print(F("AT+QHTTPREADFILE=\"RAM:"));
	}
	else {
		return false;
	}
	gsm.print(fn);
	gsm.print(F("\","));
	gsm.println(t_out_sec);
	
	gsm.waitStr(F("OK"));
	String resp = gsm.waitStr(F("+QHTTPREADFILE:"));
	
	return (resp.indexOf(F(" 0")) != -1);
}









