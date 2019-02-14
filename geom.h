#ifndef _GEOM_H
#define _GEOM_H


// positive if xy2 is left of xy1--xy2, negative if right
double leftness(double x0, double y0,
				double x1, double y1,
				double x2, double y2);

/*
are --xy1--xy2-- and xy3--xy4 intersecting
*/
double ls_intersect(double x1, double y1,
					double x2, double y2,
					double x3, double y3,
					double x4, double y4);
					
/*
are xy1--xy2 and xy3--xy4 intersecting
*/
double ss_intersect(double x1, double y1,
					double x2, double y2,
					double x3, double y3,
					double x4, double y4);



#endif