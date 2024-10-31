#include <stdio.h>
#include <string.h>

#include "ek.h"

static const test_t tests[] = {
	TEST_PAD
};

//static void test_ejint_speed(void) {
//	static const char *const strings[] = {
//		"0", "-1", "1234", "-1234", "12345678", "-12345678",
//		"9223372036854775807", "-9223372036854775808",
//		"9223372036854775900", "-9223372036854775900",
//	};
//	static const int64_t numbers[] = {
//		0, -1, 1234, -1234, 12345678, -12345678,
//		INT64_MAX, INT64_MIN, INT64_MAX, INT64_MIN,
//	};
//
//	static const int niters = 10000000, nums = 10;
//
//	// Calculate number of bytes
//	double ngigs = 0;
//	for (int j = 0; j < nums; j++) ngigs += strlen(strings[j]);
//	ngigs *= niters;
//	ngigs /= 1024 * 1024 * 1024;
//
//	// test functions
//	clock_t start;
//	double time;
//	printf("\n\nejint tests\n");
//
//	// test strtoll
//	start = clock();
//	for (int i = 0; i < niters; i++) {
//		for (int j = 0; j < nums; j++) {
//			strtoll(strings[j], NULL, 10);
//		}
//	}
//	time = (double)(clock() - start) / (double)CLOCKS_PER_SEC;
//	printf("strtoll %d iters time (s): %.4f\n", niters, time);
//	printf("strtoll throughput (GB/s): %.2f\n", ngigs / time);
//	printf("strtoll throughput (millions N/s): %.2f\n",
//		((double)(niters * nums) / 1000000.0) / time);
//	
//	// test atoll
//	start = clock();
//	volatile uint64_t n = 0;
//	for (int i = 0; i < niters; i++) {
//		for (int j = 0; j < nums; j++) {
//			n += atoll(strings[j]);
//		}
//	}
//	time = (double)(clock() - start) / (double)CLOCKS_PER_SEC;
//	printf("atoll %d iters time (s): %.4f\n", niters, time);
//	printf("atoll throughput (GB/s): %.2f\n", ngigs / time);
//	printf("atoll throughput (millions N/s): %.2f\n",
//		((double)(niters * nums) / 1000000.0) / time);
//
//	// test ejint
//	start = clock();
//	for (int i = 0; i < niters; i++) {
//		for (int j = 0; j < nums; j++) {
//			ejint(strings[j]);
//		}
//	}
//	time = (double)(clock() - start) / (double)CLOCKS_PER_SEC;
//	printf("ejint %d iters time (s): %.4f\n", niters, time);
//	printf("ejint throughput (GB/s): %.2f\n", ngigs / time);
//	printf("ejint throughput (millions N/s): %.2f\n",
//		((double)(niters * nums) / 1000000.0) / time);
//}

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

