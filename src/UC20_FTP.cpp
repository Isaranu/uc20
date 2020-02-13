#include "UC20_FTP.h"

UC20_FTP::UC20_FTP() {

}

bool UC20_FTP::setContext(int8_t ctxid) {
	gsm.print(F("AT+QFTPCFG=\"contextid\","));
	gsm.println(ctxid);
	return (gsm.waitStr(F("OK")) != "");
}

bool UC20_FTP::setUserPassword(String user, String pass) {
	gsm.print(F("AT+QFTPCFG=\"account\",\""));
	gsm.print(user);
	gsm.print(F("\",\""));
	gsm.print(pass);
	gsm.println(F("\""));
	return (gsm.waitStr(F("OK")) != "");
}

bool UC20_FTP::setFiletype(bool ascii) {
	gsm.print(F("AT+QFTPCFG=\"filetype\","));
	gsm.println(ascii, DEC);					// 0:Binary , 1:Ascii
	return (gsm.waitStr(F("OK")) != "");
}

bool UC20_FTP::enableSSL() {
	bool success = true;
	gsm.println(F("AT+QFTPCFG=\"ssltype\",1"));
	success &= gsm.waitStr(F("OK"),1000) != "";
	gsm.println(F("AT+QFTPCFG=\"sslctxid\",1"));
	success &= gsm.waitStr(F("OK"),1000) != "";
	gsm.println(F("AT+QSSLCFG=\"ciphersuite\",1,0XFFFF"));
	success &= gsm.waitStr(F("OK"),1000) != "";
	gsm.println(F("AT+QSSLCFG=\"seclevel\",1,0"));
	success &= gsm.waitStr(F("OK"),1000) != "";
	gsm.println(F("AT+QSSLCFG=\"sslversion\",1,1"));
	success &= gsm.waitStr(F("OK"),1000) != "";
	return success;
}

bool UC20_FTP::login(String server, unsigned int port) {
	gsm.print(F("AT+QFTPOPEN=\""));
	gsm.print(server);
	gsm.print(F("\","));
	gsm.println(port);
	
	String resp = gsm.waitStr(F("+QFTPOPEN:"),12000);
	if(resp != "") {
		if(resp.indexOf(F(" 0,0")) != -1) 
			return true;
		else 
			return false;
	}
	else 
		return false;	// time out
}

bool UC20_FTP::logout() {
	gsm.println(F("AT+QFTPCLOSE"));
	
	String resp = gsm.waitStr(F("+QFTPCLOSE:"),12000);
	if(resp != "") {
		if(resp.indexOf(F(" 0,0")) != -1)
			return true;
		else 
			return false;
	}
	else
		return false;
}

String UC20_FTP::getPath() {
	gsm.println(F("AT+QFTPPWD"));
	gsm.waitStr(F("OK"));
	String resp = gsm.waitStr(F("+QFTPPWD:"));
	if(resp != "") {
		if(resp.indexOf(" 0,")) {
			int8_t index1 = resp.indexOf(F("\"")) + 1;
			int8_t index2 = resp.lastIndexOf(F("\""));
			return resp.substring(index1,index2);
		}
		else 
			return "";
	}
	else 
		return "";
}

bool UC20_FTP::setPath(String path) {
	gsm.print(F("AT+QFTPCWD=\""));
	gsm.print(path);
	gsm.println(F("\""));
	gsm.waitStr(F("OK"));
	
	String resp = gsm.waitStr(F("+QFTPCWD:"));
	if(resp != "") {
		if(resp.indexOf(F(" 0,0")) != -1) 
			return true;
		else 
			return false;
	}
	else 
		return false;
}

void UC20_FTP::list(String path, String savename, String storage) {
	gsm.print(F("AT+QFTPLIST=\""));
	gsm.print(path);
	gsm.print(F("\",\""));
	if(storage == COM) {
		gsm.println(F("COM:\""));
		
		if(gsm.waitStr(F("CONNECT"),8000) == "") 
			return;
		
		gsm.readCharUntil_ser("\r\nOK\r\n");
		String resp = gsm.waitStr(F("+QFTPLIST:"));
		if(resp != "") {
			// success
		}
	}
	else {
		if(storage == UFS) {
			gsm.print(savename);
			gsm.println(F("\""));
		}
		else if(storage == RAM) {
			gsm.print(F("RAM:"));
			gsm.print(savename);
			gsm.println(F("\""));
		}
		else {
			gsm.println();
			gsm.waitStr(F("ERROR"));
			return;		// storage error
		}
		gsm.waitStr(F("OK"));
		gsm.waitStr(F("+QFTPLIST:"));
	}
}

bool UC20_FTP::makeDir(String dirname) {
	gsm.print(F("AT+QFTPMKDIR=\""));
	gsm.print(dirname);
	gsm.println(F("\""));
	
	gsm.waitStr(F("OK"));
	String resp = gsm.waitStr(F("+QFTPMKDIR:"),5000);
	if(resp != "") {
		if(resp.indexOf(F(" 0,0")) != -1) 
			return true;
		else 
			return false;
	}
	else 
		return false;
}

