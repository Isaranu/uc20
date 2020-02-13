#include "UC20_Call.h"

UC20_Call::UC20_Call() {
	
}

bool UC20_Call::call(String call_number) {
	gsm.print(F("ATD"));
	gsm.print(call_number);
	gsm.println(F(";"));
	return (gsm.waitStr(F("OK")) != "");
}

bool UC20_Call::answer() {
	gsm.println(F("ATA"));
	return (gsm.waitStr(F("OK")) != "");
}

bool UC20_Call::discall() {
	gsm.println(F("ATH"));
	return (gsm.waitStr(F("OK")) != "");
}

bool UC20_Call::ringSignal() {
	if(!_ringing)
		return false;
	else {
		_ringing = false;
		return true;
	}
}

int8_t UC20_Call::checkCallStat() {
	gsm.println(F("AT+CPAS"));
	String resp = gsm.waitStr(F("+CPAS:"));
	if(resp != "") {
		int8_t index = resp.indexOf(F(" ")) + 1;
		int8_t state = resp.substring(index).toInt();
		gsm.waitStr(F("OK"));
		return state;
	}
	return -1;
}

void UC20_Call::reportCallStat(unsigned long interval) {
	unsigned long curr_t = millis();
	if(curr_t - _prev_t > interval) {
		int8_t stat = checkCallStat();
		if(stat != IDLE)
			_idle = false;
		if(stat == RING) {
			_ringing = true;
			_call_in = true;
			gsm.debug->print(F("RING ("));
			gsm.debug->print(current_call_number());
			gsm.debug->println(F(")"));
		}
		else if(stat == CALL && !_report) {
			_ringing = false;
			_report  = true;
			if(_call_in)
				gsm.debug->println(F("Speaking"));
			else {
				gsm.debug->print(F("Dialing... ("));
				gsm.debug->print(current_call_number());
				gsm.debug->println(F(")"));
			}
		}
		else if(!_idle && stat == IDLE) {
			_idle    = true;
			_ringing = false;
			_call_in = false;
			_report  = false;
			gsm.debug->println(F("Call End"));
		}
		_prev_t = curr_t;
	}
}

String UC20_Call::current_call_number() {
	gsm.println(F("AT+CLCC"));
	String number = "";
	String resp = gsm.waitStr(F("+CLCC:"));
	if(resp != "") {
		int8_t index1 = resp.indexOf(F("\"")) + 1;
		int8_t index2 = resp.lastIndexOf(F("\""));
		number = resp.substring(index1, index2);
	}
	gsm.waitStr(F("OK"),1000);
	return number;
}









