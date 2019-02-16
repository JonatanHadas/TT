#ifndef _GEOM_H
#define _GEOM_H

#define _USE_MATH_DEFINES
#include "math.h"

#define DEG2RAD(x) ((x) * M_PI / 180.0)
#define RAD2DEG(x) ((x) * 180.0 / M_PI)

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

// rotates x,y by ang and adds to rx,ry
void rotate_add(double ang,double x, double y, double& rx, double& ry);

// if a radius r circle moves at vx,vy from ox,oy, when will it hit xs[],ys[] -1 if no colisions
double circ_lines_coltime(double ox, double oy,
						double vx, double vy,
						double r,
						double* xs, double* ys, int num, bool closed);

						
#endif