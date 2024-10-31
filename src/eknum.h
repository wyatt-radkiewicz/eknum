#ifndef _eknum_h_
#define _eknum_h_
// To use eknum, include eknum.h.
// Since eknum is a single header library include eknum.h into a single c file
// in your project and define EKNUM_IMPL before it to include the
// implementation
//
// Implementation options:
// #define EKNUM_SPACE_EFFICIENT 1
//	This makes some tables smaller at the cost of performance
// #define EKNUM_NO_BITWISE 1
//	This disables 64-bit tricks to make the code faster
//

#include <stdint.h>
#include <stdbool.h>


// Parse a float from a string.
// If str does not start with a valid float of either base 10 or 16
// If base is 0, then the parser will see what base it is by the prefix,
// otherwise one may put 10 or 16 for base.
// If end is specified, it will return the character after the end of the
// number or NULL if there was an error. If there was an error, then
// the returned value is 0.
//
// Notes: The integer parsers can also do base 2, 8, 10, and 16
double ekflt_parse(const char *str, unsigned base, const char **end);
int64_t ekint_parse(const char *str, unsigned base, const char **end);
uint64_t ekuint_parse(const char *str, unsigned base, const char **end);

// Quickly validates a string to see if its a valid flt/int/uint etc
bool ekflt_validate(const char *str, unsigned base, const char **end);
bool ekint_validate(const char *str, unsigned base, const char **end);
bool ekuint_validate(const char *str, unsigned base, const char **end);

// Parses a number without any error checking. This is useful if you know
// your string is already a valid float/int etc.
double ekflt_parse_unsafe(const char *str, unsigned base);
int64_t ekint_parse_unsafe(const char *str, unsigned base);
uint64_t ekuint_parse_unsafe(const char *str, unsigned base);

#define EKNUM_IMPL
#ifdef EKNUM_IMPL

// Always inline macros
#ifdef __GNUC__
#define EKNUM_ALWAYS_INLINE inline __attribute__((always_inline))
#else
#define EKNUM_ALWAYS_INLINE
#endif

// (except for in space efficient mode)
#if EKNUM_SPACE_EFFICENT
#	define EKNUM_INLINE
#else
#	define EKNUM_INLINE EKNUM_ALWAYS_INLINE
#endif

// Bitwise operations
#if __GNUC__ // Implement using GNU compiler compatible extensions
// Counts the number of trailing zeros in the number (starts from 0th bit)
#define _ekctz(x) _Generic(x, \
	unsigned long long: __builtin_ctzll(x), \
	unsigned long: __builtin_ctzl(x), \
	unsigned int: __builtin_ctz(x))
// Counts number of leading zeros in the number (starts from nth bit)
#define _ekclz(x) _Generic(x, \
	unsigned long long: __builtin_clzll(x), \
	unsigned long: __builtin_clzl(x), \
	unsigned int: __builtin_clz(x))
#else // Generic compiler implementations of these bitwise functions
// Counts the number of trailing zeros in the number (starts from 0th bit)
static EKJSON_ALWAYS_INLINE uint64_t _ekctz(uint64_t x) {
	x &= -x;	// Isolate the first 1 so we can get its position
	uint64_t n = 0;	// Number of trailing zeros
	
	// Divide and concore approach
	n += x & 0xFFFFFFFF00000000 ? 32 : 0;
	n += x & 0xFFFF0000FFFF0000 ? 16 : 0;
	n += x & 0xFF00FF00FF00FF00 ? 8 : 0;
	n += x & 0xF0F0F0F0F0F0F0F0 ? 4 : 0;
	n += x & 0xCCCCCCCCCCCCCCCC ? 2 : 0;
	n += x & 0xAAAAAAAAAAAAAAAA ? 1 : 0;
	return n;
}
#endif // __GNUC__

