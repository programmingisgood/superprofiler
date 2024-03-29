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

#ifndef SUPERFUNCTIONDATA_H
#define SUPERFUNCTIONDATA_H

#include <string>

namespace SuperProfiler
{
	class SuperFunctionData
	{
	public:
		SuperFunctionData(size_t setID, const char * setFuncName);
		~SuperFunctionData();

		size_t GetID(void) const;
		const char * GetName(void) const;
		/**
		* The name is stored as a pointer for quick comparisons, the name pointer may be deleted at some point however
		* Use the copy if you are interested in the actual text name and not the pointer address
		**/
		const std::string & GetNameCopy(void) const;

		void SetTotalTime(double setTime);
		void AddToTotalTime(double addTime);
		double GetTotalTime(void) const;

		void SetTotalNumTimesCalled(size_t setTimes);
		/**
		* Notify this function data that the function was called.
		**/
		void FunctionCalled(void);
		size_t GetTotalNumTimesCalled(void) const;

	private:
		//No default construction!
		SuperFunctionData();
		//No copies!
		SuperFunctionData(const SuperFunctionData &);
		const SuperFunctionData & operator=(const SuperFunctionData &);

		size_t ID;
		const char * funcName;
		/**
		* A copy is needed in case funcName gets deleted
		**/
		std::string funcNameCopy;
		double totalTime;
		size_t totalNumTimesCalled;
	};
}

#endif