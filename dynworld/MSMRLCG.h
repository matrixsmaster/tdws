/*
        MatrixS_Master's Random Numbers Generator (RNG)
        version One
*/

#ifndef msmrngH
#define msmrngH

class MSMRLCG
{
public:
	MSMRLCG(bool time_seed);
	virtual ~MSMRLCG() {}

	void SetSeed(long new_seed);
	long GetSeed() { return m_seed; }
	long TimeSeed();

	int NextNumber();
	int RangedNumber(int Max);
	float FloatNum();

private:
	long m_seed;
};

#endif
