#include "geom.h"

#include <math.h>
#include <stdio.h>

double leftness(double x0, double y0,
				double x1, double y1,
				double x2, double y2){
	return (x0-x1)*(y2-y1)-(x2-x1)*(y0-y1);
}

/*
are --xy1--xy2-- and xy3--xy4 intersecting
*/
double ls_intersect(double x1, double y1,
					double x2, double y2,
					double x3, double y3,
					double x4, double y4){
	
	return leftness(x3,y3,x1,y1,x2,y2)*leftness(x4,y4,x1,y1,x2,y2) <= 0;
}

double ss_intersect(double x1, double y1,
					double x2, double y2,
					double x3, double y3,
					double x4, double y4){
	return ls_intersect(x1,y1,x2,y2,x3,y3,x4,y4) && ls_intersect(x3,y3,x4,y4,x1,y1,x2,y2);
}

void rotate_add(double ang,double x, double y, double& rx, double& ry){
	rx += x*cos(ang) - y*sin(ang);
	ry += x*sin(ang) + y*cos(ang);
}

double circ_poly_coltime(double ox, double oy,
						double vx, double vy,
						double r,
						double* xs, double* ys, int num,
						double& nx, double& ny){
	
	double t = -1;
	bool first = true;
	for(int i = 0; i<num; i++){
		// corner
		double x = ox-xs[i], y = oy-ys[i];
		double a = vx*vx + vy*vy;
		double b = vx*x + vy*y;
		double c = x*x + y*y - r*r;
		if(b*b > a*c){
			double tt = -(b + sqrt(b*b-a*c))/a;
			if( first || tt<t){
				t = tt;
				nx = x + t*vx;
				ny = y + t*vy;
				nx/=r;
				ny/=r;
				first = false;
			}
		}
		
		//edge
		double dx = xs[(i+1)%num] - xs[i];
		double dy = ys[(i+1)%num] - ys[i];
		
		double nnx = dy;
		double nny = -dx;
		double l = sqrt(nnx*nnx+nny*nny);
		nnx/=l; nny/=l;
		if(vx*nnx + vy*nny == 0) continue;
		double tt = (r - x*nnx - y*nny) / (vx*nnx + vy*nny);
		if( (first || tt<t) && ls_intersect(x,y,x+vx,y+vy,nnx*r,nny*r,dx+nnx*r,dy+nny*r) ){
			t = tt;
			nx = nnx;
			ny = nny;
			first = false;
		}
	}
	return t;
}
				

bool poly_coll(	double* x1s, double* y1s, int num1,
				double* x2s, double* y2s, int num2,
				double& nx, double& ny, double& dp,
				double& px, double& py){
	
	for(int i = 0,j = num2; i<num1; i++){
		double cdx = x1s[(i+1)%num1]-x1s[i], cdy = y1s[(i+1)%num1]-y1s[i];
		while(	leftness(	x2s[(j+1)%num2]-x2s[(j)%num2],y2s[(j+1)%num2]-y2s[(j)%num2],
							0,0,cdx,cdy) <= 0) j++;
		while(	leftness(	x2s[(j-1)%num2]-x2s[(j)%num2],y2s[(j-1)%num2]-y2s[(j)%num2],
							0,0,cdx,cdy) < 0) j--;
		double l = sqrt(cdx*cdx+cdy*cdy);
		cdx /= l; cdy /= l;
		double cdp = (cdy * (x2s[j%num2]-x1s[i]) - cdx * (y2s[j%num2]-y1s[i]));
		if(cdp >= 0) {
			return false;
		}
		if(-cdp < dp || i==0){
			dp = -cdp;
			nx = cdy; ny = -cdx;
			px = x2s[j%num2]; py = y2s[j%num2];
		}
	}

	for(int i = 0,j = num1; i<num2; i++){
		double cdx = x2s[(i+1)%num2]-x2s[i], cdy = y2s[(i+1)%num2]-y2s[i];
		while(	leftness(	x1s[(j+1)%num1]-x1s[(j)%num1],y1s[(j+1)%num1]-y1s[(j)%num1],
							0,0,cdx,cdy) <= 0) j++;
		while(	leftness(	x1s[(j-1)%num1]-x1s[(j)%num1],y1s[(j-1)%num1]-y1s[(j)%num1],
							0,0,cdx,cdy) < 0) j--;
		double l = sqrt(cdx*cdx+cdy*cdy);
		cdx /= l; cdy /= l;
		double cdp = (cdy * (x1s[j%num1]-x2s[i]) - cdx * (y1s[j%num1]-y2s[i]));
		if(cdp >= 0){
			return false;
		}
		if(-cdp < dp){
			dp = -cdp;
			nx = -cdy; ny = cdx;
			px = x1s[j%num1]; py = y1s[j%num1];
		}
	}
	
	return true;
}


void gen_rect(double x, double y ,double w, double h, double* xs, double* ys){
	xs[0] = xs[3] = x;
	xs[2] = xs[1] = x+w;
	ys[0] = ys[1] = y;
	ys[3] = ys[2] = y+h;
}

void gen_rot_rect(double cx, double cy ,double w, double h, double ang, double* xs, double* ys){
	xs[0] = xs[1] = xs[2] = xs[3] = cx;
	ys[0] = ys[1] = ys[2] = ys[3] = cy;
	rotate_add(ang,-w/2,-h/2,xs[0],ys[0]);
	rotate_add(ang,w/2,-h/2,xs[1],ys[1]);
	rotate_add(ang,w/2,h/2,xs[2],ys[2]);
	rotate_add(ang,-w/2,h/2,xs[3],ys[3]);
}