// Bit trick functions and macros
#if !EKNUM_NO_BITWISE
// Wrappers for loading u16/u32/u64 on unaligned addresses
static EKNUM_ALWAYS_INLINE uint64_t _ekldu64_unaligned(const void *const buf) {
	const uint8_t *const bytes = buf;
	return (uint64_t)bytes[0] | (uint64_t)bytes[1] << 8
		| (uint64_t)bytes[2] << 16 | (uint64_t)bytes[3] << 24
		| (uint64_t)bytes[4] << 32 | (uint64_t)bytes[5] << 40
		| (uint64_t)bytes[6] << 48 | (uint64_t)bytes[7] << 56;
}
static EKNUM_ALWAYS_INLINE void _ekstu64_unaligned(void *const buf,
						const uint64_t x) {
    uint8_t *const bytes = buf;
    bytes[0] = x & 0xFF, bytes[1] = x >> 8 & 0xFF;
    bytes[2] = x >> 16 & 0xFF, bytes[3] = x >> 24 & 0xFF;
    bytes[4] = x >> 32 & 0xFF, bytes[5] = x >> 40 & 0xFF;
    bytes[6] = x >> 48 & 0xFF, bytes[7] = x >> 56 & 0xFF;
}
#endif // EKNUM_NO_BITWISE

// Overflow detecting math ops
#if __GNUC__
// Returns true if the signed addition overflowed or underflowed
#define _ekadd_overflow(x, y, out) _Generic(x + y, \
	unsigned long long: \
		__builtin_uaddll_overflow(x, y, (unsigned long long *)out), \
	unsigned long: \
		__builtin_uaddl_overflow(x, y, (unsigned long *)out), \
	long long: __builtin_saddll_overflow(x, y, (long long *)out), \
	long: __builtin_saddl_overflow(x, y, (long *)out), \
	int: __builtin_sadd_overflow(x, y, (int *)out))
// Returns true if the signed multiplication overflowed or underflowed
#define _ekmul_overflow(x, y, out) _Generic(x + y, \
	unsigned long long: \
		__builtin_umulll_overflow(x, y, (unsigned long long *)out), \
	unsigned long: \
		__builtin_umull_overflow(x, y, (unsigned long *)out), \
	long long: __builtin_smulll_overflow(x, y, (long long *)out), \
	long: __builtin_smull_overflow(x, y, (long *)out), \
	int: __builtin_smul_overflow(x, y, (int *)out))
#else // Generic implementations
static EKNUM_ALWAYS_INLINE bool _ekadd_overflow(int64_t x, int64_t y,
						int64_t *out) {
	*out = x + y;
	return x > 0 && y > INT64_MAX - x || x < 0 && y < INT64_MIN - x;
}
static EKNUM_ALWAYS_INLINE bool _ekmul_overflow(int64_t x, int64_t y,
						int64_t *out) {
	*out = x * y;
	y = y < 0 ? -y : y;
	return x > 0 && x > INT64_MAX / y || x < 0 && x < INT64_MIN / y;
}
#endif // __GNUC__

