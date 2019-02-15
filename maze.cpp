#include "maze.h"
#include <stdlib.h>

#include "utils.h"

void wall_rem(int w, int h, bool** hw, bool** vw, std::vector<std::pair<int,int>> c_points){
	UnionFind f(w*h);
	std::vector<int> cp;
	for(int i = 0; i<c_points.size(); i++) cp.push_back(c_points[i].first + c_points[i].second * w);
	for(int i = 0; i<w; i++){
		for(int j = 0; j<h; j++){
			if(i>0) vw[i-1][j] = true;
			if(j>0) hw[i][j-1] = true;
		}
	}
	while(true){		
		bool conn = true;
		for(int i = 1; i<cp.size(); i++) if(!f.same(cp[0],cp[i])) conn = false;
		if(conn) break;
		
		
		if(rand_range(0,2)){
			int x = rand_range(0,w), y = rand_range(0,h-1);
			hw[x][y] = false;
			f.unite(x+w*y, x + w*y + w);
		}
		else{
			int x = rand_range(0,w-1), y = rand_range(0,h);
			vw[x][y] = false;
			f.unite(x+w*y, x + w*y + 1);
		}
	}
}
void exp_tree(int w, int h, bool** hw, bool** vw, int avg){
	
}


// saved: right and bottom of point
Maze::Maze(int wd, int ht){
	w=wd;h=ht;
	
	hwalls = new bool*[w];
	vwalls = new bool*[w-1];
	for(int i = 0; i<w; i++){
		if(w>0) vwalls[i-1] = new bool[h];
		hwalls[i] = new bool[h-1];
		for(int j = 0; j<h; j++){
			if(w>0) vwalls[i-1][j] = false;
			if(h>0) hwalls[i][j-1] = false;
		}
	}
}
Maze::~Maze(){
	for(int i = 0; i<w; i++){
		if(w>0) delete[] vwalls[i-1];
		delete[] hwalls[i];
	}
	delete[] hwalls;
	delete[] vwalls;
}
int Maze::get_w(){
	return w;
}
int Maze::get_h(){
	return h;
}
bool Maze::hwall(int x, int y){
	if(x < 0 || x >= w || y < 0 || y >= h-1) return true;
	return hwalls[x][y];
}
bool Maze::vwall(int x, int y){
	if(x < 0 || x >= w-1 || y < 0 || y >= h) return true;
	return vwalls[x][y];
}
void Maze::generate(std::vector<std::pair<int,int>> c_points, GenMethod m){
	switch(m){
	// TODO
	case GEN_NONE:
		break;
	case GEN_EXP_TREE:
		exp_tree(w,h,hwalls,vwalls,(w+h)*10);
		break;
	case GEN_WALL_REM:
		wall_rem(w,h,hwalls,vwalls,c_points);
		break;
	}
}
