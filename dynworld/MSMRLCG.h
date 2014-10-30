#ifndef msmrngH
#define msmrngH

class MSMRLCG
{
private:
	long m_seed;
public:
	void SetSeed(long new_seed);
	long TimeSeed();
	int NextNumber();
	int RangedNumber(int Max);
	float FloatNum();
	long GetSeed() { return m_seed; }
	MSMRLCG(bool time_seed);
	~MSMRLCG();
};

#endif

