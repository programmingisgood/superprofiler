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

#include "SuperStackNode.h"
#include "SuperFunctionData.h"

namespace SuperProfiler
{
	SuperStackNode::SuperStackNode(SuperStackNode * setParent, SuperFunctionData * setFuncData) : parent(setParent), funcData(setFuncData),
																								  startTime(0), endTime(0), totalTime(0),
																								  lastMeasureTime(0), currentMeasureTime(0),
																								  currentHighestTimeWhileMeasuring(0),
																								  lastHighestTimeWhileMeasuring(0),
																								  currentAvgBufferPos(0),
																								  numTimesCalled(0)
	{
		avgBuffer.reserve(AvgBufferSize);
	}


	SuperStackNode::~SuperStackNode()
	{
		Reset();
	}


	void SuperStackNode::Reset(void)
	{
		funcData = NULL;
		startTime = 0;
		endTime = 0;
		totalTime = 0;
		currentMeasureTime = 0;
		lastMeasureTime = 0;
		currentHighestTimeWhileMeasuring = 0;
		lastHighestTimeWhileMeasuring = 0;
		numTimesCalled = 0;
		avgBuffer.clear();

		//Delete all children
		NodeList::iterator iter;
		for (iter = children.begin(); iter != children.end(); iter++)
		{
			delete (*iter);
		}
		children.clear();

		parent = NULL;
	}


	void SuperStackNode::SetFuncData(SuperFunctionData * setFuncData)
	{
		funcData = setFuncData;
	}


	SuperFunctionData * SuperStackNode::GetFuncData(void) const
	{
		return funcData;
	}


	SuperStackNode * SuperStackNode::GetParent(void)
	{
		return parent;
	}


	size_t SuperStackNode::GetNumChildren(void) const
	{
		return children.size();
	}


	SuperStackNode * SuperStackNode::GetChild(size_t atIndex)
	{
		size_t currIndex = 0;
		NodeList::iterator iter;
		for (iter = children.begin(); iter != children.end(); iter++)
		{
			if (currIndex == atIndex)
			{
				return (*iter);
			}
			currIndex++;
		}

		return NULL;
	}


	SuperStackNode * SuperStackNode::GetChild(SuperFunctionData * childFunc)
	{
		NodeList::iterator iter;
		for (iter = children.begin(); iter != children.end(); iter++)
		{
			if ((*iter)->GetFuncData() == childFunc)
			{
				return (*iter);
			}
		}
		return NULL;
	}


	void SuperStackNode::SetStartTime(double setStartTime)
	{
		startTime = setStartTime;

		//The function has been called, notify it
		funcData->FunctionCalled();

		//We also keep track of how many times this node has been called
		numTimesCalled++;
	}


	double SuperStackNode::GetStartTime(void) const
	{
		return startTime;
	}


	void SuperStackNode::SetEndTime(double setEndTime)
	{
		endTime = setEndTime;

		double timeDiff = endTime - startTime;
		funcData->AddToTotalTime(timeDiff);

		AddToTotalTime(timeDiff);
		AddAvgSample(timeDiff);
		currentMeasureTime += timeDiff;

		if (timeDiff > currentHighestTimeWhileMeasuring)
		{
			currentHighestTimeWhileMeasuring = timeDiff;
		}
	}


	double SuperStackNode::GetEndTime(void) const
	{
		return endTime;
	}


	void SuperStackNode::SetTotalTime(double setTime)
	{
		totalTime = setTime;
	}


	double SuperStackNode::GetTotalTime(void) const
	{
		return totalTime;
	}


	double SuperStackNode::GetAverageTime(void) const
	{
		double avgTime = 0;
		size_t numAvgTimes = avgBuffer.size();
		for (size_t a = 0; a < numAvgTimes; a++)
		{
			avgTime += avgBuffer[a];
		}

		return avgTime / numAvgTimes;
	}


	void SuperStackNode::ResetMeasure(void)
	{
		lastMeasureTime = currentMeasureTime;
		currentMeasureTime = 0;

		lastHighestTimeWhileMeasuring = currentHighestTimeWhileMeasuring;
		currentHighestTimeWhileMeasuring = 0;

		NodeList::iterator iter;
		for (iter = children.begin(); iter != children.end(); iter++)
		{
			(*iter)->ResetMeasure();
		}
	}


	double SuperStackNode::GetLastMeasureTime(void) const
	{
		return lastMeasureTime;
	}


	double SuperStackNode::GetLastHighestTimeWhileMeasuring(void) const
	{
		return lastHighestTimeWhileMeasuring;
	}


	void SuperStackNode::AddToTotalTime(double sample)
	{
		totalTime += sample;
	}


	void SuperStackNode::AddAvgSample(double sample)
	{
		if (currentAvgBufferPos >= AvgBufferSize)
		{
			currentAvgBufferPos = 0;
		}

		if (avgBuffer.size() <= currentAvgBufferPos)
		{
			avgBuffer.push_back(sample);
		}
		else
		{
			avgBuffer[currentAvgBufferPos] = sample;
		}

		currentAvgBufferPos++;
	}


	void SuperStackNode::SetNumTimesCalled(size_t setNumTimes)
	{
		numTimesCalled = setNumTimes;
	}


	size_t SuperStackNode::GetNumTimesCalled(void) const
	{
		return numTimesCalled;
	}


	void SuperStackNode::AddChild(SuperStackNode * addChild)
	{
		children.push_back(addChild);
	}


	void SuperStackNode::RemoveChild(SuperStackNode * removeChild)
	{
		NodeList::iterator iter;
		for (iter = children.begin(); iter != children.end(); iter++)
		{
			if ((*iter) == removeChild)
			{
				children.erase(iter);
				break;
			}
		}
	}
}