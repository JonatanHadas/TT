#ifndef _MAZE_H
#define _MAZE_H

class Maze{
	bool** hwalls;
	bool** vwalls;
	int w,h;
public:
	enum GenMethod{
		GEN_NONE,
	}
	Maze(int w, int h);
	~Maze();
	int get_w();
	int get_h();
	bool hwall(int x, int y); // below point
	bool wwall(int x, int y); // to right of point
	void generate(std::vector<std::pair<int,int>> c_points, GenMethod m); // critical points must be connected
}

#endif