#if !EKNUM_NO_BITWISE
// Parses up to 8 digits and writes it to the out pointer. It how many of the
// 8 bytes in this part of the string make up the number starting at the string
static int _ekparsedigits8(const char *const src, uint64_t *const out) {
	// Load up 8 bytes of the source and set default amount of right bytes
	// A 'right' byte is a byte that is a digit between 0 - 9
	uint64_t val = _ekldu64_unaligned(src), nright = 8;

	// The high nibble of each byte that is not between 0-9 is set
	const uint64_t wrong_bytes = (val ^ 0x3030303030303030)
		+ 0x0606060606060606 & 0xF0F0F0F0F0F0F0F0;

	// Skip the shifting code if there are no wrong bytes. Also we do this
	// because getting the leading zero count on 0 depends on what
	// architecture you're specifically on
	if (!wrong_bytes) goto convert;

	// The index of the first wrong byte is the number of right bytes
	// Due to the UB of shifting by the int width, exit early here.
	// It also doubles up as an early exit optimization
	if (!(nright = _ekctz(wrong_bytes) / 8)) {
		*out = 0;
		return 0;
	}

	// Shift out the invalid bytes and since the conversion down here only
	// works when the msb is the ones place digit, shift the ones place
	// to the msb
	val <<= (8 - nright) * 8;

convert:
	// Convert the chars in val to a digit. Found this at this amazing talk
	// https://lemire.me/en/talk/gosystems2020/
	// This is also a variation of the algorithm found here:
	// https://kholdstare.github.io/technical/2020/05/26/faster-integer-parsing.html
	val = (val & 0x0F0F0F0F0F0F0F0F) * (0x100 * 10 + 1) >> 8;
	val = (val & 0x00FF00FF00FF00FF) * (0x10000 * 100 + 1) >> 16;
	val = (val & 0x0000FFFF0000FFFF) * (0x100000000 * 10000 + 1) >> 32;
	*out = val;
	return nright;
}
#endif

// Parses base 10 signed numbers
int64_t _ekint10(const char **src) {
#if EKNUM_NO_BITWISE
	// Get the sign of the number and skip the negative char if nessesary
	int64_t sign = *src == '-' ? -1 : 1, x = 0;
	src += *src == '-';

	// Loop through all the digits
	while (*src >= '0' && *src <= '9') {
		const int64_t d = *src++ - '0'; // Convert char to number

		// Shift current value up by a decimal place
		if (mul_overflow(x, 10, &x)) goto overflow; // Check overflow

		// Add the new ones place we found
		if (add_overflow(x, d * sign, &x)) goto overflow;
	}

	return x;
overflow: // Check are sign to see what limit to saturate to
	return sign == -1 ? INT64_MIN : INT64_MAX;
#else
	// Precalculate all the powers of 10 needed for float/int parsers
	static const int64_t pow10[9] = {
		1, 10, 100, 1000, 10000, 100000, 1000000, 10000000, 100000000,
	};

	// Get the sign of the number
	const int64_t sign = *src == '-' ? -1 : 1;
	int64_t x, tmp; // Our num. we are making, and current number bit (tmp)
	int n;	// The number of right chars in the 8-byte sequence we parsed

	src += *src == '-';		// Skip the negative char if nessesary
	
	// Parse first 1-8 bytes of the number. If the number is 7 bytes or
	// less, then we can be sure that we are done.
	if (_ekparsedigits8(src, (uint64_t *)&x) < 8) {
		return x * sign;	// Make sure to apply sign
	}

	// Parse next 8 byte section (this also accounts for the case that
	// the number is truely a 8 byte number. So this section might have
	// no number in it at all). We also apply the sign in this section
	src += 8;			// Skip past the 8 bytes we parsed
	n = _ekparsedigits8(src, (uint64_t *)&tmp);// Put next 8 bytes into tmp
	
	// Make 'room' for the new digits we are adding by shifting the old
	// ones by n number of decimal places
	x *= pow10[n] * sign;		// Apply sign
	x += tmp * sign; // Add the numbers we parsed and apply sign to them
	
	if (n < 8) return x; // Return if we're sure we're at the end

	// Since int64_t can hold 16 digit values easily, we have to now check
	// for overflow since we're going over that.
	src += 8;			// Skip past the 8 bytes we parsed
	n = _ekparsedigits8(src, (uint64_t *)&tmp);// Put next 8 bytes into tmp
	
	// Do the same as above but check if we overflowed
	if (_ekmul_overflow(x, pow10[n], &x)
		|| _ekadd_overflow(x, sign * tmp, &x)) {
		// Saturate to the limit corresponding to the sign
		return sign == -1 ? INT64_MIN : INT64_MAX;
	} else {
		return x;	// Otherwise our x is already good
	}
#endif
}

#endif // EKNUM_IMPL
#endif // _eknum_h_

