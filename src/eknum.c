#include "eknum.h"

unsigned ekdbl_validate(const char *str, const char **end) {
#include "dbldfa.h"
	int state = DBLACCEPT;

	while (state < DBLFINISHSTATES) {
#if EKNUM_SPACE_EFFICIENT
		state = dbltrans[state][dbledges[*str++]];
#else
		state = dbltrans[state][*str++];
#endif
	}

	if (*end) *end = str;
	switch (state) {
	case DBLDONE:		return 10;
	case DBLHEXDONE:	return 16;
	default:
		if (*end) --*end;
		return 0;
	}
}
static unsigned int_validate(const char *str, const char **end, int state) {
#include "intdfa.h"
	while (state < INTFINISHSTATES) {
#if EKNUM_SPACE_EFFICIENT
		state = inttrans[state][intedges[*str++]];
#else
		state = inttrans[state][*str++];
#endif
	}

	if (*end) *end = str;
	switch (state) {
	case INTBINDONE:	return 2;
	case INTOCTDONE:	return 8;
	case INTDONE:		return 10;
	case INTHEXDONE:	return 16;
	default:
		if (*end) --*end;
		return 0;
	}
}
unsigned ekint_validate(const char *str, const char **end) {
	return int_validate(str, end, INTINT_ACCEPT);
}
unsigned ekuint_validate(const char *str, const char **end) {
	return int_validate(str, end, INTUINT_ACCEPT);
}

double ekdbl_parse_unsafe(const char *str, unsigned base, bool *overflow) {
	return 0.0;
}
int64_t ekint_parse_unsafe(const char *str, unsigned base, bool *overflow) {
	return 0;
}
uint64_t ekuint_parse_unsafe(const char *str, unsigned base, bool *overflow) {
	return 0;
}

