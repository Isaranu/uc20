#include "UC20_MQTT.h"

UC20_MQTT::UC20_MQTT() {
	
}

void UC20_MQTT::setSSL(bool enable) {
	mq_tcp.closeSocket(MQTT_SOCKET);
	if(enable) {
		mq_ssl.sslversion(1,1);
		mq_ssl.ciphersuite(1,0xffff);
		mq_ssl.seclevel(1,0);
		ssl_en = true;
	}
	else 
		ssl_en = false;
}

bool UC20_MQTT::connectServer(String broker_ip, unsigned int port) {
	if(ssl_en) 
		return mq_ssl.openSocket(broker_ip, port, MQTT_SOCKET);
	else 
		return mq_tcp.openSocket(TCP, broker_ip, port, MQTT_SOCKET);
}

bool UC20_MQTT::connect(const char* id, const char* user, const char* pass, const char* willTopic, uint8_t willQos, bool willRetain, const char* willMessage, bool cleanSession) {
	unsigned int id_len, wTopic_len, wMsg_len, user_len, pass_len;
	
	uint8_t length_packet = 0x00;
	uint8_t header_ver[] = {0x00,0x04,'M','Q','T','T',MQTT_VERSION_3_1_1};
	length_packet += sizeof(header_ver);
	
	uint8_t conn_flags = 0x00;
	if(willTopic)
		conn_flags |= 0x04|(willQos<<3)|(willRetain<<5);
	if(cleanSession)
		conn_flags |= 0x02;
	if(user != NULL) {
		conn_flags |= 0x80;
		if(pass != NULL) 
			conn_flags |= 0x40;
	}
	length_packet += 1;						// conn_flags length
	length_packet += 2;						// KEEPALIVE length
	id_len = strlen(id);
	length_packet += id_len+2;				// clientID length
	if(willTopic) {
		wTopic_len = strlen(willTopic);
		length_packet += wTopic_len+2;		// willTopic length
		wMsg_len = strlen(willMessage);
		length_packet += wMsg_len+2;		// willMessage length
	}
	if(user != NULL) {
		user_len = strlen(user);
		length_packet += user_len+2;		// username length
		if(pass != NULL) {
			pass_len = strlen(pass);
			length_packet += pass_len+2;	// password length
		}
	}
	
	if(length_packet < 126) {
		if(ssl_en) 
			mq_ssl.beginSend(MQTT_SOCKET,length_packet+2);
		else 
			mq_tcp.beginSend(MQTT_SOCKET,length_packet+2);
		
		gsm.write(0x10);								// connect
		gsm.write(length_packet);
		
		gsm.write(header_ver, sizeof(header_ver));
		gsm.write(conn_flags);
		gsm.write((MQTT_KEEPALIVE&0xFF00) >> 8);
		gsm.write(MQTT_KEEPALIVE&0xFF);
		
		gsm.write((id_len&0xFF00) >> 8);				// clientID
		gsm.write(id_len&0xFF);
		gsm.write(id, id_len);
		
		if(willTopic) {
			gsm.write((wTopic_len&0xFF00) >> 8);		// willTopic
			gsm.write(wTopic_len&0xFF);
			gsm.write(willTopic, wTopic_len);
			gsm.write((wMsg_len&0xFF00) >> 8);			// willMessage
			gsm.write(wMsg_len&0xFF);
			gsm.write(willMessage, wMsg_len);
		}
		
		if(user != NULL) {
			gsm.write((user_len&0xFF00) >> 8);		// username
			gsm.write(user_len&0xFF);
			gsm.write(user, user_len);
			if(pass != NULL) {
				gsm.write((pass_len&0xFF00) >> 8);	// password
				gsm.write(pass_len&0xFF);
				gsm.write(pass, pass_len);
			}
		}
		if(gsm.waitStr(F("SEND OK")) != "") {
			int8_t connid;
			if(ssl_en) 
				connid = mq_ssl.waitRecv();
			else 
				connid = mq_tcp.waitRecv();
			
			if(connid == MQTT_SOCKET) { 
				// connack : 20 02 00 00
				String connack;
				if(ssl_en) 
					connack = mq_ssl.readRecv(MQTT_SOCKET);
				else 
					connack = mq_tcp.readRecv(MQTT_SOCKET);
				
				if(connack[3] == 0x00) 
					return true;
				else {
					gsm.debug->print(F("connack : "));				// ## debug
					for(uint8_t i=0; i<connack.length(); i++) {		// ## debug
						gsm.debug->print(connack[i]&0xFF,HEX);		// ## debug
						gsm.debug->print(" ");						// ## debug
					}												// ## debug
					gsm.debug->println();							// ## debug
					return false;		// error return from server
				}
			}
			else 
				return false;			// no incoming message
		}
		else
			return false;				// send error
	}
	else
		return(false);					// packet too long
}

