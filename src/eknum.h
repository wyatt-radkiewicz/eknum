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

unsigned ekdbl_validate(const char *str, unsigned base, const char **end);
unsigned ekint_validate(const char *str, unsigned base, const char **end);
unsigned ekuint_validate(const char *str, unsigned base, const char **end);

double ekdbl_parse_unsafe(const char *str, unsigned base);
int64_t ekint_parse_unsafe(const char *str, unsigned base);
uint64_t ekuint_parse_unsafe(const char *str, unsigned base);

#define EKNUM_IMPL
#ifdef EKNUM_IMPL

#endif // EKNUM_IMPL
#endif // _eknum_h_

