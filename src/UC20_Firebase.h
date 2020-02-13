#ifndef UC20_FIREBASE_H
#define UC20_FIREBASE_H

#include "Arduino.h"
#include "UC20_lib.h"
#include "UC20_HTTP.h"

class UC20_Firebase {
	public:
		UC20_Firebase();
		bool begin(String host, String auth);
		
		bool set(String thing, String JsObj);
		bool setInt(String thing, long data);
		bool setFloat(String thing, double data);
		bool setBool(String thing, bool data);
		bool setString(String thing, String data);
		
		String get(String thing);
		long getInt(String thing);
		double getFloat(String thing);
		bool getBool(String thing);
		String getString(String thing);
		
		String push(String thing, String JsObj);
		String pushInt(String thing, long data);
		String pushFloat(String thing, double data);
		String pushBool(String thing, bool data);
		String pushString(String thing, String data);
		
		bool remove(String thing);
		
	private:
		bool send_http(String method, String thing, String data = "");
		UC20_HTTP fb_http;
		String fb_host;
		String fb_auth;
};

#endif

// example : https://www.ioxhop.com/article/45/esp8266-esp8285-%E0%B8%81%E0%B8%B1%E0%B8%9A%E0%B8%81%E0%B8%B2%E0%B8%A3%E0%B9%83%E0%B8%8A%E0%B9%89%E0%B8%87%E0%B8%B2%E0%B8%99-firebase-%E0%B8%A3%E0%B8%B0%E0%B8%9A%E0%B8%9A%E0%B8%90%E0%B8%B2%E0%B8%99%E0%B8%82%E0%B9%89%E0%B8%AD%E0%B8%A1%E0%B8%B9%E0%B8%A5%E0%B9%80%E0%B8%A3%E0%B8%B5%E0%B8%A2%E0%B8%A5%E0%B9%84%E0%B8%97%E0%B8%A1%E0%B9%8C%E0%B8%88%E0%B8%B2%E0%B8%81-google
// firebase setup : https://www.instructables.com/id/Esp8266-Firebase-Connection/