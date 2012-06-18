#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "k8055.h"

static int port = 0;


int test_all_analog(k8055_device* device) {
	
	for (int i = 0; i < 256; ++i)
		if (k8055_set_all_analog(device, i, i) !=0 )
			return -1;
	return 0;
}

int test_all_digital(k8055_device* device) {
	for (int i = 0; i < 256; ++i)
		if (k8055_set_all_digital(device, i) !=0 )
			return -1;
	return 0;
}

int test_analog(k8055_device* device) {
	struct timespec reqtime;
        reqtime.tv_sec = 0;
        reqtime.tv_nsec = 500000000;

	if (k8055_set_analog(device, 0, 0) != 0) return -1;
	if (k8055_set_analog(device, 1, 0) != 0) return -1;
        nanosleep(&reqtime, NULL);
	if (k8055_set_analog(device, 0, 255) != 0) return -1;
	if (k8055_set_analog(device, 1, 255) != 0) return -1;
        nanosleep(&reqtime, NULL);
	return 0;
}

int test_digital(k8055_device* device) {
	struct timespec reqtime;
        reqtime.tv_sec = 0;
        reqtime.tv_nsec = 500000000;

	for (int i = 0; i < 8; ++i)
		if (k8055_set_digital(device, i, false) != 0) return -1;
        nanosleep(&reqtime, NULL);
	for (int i = 0; i < 8; ++i)
		if (k8055_set_digital(device, i, true) != 0) return -1;
        nanosleep(&reqtime, NULL);
	return 0;
}

int test_get_all_input(k8055_device* device) {
	if (k8055_get_all_input(device, NULL, NULL, NULL, NULL, NULL, false) != 0) return -1;
	if (k8055_get_all_input(device, NULL, NULL, NULL, NULL, NULL, true) != 0) return -1;
	return 0;
}

int test_get_all_output(k8055_device* device) {
	unsigned int iseed = (unsigned int)time(NULL);
	srand(iseed);
	
	
	int a0 = rand() % 256;
	int a1 = rand() % 256;
	int d = rand() % 256;
	
	if (k8055_set_all_analog(device, a0, a1) != 0) return -1;
	if (k8055_set_all_digital(device, d) != 0) return -1;
	
	int ga0;
	int ga1;
	int gd;
	k8055_get_all_output(device, &gd, &ga0, &ga1, NULL, NULL);

	if ((a0 != ga0) || (a1 != ga1) || (d != gd)) return -1;
	return 0;
}

int run_test(const char* name, int (*f)(k8055_device*), k8055_device* device) {
	puts(name);
	int result = f(device);
	if (result == 0) puts("= success =");
	else puts("= FAILED =");
	puts("");
	return result;
}

int run_all() {
	k8055_debug(true);
	k8055_device* device = NULL;
	
	size_t n = 6;
	char* names[] = {
		"= write all analog =",
		"= write all digital =",
		"= write analog =",
		"= write digital =",
		"= read input =",
		"= read output ="
	};
	
	int (*tests[])(k8055_device*) = {
		test_all_analog,
		test_all_digital,
		test_analog,
		test_digital,
		test_get_all_input,
		test_get_all_output
	};
	


	printf("= open k8055 on port %i =\n", port);
	if (k8055_open_device(port, &device) == 0) {
		puts("= success =");
		puts("");
	} else {
		puts("= FAILED =\n");
		puts("");
		return -1;
	}

	for (int j = 0; j < n; ++j)
		run_test(names[j], tests[j], device);

	printf("= reopen k8055 on port %i =\n", port);
	k8055_close_device(device);
	if (k8055_open_device(port, &device) == 0) {
	puts("= success =");
		puts("");
	} else {
		puts("= FAILED =\n");
		puts("");
		return -1;
	}

	for (int j = 0; j < n; ++j)
		run_test(names[j], tests[j], device);

	
	puts("turning everything off");
	k8055_set_all_analog(device, 0, 0);
	k8055_set_all_digital(device, 0);
	k8055_close_device(device);

	return 0;	
}

int main(int argc, char *argv[]) {
	if (argc <= 1) port = 0;
	else port = atoi(argv[1]);

	int r = run_all();
	puts("");
	if (r == 0) puts("all tests completed successfully");
	else puts("some tests FAILED");
	return r;
}
