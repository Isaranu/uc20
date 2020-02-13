#include "UC20_lib.h"

UC20 gsm;

UC20::UC20() {
	
}

void UC20::initSerial(Stream &serial) {
	_Serial = &serial;
}

void UC20::setDebugSerial(Stream &serial) {
	debug = &serial;
}

int UC20::available() {
	return _Serial->available();
}

int UC20::read() {
	return _Serial->read();
}

size_t UC20::write(uint8_t buf[], unsigned int len) {
	return _Serial->write(buf, len);
}

size_t UC20::write(const char* buf, unsigned int len) {
	return _Serial->write(buf, len);
}

size_t UC20::println() {
	return _Serial->print(F("\r\n"));
}

String UC20::readStringUntil(char terminate) {
	return _Serial->readStringUntil(terminate);
}

String UC20::readCharUntil_ser(const char* terminate) {
	int index = 0;
	int len = strlen(terminate);
	char c[len+1];
	unsigned long prev_t;
	while(1) {
		prev_t = millis();
		while(!available() > 0) {
			if(millis() - prev_t > 1200) 
				return "";
		}
		c[index] = read();
		c[index+1] = 0;
		if(c[index] == terminate[index]) {
			if(index == len-1) {
				String ret = String(c);
				ret += readStringUntil('\r');
				return ret;
			}
			index++;
		}
		else {
			if(index == 0) {
				debug->write(c[0]);							// ##### Serial #####
			}
			else {
				debug->write(c, index);						// ##### Serial #####
				if(c[index] == terminate[0]) {
					c[0] = c[index];
					index = 1;
				}
				else {
					debug->write(c[index]);					// ##### Serial #####
					index = 0;
				}
			}
		}
	}
}

String UC20::readCharUntil_buf(const char* terminate) {
	int index = 0;
	int len = strlen(terminate);
	char c[len+1];
	String data = "";
	unsigned long prev_t;
	while(1) {
		prev_t = millis();
		while(!available() > 0) {
			if(millis() - prev_t > 1200) 
				break;
		}
		c[index] = read();
		c[index+1] = 0;
		if(c[index] == terminate[index]) {
			if(index == len-1) {
				break;
			}
			index++;
		}
		else {
			if(index == 0) {
				data += c[0];
			}
			else {
				for(int i=0; i<index; i++) 
					data += c[i];
				if(c[index] == terminate[0]) {
					c[0] = c[index];
					index = 1;
				}
				else {
					data += c[index];
					index = 0;
				}
			}
		}
	}
	return data;
}

String UC20::waitStr(String str, unsigned long t_out, bool cleanLF) {
	unsigned long start_t = millis();
	do {
		String buff = readStringUntil('\r');
		// String buff_dum = String(buff);					//##### debug
		// buff_dum.replace("\r","<CR>");					//##### debug
		// buff_dum.replace("\n","<LF>");					//##### debug
		// debug->print("> ");								//##### debug
		// debug->println(buff_dum);						//##### debug
		if(buff.indexOf(str) != -1) {
			if(cleanLF) 
				readStringUntil('\n');
			return buff;
		}
		else {
			// do something
		}
	}
	while(millis()-start_t < t_out);
	
	return "";
}

bool UC20::powerOn(int pin) {
	println(F("AT"));
	if(waitStr(F("OK")) != "") {
		return true;
	}
	
	pinMode(pin, OUTPUT);
	digitalWrite(pin, HIGH);
	delay(1000);
	digitalWrite(pin, LOW);
	delay(1000);
	
	if(waitStr(F("RDY"),8000) != "") {
		waitStr(F("PB DONE"),15000);
		return true;
	}
	else
		return false;
}

