#include "eknum.h"

static uint64_t uint_parse_base2(const char *str, bool *overflow) {
	return 0;
}
static uint64_t uint_parse_base8(const char *str, bool *overflow) {
	return 0;
}
static uint64_t uint_parse_base10(const char *str, bool *overflow) {
	return 0;
}
static uint64_t uint_parse_base16(const char *str, bool *overflow) {
	return 0;
}

uint64_t ekuint_parse_unsafe(const char *str, unsigned base, bool *overflow) {
	if (*overflow) *overflow = false;
	switch (base) {
	case 2: return uint_parse_base2(str, overflow);
	case 8: return uint_parse_base8(str, overflow);
	case 10: return uint_parse_base10(str, overflow);
	case 16: return uint_parse_base16(str, overflow);
	default: return 0;
	}
}

