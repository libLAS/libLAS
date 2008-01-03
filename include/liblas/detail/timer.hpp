#ifndef LIBLAS_DETAIL_TIMER_HPP_INCLUDED
#define LIBLAS_DETAIL_TIMER_HPP_INCLUDED

#ifdef WIN32
#include <windows.h>
#else
#include <sys/time.h> // gettimeofday
#endif

namespace liblas { namespace detail {

class Timer
{
public:

    Timer()
    {
#ifdef WIN32
        LARGE_INTEGER rfreq = { 0 };
        QueryPerformanceFrequency(&rfreq);
        m_freq = rfreq.LowPart / double(1000);
#endif
  }

  void start()
  {
#ifdef WIN32
      QueryPerformanceCounter(&m_start);
#else
      gettimeofday(&m_start, NULL);
#endif
  }

  double stop()
  {
#ifdef WIN32
  QueryPerformanceCounter(&m_stop);
  return (m_stop.LowPart - m_start.LowPart) / m_freq;
#else
  gettimeofday(&m_stop, NULL);
  return (m_stop.tv_sec - m_start.tv_sec) * double(1000)
          + (m_stop.tv_usec - m_start.tv_usec) / double(1000);
#endif
  }

private:

#ifdef WIN32
  double m_freq;
  LARGE_INTEGER m_start;
  LARGE_INTEGER m_stop;
#else
  timeval m_start;
  timeval m_stop;
#endif

};

}} // namespace liblas::detail

#endif // LIBLAS_DETAIL_TIMER_HPP_INCLUDED