bool UC20::powerOff(int pin, bool soft_powd) {
	if(soft_powd) {		// Software power off
		println(F("AT+QPOWD"));
		waitStr(F("OK"));
		if(waitStr(F("POWERED DOWN"),5000) != "") {
			return true;
		}
		return false;
	}
	else {		// Hardware power off
		pinMode(pin, OUTPUT);
		for(uint8_t i=0; i<2; i++) {
			digitalWrite(pin, HIGH);
			delay(1000);
			digitalWrite(pin, LOW);
			delay(1000);
			
			if(waitStr(F("POWERED DOWN"),5000) != "") {
				return true;
			}
		}
		return false;
	}
}

void UC20::initConfig() {
	println(F("ATV1"));
	waitStr(F("OK"),1000);
	println(F("ATE0"));
	waitStr(F("OK"),1000);
	println(F("AT+QURCCFG=\"urcport\",\"uart1\""));
	waitStr(F("OK"),1000);
}

bool UC20::network_regis_state() {
	println(F("AT+CREG?"));
	String resp = waitStr(F("+CREG:"));
	if(resp != "") {
		int8_t index = resp.indexOf(F(",")) + 1;
		int8_t state = resp.substring(index).toInt();
		waitStr(F("OK"),1000);
		if(state == 1) 
			return true;
		else
			return false;
	}
	else
		//timeout
		return false;
}

int8_t UC20::get_signal_quality() {
	println(F("AT+CSQ"));
	String resp = waitStr(F("+CSQ:"));
	if(resp != "") {
		int8_t index1 = resp.indexOf(F(" ")) + 1;
		int8_t index2 = resp.indexOf(F(","));
		int8_t rssi = resp.substring(index1,index2).toInt();
		waitStr(F("OK"),1000);
		return rssi;
	}
	else
		return -1;
}

String UC20::get_IMEI() {
	println(F("ATE1"));
	waitStr(F("OK"));
	println(F("AT+GSN"));
	String imei = "";
	if(waitStr(F("AT+GSN"),3000,false) != "") {
		imei = waitStr(F("\n"));
		imei.remove(0,1);
		waitStr(F("OK"),1000);
	}
	println(F("ATE0"));
	waitStr(F("OK"),1000);
	return imei;
}

String UC20::get_IMSI() {
	println(F("ATE1"));
	waitStr(F("OK"));
	println(F("AT+CIMI"));
	String imsi = "";
	if(waitStr(F("AT+CIMI"),3000,false) != "") {
		imsi = waitStr(F("\n"));
		imsi.remove(0,1);
		waitStr(F("OK"),1000);
	}
	println(F("ATE0"));
	waitStr(F("OK"),1000);
	return imsi;
}

String UC20::get_ICCID() {
	println(F("AT+QCCID"));
	String resp = waitStr(F("+QCCID:"));
	String iccid = "";
	if(resp != "") {
		int8_t index = resp.indexOf(F(" ")) + 1;
		iccid = resp.substring(index);
	}
	waitStr(F("OK"),1000);
	return iccid;
}

String UC20::get_operator() {
	println(F("AT+COPS?"));
	String resp = waitStr(F("+COPS:"));
	String oper = "";
	if(resp != "") {
		int8_t index1 = resp.indexOf(F("\"")) + 1;
		int8_t index2 = resp.lastIndexOf(F("\""));
		oper = resp.substring(index1,index2);
	}
	waitStr(F("OK"),1000);
	return oper;
}

void UC20::listFile(String filename, String storage) {
	if(storage == UFS) 
		print(F("AT+QFLST=\""));
	else if(storage == RAM) 
		print(F("AT+QFLST=\"RAM:"));
	else 
		return;
	print(filename);
	println(F("\""));
	String resp = "";
	do {
		resp = waitStr(F("+QFLST:"),1000);
		if(resp != "") {
			int8_t index1 = resp.indexOf(F("\"")) + 1;
			int8_t index2 = resp.lastIndexOf(F("\""));
			int8_t index3 = resp.indexOf(F(",")) + 1;
			debug->print(F("name: \""));						// ##### Serial #####
			debug->print(resp.substring(index1,index2));		// ##### Serial #####
			debug->print(F("\", size: "));						// ##### Serial #####
			debug->print(resp.substring(index3));				// ##### Serial #####
			debug->println(F(" bytes"));						// ##### Serial #####
		}
	}
	while(resp != "");
}