bool UC20_MQTT::connect(const char* id, const char* user, const char* pass) {
	return connect(id,user,pass,0,0,0,0,1);
}

bool UC20_MQTT::connect(const char* id) {
	return connect(id,NULL,NULL,0,0,0,0,1);
}

bool UC20_MQTT::publish(const char* topic, const char* msg, bool retain) {
	unsigned int topic_len, msg_len;
	
	uint8_t header = 0x30;
	if(retain)
		header |= 0x01;
	uint8_t length_packet = 0x00;
	topic_len = strlen(topic);
	length_packet += 2+topic_len;		// topic_length
	msg_len = strlen(msg);
	length_packet += msg_len;			// message_length
	
	if(length_packet < 126) {
		if(ssl_en) 
			mq_ssl.beginSend(MQTT_SOCKET,length_packet+2);
		else 
			mq_tcp.beginSend(MQTT_SOCKET,length_packet+2);
		
		gsm.write(header);
		gsm.write(length_packet);
		gsm.write((topic_len&0xFF00) >> 8);
		gsm.write(topic_len&0xFF);
		gsm.write(topic, topic_len);
		gsm.write(msg, msg_len);
		
		return (gsm.waitStr(F("SEND OK")) != "");
	}
	else 
		return false;
}

bool UC20_MQTT::publish(String topic, String msg, bool retain) {
	int length = topic.length() + msg.length() + 4;
	if(length < 128) {
		char cTop[topic.length()+2];
		char cMsg[msg.length()+2];
		for(uint8_t i=0; i<topic.length(); i++) {
			cTop[i]   = topic[i];
			cTop[i+1] = '\0';
		}
		for(uint8_t i=0; i<msg.length(); i++) {
			cMsg[i]   = msg[i];
			cMsg[i+1] = '\0';
		}
		return publish(cTop, cMsg, retain);
	}
	else
		return false;
}

bool UC20_MQTT::subscribe(const char* topic, unsigned int packetID, uint8_t QoS) {
	unsigned int topic_len;
	
	uint8_t length_packet = 0x00;
	length_packet += 2;					// packetid_length
	topic_len = strlen(topic);
	length_packet += topic_len+2;		// topic_length
	length_packet += 1;					// QoS_length
	
	if(length_packet < 126) {
		if(ssl_en) 
			mq_ssl.beginSend(MQTT_SOCKET,length_packet+2);
		else 
			mq_tcp.beginSend(MQTT_SOCKET,length_packet+2);
		
		gsm.write(0x82);
		gsm.write(length_packet);
		gsm.write((packetID&0xFF00) >> 8);
		gsm.write(packetID&0xFF);
		gsm.write((topic_len&0xFF00) >> 8);
		gsm.write(topic_len&0xFF);
		gsm.write(topic, topic_len);
		gsm.write(QoS);
		
		if(gsm.waitStr(F("SEND OK")) != "") {
			int8_t connid;
			if(ssl_en) 
				connid = mq_ssl.waitRecv();
			else 
				connid = mq_tcp.waitRecv();
			
			if(connid == MQTT_SOCKET) {
				// suback : 90 03 00xx 00
				String suback;
				if(ssl_en) 
					suback = mq_ssl.readRecv(MQTT_SOCKET);
				else 
					suback = mq_tcp.readRecv(MQTT_SOCKET);
				
				if((suback[4]&0x80) != 0x80) 
					return true;
				else {
					gsm.debug->print(F("suback : "));				// ## debug
					for(uint8_t i=0; i<suback.length(); i++) {		// ## debug
						gsm.debug->print(suback[i]&0xFF,HEX);		// ## debug
						gsm.debug->print(" ");						// ## debug
					}												// ## debug
					gsm.debug->println();							// ## debug
					return false;		// return code : FAIL
				}
			}
			else
				return false;			// no incoming msg
		}
		else
			return false;				// send error
	}
	else
		return false;					// packet too long
}

