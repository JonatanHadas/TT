#ifndef _MAZE_H
#define _MAZE_H

#include <vector>

class Maze{
	bool** hwalls;
	bool** vwalls;
	int**** dxs;
	int**** dys;
	int**** dists;
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
	void generate(std::vector<std::pair<int,int>> c_points, GenMethod m); // critical points must be connected
	
	void calc_dists();
	void clear_dists();
	int dist(int x1,int y1, int x2, int y2, int& dx, int& dy);
};

#endif