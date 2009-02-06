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

#include "Root.h"
#include "SuperOutput.h"
#include "SuperFunctionData.h"

SuperProfiler::SuperTimer SuperProfiler::Root::superTimer;
SuperProfiler::SuperStack SuperProfiler::Root::superStack;
SuperProfiler::FuncDataListWrapper SuperProfiler::Root::funcDataListWrapper;

namespace SuperProfiler
{
	void Root::Reset(void)
	{
		superTimer.Reset();
		superStack.Reset();
		funcDataListWrapper.Reset();
	}


	bool Root::OutputResults(SuperOutput & output)
	{
		if (superStack.GetCurrentDepth() != 0)
		{
			return false;
		}

		//Calculate the total run time by adding the times of all children times off the root together
		double totalRunTime = 0;
		size_t numChildren = superStack.GetNumChildren();
		for (size_t c = 0; c < numChildren; c++)
		{
			SuperStackNode * child = superStack.GetChild(c);
			totalRunTime += child->GetFuncData()->GetTotalTime();
		}
		output.OutputFunctionData(funcDataListWrapper.funcDataList, totalRunTime);
		output.OutputCallTree(&superStack);

		return true;
	}


	void Root::PushProfile(SuperProfile * setStart)
	{
		SuperFunctionData * foundFunc = FindFuncData(setStart->GetName());
		if (!foundFunc)
		{
			foundFunc = new SuperFunctionData(setStart->GetName());
			AddNewFuncData(foundFunc);
		}
		superStack.Push(foundFunc, superTimer.GetTimeSeconds());
	}


	void Root::PopProfile(void)
	{
		superStack.Pop(superTimer.GetTimeSeconds());
	}


	SuperFunctionData * Root::FindFuncData(const std::string & name)
	{
		FuncDataList::iterator iter;
		for (iter = funcDataListWrapper.funcDataList.begin(); iter != funcDataListWrapper.funcDataList.end(); iter++)
		{
			if ((*iter)->GetName() == name)
			{
				return (*iter);
			}
		}
		return NULL;
	}


	void Root::AddNewFuncData(SuperFunctionData * newFuncData)
	{
		if (FindFuncData(newFuncData->GetName()))
		{
			//Already exists
			return;
		}

		funcDataListWrapper.funcDataList.push_back(newFuncData);
	}
}