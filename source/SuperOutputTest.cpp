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

#include "SuperOutputTest.h"
#include "SuperFunctionData.h"
#include "SuperStackNode.h"

namespace SuperProfiler
{
	SuperOutputTest::SuperOutputTest() : root("ROOT"), maxDepth(0), totalRunTime(0)
	{
	}


	SuperOutputTest::~SuperOutputTest()
	{
	}


	void SuperOutputTest::OutputFunctionData(SuperFuncDataList & funcData, double totalRunTime)
	{
		//First, sort the function list
		SortByTime(funcData, true);

		this->totalRunTime = totalRunTime;

		SuperFuncDataList::iterator iter;
		for (iter = funcData.begin(); iter != funcData.end(); iter++)
		{
			funcTimeList[(*iter)->GetName()] = (*iter)->GetTotalTime();
			funcCallAmountList[(*iter)->GetName()] = (*iter)->GetTotalNumTimesCalled();
		}
	}


	void SuperOutputTest::OutputCallTree(SuperStackNode * stack)
	{
		Record(stack, root);

		FindMaxDepth(stack, 0);
	}


	void SuperOutputTest::Record(SuperStackNode * stack, SuperOutputTestCallNode & currNode)
	{
		currNode.SetCallAmount(stack->GetNumTimesCalled());
		currNode.SetRunTime(stack->GetTotalTime());
		currNode.SetAvgRunTime(stack->GetAverageTime());

		size_t numChildren = stack->GetNumChildren();
		for (size_t i = 0; i < numChildren; i++)
		{
			SuperStackNode * childNode = stack->GetChild(i);
			SuperOutputTestCallNode * newNode = new SuperOutputTestCallNode(childNode->GetFuncData()->GetName());
			currNode.AddChild(newNode);
			Record(childNode, *newNode);
		}
	}


	void SuperOutputTest::FindMaxDepth(SuperStackNode * currNode, size_t currDepth)
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


	size_t SuperOutputTest::GetMaxDepth(void)
	{
		return maxDepth;
	}


	double SuperOutputTest::GetTotalRunTime(void) const
	{
		return totalRunTime;
	}


	double SuperOutputTest::GetFunctionRunTime(const std::string & functionName) const
	{
		SuperFuncTimeList::const_iterator foundIter = funcTimeList.find(functionName);
		if (foundIter != funcTimeList.end())
		{
			return foundIter->second;
		}
		//Did not find the function, -1 is an invalid time
		return -1;
	}


	size_t SuperOutputTest::GetFunctionCallAmount(const std::string & functionName) const
	{
		SuperFuncCallAmountList::const_iterator foundIter = funcCallAmountList.find(functionName);
		if (foundIter != funcCallAmountList.end())
		{
			return foundIter->second;
		}
		//Did not find the function
		return 0;
	}


	SuperOutputTest::SuperOutputTestCallNode & SuperOutputTest::GetCallTree(void)
	{
		return root;
	}


	SuperOutputTest::SuperOutputTestCallNode::SuperOutputTestCallNode(const std::string & setName) : name(setName), callAmount(0),
																									 runTime(0), avgRunTime(0)
	{
	}


	SuperOutputTest::SuperOutputTestCallNode::~SuperOutputTestCallNode()
	{
		//Clean up children
		CallNodeList::iterator iter;
		for (iter = functions.begin(); iter != functions.end(); iter++)
		{
			delete (*iter);
		}
		functions.clear();
	}


	const std::string SuperOutputTest::SuperOutputTestCallNode::GetName(void) const
	{
		return name;
	}


	void SuperOutputTest::SuperOutputTestCallNode::SetCallAmount(size_t setAmount)
	{
		callAmount = setAmount;
	}


	size_t SuperOutputTest::SuperOutputTestCallNode::GetCallAmount(void) const
	{
		return callAmount;
	}


	void SuperOutputTest::SuperOutputTestCallNode::SetRunTime(double setTime)
	{
		runTime = setTime;
	}


	double SuperOutputTest::SuperOutputTestCallNode::GetRunTime(void) const
	{
		return runTime;
	}


	void SuperOutputTest::SuperOutputTestCallNode::SetAvgRunTime(double setTime)
	{
		avgRunTime = setTime;
	}


	double SuperOutputTest::SuperOutputTestCallNode::GetAvgRunTime(void) const
	{
		return avgRunTime;
	}


	void SuperOutputTest::SuperOutputTestCallNode::AddChild(SuperOutputTestCallNode * addChild)
	{
		functions.push_back(addChild);
	}


	SuperOutputTest::SuperOutputTestCallNode * SuperOutputTest::SuperOutputTestCallNode::GetFunction(const std::string & funcName)
	{
		CallNodeList::iterator iter;
		for (iter = functions.begin(); iter != functions.end(); iter++)
		{
			if ((*iter)->GetName() == funcName)
			{
				return (*iter);
			}
		}

		return NULL;
	}
}