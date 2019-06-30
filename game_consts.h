#ifndef _GAME_CONSTS_H
#define _GAME_CONSTS_H

#define TANK_TURN_N 72
#define TANK_TURN (2*M_PI/TANK_TURN_N)

#define WALL_THK 0.05

#define TANK_W 0.3
#define TANK_H 0.45

#define MAX_SHOTS 5

#define STEP_ANG TANK_TURN
#define STEP_DST 0.03
#define REV_RAT 0.5


#define CANNON_L 0.17

#define STEP_SHOT 0.04
#define SHOT_TTL 1200

#define SHOT_R 0.03

#define STEP_GATLING 0.05
#define GATLING_TTL 600

#define GATLING_R 0.015

#define GATLING_DIV DEG2RAD(5.0)

#define STEP_LASER 0.7
#define LASER_TTL 20

#define LASER_R 0.01

#define END_TIME 300

#define UPG_ANG 30.0

#define UPG_NUM 2
#define UPG_SIZE 0.3

struct Upgrade{
	int x,y;
	enum Type{
		GATLING,LASER,
	};
	Upgrade::Type type;
};

extern Upgrade::Type upg_types[UPG_NUM];

#endif