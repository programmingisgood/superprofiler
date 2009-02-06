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

#include "SuperOutput.h"
#include "SuperFunctionData.h"
#include <algorithm>

namespace SuperProfiler
{
	bool TimeSortHighFirst(const SuperFunctionData * first, const SuperFunctionData * second)
	{
		if (first->GetTotalTime() > second->GetTotalTime())
		{
			return true;
		}
		return false;
	}


	bool TimeSortLowFirst(const SuperFunctionData * first, const SuperFunctionData * second)
	{
		if (first->GetTotalTime() < second->GetTotalTime())
		{
			return true;
		}
		return false;
	}


	void SuperOutput::SortByTime(FuncDataList & funcData, bool highFirst)
	{
		if (highFirst)
		{
			funcData.sort(TimeSortHighFirst);
		}
		else
		{
			funcData.sort(TimeSortLowFirst);
		}
	}


	size_t SuperOutput::GetTotalNumFunctionCalls(FuncDataList & funcData)
	{
		size_t totalNum = 0;
		FuncDataList::iterator iter;
		for (iter = funcData.begin(); iter != funcData.end(); iter++)
		{
			totalNum += (*iter)->GetTotalNumTimesCalled();
		}

		return totalNum;
	}
}