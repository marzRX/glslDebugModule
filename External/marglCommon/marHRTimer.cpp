#include <stdio.h>
#include "marHRTimer.h"

// ＠＠
#ifdef _WIN32

#include <windows.h>

static LARGE_INTEGER g_freq;
LARGE_INTEGER s_start, s_stop;

#elif __linux

#include <unistd.h>
#include <time.h>

struct timespec nowTime;
double start_nanosec;
#endif

THighRezoTimer::THighRezoTimer()
{
  init();
}

void THighRezoTimer::init()
{
#ifdef _WIN32
   QueryPerformanceFrequency(&g_freq);
   QueryPerformanceCounter(&s_start);
#elif __linux
  clock_gettime(CLOCK_REALTIME, &nowTime);
  start_nanosec = nowTime.tv_sec * 1000000000 + nowTime.tv_nsec;
#endif
}  

// 経過時間を秒数で返却
double THighRezoTimer::getELT()
{
  double sec;
#ifdef _WIN32
  QueryPerformanceCounter(&s_stop);
  sec = (double)(s_stop.QuadPart - s_start.QuadPart) / (double)g_freq.QuadPart;
  s_start.QuadPart = s_stop.QuadPart;
#elif __linux
  clock_gettime(CLOCK_REALTIME, &nowTime);
  double nanosec = nowTime.tv_sec * 1000000000 + nowTime.tv_nsec;
  sec = (nanosec - start_nanosec) / 1000000000.0;
  start_nanosec = nanosec;
#endif
  return sec;
}
