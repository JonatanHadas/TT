#include "net_ex.h"
#include "../network/encoding.h"

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
				switch(hh){
				case '\x03':
					return new NExInEventEndGame(cur, e.data);
					break;
				}
				break;
			case '\x02':
				switch(hh){
				case '\x00':
					return new NExInEventStartRound(cur,e.data);
					break;
				case '\x01':
					return new NExInEventTankUpdate(cur,e.data);
					break;
				case '\x02':
					return new NExInEventTankDeath(cur,e.data);
					break;
				case '\x03':
					return new NExInEventScore(cur,e.data);
					break;
				case '\x10':
					return new NExInEventCreateShot(cur,e.data);
					break;
				case '\x11':
					return new NExInEventRemoveShot(cur,e.data);
					break;
				case '\x12':
					return new NExInEventCreateFragment(cur, e.data);
					break;
				case '\x13':
					return new NExInEventCreateDeathRay(cur, e.data);
					break;
				case '\x20':
					return new NExInEventCreateUpgrade(cur,e.data);
					break;
				case '\x21':
					return new NExInEventRemoveUpgrade(cur,e.data);
					break;
				case '\x30':
					return new NExInEventCreateMine(cur, e.data);
					break;
				case '\x31':
					return new NExInEventRemoveMine(cur, e.data);
					break;
				case '\x32':
					return new NExInEventActivateMine(cur, e.data);
					break;
				case '\x33':
					return new NExInEventStartMine(cur, e.data);
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
	end = encode_char(end, '\x02');
	end = encode_char(end, '\x01');
	end = encode_int(end, ind);
	end = encode_int(end, rnd);
	end = encode_bool(end, ctrl.fd);
	end = encode_bool(end, ctrl.bk);
	end = encode_bool(end, ctrl.rt);
	end = encode_bool(end, ctrl.lt);
	end = encode_bool(end, ctrl.sht);
	clnt->send(data, end-data, PROTO_REL);
}

void NetEx::leave(){
	delete clnt;
	clnt = NULL;
}

NExInEventStartRound::NExInEventStartRound(char* data, char* del){
	int tn;
	data = decode_int(data, tn);
	
	for(int i = 0; i<tn; i++){
		int x,y;
		double a;
		
		data = decode_int(data, x);
		data = decode_int(data, y);
		data = decode_double(data, a);
		
		tanks.push_back({{x,y},a});
	}
	
	int w,h;
	
	data = decode_int(data,w);
	data = decode_int(data,h);
	
	maze = new Maze(w,h);
	for(int x = 0;x<w; x++){
		for(int y = 0; y<h-1; y++){
			bool val;
			data = decode_bool(data, val);
			maze->set_hwall(x,y,val);
		}
	}
	for(int x = 0;x<w-1; x++){
		for(int y = 0; y<h; y++){
			bool val;
			data = decode_bool(data, val);
			maze->set_vwall(x,y,val);
		}
	}
	
	delete del;
}
NExInEventStartRound::~NExInEventStartRound(){
}
Maze* NExInEventStartRound::get_maze(){
	return maze;
}
int NExInEventStartRound::get_x(int i){
	return tanks[i].first.first;
}
int NExInEventStartRound::get_y(int i){
	return tanks[i].first.second;
}
double NExInEventStartRound::get_ang(int i){
	return tanks[i].second;
}

NExInEventTankDeath::NExInEventTankDeath(char* data, char* del){
	data = decode_int(data, ind);
	delete del;
}
NExInEventTankDeath::~NExInEventTankDeath(){
	
}
int NExInEventTankDeath::get_ind(){
	return ind;
}

NExInEventScore::NExInEventScore(char* data, char* del){
	data = decode_int(data, ind);
	data = decode_int(data, diff);
	delete del;
}
NExInEventScore::~NExInEventScore(){
	
}
int NExInEventScore::get_ind(){
	return ind;
}
int NExInEventScore::get_diff(){
	return diff;
}

