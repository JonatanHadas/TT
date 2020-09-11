#ifndef _NETWORK_H
#define _NETWORK_H

#include "enet/enet.h"

#include <map>
#include <set>

enum Proto{
	PROTO_REL,
	PROTO_UNREL,
};

struct NetEvent{
	enum Type{
		TYPE_NONE,
		TYPE_CONN,
		TYPE_DISC,
		TYPE_RECV,
	};
	Type type;
	int len;
	char* data;
	int peer_id; // server side only
};

class Server{
	ENetHost* host;
	std::map<int, ENetPeer*> peers;
	std::map<ENetPeer*, int> ids;
	
	bool error;
	const char* error_msg;
	
	int nid;
		
	ENetAddress addr;
public:
	Server();
	~Server();
	
	void get_address(char* name, int max_len);
	
	bool is_error();
	const char* get_error();
	NetEvent get_event(int time);
	
	void send(char* data, int len, int peer_id, Proto p);
	void send_all(char* data, int len, Proto p);
};

class Client{
	ENetHost* host;
	ENetPeer* peer;
	
	bool error;
	const char* error_msg;
	const char* addr;
public:
	Client(const char* address);
	~Client();
	
	const char* get_address();
	
	bool is_error();
	const char* get_error();
	NetEvent get_event();
	
	void send(char* data, int len, Proto p);
	
};

#endif