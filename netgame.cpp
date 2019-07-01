#include "netgame.h"
#include "encoding.h"
#include <stdio.h>


NetGame::NetGame(GameConfig& cf, GameSetup* s){
	set = s;
	game = new Game(cf);
	for(int i = 0; i<cf.tank_num; i++){
		ids.push_back(cf.keys[i]);
	}
}
NetGame::~NetGame(){
	delete game;
}

void NetGame::start_round(){
	char* data;
	Maze* maze = game->get_round()->get_maze();
	
	int w = maze->get_w();
	int h = maze->get_h();
	size_t size = 2+game->get_tank_num() * (sizeof(int)*2 + sizeof(double)) + sizeof(bool) * (2*w*h-w-h) + 3*sizeof(int);
	
	data = new char[size];
	
	char* cur = data;
	
	cur = encode_char(cur, '\x02');
	cur = encode_char(cur, '\x00');
	
	cur = encode_int(cur, game->get_tank_num());
	
	for(int i = 0; i<game->get_tank_num(); i++){
		Tank* t = game->get_tank(i);
		
		cur = encode_int(cur, (int)(t->get_x()));
		cur = encode_int(cur, (int)(t->get_y()));
		
		cur = encode_double(cur, (t->get_ang()));
	}
	
	cur = encode_int(cur, w);
	cur = encode_int(cur, h);
	
	for(int x = 0; x<w; x++){
		for(int y = 0; y<h-1; y++){
			cur = encode_bool(cur, maze->hwall(x,y));
		}
	}
	for(int x = 0; x<w-1; x++){
		for(int y = 0; y<h; y++){
			cur = encode_bool(cur, maze->vwall(x,y));
		}
	}
	
	set->serv->send_all(data, cur-data, PROTO_REL);
	
	delete data;
}

void NetGame::kill_tank(GameEventTankDeath* e){
	char data[100];
	
	char* end = data;
	
	end = encode_char(end, '\x02');
	end = encode_char(end, '\x02');
	
	end = encode_int(end, e->get_ind());
	
	set->serv->send_all(data, end-data, PROTO_REL);
}
void NetGame::score(GameEventScore* e){
	char data[100];
	
	char* end = data;
	
	end = encode_char(end, '\x02');
	end = encode_char(end, '\x03');
	
	end = encode_int(end, e->get_ind());
	end = encode_int(end, e->get_diff());
	
	set->serv->send_all(data, end-data, PROTO_REL);
}
void NetGame::create_shot(GameEventCreateShot* e){
	char data[1000];
	
	char* end = data;
	GenShot* gs = e->get_shot();
	
	end = encode_char(end, '\x02');
	
	Shot* s;
	Fragment* f;
	DeathRay* d;
	switch(gs->get_type()){
	case GenShot::TYPE_REG:
	case GenShot::TYPE_GATLING:
	case GenShot::TYPE_LASER:
	case GenShot::TYPE_BOMB:
		s = (Shot*) gs;
		end = encode_char(end, '\x10');
				
		end = encode_int(end, s->get_id());
		
		end = encode_long(end, game->get_time());
		end = encode_int(end, game->get_round_num());
		
		end = encode_int(end, s->get_tank()->get_ind());
		end = encode_shot_type(end, s->get_type());
	
		end = encode_double(end, s->get_x());
		end = encode_double(end, s->get_y());
		end = encode_double(end, s->get_vx());
		end = encode_double(end, s->get_vy());
		break;
	case GenShot::TYPE_FRAGMENT:
		f = (Fragment*)gs;
		end = encode_char(end, '\x12');
		
		end = encode_int(end, f->get_id());
		
		end = encode_long(end, game->get_time());
		end = encode_int(end, game->get_round_num());
		
		end = encode_double(end, f->get_x());
		end = encode_double(end, f->get_y());
		end = encode_double(end, f->get_ang());
		break;
	case GenShot::TYPE_DEATH_RAY:
		d = (DeathRay*)gs;
		end = encode_char(end, '\x13');
		
		end = encode_int(end, d->get_id());
		
		end = encode_int(end, game->get_round_num());
		
		end = encode_int(end, d->get_tank()->get_ind());
		
		end = encode_int(end, d->get_point_num());
		
		for(int i = 0; i<d->get_point_num(); i++){
			end = encode_double(end, d->get_point(i).first);
			end = encode_double(end, d->get_point(i).second);
		}
		
		break;
	}
	
	set->serv->send_all(data, end-data, PROTO_REL);
	
}
void NetGame::remove_shot(GameEventRemoveShot* e){
	char data[100];
	
	char* end = data;
	
	end = encode_char(end, '\x02');
	end = encode_char(end, '\x11');
	
	end = encode_int(end, e->get_id());
	
	set->serv->send_all(data, end-data, PROTO_REL);
}
void NetGame::create_upg(GameEventCreateUpgrade* e){
	char data[100];
	
	char* end = data;
	
	end = encode_char(end, '\x02');
	end = encode_char(end, '\x20');
	
	end = encode_int(end, game->get_round_num());
	end = encode_int(end, e->get_upg().x);
	end = encode_int(end, e->get_upg().y);
	end = encode_upgrade(end, e->get_upg().type);
	
	set->serv->send_all(data, end-data, PROTO_REL);
}
void NetGame::remove_upg(GameEventRemoveUpgrade* e){
	char data[100];
	
	char* end = data;
	
	end = encode_char(end, '\x02');
	end = encode_char(end, '\x21');
	
	end = encode_int(end, game->get_round_num());
	end = encode_int(end, e->get_x());
	end = encode_int(end, e->get_y());
	
	set->serv->send_all(data, end-data, PROTO_REL);
}