bool UC20_FTP::deleteDir(String dirname) {
	gsm.print(F("AT+QFTPRMDIR=\""));
	gsm.print(dirname);
	gsm.println(F("\""));
	
	gsm.waitStr(F("OK"));
	String resp = gsm.waitStr(F("+QFTPRMDIR:"),5000);
	if(resp != "") {
		if(resp.indexOf(F(" 0,0")) != -1) 
			return true;
		else 
			return false;
	}
	else 
		return false;
}

bool UC20_FTP::deleteFile(String fn) {
	gsm.print(F("AT+QFTPDEL=\""));
	gsm.print(fn);
	gsm.println(F("\""));
	
	gsm.waitStr(F("OK"));
	String resp = gsm.waitStr(F("+QFTPDEL:"),5000);
	if(resp != "") {
		if(resp.indexOf(F(" 0,0")) != -1)
			return true;
		else
			return false;
	}
	else
		return false;
}

bool UC20_FTP::rename(String old_name, String new_name) {
	gsm.print(F("AT+QFTPRENAME=\""));
	gsm.print(old_name);
	gsm.print(F("\",\""));
	gsm.print(new_name);
	gsm.println(F("\""));
	
	gsm.waitStr(F("OK"));
	String resp = gsm.waitStr(F("+QFTPRENAME:"),5000);
	if(resp != "") {
		if(resp.indexOf(F(" 0,0")) != -1) 
			return true;
		else 
			return false;
	}
	else 
		return false;
}

bool UC20_FTP::putFile(String fn, String localname, String storage, int startpos) {
	gsm.print(F("AT+QFTPPUT=\""));
	gsm.print(fn);
	gsm.print(F("\",\""));
	if(storage == UFS) {
		gsm.print(localname);
		gsm.print(F("\""));
	}
	else if(storage == RAM) {
		gsm.print(F("RAM:"));
		gsm.print(localname);
		gsm.print(F("\""));
	}
	else {
		gsm.println();
		gsm.waitStr(F("ERROR"));
		return false;
	}
	if(startpos != -1) {
		gsm.print(F(","));
		gsm.print(startpos);
	}
	gsm.println();
	
	return (gsm.waitStr(F("OK")) != "");
}

bool UC20_FTP::putFileCom(String fn, int len, int startpos, bool beof) {
	gsm.print(F("AT+QFTPPUT=\""));
	gsm.print(fn);
	gsm.print(F("\",\"COM:\""));
	if(startpos != -1) {
		gsm.print(F(","));
		gsm.print(startpos);
		if(len != -1) {
			gsm.print(F(","));
			gsm.print(len);
			gsm.print(F(","));
			gsm.print(beof);
		}
	}
	gsm.println();
	
	return (gsm.waitStr(F("CONNECT"),8000) != "");
}


size_t UC20_FTP::sendCom(String data) {
	return gsm.print(data);
}

size_t UC20_FTP::sendCom(const char* data) {
	return gsm.write(data,strlen(data));
}

bool UC20_FTP::waitPut() {
	String resp = gsm.waitStr(F("+QFTPPUT:"), 12000);
	if(resp != "") {
		int8_t index = resp.indexOf(F(" ")) + 1;
		int err = resp.substring(index).toInt();
		if(err == 0) 
			return true;
		else 
			return false;
	}
	else 
		return false;
}

bool UC20_FTP::getFile(String fn, String localname, String storage, int startpos) {
	gsm.print(F("AT+QFTPGET=\""));
	gsm.print(fn);
	gsm.print(F("\",\""));
	if(storage == UFS) {
		gsm.print(localname);
		gsm.print(F("\""));
	}
	else if(storage == RAM) {
		gsm.print(F("RAM:"));
		gsm.print(localname);
		gsm.print(F("\""));
	}
	else {
		gsm.println();
		gsm.waitStr(F("ERROR"));
		return false;
	}
	if(startpos != -1) {
		gsm.print(F(","));
		gsm.print(startpos);
	}
	gsm.println();
	
	gsm.waitStr(F("OK"));
	String resp = gsm.waitStr(F("+QFTPGET:"),12000);
	if(resp != "") {
		int8_t index = resp.indexOf(F(" ")) + 1;
		int err = resp.substring(index).toInt();
		if(err == 0) 
			return true;
		else
			return false;
	}
	else
		return false;
}

bool UC20_FTP::getFileCom(String fn, int startpos, int len) {
	gsm.print(F("AT+QFTPGET=\""));
	gsm.print(fn);
	gsm.print(F("\",\"COM:\""));
	if(startpos != -1) {
		gsm.print(F(","));
		gsm.print(startpos);
		if(len != -1) {
			gsm.print(F(","));
			gsm.print(len);
		}
	}
	gsm.println();
	
	if(gsm.waitStr(F("CONNECT"),12000) == "") 
		return false;
	
	gsm.readCharUntil_ser("\r\nOK\r\n");
	String resp = gsm.waitStr(F("+QFTPGET:"));
	if(resp != "") {
		int8_t index = resp.indexOf(F(" ")) + 1;
		int err = resp.substring(index).toInt();
		if(err == 0)
			return true;
		else
			return false;
	}
}









