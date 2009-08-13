/**
Copyright (c) 2009, Brian Cronin
All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

Redistributions of source code must retain the above copyright notice, this list of conditions
and the following disclaimer.
Redistributions in binary form must reproduce the above copyright notice, this list of conditions
and the following disclaimer in the documentation and/or other materials provided with the distribution.
Neither the name of the <ORGANIZATION> nor the names of its contributors may be used to endorse or
promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**/

#include "SuperTimer.h"

#if defined(WIN32) || defined(_WIN32)

#define USE_WINDOWS_TIMERS
#define WIN32_LEAN_AND_MEAN
#define NOWINRES
#define NOMCX
#define NOIME
#define NOMINMAX
#include <windows.h>
#include <time.h>

#else

#include <sys/time.h>

#endif

#include <algorithm>

namespace SuperProfiler
{
	class SuperTimerImp
	{
	public:
		SuperTimerImp() : timerMask(0)
		{
		}

#ifdef USE_WINDOWS_TIMERS
		clock_t zeroClock;

		DWORD startTick;
		LONGLONG lastTime;
		LARGE_INTEGER startTime;
		LARGE_INTEGER frequency;

		DWORD timerMask;
#endif
	};


	SuperTimer::SuperTimer() : timerImp(new SuperTimerImp())
	{
		RealReset();
	}


	SuperTimer::~SuperTimer()
	{
		delete timerImp;
		timerImp = NULL;
	}


	void SuperTimer::Reset(void)
	{
		QueryPerformanceCounter(&timerImp->startTime);
		timerImp->startTick = GetTickCount();
		timerImp->lastTime = 0;
	}


	void SuperTimer::RealReset(void)
	{
		//Get the current process core mask
		DWORD procMask;
		DWORD sysMask;
#if _MSC_VER >= 1400 && defined (_M_X64)
		GetProcessAffinityMask(GetCurrentProcess(), (PDWORD_PTR) &procMask, (PDWORD_PTR) &sysMask);
#else
		GetProcessAffinityMask(GetCurrentProcess(), &procMask, &sysMask);
#endif

		//If procMask is 0, consider there is only one core available
		//(using 0 as procMask will cause an infinite loop below)
		if (procMask == 0)
		{
			procMask = 1;
		}

		//Find the lowest core that this process uses
		if (timerImp->timerMask == 0)
		{
			timerImp->timerMask = 1;
			while ((timerImp->timerMask & procMask) == 0)
			{
				timerImp->timerMask <<= 1;
			}
		}

		HANDLE thread = GetCurrentThread();

		//Set affinity to the first core
		DWORD oldMask = SetThreadAffinityMask(thread, timerImp->timerMask);

		//Get the constant frequency
		QueryPerformanceFrequency(&timerImp->frequency);

		//Query the timer
		QueryPerformanceCounter(&timerImp->startTime);
		timerImp->startTick = GetTickCount();

		//Reset affinity
		SetThreadAffinityMask(thread, oldMask);

		timerImp->lastTime = 0;
		timerImp->zeroClock = clock();
	}


	double SuperTimer::GetTimeSeconds(void)
	{
		LARGE_INTEGER curTime;

		HANDLE thread = GetCurrentThread();

		//Set affinity to the first core
		DWORD oldMask = SetThreadAffinityMask(thread, timerImp->timerMask);

		//Query the timer
		QueryPerformanceCounter(&curTime);

		//Reset affinity
		SetThreadAffinityMask(thread, oldMask);

		LONGLONG newTime = curTime.QuadPart - timerImp->startTime.QuadPart;
	    
		//scale by 1000 for milliseconds
		unsigned long newTicks = (unsigned long) (1000 * newTime / timerImp->frequency.QuadPart);

		//detect and compensate for performance counter leaps
		//(surprisingly common, see Microsoft KB: Q274323)
		unsigned long check = GetTickCount() - timerImp->startTick;
		signed long msecOff = (signed long)(newTicks - check);
		if (msecOff < -100 || msecOff > 100)
		{
			//We must keep the timer running forward :)
			LONGLONG adjust = (std::min)(msecOff * timerImp->frequency.QuadPart / 1000, newTime - timerImp->lastTime);
			timerImp->startTime.QuadPart += adjust;
			newTime -= adjust;

			//Re-calculate milliseconds
			newTicks = (unsigned long) (1000 * newTime / timerImp->frequency.QuadPart);
		}

		//Record last time for adjust
		timerImp->lastTime = newTime;

		return newTicks / static_cast<double>(1000);
	}


	unsigned long SuperTimer::GetTimeMicroseconds(void)
	{
		LARGE_INTEGER curTime;

		HANDLE thread = GetCurrentThread();

		//Set affinity to the first core
		DWORD oldMask = SetThreadAffinityMask(thread, timerImp->timerMask);

		//Query the timer
		QueryPerformanceCounter(&curTime);

		//Reset affinity
		SetThreadAffinityMask(thread, oldMask);

		LONGLONG newTime = curTime.QuadPart - timerImp->startTime.QuadPart;

		//get milliseconds to check against GetTickCount
		unsigned long newTicks = (unsigned long) (1000 * newTime / timerImp->frequency.QuadPart);

		//detect and compensate for performance counter leaps
		//(surprisingly common, see Microsoft KB: Q274323)
		unsigned long check = GetTickCount() - timerImp->startTick;
		signed long msecOff = (signed long) (newTicks - check);
		if (msecOff < -100 || msecOff > 100)
		{
			// We must keep the timer running forward :)
			LONGLONG adjust = (std::min)(msecOff * timerImp->frequency.QuadPart / 1000, newTime - timerImp->lastTime);
			timerImp->startTime.QuadPart += adjust;
			newTime -= adjust;
		}

		//Record last time for adjust
		timerImp->lastTime = newTime;

		//scale by 1000000 for microseconds
		unsigned long newMicro = (unsigned long) (1000000 * newTime / timerImp->frequency.QuadPart);

		return newMicro;
	}
}