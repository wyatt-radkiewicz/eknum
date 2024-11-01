#ifndef _eknum_h_
#define _eknum_h_
// To use eknum, include eknum.h.
// Since eknum is a single header library include eknum.h into a single c file
// in your project and define EKNUM_IMPL before it to include the
// implementation.
//
// Number formats: (NOTE: if this ever becomes outdated, check *.jff files in
//                  tools to see exactly what the number format is)
// In practice integers have the same rules as standard C99 integers. Some
// features I want to add but havent yet include optional underscores inbetween
// numbers.
//
// Floats are also simular to C99 floats but have a few differences. Unlike C
// floats, you must have atleast 1 digit (hex or not) on each side of a decimal
// point. For example:
// 0.1		<- ok
// 0.001	<- ok
// 0.		<- error
// .0		<- error
// -.1		<- error
// There are also no octal floats so:
// 0001.02	<- error
// is also an error.
//
// Implementation options:
// #define EKNUM_SPACE_EFFICIENT 1
//	This makes some tables smaller at the cost of performance
// #define EKNUM_NO_BITWISE 1
//	This disables 64-bit tricks to make the code faster
//

#include <stdint.h>
#include <stdbool.h>

// This family of functions validate a string to check if its a valid number.
//
// str is a pointer to the start of the number to validate (no whitespace)
// end is a pointer to a place to store either the character after the end
//	of the valid number, or the first character where an error occured.
//	This field may be left NULL.
//
// These functions return the found base of the number that can be passed into
// the parsing functions. If they return 0, then the string is not a valid
// number.
unsigned ekdbl_validate(const char *str, const char **end);
unsigned ekint_validate(const char *str, const char **end);
unsigned ekuint_validate(const char *str, const char **end);

// This family of functions parses a string with the given base with NO error
// checking. If you need to validate the string before, either do it with your
// own code according to the number format described at the top of eknum.h or
// call ek*_validate on the string first to get the base and if its valid.
//
// str points to the beggining of the number
// base is the base that the parser will use to parse the number. Unlike other
//	parsing functions like strtod, these will NOT accept a base of 0 and
//	detect the base automatically. One must do that themselves or call
//	ek*_validate on the string before hand to acquire the base of the
//	number.
// overflow points to a bool to be set if the number overflowed its storage
//	value. This field may be left NULL.
//
// Returns the parsed number, saturating towards any limits present for the
// given return type. If an overflow did occur (towards infinity, INT_MAX, etc)
// then these functions will set overflow to true
double ekdbl_parse_unsafe(const char *str, unsigned base, bool *overflow);
int64_t ekint_parse_unsafe(const char *str, unsigned base, bool *overflow);
uint64_t ekuint_parse_unsafe(const char *str, unsigned base, bool *overflow);

#endif // _eknum_h_

