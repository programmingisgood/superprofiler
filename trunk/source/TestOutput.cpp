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

#include "TestOutput.h"
#include "SuperFunctionData.h"
#include "SuperStackNode.h"

namespace SuperProfiler
{
	TestOutput::TestOutput() : maxDepth(0), totalRunTime(0)
	{
	}


	TestOutput::~TestOutput()
	{
	}


	void TestOutput::OutputFunctionData(FuncDataList & funcData, double totalRunTime)
	{
		//First, sort the function list
		SortByTime(funcData, true);

		this->totalRunTime = totalRunTime;

		FuncDataList::iterator iter;
		for (iter = funcData.begin(); iter != funcData.end(); iter++)
		{
			funcTimeList[(*iter)->GetName()] = (*iter)->GetTotalTime();
			funcCallAmountList[(*iter)->GetName()] = (*iter)->GetTotalNumTimesCalled();
		}
	}


	void TestOutput::OutputCallTree(SuperStackNode * stack)
	{
		FindMaxDepth(stack, 0);
	}


	void TestOutput::FindMaxDepth(SuperStackNode * currNode, size_t currDepth)
	{
		if (currDepth > maxDepth)
		{
			maxDepth = currDepth;
		}

		size_t numChildren = currNode->GetNumChildren();
		for (size_t i = 0; i < numChildren; i++)
		{
			SuperStackNode * childNode = currNode->GetChild(i);
			FindMaxDepth(childNode, currDepth + 1);
		}
	}


	size_t TestOutput::GetMaxDepth(void)
	{
		return maxDepth;
	}


	double TestOutput::GetTotalRunTime(void) const
	{
		return totalRunTime;
	}


	double TestOutput::GetFunctionRunTime(const std::string & functionName) const
	{
		FuncTimeList::const_iterator foundIter = funcTimeList.find(functionName);
		if (foundIter != funcTimeList.end())
		{
			return foundIter->second;
		}
		//Did not find the function, -1 is an invalid time
		return -1;
	}


	size_t TestOutput::GetFunctionCallAmount(const std::string & functionName) const
	{
		FuncCallAmountList::const_iterator foundIter = funcCallAmountList.find(functionName);
		if (foundIter != funcCallAmountList.end())
		{
			return foundIter->second;
		}
		//Did not find the function
		return 0;
	}
}