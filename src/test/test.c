#include <stdio.h>
#include <string.h>
#include <time.h>

#include "ek.h"
#include "eknum.h"

#define VALIDATE(name, type, expected_base, number) \
	static bool test_##name(unsigned test_id) { \
		return ek##type##_validate(number, NULL) == expected_base; \
	}

VALIDATE(int1, int, 10, "0")
VALIDATE(int2, int, 10, "12345678")
VALIDATE(int3, int, 10, "1234567812345678")
VALIDATE(int4, int, 0, "a")
VALIDATE(int_neg1, int, 10, "-1")
VALIDATE(int_neg2, int, 10, "-12345678")
VALIDATE(int_neg3, int, 10, "-1234567812345678")
VALIDATE(inthex1, int, 16, "0x0")
VALIDATE(inthex2, int, 16, "0x1234abCD")
VALIDATE(inthex3, int, 16, "0x0ABCDEF123451234")
VALIDATE(inthex_neg1, int, 16, "-0x01")
VALIDATE(inthex_neg2, int, 16, "-0x012345678")
VALIDATE(inthex_neg3, int, 16, "-0x012345678123abCD")
VALIDATE(intoct1, int, 8, "01")
VALIDATE(intoct2, int, 8, "012345607")
VALIDATE(intoct3, int, 8, "012345120367")
VALIDATE(intoct_neg1, int, 8, "-01")
VALIDATE(intoct_neg2, int, 8, "-01234567")
VALIDATE(intoct_neg3, int, 8, "-012345067123")
VALIDATE(intbin1, int, 2, "0b1")
VALIDATE(intbin2, int, 2, "0b00100101010")
VALIDATE(intbin3, int, 2, "0b10010101010")
VALIDATE(intbin_neg1, int, 2, "-0b1")
VALIDATE(intbin_neg2, int, 2, "-0b00100101010")
VALIDATE(intbin_neg3, int, 2, "-0b11010010101")

VALIDATE(uint1, uint, 10, "0")
VALIDATE(uint2, uint, 10, "12345678")
VALIDATE(uint3, uint, 10, "1234567812345678")
VALIDATE(uint4, uint, 0, "-0")
VALIDATE(uinthex1, uint, 16, "0x0")
VALIDATE(uinthex2, uint, 16, "0x1234abCD")
VALIDATE(uinthex3, uint, 16, "0x0ABCDEF123451234")
VALIDATE(uintoct1, uint, 8, "01")
VALIDATE(uintoct2, uint, 8, "012345607")
VALIDATE(uintoct3, uint, 8, "012345120367")
VALIDATE(uintbin1, uint, 2, "0b1")
VALIDATE(uintbin2, uint, 2, "0b00100101010")
VALIDATE(uintbin3, uint, 2, "0b10010101010")

static const test_t tests[] = {
	TEST_ADD(test_int1)
	TEST_ADD(test_int2)
	TEST_ADD(test_int3)
	TEST_ADD(test_int4)
	TEST_ADD(test_int_neg1)
	TEST_ADD(test_int_neg2)
	TEST_ADD(test_int_neg3)
	TEST_ADD(test_inthex1)
	TEST_ADD(test_inthex2)
	TEST_ADD(test_inthex3)
	TEST_ADD(test_inthex_neg1)
	TEST_ADD(test_inthex_neg2)
	TEST_ADD(test_inthex_neg3)
	TEST_ADD(test_intoct1)
	TEST_ADD(test_intoct2)
	TEST_ADD(test_intoct3)
	TEST_ADD(test_intoct_neg1)
	TEST_ADD(test_intoct_neg2)
	TEST_ADD(test_intoct_neg3)
	TEST_ADD(test_intbin1)
	TEST_ADD(test_intbin2)
	TEST_ADD(test_intbin3)
	TEST_ADD(test_intbin_neg1)
	TEST_ADD(test_intbin_neg2)
	TEST_ADD(test_intbin_neg3)
	TEST_PAD
	TEST_ADD(test_uint1)
	TEST_ADD(test_uint2)
	TEST_ADD(test_uint3)
	TEST_ADD(test_uint4)
	TEST_ADD(test_uinthex1)
	TEST_ADD(test_uinthex2)
	TEST_ADD(test_uinthex3)
	TEST_ADD(test_uintoct1)
	TEST_ADD(test_uintoct2)
	TEST_ADD(test_uintoct3)
	TEST_ADD(test_uintbin1)
	TEST_ADD(test_uintbin2)
	TEST_ADD(test_uintbin3)
};

void usage(void) {
	printf("usage: \n");
	printf("test [test | speed]\n");
}

int main(int argc, char **argv) {
	bool speed_test = false;

	if (argc > 2) {
		usage();
		return 1;
	}
	if (argc == 2) {
		if (strcmp(argv[1], "test") == 0) {
			speed_test = false;
		} else if (strcmp(argv[1], "speed") == 0) {
			speed_test = true;
		} else {
			usage();
			return 1;
		}
	}

	int res = tests_run_foreach(NULL, tests, arrlen(tests), stdout)
		? 0 : -1;

	return res;
}

