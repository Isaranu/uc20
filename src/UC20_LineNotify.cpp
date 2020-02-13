#include "UC20_LineNotify.h"

UC20_LineNotify::UC20_LineNotify() {
	
}

void UC20_LineNotify::setInit() {
	line_http.setInit();
	line_http.setRequestHeader(true);
	line_http.enableSSL();
	line_http.setUrl(LINEHOST);
}

void UC20_LineNotify::setToken(String token) {
	line_token = token;
}

bool UC20_LineNotify::sendMessage(String msg) {
	String message = "message=" + msg;
	unsigned int m_len = message.length();
	createHeader(m_len);
	unsigned int all_len = _header.length() + m_len;
	
	if(line_http.beginPost(all_len)) {
		gsm.print(_header);
		gsm.print(message);
		gsm.waitStr(F("OK"));
		if(line_http.waitPost()) 
			return true;
		else 
			return false;
	}
	else
		return false;
	
}

String UC20_LineNotify::readResp(unsigned int t_out_sec) {
	return line_http.readResp(false, t_out_sec);
}

void UC20_LineNotify::createHeader(unsigned int msg_len) {
	_header =   "";
	
	_header +=	"POST /api/notify HTTP/1.1\r\n"
				"Host: notify-api.line.me\r\n"
				"Authorization: Bearer ";
	_header +=	line_token;
	_header +=	"\r\n"
				"Cache-Control: no-cache\r\n"
				"Connection: Keep-Alive\r\n"
				"Content-Type: application/x-www-form-urlencoded\r\n"
				"Content-Length: ";
	_header +=	String(msg_len, DEC);
	_header +=	"\r\n\r\n";
}









