#ifndef UC20_SSL_H
#define UC20_SSL_H

#include "Arduino.h"
#include "UC20_lib.h"

class UC20_SSL {
	public:
		UC20_SSL();
		bool sslversion(int8_t sctxid, int8_t version);
		bool ciphersuite(int8_t sctxid, unsigned int type);
		bool seclevel(int8_t sctxid, int8_t level);
		bool openSocket(int8_t contextid, int8_t sctxid, int8_t clientid, String ipaddr, unsigned int port, int8_t access_mode);
		bool openSocket(String ipaddr, unsigned int port, int8_t clientid);
		bool closeSocket(int8_t clientid);
		bool socketReady(int8_t clientid);
		bool beginSend(int8_t clientid, int len = -1);
		void send(String data);
		void send(uint8_t val);
		void send(uint8_t buf[], unsigned int len);
		void send(const char* buf, unsigned int len);
		bool endSend();
		int8_t waitRecv(unsigned long t_out = 12000);
		String readRecv(int8_t clientid, bool toPrint = false);
		bool recvEmpty();
		
	private:
		bool readbuff_empty = true;
};

#endif
