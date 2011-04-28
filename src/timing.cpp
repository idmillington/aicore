/*
 * Timing functions, frame management and profiling.
 *
 * Part of the Artificial Intelligence for Games system.
 *
 * Copyright (c) Ian Millington 2003-2006. All Rights Reserved.
 *
 * This software is distributed under licence. Use of this software
 * implies agreement with all terms and conditions of the accompanying
 * software licence.
 */

#include <aicore/aicore.h>

#ifdef _WIN32
// Import the high performance timer (c. 4ms).
#include <windows.h>
#include <mmsystem.h>
#elif __MACH__
#include <mach/mach_time.h>  
#else
#include <time.h>
#include <stdint.h>
#endif

namespace aicore
{

#ifdef _WIN32
    // Hold internal timing data for the performance counter.
    static bool qpcFlag;
    static double qpcFrequency;
#endif

    // Internal time and clock access functions
    unsigned systemTime()
    {
#ifdef _WIN32
        if(qpcFlag)
        {
            static LONGLONG qpcMillisPerTick;
            QueryPerformanceCounter((LARGE_INTEGER*)&qpcMillisPerTick);
            return (unsigned)(qpcMillisPerTick * qpcFrequency);
        }
        else
        {
            return unsigned(timeGetTime());
        }
#elif __MACH__
        uint64_t t = mach_absolute_time();
        mach_timebase_info_data_t info = {0,0};
        mach_timebase_info(&info);
        uint64_t timenano = t * (info.numer / info.denom);
        return timenano/1000000;
#else
        struct timespec cur;
        clock_gettime(CLOCK_REALTIME, &cur);
        return cur.tv_sec*1000 + cur.tv_nsec/1000000;
#endif
    }

    unsigned TimingData::getTime()
    {
        return systemTime();
    }

    unsigned long systemClock()
    {
#ifdef _WIN32
        __asm {
            rdtsc;
        }
#else
        volatile uint32_t lo, hi;
        asm volatile ("rdtsc" : "=a" (lo), "=d" (hi));
        uint64_t res = ((uint64_t)hi << 32) + lo;
        return res;
#endif
    }

    unsigned long TimingData::getClock()
    {
        return systemClock();
    }

    // Sets up the timing system and registers the performance timer.
    void initTime()
    {
#ifdef _WIN32
        LONGLONG time;

        qpcFlag = (QueryPerformanceFrequency((LARGE_INTEGER*)&time) > 0);

        // Check if we have access to the performance counter at this
        // resolution.
        if (qpcFlag) qpcFrequency = 1000.0 / time;
#endif
    }


    // Holds the global frame time that is passed around
    static TimingData *timingData = NULL;

    // Retrieves the global frame info instance
    TimingData& TimingData::get()
    {
        return (TimingData&)*timingData;
    }

    // Updates the global frame information. Should be called once per frame.
    void TimingData::update()
    {
        if (!timingData) return;

        // Advance the frame number.
        if (!timingData->isPaused)
        {
            timingData->frameNumber++;
        }

        // Update the timing information.
        unsigned thisTime = systemTime();
        timingData->lastFrameDuration = thisTime -
            timingData->lastFrameTimestamp;
        timingData->lastFrameTimestamp = thisTime;

        // Update the tick information.
        unsigned long thisClock = systemClock();
        timingData->lastFrameClockTicks =
        thisClock - timingData->lastFrameClockstamp;
        timingData->lastFrameClockstamp = thisClock;

        // Update the RWA frame rate if we are able to.
        if (timingData->frameNumber > 1) {
            if (timingData->averageFrameDuration <= 0)
            {
                timingData->averageFrameDuration =
                    (double)timingData->lastFrameDuration;
            }
            else
            {
                // RWA over 100 frames.
                timingData->averageFrameDuration *= 0.99;
                timingData->averageFrameDuration +=
                    0.01 * (double)timingData->lastFrameDuration;

                // Invert to get FPS
                timingData->fps =
                    (float)(1000.0/timingData->averageFrameDuration);
            }
        }
    }

    void TimingData::init()
    {
        // Set up the timing system.
        initTime();

        // Create the frame info object
        if (!timingData) timingData = new TimingData();

        // Set up the frame info structure.
        timingData->frameNumber = 0;

        timingData->lastFrameTimestamp = systemTime();
        timingData->lastFrameDuration = 0;

        timingData->lastFrameClockstamp = systemClock();
        timingData->lastFrameClockTicks = 0;

        timingData->isPaused = false;

        timingData->averageFrameDuration = 0;
        timingData->fps = 0;
    }

    void TimingData::deinit()
    {
            delete timingData;
            timingData = NULL;
    }

}; // end of namespace
