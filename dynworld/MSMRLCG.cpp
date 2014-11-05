/*
        MatrixS_Master's Random Numbers Generator (RNG)
        version One
 */

#include "time.h"
#include "MSMRLCG.h"

/* Park & Miller constants */

static const int a = 16807;
static const int c = 0;
static const int m = 2147483647;


MSMRLCG::MSMRLCG(bool time_seed)
{
	if (time_seed) TimeSeed();
}

void MSMRLCG::SetSeed(long new_seed)
{
	m_seed = new_seed ^ 0x5EECE66D;
}

long MSMRLCG::TimeSeed()
{
	time_t t;
	time(&t);
	long tt = (long) t;
	SetSeed(tt);
	return tt;
}

int MSMRLCG::NextNumber()
{
	m_seed = ( a * m_seed + c ) % m;
	int v = m_seed;
	m_seed = ( a * m_seed + c ) % m;
	v += (m_seed >> 8) & 0xFF;
	return v;
}

int MSMRLCG::RangedNumber(int Max)
{
	float a;
	int p = 1;
	int tmp = Max;
	if (Max < 1) return 0;
	do {
		tmp /= 10;
		p *= 10;
	} while (tmp >= 1.0);
	do {
		a = FloatNum();
		a *= p;
		tmp = static_cast<int> (a);
	} while (tmp >= Max);
	return tmp;
}

float MSMRLCG::FloatNum()
{
	int v = NextNumber();
	v &= 0x7FFFFFF;
	return float(v) / float(0x8000000);
}
