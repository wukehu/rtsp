#include "Utils.h"
#include <time.h>

unsigned long RealSeconds()
{
	struct timespec tp;
	int ret = clock_gettime(CLOCK_MONOTONIC, &tp);
	if (ret < 0)
		return 0;

	return  tp.tv_sec;
	return 0;
}



