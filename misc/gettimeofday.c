#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>

int time_diff(struct timeval * t1, struct timeval * t2)
{
	int sec_diff  = t2 -> tv_sec - t1 -> tv_sec;
	int msec = sec_diff * 1000000 + t2 -> tv_usec - t1 -> tv_usec;
	return msec;
}

int measure(int peaks)
{
	int time = 0;
	struct timeval t, u;
	int i;
	gettimeofday(&t, NULL);
	for (i = 0; i < peaks; i++){
		gettimeofday(&u, NULL);
		time += time_diff(&t, &u);
		u = t;
	}
	return time;
}

int main(int argc, char * argv[])
{
	printf("let's profile the 'gettimeofday'-call\n");

	int max = 5;
	int start = 1;
	int factor = 10;

	int i;
	for (i = 0; i < max; i++){
		int time = measure(start);
		printf("#calls: %d time: %d\n", start, time);
		start *= factor;
	}
	int x = 20000;
	int time = measure(x);
	printf("#calls: %d time: %d\n", x, time);
}
