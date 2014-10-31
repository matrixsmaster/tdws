/*
 * crazymath.cpp
 *
 *  Created on: Oct 26, 2014
 *      Author: matrixsmaster
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <time.h>
#include <math.h>
#include "crazymath.h"

inline double mPow(const double bas, const int ex)
{
	if (ex<1) return 1.0;
	double tmp = (double)ex;
	return (pow(bas,tmp));
}

//inline double fB(const int N, const int i, const double t, const double x)
//{
//	return (static_cast<double>(binom[i]) * mPow(t,i) * mPow((1-t),(N-i)) * x);
//}

inline float mfabs(const float x)
{
	if (x<0) return -x;
	return x;
}

int mcpow(float cp)
{
	int p = 0;
	if (cp>1) {
		do {
			cp /= 10;
			p++;
		} while (cp>1);
	} else if (cp<1) {
		do {
			cp *= 10;
			p--;
		} while (cp<1);
	} else p = 1;
	return p;
}

inline int msig(const float x)
{
	if (x<0) return -1;
	return 1;
}

inline int imax(const int a, const int b)
{
	return ((a>b)? a:b);
}

inline int imin(const int a, const int b)
{
	return ((a<b)? a:b);
}

float ranged(MSMRLCG* rnd, const float in)
{
//	int i;
	float x = (float)rnd->NextNumber()/(float)RAND_MAX;
	int p = mcpow(mfabs(in));
//	if (p>0)
//		for (i=0; i<p; i++) x *= 10;
//	else
//		for (i=p; i<0; i++) x /= 10;
	x *= powf(10.0,(float)p);
	return x;
}

float devi(MSMRLCG* rnd, const float org, const float bmin, const float bpls)
{
	float a;
	if (rnd->NextNumber() >= (RAND_MAX/2)) {
		while ((a=ranged(rnd,bmin)) > mfabs(bmin)) ;
		a *= (float)msig(bmin);
	} else
		while ((a=ranged(rnd,bpls)) > mfabs(bpls)) ;
	return (org+a);
}

//static int cmpint(const void* a, const void* b)
//{
//	if (*(int*)a < *(int*)b) return -1;
//	else if (*(int*)a == *(int*)b) return 0;
//	else return 1;
//}
//
//static int cmpint_rev(const void* a, const void* b)
//{
//	return (-cmpint(a,b));
//}

int selfrom(MSMRLCG* rnd, int* arr, const int len)
{
	int i,r;
	if ((len < 2) || (!arr)) return 0;
	r = -1;
	do {
		for (i = 0; i < len; i++)
			if (rnd->RangedNumber(100) <= arr[i]) {
				r = i;
				break;
			}
	} while (r < 0);
	return r;
}

CPoint2D pointonline(CPoint2D s, CPoint2D e, int step)
{
	int eps,cx,cy;
	CPoint2D d;
	if (s.X > e.X) {
		d = s;
		s = e;
		e = d;
	}
	d = e - s;
	if (step < 1) return s;
	eps = 2 * d.Y - d.X;
	cy = s.Y;
	for (cx = s.X+1; cx <= e.X; cx++) {
		if (eps > 0) {
			cy++;
			eps += 2*d.Y - 2*d.X;
		} else
			eps += 2*d.Y;
		if (!(step--)) break;
	}
	return CPoint2D(cx,cy);
}

CPoint2D getnextpoint(CPoint2D s, CPoint2D e)
{
	int i;
	float d,cd;
	CPoint2D r,c;
	e = e - s;
	d = e.Module();
	for (i = 0; i < 8; i++) {
		c = CPoint2D(0);
		switch (i) {
		case 0: c.Y++; break;
		case 1: c.X++; c.Y++; break;
		case 2: c.X++; break;
		case 3: c.X++; c.Y--; break;
		case 4: c.Y--; break;
		case 5: c.X--; c.Y--; break;
		case 6: c.X--; break;
		case 7: c.X--; c.Y++; break;
		}
		cd = distance(c,e);
		if (cd < d) {
			r = c;
			d = cd;
		}
	}
	return r + s;
}

void normpoint(CPoint2D* pnt, int x0, int y0, int x1, int y1)
{
	if (pnt->X < x0) pnt->X = x0;
	if (pnt->Y < y0) pnt->Y = y0;
	if (pnt->X >= x1) pnt->X = x1;
	if (pnt->Y >= y1) pnt->Y = y1;
}

bool ispointin(CPoint2D* pnt, int x0, int y0, int x1, int y1)
{
	return (!((pnt->X < x0) || (pnt->Y < y0) || (pnt->X >= x1) || (pnt->Y >= y1)));
}

float distance(CPoint2D a, CPoint2D b)
{
	if (a == b) return 0;
	float d = (a.X - b.X) * (a.X - b.X);
	d += (a.Y - b.Y) * (a.Y - b.Y);
	return (sqrtf(d));
}
