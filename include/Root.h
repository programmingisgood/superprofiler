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

#ifndef SP_ROOT_H
#define SP_ROOT_H

#include "SuperProfile.h"
#include "SuperTimer.h"
#include "SuperStack.h"
#include "FuncDataList.h"

namespace SuperProfiler
{
	class SuperOutput;

	class Root
	{
	public:
		static void Reset(void);
		static bool OutputResults(SuperOutput & output);

	private:
		//No default construction!
		Root();
		//No copies!
		Root(const Root &);
		const Root & operator=(const Root &);

		friend class SuperProfile;
		static void PushProfile(SuperProfile * setStart);
		static void PopProfile(void);

		/**
		* Find an existing SuperFunctionData that matches the passed in name.
		**/
		static SuperFunctionData * FindFuncData(const std::string & name);
		static void AddNewFuncData(SuperFunctionData * newFuncData);

		static SuperTimer superTimer;
		static SuperStack superStack;
		static FuncDataListWrapper funcDataListWrapper;
	};
}

#endif