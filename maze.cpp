#include "maze.h"
#include <stdlib.h>


// saved: right and bottom of point
Maze::Maze(int wd, int ht){
	w=wd;h=ht;
	
	hwall = new bool*[w];
	vwall = new bool*[w-1];
	for(int i = 0; i<w; i++){
		if(w>0) vwall[i-1] = new bool[h];
		hwall[i] = new bool[h-1];
		for(int j = 0; j<h; j++){
			if(w>0) vwall[i-1][j] = false;
			if(h>0) hwall[i][j-1] = false;
		}
	}
}
Maze::~Maze(){
	for(int i = 0; i<w; i++){
		if(w>0) delete[] vwall[i-1];
		delete[] hwall[i];
	}
	delete[] hwall;
	delete[] vwall;
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
bool Maze::wwall(int x, int y){
	if(x < 0 || x >= w-1 || y < 0 || y >= h) return true;
	return vwalls[x][y];
}
void Maze::generate(std::vector<std::pair<int,int>> c_points, GenMethod m){
	switch(m){
	// TODO
	case GEN_NONE:
		break;
	}
}
