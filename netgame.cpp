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
	
	game->advance();
	GameEvent* e;
	while(e = game->get_event()){
		switch(e->get_type()){
		case GameEvent::TYPE_START_RND:
			break;
		case GameEvent::TYPE_TANK_DEAD:
			break;
		case GameEvent::TYPE_SCORE:
			break;
		case GameEvent::TYPE_END_GAME:
			break;
		case GameEvent::TYPE_SHOT_CRT:
			break;
		case GameEvent::TYPE_SHOT_RMV:
			break;
		}
	}
}
	
void NetGame::mainloop(){
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
