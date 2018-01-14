#include "timing.h"

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>

namespace srend
{

int64_t GetTime() {
	LARGE_INTEGER li;
	QueryPerformanceCounter(&li);
	int64_t i64 = li.QuadPart;
	return i64;
}

int64_t GetFrequency() {
	LARGE_INTEGER li;
	QueryPerformanceFrequency(&li);
	int64_t i64 = li.QuadPart;
	return i64;
}

} // srend