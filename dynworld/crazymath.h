/*
 * crazymath.h
 *
 *  Created on: Nov 13, 2013
 *      Author: master
 */

#include "MSMRLCG.h"
#include "CPoint2D.h"

#ifndef CRAZYMATH_H_
#define CRAZYMATH_H_

/*
 * adapted to use msm's rlc generator (oct'14)
 */

double mPow(const double bas, const int ex);
//double fB(const int N, const int i, const double t, const double x);
float mfabs(const float x);
int mcpow(float cp);
int msig(const float x);
inline int imax(const int a, const int b);
inline int imin(const int a, const int b);
float ranged(MSMRLCG* rnd, const float in);
float devi(MSMRLCG* rnd, const float org, const float bmin, const float bpls);
int selfrom(MSMRLCG* rnd, int* arr, const int len);
CPoint2D pointonline(CPoint2D s, CPoint2D e, int step);
CPoint2D getnextpoint(CPoint2D s, CPoint2D e);
void normpoint(CPoint2D* pnt, int x0, int y0, int x1, int y1);
bool ispointin(CPoint2D* pnt, int x0, int y0, int x1, int y1);
float distance(CPoint2D a, CPoint2D b);

#endif /* CRAZYMATH_H_ */