bool UC20_MQTT::unsubscribe(const char* topic, unsigned int packetID) {
	unsigned int topic_len;
	
	uint8_t length_packet = 0x00;
	length_packet += 2;						// packetid_length
	topic_len = strlen(topic);
	length_packet += topic_len+2;			// topic_length
	
	if(length_packet < 126) {
		if(ssl_en) 
			mq_ssl.beginSend(MQTT_SOCKET,length_packet+2);
		else 
			mq_tcp.beginSend(MQTT_SOCKET,length_packet+2);
		
		gsm.write(0xa2);
		gsm.write(length_packet);
		gsm.write((packetID&0xFF00) >> 8);
		gsm.write(packetID&0xFF);
		gsm.write((topic_len&0xFF00) >> 8);
		gsm.write(topic_len&0xFF);
		gsm.write(topic, topic_len);
		
		if(gsm.waitStr(F("SEND OK")) != "") {
			int8_t connid;
			if(ssl_en) 
				connid = mq_ssl.waitRecv();
			else 
				connid = mq_tcp.waitRecv();
			
			if(connid == MQTT_SOCKET) {
				// unsuback : b0 02 00xx
				// String unsuback;
				// if(ssl_en) 
					// unsuback = mq_ssl.readRecv(MQTT_SOCKET);
				// else 
					// unsuback = mq_tcp.readRecv(MQTT_SOCKET);
				
				return true;
			}
			else 
				return false;			// no incoming msg
		}
		else 
			return false;				// send error
	}
	else
		return false;					// packet too long
}

bool UC20_MQTT::pingreq() {
	if(ssl_en) 
		mq_ssl.beginSend(MQTT_SOCKET,2);
	else 
		mq_tcp.beginSend(MQTT_SOCKET,2);
	
	gsm.write(0xc0);
	gsm.write(0x00);
	
	if(gsm.waitStr(F("SEND OK")) != "") {
		int8_t connid;
		if(ssl_en) 
			connid = mq_ssl.waitRecv();
		else 
			connid = mq_tcp.waitRecv();
		if(connid == MQTT_SOCKET) {
			// pingresp : d0 00
			// String pingresp;
			// if(ssl_en) 
				// pingresp = mq_ssl.readRecv(MQTT_SOCKET);
			// else 
				// pingresp = mq_tcp.readRecv(MQTT_SOCKET);
			
			return true;
		}
		else
			return false;
	}
	else
		return false;
}

bool UC20_MQTT::disconnect() {
	if(ssl_en) 
		mq_ssl.beginSend(MQTT_SOCKET,2);
	else 
		mq_tcp.beginSend(MQTT_SOCKET,2);
	
	gsm.write(0xe0);
	gsm.write(0x00);
	
	return (gsm.waitStr(F("SEND OK")) != "");
}

bool UC20_MQTT::recvPublish() {
	int8_t connid;
	if(ssl_en) 
		connid = mq_ssl.waitRecv(1);
	else 
		connid = mq_tcp.waitRecv(1);
	
	if(connid != -1) {
		if(connid == MQTT_SOCKET) {
			String data;
			if(ssl_en) 
				data = mq_ssl.readRecv(MQTT_SOCKET);
			else 
				data = mq_tcp.readRecv(MQTT_SOCKET);
			
			if((data[0]&0xF0) == 0x30) {			// publish packet
				uint8_t t_len = int(data[3]);
				topic_i = "";
				message_i = "";
				uint8_t length = 4;
				for(uint8_t i=0; i<t_len; i++) {
					topic_i += data[length++];		// topic_recv
				}
				if((data[0]&0x06) != 0) {			// QoS > 0
					length += 2;
				}
				uint8_t len_n = length;
				for(uint8_t i=0; i<(int(data[1])+2-len_n); i++) {
					message_i += data[length++];	// message_recv
				}
				return true;
			}
			else
				return false;		// is not publish packet
		}
		else {
			return false;			// data send to the other socket
		}
	}
	return false;
}

String UC20_MQTT::getTopic() {
	return topic_i;
}

String UC20_MQTT::getMessage() {
	return message_i;
}

bool UC20_MQTT::socketReady() {
	gsm.print(F("AT+QISTATE=1,"));
	gsm.println(MQTT_SOCKET);
	String resp = gsm.waitStr(F("+QISTATE:"));
	if(resp != "") {
		int8_t index = 0;
		for(uint8_t i=0; i<5; i++) {
			index = resp.indexOf(F(","), index) + 1;
		}
		int8_t index2 = resp.indexOf(F(","), index);
		int8_t state = resp.substring(index, index2).toInt();
		return (state == 2);
	}
	return false;
}

bool UC20_MQTT::closeSocket() {
	gsm.print(F("AT+QICLOSE="));
	gsm.println(MQTT_SOCKET);
	return (gsm.waitStr(F("OK"),12000) != "");
}









