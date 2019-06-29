#include "network.h"

#define PORT 54321

Server::Server(){
	nid = 0;
	error_msg = NULL;
	error = false;
	addr.port = PORT;
	addr.host = ENET_HOST_ANY;
	host = enet_host_create(&addr, 32, 2, 0, 0);
	if(host == NULL){
		error = true;
		error_msg = "Error creating server";
		return;
	}
}
Server::~Server(){
	for(auto it = peers.begin(); it != peers.end(); it++){
		enet_peer_disconnect_now(it->second, 0);
	}
	enet_host_destroy(host);
}

void Server::get_address(char* name, int max_len){
	if(enet_address_get_host(&addr, name, max_len)){ // try name
		if(enet_address_get_host_ip(&addr, name, max_len)){ // try ip
			*name = '\0'; // empty
		}
	}
}

bool Server::is_error(){
	return error;
}
const char* Server::get_error(){
	return error_msg;
}
NetEvent Server::get_event(int time){
	ENetEvent event;
	NetEvent ret = {NetEvent::TYPE_NONE, 0, NULL, 0};
	while(enet_host_service(host, &event, time) > 0){
		switch(event.type){
		case ENET_EVENT_TYPE_CONNECT:
			if(ids.count(event.peer)==0){
				ret.type = NetEvent::TYPE_CONN;
				ret.peer_id = nid++;
				peers.insert({ret.peer_id, event.peer});
				ids.insert({event.peer, ret.peer_id});
				return ret;
			}
			break;
		case ENET_EVENT_TYPE_RECEIVE:
			if(ids.count(event.peer)>0){
				ret.type = NetEvent::TYPE_RECV;
				ret.len = event.packet->dataLength;
				ret.data = new char[ret.len];
				ret.peer_id = ids[event.peer];
				memcpy(ret.data, event.packet->data, ret.len);
				enet_packet_destroy(event.packet);
				return ret;
			}
			break;
		case ENET_EVENT_TYPE_DISCONNECT:
			if(ids.count(event.peer)>0){
				ret.type = NetEvent::TYPE_DISC;
				ret.peer_id = ids[event.peer];
				peers.erase(ret.peer_id);
				ids.erase(event.peer);
				return ret;
			}
		}
	}
	return ret;
}

void Server::send(char* data, int len, int peer_id, Proto p){
	if(peers.count(peer_id)>0){
		int flags = 0;
		if(p==PROTO_REL) flags |= ENET_PACKET_FLAG_RELIABLE;
		ENetPacket* pack = enet_packet_create(data, len, flags);
		enet_peer_send(peers[peer_id],  p==PROTO_REL ? 0 : 1, pack);
		enet_host_flush(host);
	}
}
void Server::send_all(char* data, int len, Proto p){
	for(auto it = peers.begin(); it!=peers.end(); it++) send(data,len, it->first, p);
}

Client::Client(const char* address){
	addr = address;
	error_msg = NULL;
	error = false;
	peer = NULL;
	host = enet_host_create(NULL, 1, 2, 0, 0);
	if(host == NULL){
		error = true;
		error_msg = "Error creating client";
		return;
	}
	ENetAddress a;
	if(enet_address_set_host(&a, addr)){
		error = true;
		error_msg = "Error resolving server address";
		return;
	}
	a.port = PORT;
	
	peer = enet_host_connect(host, &a, 2, 0);
	if(peer == NULL){
		error = true;
		error_msg = "Error opening connection";
		return;
	}
}
Client::~Client(){
	if(peer) enet_peer_disconnect_now(peer, 0);
	if(host) enet_host_destroy(host);
}
	
const char* Client::get_address(){
	return addr;
}
	
bool Client::is_error(){
	return error;
}
const char* Client::get_error(){
	return error_msg;
}
NetEvent Client::get_event(){
	ENetEvent event;
	NetEvent ret = {NetEvent::TYPE_NONE, 0, NULL, 0};
	while(enet_host_service(host, &event, 0) > 0){
		switch(event.type){
		case ENET_EVENT_TYPE_CONNECT:
			if(event.peer == peer){
				ret.type = NetEvent::TYPE_CONN;
				return ret;
			}
			else{
				enet_peer_reset(event.peer);
			}
			break;
		case ENET_EVENT_TYPE_RECEIVE:
			if(event.peer == peer){
				ret.type = NetEvent::TYPE_RECV;
				ret.len = event.packet->dataLength;
				ret.data = new char[ret.len];
				memcpy(ret.data, event.packet->data, ret.len);
				enet_packet_destroy(event.packet);
				return ret;
			}
			break;
		case ENET_EVENT_TYPE_DISCONNECT:
			if(event.peer == peer){
				ret.type = NetEvent::TYPE_DISC;
				return ret;
			}
		}
	}
	return ret;
}
	
void Client::send(char* data, int len, Proto p){
	int flags = 0;
	if(p==PROTO_REL) flags |= ENET_PACKET_FLAG_RELIABLE;
	ENetPacket* pack = enet_packet_create(data, len, flags);
	enet_peer_send(peer,  p==PROTO_REL ? 0 : 1, pack);
	enet_host_flush(host);
}
