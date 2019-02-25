#include "game_setup.h"
#include "encoding.h"
#include <stdio.h>

bool GameSetup::color_taken(int colnum){
	for(auto it = peers.begin(); it!=peers.end(); it++){
		for(int i = 0; i<it->second->size(); i++){
			if(it->second->at(i).color == colnum) return true;
		}
	}
	return false;
}
int GameSetup::get_free_col(){
	int col = 0;
	while(color_taken(col)) col++;
	return col;
}

GameSetup::GameSetup(Server* s){
	serv = s;
	set.scr_mth = GameSettings::SCR_LAST;
	set.allow_dif = 0;
	set.end_mth = GameSettings::END_NONE;
	set.lim = 10;
	
	id = 0;
}
GameSetup::~GameSetup(){
	
}

void GameSetup::send_all(int peer_id){
	char data[100];
	char* end;
	
	end = data;
	end = encode_char(end, '\x00');
	end = encode_char(end, '\x00');
	end = encode_gamesett(end, set);
	serv->send(data, end-data, peer_id, PROTO_REL);
	
	for(auto it = players.begin(); it!=players.end(); it++){
		PlayerData pl = peers[it->second.first]->at(it->second.second);
		end = data;
		end = encode_char(end, '\x00');
		end = encode_char(end, '\x01');
		end = encode_int(end, it->first); // id
		end = encode_int(end, pl.team);
		end = encode_int(end, pl.color);
		serv->send(data, end-data, peer_id, PROTO_REL);

		end = data;
		end = encode_char(end, '\x00');
		end = encode_char(end, '\x03');
		end = encode_int(end, it->first); // id
		end = encode_str(end, pl.name.c_str()); // name
		serv->send(data, end-data, peer_id, PROTO_REL);
	}
}

void GameSetup::add_player(int peer_id){
	PlayerData pl;
	pl.color = get_free_col();
	pl.team = 0;
	pl.peer_id = peer_id;
	pl.id = id++;
	players.insert({pl.id, {peer_id, peers[peer_id]->size()}});
	peers[peer_id]->push_back(pl);
	
	char data[100];
	char* end;
	
	end = data;
	end = encode_char(end, '\x00');
	end = encode_char(end, '\x01');
	end = encode_int(end, pl.id);
	end = encode_int(end, pl.team);
	end = encode_int(end, pl.color);
	serv->send_all(data, end-data, PROTO_REL);
	
	end = data;
	end = encode_char(end, '\x00');
	end = encode_char(end, '\x05');
	end = encode_int(end, players[pl.id].second);
	end = encode_int(end, pl.color);
	serv->send(data, end-data, peer_id, PROTO_REL);
	
	if(peer_id == peers.begin()->first){
		end = data;
		end = encode_char(end, '\x00');
		end = encode_char(end, '\x07');
		end = encode_int(end, pl.id);
		serv->send_all(data, end-data, PROTO_REL);
	}
}
void GameSetup::remove_player(int peer_id, int ind){
	for(int i = ind+1; i<peers[peer_id]->size(); i++){
		players[peers[peer_id]->at(i).id].second--;
	}
	players.erase(peers[peer_id]->at(ind).id);
	
	PlayerData pl = peers[peer_id]->at(ind);
	
	peers[peer_id]->erase(peers[peer_id]->begin()+ind);
	
	char data[100];
	char* end;
	
	end = data;
	end = encode_char(end, '\x00');
	end = encode_char(end, '\x02');
	end = encode_int(end, pl.id);
	serv->send_all(data, end-data, PROTO_REL);
}
void GameSetup::update_name(int peer_id, int ind, const char* n){
	peers[peer_id]->at(ind).name = n;
	int id = peers[peer_id]->at(ind).id;
	
	char data[1000];
	char* end;
	
	end = data;
	end = encode_char(end, '\x00');
	end = encode_char(end, '\x03');
	end = encode_int(end, id);
	end = encode_str(end, n);
	serv->send_all(data, end-data, PROTO_REL);
	
}
void GameSetup::update_col(int peer_id, int ind, int col){
	if(color_taken(col)) return;
	
	peers[peer_id]->at(ind).color = col;
	int id = peers[peer_id]->at(ind).id;
	
	char data[100];
	char* end;
	
	end = data;
	end = encode_char(end, '\x00');
	end = encode_char(end, '\x04');
	end = encode_int(end, id);
	end = encode_int(end, col);
	serv->send_all(data, end-data, PROTO_REL);
	
	end = data;
	end = encode_char(end, '\x00');
	end = encode_char(end, '\x05');
	end = encode_int(end, ind);
	end = encode_int(end, col);
	serv->send(data, end-data, peer_id, PROTO_REL);
	
}
void GameSetup::assign_host(){
	if(peers.size() == 0) return;
	int peer_id = peers.begin()->first;
	
	char data[100];
	char* end;
	
	end = data;
	end = encode_char(end, '\x00');
	end = encode_char(end, '\x06');
	serv->send(data, end-data, peer_id, PROTO_REL);
	
	for(int i = 0; i<peers.begin()->second->size(); i++){
		end = data;
		end = encode_char(end, '\x00');
		end = encode_char(end, '\x07');
		end = encode_int(end, peers.begin()->second->at(i).id);
		serv->send_all(data, end-data, PROTO_REL);
	}
	
}