void NetGame::end_game(GameEventEndGame* e){
	char* data;
	
	size_t size = sizeof(int)* (e->get_scores().size()+1) + 2;
	
	data = new char[size];
	
	char* end = data;
	
	end = encode_char(end, '\x01');
	end = encode_char(end, '\x03');
	
	end = encode_int(end, e->get_scores().size());
	
	for(int i = 0; i<e->get_scores().size(); i++){
		end = encode_int(end, e->get_scores()[i]);
	}
	
	set->serv->send_all(data, end-data, PROTO_REL);
	
	delete data;
}

void NetGame::push_ctrl(int peer_id,char* data){
	int ind,rnd;
	ControlState ctrl;
	data = decode_int(data, ind);
	if(set->players[ids[ind]].first != peer_id) return;
	data = decode_int(data, rnd);
	if(rnd != game->get_round_num()) return;
	data = decode_bool(data, ctrl.fd);
	data = decode_bool(data, ctrl.bk);
	data = decode_bool(data, ctrl.rt);
	data = decode_bool(data, ctrl.lt);
	data = decode_bool(data, ctrl.sht);
	game->get_tank(ind)->push_control(ctrl);
	
	advance();
}

void NetGame::advance(){
	game->advance();
	GameEvent* e;
	while(e = game->get_event()){
		switch(e->get_type()){
		case GameEvent::TYPE_START_RND:
			start_round();
			break;
		case GameEvent::TYPE_TANK_DEAD:
			kill_tank((GameEventTankDeath*)e);
			break;
		case GameEvent::TYPE_SCORE:
			score((GameEventScore*)e);
			break;
		case GameEvent::TYPE_END_GAME:
			end_game((GameEventEndGame*)e);
			break;
		case GameEvent::TYPE_SHOT_CRT:
			create_shot((GameEventCreateShot*)e);
			break;
		case GameEvent::TYPE_SHOT_RMV:
			remove_shot((GameEventRemoveShot*)e);
			break;
		case GameEvent::TYPE_UPG_CRT:
			create_upg((GameEventCreateUpgrade*)e);
			break;
		case GameEvent::TYPE_UPG_RMV:
			remove_upg((GameEventRemoveUpgrade*)e);
			break;
		}
	}
	
	send_update();
}

void NetGame::send_update(){
	char data[100];
	
	for(int i = 0; i<game->get_tank_num(); i++){
		char* end = data;
		end = encode_char(end, '\x02');
		end = encode_char(end, '\x01');
		
		end = encode_int(end, i);
		
		end = encode_long(end, game->get_time());
						
		Tank* t = game->get_tank(i);
		end = encode_double(end, t->get_x());
		end = encode_double(end, t->get_y());
		end = encode_double(end, t->get_ang());
		
		end = encode_bool(end, t->get_ctrl().fd);
		end = encode_bool(end, t->get_ctrl().bk);
		end = encode_bool(end, t->get_ctrl().rt);
		end = encode_bool(end, t->get_ctrl().lt);
		end = encode_bool(end, t->get_ctrl().sht);
		
		end = encode_tank_state(end, t->get_state());
		
		
		set->serv->send_all(data, end-data, PROTO_UNREL);		
	}
}
	
void NetGame::mainloop(){
	advance();
	while(true){
		char h,hh;
		char* cur;
		
		NetEvent e = set->serv->get_event(1000);
		switch(e.type){
		case NetEvent::TYPE_CONN:
			printf(	"Connection   : id=%10d\n", e.peer_id);
			set->peers.insert({e.peer_id, new std::vector<PlayerData>});
			break;
		case NetEvent::TYPE_DISC:
			printf(	"Disconnection: id=%10d\n", e.peer_id);
			while(set->peers[e.peer_id]->size()>0) set->remove_player(e.peer_id, set->peers[e.peer_id]->size()-1);
			delete set->peers[e.peer_id];
			set->peers.erase(e.peer_id);
			break;
		case NetEvent::TYPE_RECV:
			cur = decode_char(e.data, h);
			cur = decode_char(cur, hh);
			if(h=='\x02')if(hh=='\x01') push_ctrl(e.peer_id, cur);
			delete e.data;
			break;
		}
	}
	
}
