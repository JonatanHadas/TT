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

double circ_lines_coltime(double ox, double oy,
						double vx, double vy,
						double r,
						double* xs, double* ys, int num, bool closed){
	
	double t = -1;
	for(int i = 0; i<num; i++){
		double x = xs[i]-ox, y = ys[i]-oy;
		double a = vx*vx+vy*vy, b = 2*(vx*x+vy*y), c = x*x+y*y-r*r;
		double tt;
		if( b*b-4*a*c >= 0){
			tt = (b-sqrt(b*b-4*a*c))/(2*a);
		}
		if(tt>=0 && tt<t) t=tt;
		
		if(i<num-1 || closed){
			double xx = xs[(i+1)%num]-ox, yy = ys[(i+1)%num]-oy;
			double lsq = (x-xx)*(x-xx)+(y-yy)*(y-yy);
			double dx = r*(y-yy)/sqrt(lsq), dy = -r*(x-xx)/sqrt(lsq);
			if(leftness(0,0,x+dx,y+dy,xx+dx,yy+dy)*leftness(0,0,x-dx,y-dy,xx-dx,yy-dy) > 0){
				if(ls_intersect(0,0,vx,vy,x+dx,y+dy,xx+dx,yy+dy)){
					tt = ((x+dx)*dx + (y+dy)*dy)/(vx*dx-vy*dy);
					if(tt>0 && tt<t) t=tt;
				}
				if(ls_intersect(0,0,vx,vy,x-dx,y-dy,xx-dx,yy-dy)){
					tt = ((x-dx)*dx + (y-dy)*dy)/(vx*dx-vy*dy);
					if(tt>0 && tt<t) t=tt;
				}
			}
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
			printf("%d %d\n",i,j);
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
			printf(" %d %d\n",i,j);
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
