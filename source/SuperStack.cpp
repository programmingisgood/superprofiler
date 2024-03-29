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

#include "SuperStack.h"
#include "SuperFunctionData.h"
#include "SuperException.h"

namespace SuperProfiler
{
	SuperStack::SuperStack() : SuperStackNode(NULL, NULL), currentDepth(0), currentChild(NULL)
	{
	}


	SuperStack::~SuperStack()
	{
	}


	void SuperStack::Push(SuperFunctionData * setFuncData, double startTime)
	{
		if (currentChild)
		{
			SuperStackNode * existingChild = currentChild->GetChild(setFuncData);
			if (existingChild)
			{
				currentChild = existingChild;
			}
			else
			{
				existingChild = new SuperStackNode(currentChild, setFuncData);
				currentChild->AddChild(existingChild);
				currentChild = existingChild;
			}
		}
		else
		{
			//First check if there is already a child that matches the passed in THIS node
			SuperStackNode * existingChild = this->GetChild(setFuncData);
			if (existingChild)
			{
				currentChild = existingChild;
			}
			else
			{
				//Otherwise, create the new child
				currentChild = new SuperStackNode(this, setFuncData);
				AddChild(currentChild);
			}
		}

		if (currentChild)
		{
			//Mark time so we can calculate an average later
			currentChild->SetStartTime(startTime);
		}
		currentDepth++;
	}


	void SuperStack::Pop(SuperFunctionData * setFuncData, double endTime, bool allowThrow)
	{
		if (currentChild)
		{
			//Make sure the correct function is being popped
			if (allowThrow && currentChild->GetFuncData() != setFuncData)
			{
				if (setFuncData->GetName() && currentChild->GetFuncData()->GetName())
				{
					throw SUPER_EXCEPTION(std::string("Function named ") + setFuncData->GetName() + " passed into SuperStack::Pop() when " +
										  currentChild->GetFuncData()->GetName() + " was expected");
				}
				throw SUPER_EXCEPTION(std::string("Invalid pop detected in SuperStack::Pop()"));
			}

			//Mark time so we can calculate an average
			currentChild->SetEndTime(endTime);

			currentChild = currentChild->GetParent();
			if (currentChild == this)
			{
				currentChild = NULL;
			}
		}
		currentDepth--;
	}


	size_t SuperStack::GetCurrentDepth(void) const
	{
		return currentDepth;
	}


	void SuperStack::Reset(void)
	{
		SuperStackNode::Reset();

		currentDepth = 0;
		currentChild = NULL;
	}
}