#ifndef UC20_LINE_NOTIFY_H
#define UC20_LINE_NOTIFY_H

#include "Arduino.h"
#include "UC20_lib.h"
#include "UC20_HTTP.h"

#define LINEHOST "https://notify-api.line.me/api/notify"

class UC20_LineNotify {
	public:
		UC20_LineNotify();
		void setInit();
		void setToken(String token);
		bool sendMessage(String msg);
		String readResp(unsigned int t_out_sec = 80);
		
	private:
		void createHeader(unsigned int msg_len);
		UC20_HTTP line_http;
		String line_token = "";
		String _header = "";
};

#endif