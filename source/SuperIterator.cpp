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

#include "SuperIterator.h"
#include "SuperStackNode.h"
#include "SuperFunctionData.h"
#include "SuperException.h"

namespace SuperProfiler
{
	SuperIterator::SuperIterator() : currentChild(NULL), currentParent(NULL)
	{
	}


	SuperIterator::SuperIterator(SuperStackNode * start)
	{
		if (start == NULL)
		{
			throw SUPER_EXCEPTION("NULL SuperStackNode passed into SuperIterator::SuperIterator(SuperStackNode *)");
		}

		currentChild = NULL;
		currentParent = start;
		if (currentParent->GetNumChildren() > 0)
		{
			currentChild = currentParent->GetChild(static_cast<size_t>(0));
		}
	}


	SuperIterator::~SuperIterator()
	{
		currentParent = NULL;
		currentChild = NULL;
	}


	SuperIterator::SuperIterator(const SuperIterator & rhs)
	{
		currentParent = rhs.currentParent;
		currentChild = rhs.currentChild;
	}


	const SuperIterator & SuperIterator::operator=(const SuperIterator & rhs)
	{
		currentParent = rhs.currentParent;
		currentChild = rhs.currentChild;
		return *this;
	}


	void SuperIterator::SetFirst(void)
	{
		if (currentParent && currentParent->GetNumChildren() > 0)
		{
			currentChild = currentParent->GetChild(static_cast<size_t>(0));
		}
	}


	void SuperIterator::SetNext(void)
	{
		if (currentParent)
		{
			bool foundNext = false;
			size_t numChildren = currentParent->GetNumChildren();
			for (size_t currChildIndex = 0; currChildIndex < numChildren; currChildIndex++)
			{
				if (foundNext)
				{
					currentChild = currentParent->GetChild(currChildIndex);
					break;
				}
				if (currentChild == currentParent->GetChild(currChildIndex))
				{
					foundNext = true;
				}
			}
		}
	}


	void SuperIterator::SetCurrentChild(size_t index)
	{
		if (currentParent)
		{
			if (index < currentParent->GetNumChildren())
			{
				currentChild = currentParent->GetChild(index);
			}
		}
	}


	bool SuperIterator::IsEnd(void) const
	{
		if (currentParent)
		{
			size_t numChildren = currentParent->GetNumChildren();
			if (numChildren == 0)
			{
				return true;
			}
			for (size_t currChildIndex = 0; currChildIndex < numChildren; currChildIndex++)
			{
				if (currentChild == currentParent->GetChild(currChildIndex))
				{
					if (currChildIndex == numChildren - 1)
					{
						return true;
					}
					break;
				}
			}
			return false;
		}
		return true;
	}


	bool SuperIterator::IsRoot(void) const
	{
		if (currentParent)
		{
			return (currentParent->GetParent() == NULL);
		}
		return true;
	}


	size_t SuperIterator::GetNumChildren(void) const
	{
		if (currentParent)
		{
			return currentParent->GetNumChildren();
		}
		return 0;
	}


	void SuperIterator::EnterChild(size_t index)
	{
		if (currentParent && index < currentParent->GetNumChildren())
		{
			currentParent = currentParent->GetChild(index);
			currentChild = NULL;
			SetFirst();
		}
	}


	void SuperIterator::EnterParent(void)
	{
		if (currentParent && currentParent->GetParent())
		{
			currentParent = currentParent->GetParent();
			currentChild = NULL;
			SetFirst();
		}
	}


	size_t SuperIterator::GetCurrentID(void) const
	{
		if (currentChild)
		{
			return currentChild->GetFuncData()->GetID();
		}
		return NULL;
	}


	const char * SuperIterator::GetCurrentName(void) const
	{
		if (currentChild)
		{
			return currentChild->GetFuncData()->GetName();
		}
		return NULL;
	}


	size_t SuperIterator::GetCurrentTotalCalls(void) const
	{
		if (currentChild)
		{
			return currentChild->GetNumTimesCalled();
		}
		return 0;
	}


	double SuperIterator::GetCurrentTotalTime(void) const
	{
		if (currentChild)
		{
			return currentChild->GetTotalTime();
		}
		return 0;
	}


	double SuperIterator::GetCurrentMeasureTime(void) const
	{
		if (currentChild)
		{
			return currentChild->GetLastMeasureTime();
		}
		return 0;
	}


	double SuperIterator::GetCurrentLastHighestTimeWhileMeasuring(void) const
	{
		if (currentChild)
		{
			return currentChild->GetLastHighestTimeWhileMeasuring();
		}
		return 0;
	}


	size_t SuperIterator::GetCurrentParentID(void) const
	{
		if (currentParent->GetFuncData())
		{
			return currentParent->GetFuncData()->GetID();
		}
		return 0;
	}


	const char * SuperIterator::GetCurrentParentName(void) const
	{
		if (currentParent->GetFuncData())
		{
			return currentParent->GetFuncData()->GetName();
		}
		return "ROOT";
	}


	size_t SuperIterator::GetCurrentParentTotalCalls(void) const
	{
		if (currentParent)
		{
			//If this is the root, add up the children to find the total
			if (currentParent->GetFuncData() == NULL)
			{
				size_t totalCalls = 0;
				size_t numChildren = currentParent->GetNumChildren();
				for (size_t c = 0; c < numChildren; c++)
				{
					totalCalls += currentParent->GetChild(c)->GetNumTimesCalled();
				}
				return totalCalls;
			}
			//Otherwise, return the accurate parent count
			return currentParent->GetNumTimesCalled();
		}
		return 0;
	}


	double SuperIterator::GetCurrentParentTotalTime(void) const
	{
		if (currentParent)
		{
			//If this is the root, add up the children times to find the total time
			if (currentParent->GetFuncData() == NULL)
			{
				double totalTime = 0;
				size_t numChildren = currentParent->GetNumChildren();
				for (size_t c = 0; c < numChildren; c++)
				{
					totalTime += currentParent->GetChild(c)->GetTotalTime();
				}
				return totalTime;
			}
			//Otherwise, return the accurate parent time
			return currentParent->GetTotalTime();
		}
		return 0;
	}


	double SuperIterator::GetCurrentParentMeasureTime(void) const
	{
		if (currentParent)
		{
			//If this is the root, add up the children times to find the total time
			if (currentParent->GetFuncData() == NULL)
			{
				double totalMeasureTime = 0;
				size_t numChildren = currentParent->GetNumChildren();
				for (size_t c = 0; c < numChildren; c++)
				{
					totalMeasureTime += currentParent->GetChild(c)->GetLastMeasureTime();
				}
				return totalMeasureTime;
			}
			//Otherwise, return the accurate parent time
			return currentParent->GetLastMeasureTime();
		}
		return 0;
	}


	double SuperIterator::GetCurrentParentLastHighestTimeWhileMeasuring(void) const
	{
		if (currentParent)
		{
			return currentParent->GetLastHighestTimeWhileMeasuring();
		}
		return 0;
	}
}