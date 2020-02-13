#ifndef UC20_HTTP_H
#define UC20_HTTP_H

#include "Arduino.h"
#include "UC20_lib.h"

class UC20_HTTP {
	public:
		UC20_HTTP();
		bool setInit();
		bool setResponseHeader(bool enable);
		bool setRequestHeader(bool enable);
		bool enableSSL();
		
		bool setUrl(String domain, unsigned int t_out_sec = 80);
		bool get(unsigned int t_out_sec = 80);
		bool post(String data);
		bool beginPost(unsigned int len);
		bool waitPost();
		bool postFile(String fn, String storage = UFS);
		String readResp(bool toPrint = false, unsigned int t_out_sec = 80);
		bool readResp_save(String fn, String storage = UFS, unsigned int t_out_sec = 80);
		
	private:
		
};

#endif
