#ifndef _LONRAND_H
#define _LONRAND_H

#ifndef _WIN32
#define __int64 long long
#endif
#ifndef _SHL_U64
#define _SHL_U64

/* */
inline void Shl_U64(unsigned __int64 &Arg, int Shift)
{
	((unsigned long *) &Arg)[1]<<=Shift;
	((unsigned long *) &Arg)[1]|=((unsigned long *) &Arg)[0] >> (32 - Shift);
	((unsigned long *) &Arg)[0]<<=Shift;
}
#endif
#ifndef _SHR_U64
#define _SHR_U64

/* */
inline void Shr_U64(unsigned __int64 &Arg, int Shift)
{
	((unsigned long *) &Arg)[0]>>=Shift;
	((unsigned long *) &Arg)[0]|=((unsigned long *) &Arg)[1] << (32 - Shift);
	((unsigned long *) &Arg)[1]>>=Shift;
}
#endif

// Here is the random number algorithm issued by Lewis, Goodman and Miller in 1969:
// 1. Multiplier = 5 ^ 7;
// 2. Divisor = 2 ^ 31 - 1, which is a prime number;
// 3. Seed *= Multiplier;

// 4. Seed %= Divisor;
inline unsigned long longrand(unsigned long &Arg)
{
	unsigned __int64    LongSeed, LongTemp;

	// Here is a smart algorithm to multiply 100000110100111b (5 ^ 7)
	LongSeed=Arg;
	Shl_U64(LongSeed, 6);
	LongSeed+=Arg;
	Shl_U64(LongSeed, 1);
	LongSeed+=Arg;
	Shl_U64(LongSeed, 2);
	LongSeed+=Arg;
	Shl_U64(LongSeed, 3);
	LongSeed+=Arg;
	Shl_U64(LongSeed, 1);
	LongSeed+=Arg;
	Shl_U64(LongSeed, 1);
	LongSeed+=Arg;

	// Here is a smart algorithm to be divided by 7fffffffh (2 ^ 31 - 1)
	LongTemp=LongSeed;
	Shr_U64(LongTemp, 31);
	LongSeed+=LongTemp;
	Shl_U64(LongTemp, 31);
	LongSeed-=LongTemp;
	if(LongSeed >= 0x7fffffff)
	{
		LongSeed-=0x7fffffff;
	}

	Arg= (unsigned long)LongSeed;
	return Arg;
};
#endif
