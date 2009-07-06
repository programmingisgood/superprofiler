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

#ifndef SUPERROOT_H
#define SUPERROOT_H

#include "SuperTimer.h"
#include "SuperStack.h"
#include "SuperFuncDataList.h"
#include "SuperIterator.h"
#include <list>

namespace SuperProfiler
{
	class SuperRootListener;
	class SuperOutput;

	class SuperRoot
	{
	public:
		static void AddListener(SuperRootListener * addListener);
		static bool FindListener(SuperRootListener * findListener);
		static void RemoveListener(SuperRootListener * removeListener);

		static void Reset(void);

		static void PushProfile(const char * name);
		/**
		* The name must be provided here just to ensure that a pop is called
		* for every push in the correct order. Please use the SUPER_PROFILE()
		* macro to guarantee this is true.
		**/
		static void PopProfile(const char * name);

		static const SuperFuncDataList & GetFuncList(void);
		/**
		* The returned SuperIterator can be used to view the SuperProfiler results in real time.
		**/
		static SuperIterator GetIterator(void);

		static void OutputResults(SuperOutput & output);

		/**
		* Reset the measurement tracking inside the SuperStackNodes if you are using it
		**/
		static void ResetMeasure(void);

	private:
		//No default construction!
		SuperRoot();
		//No copies!
		SuperRoot(const SuperRoot &);
		const SuperRoot & operator=(const SuperRoot &);

		/**
		* Find an existing SuperFunctionData that matches the passed in name.
		**/
		static SuperFunctionData * FindFuncData(const char * name);
		static void AddNewFuncData(SuperFunctionData * newFuncData);

		typedef std::list<SuperRootListener *> ListenerList;
		static ListenerList listeners;
		static SuperTimer superTimer;
		static SuperStack superStack;
		static size_t nextFuncID;
		static SuperFuncDataListWrapper superFuncDataListWrapper;
		static bool recording;
		static bool allowThrow;
	};
}

#endif