NExInEventEndGame::NExInEventEndGame(char* data, char* del){
	delete del;
}
NExInEventEndGame::~NExInEventEndGame(){
	
}

NExInEventCreateShot::NExInEventCreateShot(char* data, char* del){
	data = decode_int(data,id);
	data = decode_long(data,time);
	data = decode_int(data,round);
	data = decode_int(data,tank_ind);
	data = decode_shot_type(data,type);
	data = decode_double(data,x);
	data = decode_double(data,y);
	data = decode_double(data,vx);
	data = decode_double(data,vy);
	
	delete del;
}
NExInEventCreateShot::~NExInEventCreateShot(){
	
}
int NExInEventCreateShot::get_id(){
	return id;
}
GenShot::Type NExInEventCreateShot::get_stype(){
	return type;
}
double NExInEventCreateShot::get_x(){
	return x;
}
double NExInEventCreateShot::get_y(){
	return y;
}
double NExInEventCreateShot::get_vx(){
	return vx;
}
double NExInEventCreateShot::get_vy(){
	return vy;
}
int NExInEventCreateShot::get_tank_ind(){
	return tank_ind;
}
long long int NExInEventCreateShot::get_time(){
	return time;
}
int NExInEventCreateShot::get_round(){
	return round;
}

NExInEventRemoveShot::NExInEventRemoveShot(char* data, char* del){
	data = decode_int(data, id);
	delete del;
}
NExInEventRemoveShot::~NExInEventRemoveShot(){
	
}
int NExInEventRemoveShot::get_id(){
	return id;
}

NExInEventTankUpdate::NExInEventTankUpdate(char* data, char* del){

	data = decode_int(data, ind);
	data = decode_long(data, time);
			
	data = decode_double(data, x);
	data = decode_double(data, y);
	data = decode_double(data, ang);
	
	data = decode_bool(data, ctrl.fd);
	data = decode_bool(data, ctrl.bk);
	data = decode_bool(data, ctrl.rt);
	data = decode_bool(data, ctrl.lt);
	data = decode_bool(data, ctrl.sht);
	
	data = decode_tank_state(data, state);
		
	data = decode_int(data, m_id);
	
	data = decode_double(data, m_x);
	data = decode_double(data, m_y);
	data = decode_double(data, m_a);

	data = decode_bool(data, m_rt);
	data = decode_bool(data, m_lt);
	
	data = decode_int(data, m_tar);
	

	delete del;
}
NExInEventTankUpdate::~NExInEventTankUpdate(){
}
int NExInEventTankUpdate::get_ind(){
	return ind;
}
double NExInEventTankUpdate::get_x(){
	return x;
}
double NExInEventTankUpdate::get_y(){
	return y;
}
double NExInEventTankUpdate::get_ang(){
	return ang;
}
long long int NExInEventTankUpdate::get_time(){
	return time;
}
ControlState NExInEventTankUpdate::get_ctrl(){
	return ctrl;
}
Tank::State NExInEventTankUpdate::get_state(){
	return state;
}
double NExInEventTankUpdate::get_missile_x(){
	return m_x;
}
double NExInEventTankUpdate::get_missile_y(){
	return m_y;
}
double NExInEventTankUpdate::get_missile_ang(){
	return m_a;
}
bool NExInEventTankUpdate::get_missile_rt(){
	return m_rt;
}
bool NExInEventTankUpdate::get_missile_lt(){
	return m_lt;
}
int NExInEventTankUpdate::get_missile_id(){
	return m_id;
}
int NExInEventTankUpdate::get_missile_target(){
	return m_tar;
}

NExInEventCreateUpgrade::NExInEventCreateUpgrade(char* data, char* del){
	data = decode_int(data, rnd);
	data = decode_int(data, u.x);
	data = decode_int(data, u.y);
	data = decode_upgrade(data, u.type);
	
	delete del;
}
NExInEventCreateUpgrade::~NExInEventCreateUpgrade(){
	
}
Upgrade NExInEventCreateUpgrade::get_upg(){
	return u;
}
int NExInEventCreateUpgrade::get_round(){
	return rnd;
}

