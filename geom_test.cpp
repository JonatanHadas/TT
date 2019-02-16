#include "geom_test.h"
#include "geom.h"

#include <stdio.h>
#include <vector>

void test_poly(SDL_Renderer* rend){
	std::vector<double> x1s,y1s,x2s,y2s;
	std::vector<SDL_Point> p1,p2;
	p1.push_back({0,0});
	p2.push_back({0,0});
	while(true){
		SDL_Event e;
		if(SDL_WaitEvent(&e)){
				switch(e.type){
				case SDL_QUIT:
					return;
				case SDL_MOUSEBUTTONDOWN:
					switch(e.button.button){
					case SDL_BUTTON_MIDDLE:
						x1s.clear(); x2s.clear(), y1s.clear(); y2s.clear();
						p1.clear(); p2.clear();
						p1.push_back({0,0});
						p2.push_back({0,0});
						break;
					case SDL_BUTTON_LEFT:
						p1[0].x = e.button.x;
						p1[0].y = e.button.y;
						p1.push_back(p1[0]);
						x1s.push_back(p1[0].x);
						y1s.push_back(p1[0].y);
						break;
					case SDL_BUTTON_RIGHT:
						p2[0].x = e.button.x;
						p2[0].y = e.button.y;
						p2.push_back(p2[0]);
						x2s.push_back(p2[0].x);
						y2s.push_back(p2[0].y);
						break;
					}
					break;
				}
		}
		SDL_SetRenderDrawColor(rend, 255,255,255,255);
		SDL_RenderClear(rend);
		double nx,ny,px,py,dp;
		SDL_SetRenderDrawColor(rend,0,0,0,255);
		if(x1s.size()>2 && x2s.size()>2){
			if(poly_coll(	&(x1s[0]),&(y1s[0]),x1s.size(),
							&(x2s[0]),&(y2s[0]),x2s.size(),
							nx,ny,dp,px,py)){
				SDL_SetRenderDrawColor(rend,0,255,255,255);
				SDL_Rect r;
				r.w = r.h = 4;
				r.x = px-r.w/2;
				r.y = py-r.h/2;
				SDL_RenderFillRect(rend,&r);
				SDL_SetRenderDrawColor(rend,0,0,255,255);
				SDL_RenderDrawLine(rend, px,py,px+dp*nx,py+dp*ny);
				SDL_SetRenderDrawColor(rend,0,255,0,255);
				SDL_RenderDrawLine(rend, px,py,px-dp*nx,py-dp*ny);
				SDL_SetRenderDrawColor(rend,255,0,0,255);
			}
		}
		SDL_RenderDrawLines(rend, &(p1[0]), p1.size());
		SDL_RenderDrawLines(rend, &(p2[0]), p2.size());
		
		SDL_RenderPresent(rend);
	}
}

void test_intersection(SDL_Renderer* rend){
	int s=0;
	int xs[4] = {-1,-1,-1,-1};
	int ys[4] = {-1,-1,-1,-1};
	while(true){
		SDL_Event e;
		if(SDL_WaitEvent(&e)){
				switch(e.type){
				case SDL_QUIT:
					return;
				case SDL_MOUSEMOTION:
					xs[s] = e.motion.x;
					ys[s] = e.motion.y;
					break;
				case SDL_MOUSEBUTTONDOWN:
					xs[s] = e.motion.x;
					ys[s] = e.motion.y;
					if(e.button.button == SDL_BUTTON_LEFT) s++;
					break;
				}
		}
		s %= 4;
		SDL_SetRenderDrawColor(rend, 255,255,255,255);
		SDL_RenderClear(rend);
				
		bool ls = ls_intersect(xs[0],ys[0],xs[1],ys[1],xs[2],ys[2],xs[3],ys[3]);
		bool ss = ss_intersect(xs[0],ys[0],xs[1],ys[1],xs[2],ys[2],xs[3],ys[3]);
		
		SDL_SetRenderDrawColor(rend, ls ? 255 : 0,0,0,255);
		SDL_RenderDrawLine(rend,xs[0],ys[0],xs[1],ys[1]);
		SDL_SetRenderDrawColor(rend, ss ? 255 : 0,0,255,255);
		SDL_RenderDrawLine(rend,xs[2],ys[2],xs[3],ys[3]);
		SDL_SetRenderDrawColor(rend, 0,0,0,255);
		for(int i = 0; i<4; i++){
			SDL_Rect r;
			r.w = r.h = 4;
			r.x = xs[i]-r.w/2; r.y = ys[i]-r.h/2;
			SDL_RenderFillRect(rend,&r);
		}
		SDL_RenderPresent(rend);
	}
}