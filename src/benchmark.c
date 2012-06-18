#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include "k8055.h"


#define ITERATIONS 1000

int main(int argc, char *argv[]) {
	int port;
	if (argc <= 1) port = 0;
	else port = atoi(argv[1]);

	k8055_device* device;
	if (k8055_open_device(port, &device) != 0) {
		printf("could not open board on port %i\n", port);
		return -1;
	};

	struct timeval t0;
	struct timeval t;

	int us = 0;
	for (int i = 0; i < ITERATIONS; ++i) {
		gettimeofday(&t0, NULL);
		k8055_get_all_input(device, NULL, NULL, NULL, NULL, NULL, false);
		gettimeofday(&t, NULL);
		us += (t.tv_sec - t0.tv_sec) * 1000000 + t.tv_usec - t0.tv_usec;
	}
	printf("average read time for %i iterations: %i [ms]\n", ITERATIONS, us / ITERATIONS / 1000);
	
	us = 0;
	for (int i = 0; i < ITERATIONS; ++i) {
		gettimeofday(&t0, NULL);
		k8055_get_all_input(device, NULL, NULL, NULL, NULL, NULL, true);
		gettimeofday(&t, NULL);
		us += (t.tv_sec - t0.tv_sec) * 1000000 + t.tv_usec - t0.tv_usec;
	}
	printf("average quick read time for %i iterations: %i [ms]\n", ITERATIONS, us / ITERATIONS / 1000);
}
