//Some code borrowed from Ogre3d.org and bulletphysics.com
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

#ifndef SUPERTIMER_H
#define SUPERTIMER_H

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

namespace SuperProfiler
{
	class SuperTimer
	{
	public:
		SuperTimer();
		~SuperTimer();

		void Reset(void);

		/**
		* Returns the time in seconds since the last call to Reset or since the SuperTimer was created.
		* If half a second has passed, 0.5 will be returned, for 2 seconds, 2.0 will be returned, etc...
		**/
		double GetTimeSeconds(void);

		/**
		* Returns the time in microseconds since the last call to Reset or since the SuperTimer was created
		**/
		unsigned long GetTimeMicroseconds(void);

	private:
		/**
		* Resets the initial reference time
		**/
		void RealReset(void);

#ifdef USE_WINDOWS_TIMERS
		clock_t zeroClock;

		DWORD startTick;
		LONGLONG lastTime;
		LARGE_INTEGER startTime;
		LARGE_INTEGER frequency;

		DWORD timerMask;

		double lastTimeDiff;
#endif
	};
}

#endif