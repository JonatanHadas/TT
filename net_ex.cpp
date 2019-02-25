#include "net_ex.h"
#include "encoding.h"

NetEx::NetEx(Client* c, GameConfig& cf){
	clnt = c;
}
NetEx::~NetEx(){
	
}
ExInEvent* NetEx::get_event(){
	NetEvent e;
	while((e=clnt->get_event()).type != NetEvent::TYPE_NONE){
		char h,hh;
		char* cur = e.data;
		
		switch(e.type){
		case NetEvent::TYPE_CONN:
			break;
		case NetEvent::TYPE_DISC:
			return NULL; // todo error
		case NetEvent::TYPE_RECV:
			cur = decode_char(cur, h);
			cur = decode_char(cur, hh);
			switch(h){
			case '\x01':
				break;
			case '\x02':
				switch(hh){
				case '\x00':
					//return new NExInEventStartRound(cur);
					break;
				case '\x01':
					//return new NExInEventTankUpdate(cur);
					break;
				case '\x02':
					//return new NExInEventTankDeath(cur);
					break;
				case '\x03':
					//return new NExInEventScore(cur);
					break;
				case '\x10':
					//return new NExInEventCreateShot(cur);
					break;
				case '\x11':
					//return new NExInEventRemoveShot(cur);
					break;
				}
				break;
			}
			delete[] e.data;
			break;
		}
	}
	return NULL;
}
void NetEx::push_ctrl(ControlState ctrl, int ind, int rnd){
	char data[100];
	char* end;
	
	end = data;
	end = encode_int(end, ind);
	end = encode_int(end, rnd);
	end = encode_bool(end, ctrl.fd);
	end = encode_bool(end, ctrl.bk);
	end = encode_bool(end, ctrl.rt);
	end = encode_bool(end, ctrl.lt);
	end = encode_bool(end, ctrl.sht);
	clnt->send(data, end-data, PROTO_REL);
}

NExInEventStartRound::NExInEventStartRound(char* data, char* del){
	
}
NExInEventStartRound::~NExInEventStartRound(){
	
}
Maze* NExInEventStartRound::get_maze(){
	
}
int NExInEventStartRound::get_x(int i){
	
}
int NExInEventStartRound::get_y(int i){
	
}
double NExInEventStartRound::get_ang(int i){
	
}

NExInEventTankDeath::NExInEventTankDeath(char* data, char* del){
	
}
NExInEventTankDeath::~NExInEventTankDeath(){
	
}
int NExInEventTankDeath::get_ind(){
}

NExInEventScore::NExInEventScore(char* data, char* del){
	
}
NExInEventScore::~NExInEventScore(){
	
}
int NExInEventScore::get_ind(){
	
}
int NExInEventScore::get_diff(){
	
}

NExInEventEndGame::NExInEventEndGame(char* data, char* del){
	
}
NExInEventEndGame::~NExInEventEndGame(){
	
}

NExInEventCreateShot::NExInEventCreateShot(char* data, char* del){
	
}
NExInEventCreateShot::~NExInEventCreateShot(){
	
}
int NExInEventCreateShot::get_id(){
	
}
GenShot::Type NExInEventCreateShot::get_stype(){
	
}
double NExInEventCreateShot::get_x(){
	
}
double NExInEventCreateShot::get_y(){
	
}
double NExInEventCreateShot::get_vx(){
	
}
double NExInEventCreateShot::get_vy(){
	
}
int NExInEventCreateShot::get_tank_ind(){
	
}
long long int NExInEventCreateShot::get_time(){
	
}
int NExInEventCreateShot::get_round(){
	
}

NExInEventRemoveShot::NExInEventRemoveShot(char* data, char* del){
	
}
NExInEventRemoveShot::~NExInEventRemoveShot(){
	
}
int NExInEventRemoveShot::get_id(){
	
}

NExInEventTankUpdate::NExInEventTankUpdate(char* data, char* del){
	
}
NExInEventTankUpdate::~NExInEventTankUpdate(){
	
}
int NExInEventTankUpdate::get_ind(){
	
}
double NExInEventTankUpdate::get_x(){
	
}
double NExInEventTankUpdate::get_y(){
	
}
double NExInEventTankUpdate::get_ang(){
	
}
long long int NExInEventTankUpdate::get_time(){
	
}
ControlState NExInEventTankUpdate::get_ctrl(){
	
}
