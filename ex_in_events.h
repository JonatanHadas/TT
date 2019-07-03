#ifndef _EX_IN_EVENTS_H
#define _EX_IN_EVENTS_H

#include "game.h"

class ExInEvents;

class ExInEvent{
public:
	enum Type{
		TYPE_TANK_UPDATE,
		TYPE_START_RND,
		TYPE_TANK_DEAD,
		TYPE_SCORE,
		TYPE_END_GAME,
		TYPE_SHT_CRT, TYPE_SHT_RMV,
		TYPE_FRG_CRT,
		TYPE_DTR_CRT,
		TYPE_UPG_CRT, TYPE_UPG_RMV,
		TYPE_MIN_CRT, TYPE_MIN_RMV,
		TYPE_MIN_ACT, TYPE_MIN_STR,
	};
	virtual Type get_type() = 0;
};
class ExInEvents{
public:
	virtual ExInEvent* get_event() = 0;
	virtual void push_ctrl(ControlState ctrl, int ind, int rnd) = 0;
};

class ExInEventStartRound : public ExInEvent{
public:
	Type get_type(){ return TYPE_START_RND; }
	virtual Maze* get_maze() = 0;
	virtual int get_x(int i) = 0; // tank positions
	virtual int get_y(int i) = 0;
	virtual double get_ang(int i) = 0;
};
class ExInEventTankDeath : public ExInEvent{
public:
	Type get_type(){ return TYPE_TANK_DEAD; }
	virtual int get_ind() = 0;
};
class ExInEventScore : public ExInEvent{
public:
	Type get_type(){ return TYPE_SCORE; }
	virtual int get_ind() = 0;
	virtual int get_diff() = 0;
};
class ExInEventEndGame : public ExInEvent{
public:
	Type get_type(){ return TYPE_END_GAME; }
};
class ExInEventCreateShot : public ExInEvent{
public:
	Type get_type(){ return TYPE_SHT_CRT; }
	virtual int get_id() = 0;
	virtual GenShot::Type get_stype() = 0;
	virtual double get_x() = 0;
	virtual double get_y() = 0;
	virtual double get_vx() = 0;
	virtual double get_vy() = 0;
	virtual int get_tank_ind() = 0;
	virtual long long int get_time() = 0;
	virtual int get_round() = 0;
};
class ExInEventRemoveShot : public ExInEvent{
public:
	Type get_type(){ return TYPE_SHT_RMV; }
	virtual int get_id() = 0;
};
class ExInEventTankUpdate : public ExInEvent{
public:
	Type get_type(){ return TYPE_TANK_UPDATE; }
	virtual int get_ind() = 0;
	virtual double get_x() = 0;
	virtual double get_y() = 0;
	virtual double get_ang() = 0;
	virtual long long int get_time() = 0;
	virtual ControlState get_ctrl() = 0;
	virtual Tank::State get_state() = 0;

	virtual double get_missile_x() = 0;
	virtual double get_missile_y() = 0;
	virtual double get_missile_ang() = 0;
	virtual bool get_missile_rt() = 0;
	virtual bool get_missile_lt() = 0;
	virtual int get_missile_id() = 0;
	virtual int get_missile_target() = 0;
	
};
class ExInEventCreateUpgrade : public ExInEvent{
public:
	Type get_type(){ return TYPE_UPG_CRT; }
	virtual Upgrade get_upg() = 0;
	virtual int get_round() = 0;
};
class ExInEventRemoveUpgrade : public ExInEvent{
public:
	Type get_type(){ return TYPE_UPG_RMV; }
	virtual int get_x() = 0;
	virtual int get_y() = 0;
	virtual int get_round() = 0;
};
class ExInEventCreateFragment : public ExInEvent{
public:
	Type get_type(){ return TYPE_FRG_CRT; }
	virtual double get_x() = 0;
	virtual double get_y() = 0;
	virtual double get_ang() = 0;
	virtual long long int get_time() = 0;
	virtual int get_round() = 0;
	virtual int get_id() = 0;
};
class ExInEventCreateDeathRay : public ExInEvent{
public:
	Type get_type(){ return TYPE_DTR_CRT; }
	virtual int get_point_num() = 0;
	virtual std::pair<double, double> get_point(int i) = 0;
	virtual int get_round() = 0;
	virtual int get_tank_ind() = 0;
	virtual int get_id() = 0;
};
class ExInEventCreateMine : public ExInEvent{
public:
	Type get_type(){ return TYPE_MIN_CRT; }
	virtual int get_id() = 0;
	virtual double get_x() = 0;
	virtual double get_y() = 0;
	virtual double get_ang() = 0;
	virtual int get_round() = 0;
	virtual int get_tank_ind() = 0;
};
class ExInEventRemoveMine : public ExInEvent{
public:
	Type get_type(){ return TYPE_MIN_RMV; }
	virtual int get_id() = 0;
};
class ExInEventStartMine : public ExInEvent{
public:
	Type get_type(){ return TYPE_MIN_STR; }
	virtual int get_id() = 0;
};
class ExInEventActivateMine : public ExInEvent{
public:
	Type get_type(){ return TYPE_MIN_ACT; }
	virtual int get_id() = 0;
};
#endif