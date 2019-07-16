#ifndef _GAME_QUERY_H
#define _GAME_QUERY_H

#include "maze.h"
#include "game.h"

class TankQ;
class RoundQ;
class GameQ;
class TeamQ;

class GenShotQ;
class ShotQ;

class MineQ;

class GameQEvent;
class GameQEventStartRnd;
class GameQEventTankDeath;
class GameQEventScore;
class GameQEventEndGame;
class GameQEventCreateShot;
class GameQEventRemoveShot;
class GameQEventCreateUpgrade;
class GameQEventRemoveUpgrade;

class GameQEvent{
public:
	enum Type{
		TYPE_RND_START,
		TYPE_TANK_DEAD,
		TYPE_SCORE,
		TYPE_END_GAME,
		TYPE_SHOT_CRT, TYPE_SHOT_RMV, 
		TYPE_UPG_CRT, TYPE_UPG_RMV,
		TYPE_MIN_CRT,TYPE_MIN_ACT,
		TYPE_TANK_STUCK,
		TYPE_COLL, TYPE_EXPL, TYPE_LOAD,
	};
	virtual Type get_type() = 0;
};

class GameQEventStartRnd : public GameQEvent{
public:
	Type get_type(){return GameQEvent::TYPE_RND_START;}
};
class GameQEventTankDeath : public GameQEvent{
public:
	Type get_type(){return GameQEvent::TYPE_TANK_DEAD;}
	virtual int get_ind() = 0;
};
class GameQEventScore : public GameQEvent{
public:
	Type get_type(){return GameQEvent::TYPE_SCORE;}
	virtual int get_ind() = 0;
	virtual int get_diff() = 0;
};
class GameQEventEndGame : public GameQEvent{
public:
	Type get_type(){return GameQEvent::TYPE_END_GAME;}
	virtual std::vector<int>& get_scores() = 0;
};
class GameQEventCreateShot : public GameQEvent{
public:
	Type get_type(){return GameQEvent::TYPE_SHOT_CRT;}
	virtual int get_id() = 0;
	virtual GenShot::Type get_stype() = 0;
	virtual double get_x() = 0;
	virtual double get_y() = 0;
	virtual double get_ang() = 0;
	virtual int get_tank_ind() = 0;
};
class GameQEventRemoveShot : public GameQEvent{
public:
	Type get_type(){return GameQEvent::TYPE_SHOT_RMV;}
	virtual int get_id() = 0;
	virtual double get_x() = 0;
	virtual double get_y() = 0;
	virtual double get_vx() = 0;
	virtual double get_vy() = 0;
	virtual GenShot::Type get_stype() = 0;
};
class GameQEventCreateUpgrade : public GameQEvent{
public:
	Type get_type(){return GameQEvent::TYPE_UPG_CRT;}
	virtual Upgrade get_upg() = 0;
};
class GameQEventRemoveUpgrade : public GameQEvent{
public:
	Type get_type(){return GameQEvent::TYPE_UPG_RMV;}
	virtual int get_x() = 0;
	virtual int get_y() = 0;
};
class GameQEventCreateMine : public GameQEvent{
public:
	Type get_type(){return GameQEvent::TYPE_MIN_CRT;}
	virtual double get_x() = 0;
	virtual double get_y() = 0;
	virtual double get_ang() = 0;
	virtual int get_ind() = 0;
};
class GameQEventTankStuck : public GameQEvent{
public:
	Type get_type(){return GameQEvent::TYPE_TANK_STUCK;}
	virtual get_ind() = 0;
	virtual double get_spd() = 0;
};
class GameQEventEtc : public GameQEvent{
	Type tp;
public:
	GameQEventEtc(Type t){tp = t;}
	Type get_type(){return tp;}
};

class GameQ{
public:
	virtual ~GameQ(){};
	virtual int get_tank_num() = 0;
	virtual TankQ* get_tank(int i) = 0;
	virtual int get_team_num() = 0;
	virtual TeamQ* get_team(int i) = 0;
	virtual RoundQ* get_round() = 0;
	virtual GameQEvent* get_event() = 0;
	
	virtual void advance() = 0;
	
	virtual void leave(){}
	virtual long long int get_time() = 0;
};
class TeamQ{
public:
	virtual int get_ind() = 0;
	virtual int get_score() = 0;
	virtual int get_tank_num() = 0;
	virtual TankQ* get_tank(int i) = 0;
};
class TankQ{
public:
	virtual double get_x() = 0;
	virtual double get_y() = 0;
	virtual double get_ang() = 0;
	virtual bool is_dead() = 0;
	
	virtual Tank::State get_state() = 0;
	
	virtual int get_ind() = 0;
	virtual TeamQ* get_team() = 0;
	
	virtual void push_ctrl(ControlState ctrl) = 0;
};
class RoundQ{
public:
	virtual Maze* get_maze() = 0;
	virtual std::set<GenShotQ*> get_shots() = 0;
	virtual std::set<MineQ*> get_mines() = 0;
	virtual std::set<Upgrade*> get_upgs() = 0;
};
class GenShotQ{
public:
	virtual double get_x() = 0;
	virtual double get_y() = 0;
	virtual double get_ang() = 0;
	virtual GenShot::Type get_type() = 0;
	virtual int get_tank_ind() = 0;
};

class ShotQ : public GenShotQ{
public:
	virtual double get_r() = 0;
	virtual std::vector<std::pair<double,double>>& get_colls() = 0;
};

class FragmentQ : public GenShotQ{
public:
	virtual double get_dst() = 0;
	virtual double get_t() = 0;
};

class DeathRayQ : public GenShotQ{
public:
	virtual int get_point_num() = 0;
	virtual double get_x(int i) = 0;
	virtual double get_y(int i) = 0;
};

class MissileQ : public GenShotQ{
public:
	virtual int get_tar_ind() = 0;
	virtual int get_time() = 0;
};

class MineQ{
public:
	virtual double get_x() = 0;
	virtual double get_y() = 0;
	virtual double get_ang() = 0;
	virtual bool get_started() = 0;
	virtual bool get_active() = 0;
	virtual int get_tank_ind() = 0;
};

#endif