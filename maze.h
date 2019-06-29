#ifndef _MAZE_H
#define _MAZE_H

#include <vector>

class Maze{
	bool** hwalls;
	bool** vwalls;
	int w,h;
public:
	enum GenMethod{
		GEN_NONE,
		GEN_WALL_REM,
		GEN_EXP_TREE,
	};
	Maze(int w, int h);
	~Maze();
	int get_w();
	int get_h();
	bool hwall(int x, int y); // below point
	bool vwall(int x, int y); // to right of point
	bool set_hwall(int x, int y, bool val);
	bool set_vwall(int x, int y, bool val);
	void generate(std::vector<std::pair<int,int>> c_points, GenMethod m); // critical points must be connected
};

#endif