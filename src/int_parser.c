#include "eknum.h"
#include "utils.h"

static int64_t int_parse_base2(const char *str, bool *overflow) {
	return 0;
}
static int64_t int_parse_base8(const char *str, bool *overflow) {
	return 0;
}
static int64_t int_parse_base10(const char *str, bool *overflow) {
	const int64_t sign = *str == '-' ? -1 : 1;
	str += sign;

	int64_t n = 0;
	while (*str >= '0' && *str <= '9') {
		const int64_t d = (*str++ - '0') * sign;
		if (mul_overflow(n, 10, &n) || add_overflow(n, d, &n)) {
			if (*overflow) *overflow = true;
			return sign ? INT64_MIN : INT64_MAX;
		}
	}

	return n;
}
static int64_t int_parse_base16(const char *str, bool *overflow) {
	return 0;
}

int64_t ekint_parse_unsafe(const char *str, unsigned base, bool *overflow) {
	if (*overflow) *overflow = false;
	switch (base) {
	case 2: return int_parse_base2(str, overflow);
	case 8: return int_parse_base8(str, overflow);
	case 10: return int_parse_base10(str, overflow);
	case 16: return int_parse_base16(str, overflow);
	default: return 0;
	}
}

