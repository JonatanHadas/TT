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
		if(i>0) vwalls[i-1] = new bool[h];
		hwalls[i] = new bool[h-1];
		for(int j = 0; j<h; j++){
			if(i>0) vwalls[i-1][j] = false;
			if(j>0) hwalls[i][j-1] = false;
		}
	}
	
	dists = dxs = dys = NULL;
}
#include <stdio.h>
Maze::~Maze(){
	clear_dists();
	for(int i = 0; i<w; i++){
		if(i>0) delete[] vwalls[i-1];
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
void Maze::set_hwall(int x, int y, bool val){
	hwalls[x][y] = val;
}
void Maze::set_vwall(int x, int y, bool val){
	vwalls[x][y] = val;
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

void Maze::calc_dists(){
	clear_dists();
	
	dists = new int***[w];
	dxs = new int***[w];
	dys = new int***[w];
	
	for(int x1 = 0; x1<w; x1++){
	dists[x1] = new int**[h];
	dxs[x1] = new int**[h];
	dys[x1] = new int**[h];
	for(int y1 = 0; y1<h; y1++){
		dists[x1][y1] = new int*[w];
		dxs[x1][y1] = new int*[w];
		dys[x1][y1] = new int*[w];
		for(int x2 = 0; x2<w; x2++){
		dists[x1][y1][x2] = new int[h];
		dxs[x1][y1][x2] = new int[h];
		dys[x1][y1][x2] = new int[h];
		for(int y2 = 0; y2<h; y2++){
			if(	(x1==x2 && y1==y2+1 && !hwall(x2,y2)) ||
				(x1==x2 && y1==y2-1 && !hwall(x1,y1)) ||
				(x1==x2+1 && y1==y2 && !vwall(x2,y2)) ||
				(x1==x2-1 && y1==y2 && !vwall(x1,y1))){
				
				dxs[x1][y1][x2][y2] = x2-x1;
				dys[x1][y1][x2][y2] = y2-y1;
				dists[x1][y1][x2][y2] = 1;
				
			}
			else if (x1 == x2 && y1 == y2){
				dists[x1][y1][x2][y2] = 0;
				dxs[x1][y1][x2][y2] = 0;
				dys[x1][y1][x2][y2] = 0;
			}
			else{
				dists[x1][y1][x2][y2] = -1;
				dxs[x1][y1][x2][y2] = 0;
				dys[x1][y1][x2][y2] = 0;
			}
		}
		}
	}
	}
	for(int x1 = 0; x1<w; x1++){
	for(int y1 = 0; y1<h; y1++){
		for(int x2 = 0; x2<w; x2++){
		for(int y2 = 0; y2<h; y2++){
			for(int x3 = 0; x3<w; x3++){
			for(int y3 = 0; y3<h; y3++){
				if(dists[x2][y2][x1][y1]<0 || dists[x1][y1][x3][y3]<0) continue;
				int nd = dists[x2][y2][x1][y1] + dists[x1][y1][x3][y3];
				if(nd<dists[x2][y2][x3][y3] || dists[x2][y2][x3][y3]<0){
					dists[x2][y2][x3][y3] = nd;
					dxs[x2][y2][x3][y3] = dxs[x2][y2][x1][y1];
					dys[x2][y2][x3][y3] = dys[x2][y2][x1][y1];
				}
			}
			}	
		}
		}	
	}
	}
}
void Maze::clear_dists(){
	if(dists){
		for(int x1=0;x1<w;x1++){
			for(int y1=0;y1<h;y1++){
				for(int x2=0;x2<w;x2++){
					delete[] dists[x1][y1][x2];
					delete[] dxs[x1][y1][x2];
					delete[] dys[x1][y1][x2];
				}
				delete[] dists[x1][y1];
				delete[] dxs[x1][y1];
				delete[] dys[x1][y1];
			}
			delete[] dists[x1];
			delete[] dxs[x1];
			delete[] dys[x1];
		}
		delete[] dists;
		delete[] dxs;
		delete[] dys;
	}
	dxs = dys = dists = NULL;
}
int Maze::dist(int x1,int y1, int x2, int y2, int& dx, int& dy){
	if(x1<0 || x2<0 || y1<0 || y2<0 || x1>=w || x2>=w || y1>=h || y2>=h) return -1;
	dx = dxs[x1][y1][x2][y2];
	dy = dys[x1][y1][x2][y2];
	return dists[x1][y1][x2][y2];
}