bool UC20::deleteFile(String filename, String storage) {
	if(storage == UFS) 
		print(F("AT+QFDEL=\""));
	else if(storage == RAM) 
		print(F("AT+QFDEL=\"RAM:"));
	else 
		return false;
	print(filename);
	println(F("\""));
	
	return (waitStr(F("OK")) != "");
}

bool UC20::loadFile(String filename, String storage) {
	if(storage == UFS) 
		print(F("AT+QFDWL=\""));
	else if(storage == RAM) 
		print(F("AT+QFDWL=\"RAM:"));
	else
		return false;
	print(filename);
	println(F("\""));
	
	if(waitStr(F("CONNECT")) == "") 
		return false;
	
	readCharUntil_ser("\r\n+QFDWL:");
  
	waitStr(F("OK"));
	return true;
}

int UC20::openFile(String filename, String storage, uint8_t mode) {
	// mode = 0 ; Create new if not exist. If exist, open it. Allow both read and write.
	//        1 ; Create new if not exist. If exist, overwrite. Allow both read and write.
	//        2 ; Respond error if not exist. If exist, open it. Allow read only.
	if(mode > 2)
		return -1;
	if(storage == UFS)
		print(F("AT+QFOPEN=\""));
	else if(storage == RAM)
		print(F("AT+QFOPEN=\"RAM:"));
	else
		return -1;
	print(filename);
	print(F("\","));
	println(mode);
	
	String resp = waitStr(F("+QFOPEN:"));
	if(resp != "") {
		int8_t index = resp.indexOf(F(" ")) + 1;
		int handle = resp.substring(index).toInt();
		latest_handle = handle;
		waitStr(F("OK"));
		return handle;
	}
	return -1;
}

bool UC20::readFile(int filehandle, long length) {
	int handle;
	if(filehandle == -1) {
		if(latest_handle == -1)
			return false;
		handle = latest_handle;
	}
	else
		handle = filehandle;
	print(F("AT+QFREAD="));
	print(handle);
	print(F(","));
	println(length);
	
	String resp = waitStr(F("CONNECT"));
	if(resp == "")
		return false;
	int8_t index = resp.indexOf(F(" ")) + 1;
	int data_size = resp.substring(index).toInt();
	
	unsigned long prev_t;
	for(int i=0; i<data_size; i++) {
		prev_t = millis();
		while(!available() > 0) {
			if(millis() - prev_t > 1200)
				return false;
		}
		debug->write(read());									//##### Serial #####
	}
	waitStr(F("OK"));
	return true;
}

bool UC20::beginWriteFile(int filehandle, long length, int t_out_sec) {
	int handle;
	if(filehandle == -1) {
		if(latest_handle == -1)
			return false;
		handle = latest_handle;
	}
	else
		handle = filehandle;
	print(F("AT+QFWRITE="));
	print(handle);
	print(F(","));
	print(length);
	print(F(","));
	println(t_out_sec);
	
	if(waitStr(F("CONNECT")) != "") 
		return true;
	else
		return false;
}

bool UC20::endWriteFile(unsigned long t_out) {
	if(waitStr(F("+QFWRITE:"),t_out) == "")
		return false;
	unsigned long prev_t = millis();
	do {
		String resp = readStringUntil('\n');
		if(resp.indexOf(F("+CME ERROR: 421")) != -1) 
			return true;
		else if(resp.indexOf(F("OK")) != -1) 
			return true;
	}
	while(millis()-prev_t < 1000);
	return true;
}

bool UC20::setFilePointer(unsigned int offset, uint8_t start_position, int filehandle) {
	// start_position = 0 ; The beginning of the file, offset to the right.
	//                = 1 ; The current position of the pointer, offset to the right.
	//                = 2 ; The end of the file, offset to the left.
	if(start_position > 2) 
		return false;
	int handle;
	if(filehandle == -1) {
		if(latest_handle == -1)
			return false;
		handle = latest_handle;
	}
	else
		handle = filehandle;
	print(F("AT+QFSEEK="));
	print(handle);
	print(F(","));
	print(offset);
	print(F(","));
	println(start_position);
	return (waitStr(F("OK")) != "");
}

