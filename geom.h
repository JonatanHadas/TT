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

// rotates x,y by ang and adds to rx,ry
void rotate_add(double ang,double x, double y, double& rx, double& ry);

// both functions assume counter-clockwise convex polygons.

// if a radius r circle moves at vx,vy from ox,oy, when will it hit convex polygon xs[],ys[]   -1 if no colisions
double circ_poly_coltime(double ox, double oy,
						double vx, double vy,
						double r,
						double* xs, double* ys, int num,
						double& nx, double& ny);
				
// check convex polygon collision, nxy receive normal to collision, dp receive collision depth, pxy receive collision position
bool poly_coll(	double* x1s, double* y1s, int num1,
				double* x2s, double* y2s, int num2,
				double& nx, double& ny, double& dp,
				double& px, double& py);
				
// rects
// xs and ys mus be of length 4+
void gen_rect(double x, double y ,double w, double h, double* xs, double* ys);
// xs and ys mus be of length 4+
void gen_rot_rect(double cx, double cy ,double w, double h, double ang, double* xs, double* ys);


#endif