NExInEventRemoveUpgrade::NExInEventRemoveUpgrade(char* data, char* del){
	data = decode_int(data, rnd);
	data = decode_int(data, x);
	data = decode_int(data, y);
	
	delete del;
}
NExInEventRemoveUpgrade::~NExInEventRemoveUpgrade(){
	
}
int NExInEventRemoveUpgrade::get_x(){
	return x;
}
int NExInEventRemoveUpgrade::get_y(){
	return y;
}
int NExInEventRemoveUpgrade::get_round(){
	return rnd;
}

NExInEventCreateFragment::NExInEventCreateFragment(char* data, char* del){
	data = decode_int(data, id);

	data = decode_long(data, time);
	data = decode_int(data, round);

	data = decode_double(data, x);
	data = decode_double(data, y);
	data = decode_double(data, ang);
	
	delete del;
}
NExInEventCreateFragment::~NExInEventCreateFragment(){
	
}
double NExInEventCreateFragment::get_x(){
	return x;
}
double NExInEventCreateFragment::get_y(){
	return y;
}
double NExInEventCreateFragment::get_ang(){
	return ang;
}
long long int NExInEventCreateFragment::get_time(){
	return time;
}
int NExInEventCreateFragment::get_round(){
	return round;
}
int NExInEventCreateFragment::get_id(){
	return id;
}

NExInEventCreateDeathRay::NExInEventCreateDeathRay(char* data, char* del){
	data = decode_int(data, id);
	
	data = decode_int(data, round);
	
	data = decode_int(data, tank_ind);
	
	int p_num;
	
	data = decode_int(data, p_num);
	
	for(int i = 0; i<p_num; i++){
		double x,y;
		data = decode_double(data, x);
		data = decode_double(data, y);
		
		ps.push_back({x,y});
	}
	
	delete del;
}
NExInEventCreateDeathRay::~NExInEventCreateDeathRay(){
	
}
int NExInEventCreateDeathRay::get_point_num(){
	return ps.size();
}
std::pair<double, double> NExInEventCreateDeathRay::get_point(int i){
	return ps[i];
}
int NExInEventCreateDeathRay::get_round(){
	return round;
}
int NExInEventCreateDeathRay::get_tank_ind(){
	return tank_ind;
}
int NExInEventCreateDeathRay::get_id(){
	return id;
}

NExInEventCreateMine::NExInEventCreateMine(char* data, char* del){
	data = decode_int(data, id);
	
	data = decode_int(data, round);
	
	data = decode_double(data, x);
	data = decode_double(data, y);
	data = decode_double(data, ang);
	
	data = decode_int(data, tank_ind);
	
	delete del;
}
NExInEventCreateMine::~NExInEventCreateMine(){
	
}
int NExInEventCreateMine::get_id(){
	return id;
}
double NExInEventCreateMine::get_x(){
	return x;
}
double NExInEventCreateMine::get_y(){
	return y;
}
double NExInEventCreateMine::get_ang(){
	return ang;
}
int NExInEventCreateMine::get_round(){
	return round;
}
int NExInEventCreateMine::get_tank_ind(){
	return tank_ind;
}

NExInEventRemoveMine::NExInEventRemoveMine(char* data, char* del){
	data = decode_int(data, id);
	
	delete del;
}
NExInEventRemoveMine::~NExInEventRemoveMine(){
	
}
int NExInEventRemoveMine::get_id(){
	return id;
}

NExInEventStartMine::NExInEventStartMine(char* data, char* del){
	data = decode_int(data, id);
	
	delete del;
}
NExInEventStartMine::~NExInEventStartMine(){
	
}
int NExInEventStartMine::get_id(){
	return id;
}

NExInEventActivateMine::NExInEventActivateMine(char* data, char* del){
	data = decode_int(data, id);
	
	delete del;
}
NExInEventActivateMine::~NExInEventActivateMine(){
	
}
int NExInEventActivateMine::get_id(){
	return id;
}