void GameSetup::update_game_lim(){
	char data[100];
	char* end;
	
	end = data;
	end = encode_char(end, '\x00');
	end = encode_char(end, '\x13');
	end = encode_int(end, set.lim);
	serv->send_all(data, end-data, PROTO_REL);
}
void GameSetup::update_tie_lim(){
	char data[100];
	char* end;
	
	end = data;
	end = encode_char(end, '\x00');
	end = encode_char(end, '\x12');
	end = encode_int(end, set.allow_dif);
	serv->send_all(data, end-data, PROTO_REL);
}
void GameSetup::update_scr_mth(){
	char data[100];
	char* end;
	
	end = data;
	end = encode_char(end, '\x00');
	end = encode_char(end, '\x10');
	end = encode_scr_mth(end, set.scr_mth);
	serv->send_all(data, end-data, PROTO_REL);
}
void GameSetup::update_end_mth(){
	char data[100];
	char* end;
	
	end = data;
	end = encode_char(end, '\x00');
	end = encode_char(end, '\x11');
	end = encode_end_mth(end, set.end_mth);
	serv->send_all(data, end-data, PROTO_REL);
}
	
void GameSetup::mainloop(){
	while(true){
		if(serv->is_error()){
			fprintf(stderr, "Error: %s", serv->get_error());
			return;
		}
		
		NetEvent e=serv->get_event();
		char* cur;
		char h,hh, str[1000];
		int i,c,t;
		switch (e.type)
		{
		case NetEvent::TYPE_CONN:
			printf(	"Connection:    id=%10d\n", e.peer_id);
			
			
			peers.insert({e.peer_id, new std::vector<PlayerData>});
			
			
			send_all(e.peer_id);
			assign_host();
			
			break;
		case NetEvent::TYPE_RECV:
			cur = e.data;
			cur = decode_char(cur, h);
			switch(h){
			case '\x00':
				cur = decode_char(cur, hh);
				printf("%02x\n", hh);
				switch(hh){
				case '\x01':
					add_player(e.peer_id);
					break;
				case '\x02':
					cur = decode_int(cur, i);
					remove_player(e.peer_id, i);
					break;
				case '\x03':
					cur = decode_int(cur, i);
					cur = decode_str(cur, str);
					update_name(e.peer_id, i, str);
					break;
				case '\x04':
					cur = decode_int(cur, i);
					cur = decode_int(cur, c);
					update_col(e.peer_id, i, c);
					break;
					
				case '\x10':
					cur = decode_scr_mth(cur, set.scr_mth);
					update_scr_mth();
					break;
				case '\x11':
					cur = decode_end_mth(cur, set.end_mth);
					update_end_mth();
					break;
				case '\x12':
					cur = decode_int(cur, set.allow_dif);
					update_tie_lim();
					break;
				case '\x13':
					cur = decode_int(cur, set.lim);
					update_game_lim();
					break;

				}
				break;
			case '\x01':
				break;
			}
			
			
			delete e.data;
			break;
		case NetEvent::TYPE_DISC:
			printf(	"Disconnection: id=%10d\n", e.peer_id);
			
			while(peers[e.peer_id]->size()>0) remove_player(e.peer_id, peers[e.peer_id]->size()-1);
			delete peers[e.peer_id];
			peers.erase(e.peer_id);
			
			assign_host();
			
			break;
		}
	}
}
