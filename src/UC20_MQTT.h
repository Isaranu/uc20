#ifndef UC20_MQTT_H
#define UC20_MQTT_H

#include "Arduino.h"
#include "UC20_lib.h"
#include "UC20_TCP.h"
#include "UC20_SSL.h"

#define MQTT_SOCKET            3
#define MQTT_VERSION_3_1_1     4
#define MQTT_KEEPALIVE         60


class UC20_MQTT {
	public:
		UC20_MQTT();
		void setSSL(bool enable);
		bool connectServer(String broker_ip, unsigned int port);
		bool connect(const char* id, const char* user, const char* pass, const char* willTopic, uint8_t willQos, bool willRetain, const char* willMessage, bool cleanSession);
		bool connect(const char* id, const char* user, const char* pass);
		bool connect(const char* id);
		bool publish(const char* topic, const char* msg, bool retain = false);
		bool publish(String topic, String msg, bool retain = false);
		bool subscribe(const char* topic, unsigned int packetID = 0x0a, uint8_t QoS = 0x00);
		bool unsubscribe(const char* topic, unsigned int packetID = 0x0b);
		bool pingreq();
		bool disconnect();
		bool recvPublish();
		String getTopic();
		String getMessage();
		bool socketReady();
		bool closeSocket();
		
	private:
		UC20_TCP mq_tcp;
		UC20_SSL mq_ssl;
		bool ssl_en = false;
		String topic_i   = "";
		String message_i = "";
};

#endif
