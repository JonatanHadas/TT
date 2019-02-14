#include "geom.h"

#include <math.h>

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
		if( b*b-4*a*c >= 0){
			double tt = (b-sqrt(b*b-4*a*c))/(2*a)
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
}