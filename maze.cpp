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
#include <stdio.h>
void exp_tree(int w, int h, bool** hw, bool** vw, int avg){
	bool** vis = new bool*[w];
	for(int i = 0; i<w; i++){
		vis[i] = new bool[h];
		for(int j = 0; j<h; j++){
			vis[i][j] = false;
			if(i<w-1) vw[i][j] = true;
			if(j<h-1) hw[i][j] = true;
		}
	}

	int x = rand_range(0,w), y = rand_range(0,h);
	vis[x][y] = true;
	int cnt = 1;
	while(cnt < w*h){
		int dx = 0, dy = 0;
		switch(rand_range(0,4)){
		case 0:
			dx=1;
			break;
		case 1:
			dx=-1;
			break;
		case 2:
			dy=1;
			break;
		case 3:
			dy=-1;
			break;
		}
		int nx = x+dx;
		int ny = y+dy;
		if(nx >= 0 && nx < w && ny >= 0 && ny < h){
			if(!vis[nx][ny]){
				cnt++;
				if(dx) vw[nx<x ? nx : x][y] = false;
				if(dy) hw[x][ny<y ? ny : y] = false;
			}
			vis[nx][ny] = true;
			x = nx; y = ny;
		}
	}
	avg = rand_range(avg/2, avg*3/2);
	for(int i = 0; i<avg; i++){
		if(rand_range(0,2)<1){
			hw[rand_range(0,w)][rand_range(0,h-1)] = false;
		}
		else{
			vw[rand_range(0,w-1)][rand_range(0,h)] = false;
		}
	}
	
	
	for(int i = 0; i<w; i++) delete[] vis[i];
	delete[] vis;
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
		exp_tree(w,h,hwalls,vwalls,(w+h)*2);
		break;
	case GEN_WALL_REM:
		wall_rem(w,h,hwalls,vwalls,c_points);
		break;
	}
}
