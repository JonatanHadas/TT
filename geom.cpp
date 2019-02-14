#include "geom.h"

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