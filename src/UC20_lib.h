#ifndef UC20_H
#define UC20_H

#include <Arduino.h>
#include <Stream.h>

#define DEBUG_SERIAL Serial
#define POWERPIN 4
#define UFS      "UFS"
#define RAM      "RAM"
#define BEGIN    0
#define CURRENT  1
#define END      2

class UC20 {
	public:
		UC20();
		template <typename t> size_t print  (t data)             {return(_Serial->print(data));}
		template <typename t> size_t print  (t data, int format) {return(_Serial->print(data, format));}
		template <typename t> size_t println(t data)             {return(_Serial->println(data));}
		template <typename t> size_t println(t data, int format) {return(_Serial->println(data, format));}
		template <typename t> size_t write(t data)               {return(_Serial->write(data));}
		void initSerial(Stream &serial);
		int available();
		int read();
		size_t write(uint8_t buf[], unsigned int len);
		size_t write(const char* buf, unsigned int len);
		size_t println();
		String readStringUntil(char data);
		String readCharUntil_ser(const char* terminate);
		String readCharUntil_buf(const char* terminate);
		String waitStr(String str, unsigned long t_out = 3000, bool cleanLF = true);
		
		bool powerOn(int pin = POWERPIN);
		bool powerOff(int pin = POWERPIN, bool soft_powd = true);
		void initConfig();
		bool network_regis_state();
		int8_t get_signal_quality();
		String get_IMEI();
		String get_IMSI();
		String get_ICCID();
		String get_operator();
		
		void listFile(String filename = "*", String storage = UFS);
		bool deleteFile(String filename, String storage = UFS);
		bool loadFile(String filename, String storage = UFS);
		int openFile(String filename, String storage = UFS, uint8_t mode = 0);
		bool readFile(int filehandle = -1, long length = 10240);
		bool beginWriteFile(int filehandle = -1, long length = 10000, int t_out_sec = 5);
		bool endWriteFile(unsigned long t_out = 6000);
		bool setFilePointer(unsigned int offset, uint8_t start_position = 0, int filehandle = -1);
		bool setFilePointer_at(uint8_t position, int filehandle = -1);
		bool setFilePointer_toRight(unsigned int offset, int filehandle = -1);
		int getFilePointer(int filehandle = -1);
		bool closeFile(int filehandle = -1);
		
		void internetConfig(String apn, String user, String password);
		bool connect_internet();
		bool disconnect_internet();
		String getIPaddress();
		bool connected();
		bool send_ntp(String server, unsigned int port = 123, bool settime = true);
		String wait_ntp(unsigned long t_out = 12000);
		String moduleTime();
		
		void setDebugSerial(Stream &serial);
		Stream* debug = &DEBUG_SERIAL;
		
	protected:
		Stream* _Serial;
		
	private:
		int latest_handle = -1;
};
extern UC20 gsm;

#endif
