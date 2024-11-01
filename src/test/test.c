#include <stdio.h>
#include <string.h>
#include <time.h>

#include "ek.h"

static const test_t tests[] = {
	TEST_PAD
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

