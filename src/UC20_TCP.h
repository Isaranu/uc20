#ifndef UC20_TCP_H
#define UC20_TCP_H

#include "Arduino.h"
#include "UC20_lib.h"

#define TCP "TCP"
#define UDP "UDP"

// +QIURC: "closed",x
// +QIURC: "pdpdeact",x
// +QIURC: "recv",x

class UC20_TCP {
	public:
		UC20_TCP();
		bool openSocket(int8_t contextid, int8_t connectid, String service_type, String ipaddr, unsigned int dest_port, unsigned int src_port, int8_t access_mode);
		bool openSocket(String service, String ipaddr, unsigned int port, int8_t connectid);
		bool closeSocket(int8_t connectid);
		bool socketReady(int8_t connectid);
		bool beginSend(int8_t connectid, int len = -1);
		void send(String data);
		void send(uint8_t val);
		void send(uint8_t buf[], unsigned int len);
		void send(const char* buf, unsigned int len);
		bool endSend();
		int8_t waitRecv(unsigned long t_out = 12000);
		long remainUnread(int8_t connectid);
		String readRecv(int8_t connectid, bool toPrint = false);
		
	private:
		
};

#endif
