#ifndef UC20_FTP_H
#define UC20_FTP_H

#include "Arduino.h"
#include "UC20_lib.h"

#define COM     "COM"

class UC20_FTP {
	public:
		UC20_FTP();
		bool setContext(int8_t ctxid);
		bool setUserPassword(String user, String pass);
		bool setFiletype(bool ascii);
		bool enableSSL();
		bool login(String server, unsigned int port = 21);
		bool logout();
		
		String getPath();
		bool setPath(String path);
		void list(String path = ".", String savename = "", String storage = COM);
		bool makeDir(String dirname);
		bool deleteDir(String dirname);
		bool deleteFile(String fn);
		bool rename(String old_name, String new_name);
		
		bool putFile(String fn, String localname, String storage = UFS, int startpos = -1);
		bool putFileCom(String fn, int len, int startpos = 0, bool beof = 1);
		size_t sendCom(String data);
		size_t sendCom(const char* data);
		bool waitPut();
		bool getFile(String fn, String localname, String storage = UFS, int startpos = -1);
		bool getFileCom(String fn, int startpos = -1, int len = -1);
		
	private:
		
};

#endif

// https://www.digitalocean.com/community/tutorials/how-to-set-up-vsftpd-for-a-user-s-directory-on-debian-9
// https://www.howtoforge.com/tutorial/ubuntu-vsftpd/
