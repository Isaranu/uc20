#ifndef UC20_CALL_H
#define UC20_CALL_H

#include "Arduino.h"
#include "UC20_lib.h"

#define IDLE 0
#define RING 3
#define CALL 4

// NO CARRIER
// BUSY
// RING
// NO ANSWER

class UC20_Call {
	public:
		UC20_Call();
		bool call(String call_number);
		bool answer();
		bool discall();
		bool ringSignal();
		int8_t checkCallStat();
		void reportCallStat(unsigned long interval);
		String current_call_number();
		
	private:
		bool _idle    = true;
		bool _ringing = false;
		bool _call_in = false;
		bool _report  = false;
		unsigned long _prev_t = 0;
};

#endif
