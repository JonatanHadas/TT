#include "geom_test.h"
#include "geom.h"

#include <stdio.h>

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