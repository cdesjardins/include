#ifndef DELTA_TIMER_Hxx
#define DELTA_TIMER_Hxx

#include <chrono>

class DeltaTimer
{
public:
    DeltaTimer() : _beg(HRClock::now())
    {
    }

    void reset()
    {
        _beg = HRClock::now();
    }

    double elapsed() const
    {
        return std::chrono::duration_cast<std::chrono::milliseconds>
                   (HRClock::now() - _beg).count();
    }

private:
    typedef std::chrono::high_resolution_clock HRClock;
    std::chrono::time_point<HRClock> _beg;
};

#endif
