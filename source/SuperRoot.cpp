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

#include "SuperRoot.h"
#include "SuperRootListener.h"
#include "SuperOutput.h"
#include "SuperFunctionData.h"
#include "SuperException.h"

SuperProfiler::SuperRoot::ListenerList SuperProfiler::SuperRoot::listeners;
SuperProfiler::SuperTimer SuperProfiler::SuperRoot::superTimer;
SuperProfiler::SuperStack SuperProfiler::SuperRoot::superStack;
size_t SuperProfiler::SuperRoot::nextFuncID = 0;
SuperProfiler::SuperFuncDataListWrapper SuperProfiler::SuperRoot::superFuncDataListWrapper;
//Ready to record right away
bool SuperProfiler::SuperRoot::recording = true;
bool SuperProfiler::SuperRoot::allowThrow = true;

namespace SuperProfiler
{
	void SuperRoot::AddListener(SuperRootListener * addListener)
	{
		if (addListener == NULL)
		{
			return;
		}
		if (FindListener(addListener))
		{
			return;
		}

		listeners.push_back(addListener);
	}


	bool SuperRoot::FindListener(SuperRootListener * findListener)
	{
		ListenerList::iterator iter;
		for (iter = listeners.begin(); iter != listeners.end(); iter++)
		{
			if ((*iter) == findListener)
			{
				return true;
			}
		}
		return false;
	}


	void SuperRoot::RemoveListener(SuperRootListener * removeListener)
	{
		ListenerList::iterator iter;
		for (iter = listeners.begin(); iter != listeners.end(); iter++)
		{
			if ((*iter) == removeListener)
			{
				listeners.erase(iter);
				break;
			}
		}
	}


	void SuperRoot::Reset(void)
	{
		superTimer.Reset();
		superStack.Reset();
		superFuncDataListWrapper.Reset();
		//Ready to record
		recording = true;
		allowThrow = true;
	}


	void SuperRoot::OutputResults(SuperOutput & output)
	{
		if (superStack.GetCurrentDepth() != 0)
		{
			throw SUPER_EXCEPTION("The call tree stack was not at the depth 0 while in SuperRoot::OutputResults()");
		}

		//Done recording once the results need to be outputted
		recording = false;

		//Calculate the total run time by adding the times of all children times off the root together
		double totalRunTime = 0;
		size_t numChildren = superStack.GetNumChildren();
		for (size_t c = 0; c < numChildren; c++)
		{
			SuperStackNode * child = superStack.GetChild(c);
			totalRunTime += child->GetFuncData()->GetTotalTime();
		}
		output.OutputFunctionData(superFuncDataListWrapper.superFuncDataList, totalRunTime);
		output.OutputCallTree(&superStack);
	}


	void SuperRoot::ResetMeasure(void)
	{
		superStack.ResetMeasure();
	}


	void SuperRoot::PushProfile(const char * name)
	{
		if (recording)
		{
			SuperFunctionData * foundFunc = FindFuncData(name);
			if (!foundFunc)
			{
				nextFuncID++;
				if (nextFuncID == 0)
				{
					throw SUPER_EXCEPTION("Ran out of function IDs in SuperRoot::PushProfile()");
				}
				foundFunc = new SuperFunctionData(nextFuncID, name);
				AddNewFuncData(foundFunc);
			}
			superStack.Push(foundFunc, superTimer.GetTimeSeconds());
		}
	}


	void SuperRoot::PopProfile(const char * name)
	{
		if (recording)
		{
			SuperFunctionData * foundFunc = FindFuncData(name);
			if (!foundFunc)
			{
				throw SUPER_EXCEPTION(std::string("Function named ") + name + " not found in SuperRoot::PopProfile()");
			}
			try
			{
				superStack.Pop(foundFunc, superTimer.GetTimeSeconds(), allowThrow);
			}
			catch (...)
			{
				allowThrow = false;
				throw;
			}
		}
	}


	const SuperFuncDataList & SuperRoot::GetFuncList(void)
	{
		return superFuncDataListWrapper.superFuncDataList;
	}


	SuperIterator SuperRoot::GetIterator(void)
	{
		return SuperIterator(&superStack);
	}


	SuperFunctionData * SuperRoot::FindFuncData(const char * name)
	{
		SuperFuncDataList::iterator iter;
		for (iter = superFuncDataListWrapper.superFuncDataList.begin(); iter != superFuncDataListWrapper.superFuncDataList.end(); iter++)
		{
			if ((*iter)->GetName() == name)
			{
				return (*iter);
			}
		}
		return NULL;
	}


	void SuperRoot::AddNewFuncData(SuperFunctionData * newFuncData)
	{
		if (FindFuncData(newFuncData->GetName()))
		{
			//Already exists
			return;
		}

		superFuncDataListWrapper.superFuncDataList.push_back(newFuncData);

		ListenerList::iterator iter;
		for (iter = listeners.begin(); iter != listeners.end(); iter++)
		{
			(*iter)->FunctionAdded(newFuncData->GetID(), newFuncData->GetName());
		}
	}
}