#include "game_setup.h"
#include "encoding.h"
#include <stdio.h>
#include <time.h>
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
	
	use_teams = false;
	team_num = 2;
	
	cnt = -1;
	
	id = 0;
	
	upg_mask = UPG_MASK_ALL;
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
		
		if(it->second.first == peer_id){
			end = data;
			end = encode_char(end, '\x00');
			end = encode_char(end, '\x09');
			end = encode_int(end, pl.id);
			end = encode_int(end, it->second.second);
			serv->send(data, end-data, peer_id, PROTO_REL);
			
			end = data;
			end = encode_char(end, '\x00');
			end = encode_char(end, '\x16');
			end = encode_int(end, upg_mask);
			
			serv->send(data, end-data, peer_id, PROTO_REL);
		}

		end = data;
		end = encode_char(end, '\x00');
		end = encode_char(end, '\x03');
		end = encode_int(end, it->first); // id
		end = encode_str(end, pl.name.c_str()); // name
		serv->send(data, end-data, peer_id, PROTO_REL);
	}
	
	update_team_num();
	update_use_teams();
}

void GameSetup::add_player(int peer_id){
	PlayerData pl;
	pl.color = get_free_col();
	pl.team = players.size() == 0 ? 0 : team_num-1;
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
	end = encode_char(end, '\x09');
	end = encode_int(end, pl.id);
	end = encode_int(end, peers[peer_id]->size()-1);
	serv->send(data, end-data, peer_id, PROTO_REL);

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
void GameSetup::change_team(int peer_id, int id, int team){
	if(team>=team_num) team = team_num-1;
	if(team<0) team = 0;
	if(players[id].first != peer_id) return;
	int ind = players[id].second;
	peers[peer_id]->at(ind).team = team;
	
	char data[100];
	char* end;
	
	end = data;
	end = encode_char(end, '\x00');
	end = encode_char(end, '\x08');
	end = encode_int(end, id);
	end = encode_int(end, team);
	serv->send_all(data, end-data, PROTO_REL);
	
}

void GameSetup::update_game_lim(){
	if(set.lim < 0) set.lim = 0;
	
	char data[100];
	char* end;
	
	end = data;
	end = encode_char(end, '\x00');
	end = encode_char(end, '\x13');
	end = encode_int(end, set.lim);
	serv->send_all(data, end-data, PROTO_REL);
}
void GameSetup::update_tie_lim(){
	if(set.allow_dif > 2) set.allow_dif = 2;
	if(set.allow_dif < 0) set.allow_dif = 0;
	
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
void GameSetup::update_team_num(){
	char data[100];
	char* end;
	
	for(auto it = players.begin(); it != players.end(); it++){
		if(peers[it->second.first]->at(it->second.second).team >= team_num){
			change_team(it->second.first, it->first, team_num-1);
		}
	}
	
	end = data;
	end = encode_char(end, '\x00');
	end = encode_char(end, '\x14');
	end = encode_int(end, team_num);
	serv->send_all(data, end-data, PROTO_REL);
}
void GameSetup::update_upg_mask(int mask, bool val){
	if(val) upg_mask |= mask;
	else upg_mask &= ~mask;
	
	char data[100];
	char* end = data;
	
	end = encode_char(end, '\x00');
	end = encode_char(end, '\x16');
	end = encode_int(end, upg_mask);
	
	serv->send_all(data, end-data, PROTO_REL);
}
void GameSetup::update_use_teams(){
	char data[100];
	char* end;
	
	end = data;
	end = encode_char(end, '\x00');
	end = encode_char(end, '\x15');
	end = encode_bool(end, use_teams);
	serv->send_all(data, end-data, PROTO_REL);
}

void GameSetup::start_count(){
	cnt = 4;
	count();
}
void GameSetup::count(){
	if(cnt<0) return;
	cnt--;
	last_tick = clock();
	char data[100];
	char* end;

	if(cnt>=0){
		end = data;
		end = encode_char(end, '\x01');
		end = encode_char(end, '\x00');
		end = encode_int(end, cnt);
		serv->send_all(data, end-data, PROTO_REL);
		
		printf(cnt<=0 ? "Go!\n" : "%d\n", cnt);
	}
	else{
		printf("Starting game\n");
		end = data;
		end = encode_char(end, '\x01');
		end = encode_char(end, '\x02');
		serv->send_all(data, end-data, PROTO_REL);
		
		GameConfig cf(players.size(), use_teams ? team_num : players.size(), upg_mask);
		cf.set = set;
		int i = 0;
		for(auto it = players.begin(); it != players.end(); it++, i++){
			cf.keys[i] = it->first;
		}
		for(int i = 0; i<players.size(); i++){
			PlayerData pl = peers[players[cf.keys[i]].first]->at(players[cf.keys[i]].second);
			cf.team_inds[i] = use_teams ? pl.team : i;
			cf.colors[i] = pl.color;
		}
		
		NetGame* net_game = new NetGame(cf, this);
		net_game->mainloop();
		delete net_game;
		
		for(auto it = peers.begin(); it != peers.end(); it++){
			send_all(it->first);
		}
		assign_host();
	}
}
void GameSetup::stop_count(){
	if(cnt>=0) printf("Countdown stopped\n");
	char data[100];
	char* end;
	cnt = -1;
	end = data;
	end = encode_char(end, '\x01');
	end = encode_char(end, '\x00');
	end = encode_int(end, cnt);
	serv->send_all(data, end-data, PROTO_REL);
}
	
void GameSetup::mainloop(){
	while(true){
		if(serv->is_error()){
			fprintf(stderr, "Error: %s", serv->get_error());
			return;
		}
		
		NetEvent e=serv->get_event(cnt >= 0 ? (last_tick - clock() + CLOCKS_PER_SEC)*1000/CLOCKS_PER_SEC : 1000);
		char* cur;
		char h,hh, str[1000];
		int i,c,t;
		bool b;
		switch (e.type)
		{
		case NetEvent::TYPE_CONN:
			printf(	"Connection:    id=%10d\n", e.peer_id);
			
			stop_count();
			
			
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
				switch(hh){
				case '\x01':
					stop_count();
					add_player(e.peer_id);
					break;
				case '\x02':
					stop_count();
					cur = decode_int(cur, i);
					remove_player(e.peer_id, i);
					break;
				case '\x03':
					stop_count();
					cur = decode_int(cur, i);
					cur = decode_str(cur, str);
					update_name(e.peer_id, i, str);
					break;
				case '\x04':
					stop_count();
					cur = decode_int(cur, i);
					cur = decode_int(cur, c);
					update_col(e.peer_id, i, c);
					break;
				case '\x08':
					stop_count();
					cur = decode_int(cur, i);
					cur = decode_int(cur, t);
					change_team(e.peer_id, i, t);
					break;
					
				case '\x10':
					if(e.peer_id != peers.begin()->first) break;
					stop_count();
					cur = decode_scr_mth(cur, set.scr_mth);
					update_scr_mth();
					break;
				case '\x11':
					if(e.peer_id != peers.begin()->first) break;
					stop_count();
					cur = decode_end_mth(cur, set.end_mth);
					update_end_mth();
					break;
				case '\x12':
					if(e.peer_id != peers.begin()->first) break;
					stop_count();
					cur = decode_int(cur, set.allow_dif);
					update_tie_lim();
					break;
				case '\x13':
					if(e.peer_id != peers.begin()->first) break;
					stop_count();
					cur = decode_int(cur, set.lim);
					update_game_lim();
					break;
				case '\x14':
					if(e.peer_id != peers.begin()->first) break;
					stop_count();
					cur = decode_int(cur, team_num);
					update_team_num();
					break;
				case '\x15':
					if(e.peer_id != peers.begin()->first) break;
					stop_count();
					cur = decode_bool(cur, use_teams);
					update_use_teams();
					break;
				case '\x16':
					if(e.peer_id != peers.begin()->first) break;
					stop_count();
					cur = decode_int(cur, i);
					cur = decode_bool(cur, b);
					update_upg_mask(i, b);
					break;
				}
				break;
			case '\x01':
				cur = decode_char(cur, hh);
				switch(hh){
				case '\x00':
					if(e.peer_id != peers.begin()->first) break;
					start_count();
					break;
				case '\x01':
					stop_count();
					break;
				}
				break;
			}
			
			
			delete e.data;
			break;
		case NetEvent::TYPE_DISC:
			printf(	"Disconnection: id=%10d\n", e.peer_id);
			
			if(peers[e.peer_id]->size()>0) stop_count();
			
			while(peers[e.peer_id]->size()>0) remove_player(e.peer_id, peers[e.peer_id]->size()-1);
			delete peers[e.peer_id];
			peers.erase(e.peer_id);
			
			assign_host();
			
			break;
		case NetEvent::TYPE_NONE:
			count();
			break;
		}
	}
}
