#include "UC20_Firebase.h"

UC20_Firebase::UC20_Firebase() {

}

bool UC20_Firebase::begin(String host, String auth) {
	host.toLowerCase();
	int index1 = host.indexOf(F("https://")) + 8;
	int index2 = host.indexOf(F("/"),index1);
	if(index2 == -1) 
		fb_host = host.substring(index1);
	else 
		fb_host = host.substring(index1,index2);
	fb_auth = auth;
	fb_http.setInit();
	fb_http.setRequestHeader(true);
	fb_http.enableSSL();
	if(fb_http.setUrl(host)) 
		return true;
	else 
		return false;
}

bool UC20_Firebase::set(String thing, String JsObj) {
	return send_http("PUT",thing,JsObj);
}

bool UC20_Firebase::setInt(String thing, long data) {
	return send_http("PUT",thing,String(data));
}

bool UC20_Firebase::setFloat(String thing, double data) {
	return send_http("PUT",thing,String(data));
}

bool UC20_Firebase::setBool(String thing, bool data) {
	if(data) 
		return send_http("PUT",thing,"true");
	else 
		return send_http("PUT",thing,"false");
}

bool UC20_Firebase::setString(String thing, String data) {
	String strdata = "\"" + data + "\"";
	return send_http("PUT",thing,strdata);
}

String UC20_Firebase::get(String thing) {
	send_http("GET",thing);
	return fb_http.readResp();
}

long UC20_Firebase::getInt(String thing) {
	send_http("GET",thing);
	String data = fb_http.readResp();
	return data.toInt();
}

double UC20_Firebase::getFloat(String thing) {
	send_http("GET",thing);
	String data = fb_http.readResp();
	return data.toFloat();
}

bool UC20_Firebase::getBool(String thing) {
	send_http("GET",thing);
	String data = fb_http.readResp();
	if(data == "true") 
		return true;
	else 
		return false;
}

String UC20_Firebase::getString(String thing) {
	send_http("GET",thing);
	String data = fb_http.readResp();
	int index1 = data.indexOf(F("\""));
	int index2 = data.indexOf(F("\""),index1+1);
	if((index1!=-1)&&(index2!=-1)) 
		return data.substring(index1+1,index2);
	else 
		return "";
}

		
String UC20_Firebase::push(String thing, String JsObj) {
	bool success = send_http("POST",thing,JsObj);
	if(success) {
		String data = fb_http.readResp();
		if(data.indexOf(F("name")) != -1) {
			int index1 = data.indexOf(F(":")) + 2;
			int index2 = data.lastIndexOf(F("\""));
			return data.substring(index1,index2);
		}
	}
	return "";
}

String UC20_Firebase::pushInt(String thing, long data) {
	bool success = send_http("POST",thing,String(data));
	if(success) {
		String data = fb_http.readResp();
		if(data.indexOf(F("name")) != -1) {
			int index1 = data.indexOf(F(":")) + 2;
			int index2 = data.lastIndexOf(F("\""));
			return data.substring(index1,index2);
		}
	}
	return "";
}

String UC20_Firebase::pushFloat(String thing, double data) {
	bool success = send_http("POST",thing,String(data));
	if(success) {
		String data = fb_http.readResp();
		if(data.indexOf(F("name")) != -1) {
			int index1 = data.indexOf(F(":")) + 2;
			int index2 = data.lastIndexOf(F("\""));
			return data.substring(index1,index2);
		}
	}
	return "";
}

String UC20_Firebase::pushBool(String thing, bool data) {
	bool success;
	if(data) 
		success = send_http("POST",thing,"true");
	else 
		success = send_http("POST",thing,"false");
	
	if(success) {
		String data = fb_http.readResp();
		if(data.indexOf(F("name")) != -1) {
			int index1 = data.indexOf(F(":")) + 2;
			int index2 = data.lastIndexOf(F("\""));
			return data.substring(index1,index2);
		}
	}
	return "";
}

String UC20_Firebase::pushString(String thing, String data) {
	String strdata = "\"" + data + "\"";
	bool success = send_http("POST",thing,strdata);
	if(success) {
		String data = fb_http.readResp();
		if(data.indexOf(F("name")) != -1) {
			int index1 = data.indexOf(F(":")) + 2;
			int index2 = data.lastIndexOf(F("\""));
			return data.substring(index1,index2);
		}
	}
	return "";
}

bool UC20_Firebase::remove(String thing) {
	return send_http("DELETE",thing,"0");
}

bool UC20_Firebase::send_http(String method, String thing, String data) {
	if((method != "GET") && (method != "POST") && (method != "PUT") && (method != "DELETE")) 
		return false;
	
	unsigned int data_len = data.length();
	String header = "";
	header += method + " /" + thing + ".json?auth=" + fb_auth + " HTTP/1.1\r\n";
	header += "Host: " + fb_host + "\r\n";
	header += "User-Agent: UC20_3G\r\n";
	if(method == "GET") {
		header += "\r\n";
	}
	else {
		header += "Content-Length: ";
		header += String(data_len);
		header += "\r\n\r\n";
	}
	// gsm.debug->print(header);	// debug
	// gsm.debug->print(data);		// debug
	fb_http.beginPost(header.length() + data_len);
	gsm.print(header);
	gsm.print(data);
	
	return fb_http.waitPost();
}









