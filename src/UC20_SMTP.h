#ifndef UC20_SMTP_H
#define UC20_SMTP_H

#include "Arduino.h"
#include "UC20_lib.h"

#define TO       1
#define CC       2
#define BCC      3

#define ASCII    0
#define UTF8     1
#define GB2312   2
#define BIG5     3

class UC20_SMTP {
	public:
		UC20_SMTP();
		void setInit();
		bool setSSL(uint8_t type);
		void setServer(String server, unsigned int port = 25);
		void setAccount(String user, String pass);
		void setSenderName(String name, String email);
		
		void receiver(String email, uint8_t type = TO);
		void subject(String sub, uint8_t charset = ASCII);
		bool body(String content, uint8_t charset = ASCII);
		bool attachFile(uint8_t index, String filename, String storage = UFS);
		bool sendEmail(unsigned int t_out_sec = 60);
		bool clearDraft();
		
		void viewDraft();
		
	private:
	
};

#endif

/*
//	ตั้งค่า "การเข้าถึงของแอปที่มีความปลอดภัยน้อย"
//	- เข้าหน้า "บัญชีผู้ใช้ Google"
//	- เข้า tab ความปลอดภัย (ขวามือ)
//	- เลื่อนลงไปที่หัวข้อ "การเข้าถึงของแอปที่มีความปลอดภัยน้อย"
//	- คลิ๊กเข้าไปยัง "เปิดการเข้าถึง (ไม่แนะนำ)"
//	- กดเลื่อน slide bar
//	- ข้อความจะแสดงว่า "อนุญาติแอปที่มีความปลอดถัยน้อย: เปิด"
*/