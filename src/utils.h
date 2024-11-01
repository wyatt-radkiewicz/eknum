#ifndef _utils_h_
#define _utils_h_

#if __GNUC__

#define mul_overflow(x, y, out) _Generic(x, \
	unsigned long long :	__builtin_umulll_overflow, \
	long long :		__builtin_smulll_overflow)(x, y, out)
#define add_overflow(x, y, out) _Generic(x, \
	unsigned long long :	__builtin_uaddll_overflow, \
	long long :		__builtin_saddll_overflow)(x, y, out)

#else // __GNUC__



#endif // __GNUC__

#endif