bool UC20::setFilePointer_at(uint8_t position, int filehandle) {
	return setFilePointer(0, position, filehandle);
}

bool UC20::setFilePointer_toRight(unsigned int offset, int filehandle) {
	return setFilePointer(offset, CURRENT, filehandle);
}

int UC20::getFilePointer(int filehandle) {
	int handle;
	if(filehandle == -1) {
		if(latest_handle == -1)
			return false;
		handle = latest_handle;
	}
	else
		handle = filehandle;
	print(F("AT+QFPOSITION="));
	println(handle);
	String resp = waitStr(F("+QFPOSITION:"));
	if(resp != "") {
		int8_t index = resp.indexOf(F(" ")) + 1;
		int position = resp.substring(index).toInt();
		waitStr(F("OK"));
		return position;
	}
	return -1;
}

bool UC20::closeFile(int filehandle) {
	int handle;
	if(filehandle == -1) {
		if(latest_handle == -1)
			return false;
		handle = latest_handle;
		latest_handle = -1;
	}
	else
		handle = filehandle;
	print(F("AT+QFCLOSE="));
	println(handle);
	return (waitStr(F("OK")) != "");
}

void UC20::internetConfig(String apn, String user, String password) {
	print(F("AT+QICSGP=1,1,\""));
	print(apn);
	print(F("\",\""));
	print(user);
	print(F("\",\""));
	print(password);
	println(F("\",1"));
	
	waitStr(F("OK"));
}

bool UC20::connect_internet() {
	println(F("AT+QIACT=1"));
	return (waitStr(F("OK"),12000) != "");
}

bool UC20::disconnect_internet() {
	println(F("AT+QIDEACT=1"));
	return (waitStr(F("OK"),12000) != "");
}

String UC20::getIPaddress() {
	println(F("AT+QIACT?"));
	String IP = "";
	String resp = waitStr(F("+QIACT:"));
	if(resp != "") {
		int8_t index1 = resp.indexOf(F("\"")) + 1;
		int8_t index2 = resp.lastIndexOf(F("\""));
		IP = resp.substring(index1, index2);
	}
	waitStr(F("OK"),1000);
	return IP;
}

bool UC20::connected() {
	println(F("AT+QIACT?"));
	if(waitStr(F("+QIACT:"),1000) != "") {
		waitStr(F("OK"));
		return true;
	}
	else {
		println(F("AT+QIACT?"));
		if(waitStr(F("+QIACT:"),1000) != "") {
			waitStr(F("OK"));
			return true;
		}
		return false;
	}
}

bool UC20::send_ntp(String server, unsigned int port, bool settime) {
	print(F("AT+QNTP=1,\""));
	print(server);
	print(F("\","));
	print(port);
	print(F(","));
	println(settime, DEC);
	return (waitStr(F("OK")) != "");
}

String UC20::wait_ntp(unsigned long t_out) {
	String resp = waitStr(F("+QNTP:"), t_out);
	if(resp != "") {
		if(resp.indexOf(F(" 0,")) != -1) {
			int8_t index1 = resp.indexOf(F("\"")) + 1;
			int8_t index2 = resp.lastIndexOf(F("\""));
			return resp.substring(index1,index2);
		}
	}
	return "";
}

String UC20::moduleTime() {
	println(F("AT+CCLK?"));
	String ret = "";
	String resp = waitStr(F("+CCLK:"));
	if(resp != "") {
		int8_t index1 = resp.indexOf(F("\"")) + 1;
		int8_t index2 = resp.lastIndexOf(F("\""));
		ret = resp.substring(index1, index2);
		waitStr(F("OK"));
	}
	return ret;
}









