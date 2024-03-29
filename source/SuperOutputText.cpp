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

#include "SuperOutputText.h"
#include "SuperFunctionData.h"
#include "SuperStackNode.h"
#include <iomanip>

namespace SuperProfiler
{
	SuperOutputText::SuperOutputText(const std::string & fileName)
	{
		outputFile.open(fileName.c_str(), std::ios::trunc);

		//Define the format for the stream up front
		outputFile << std::fixed << std::setprecision(3);
	}


	SuperOutputText::~SuperOutputText()
	{
		outputFile.close();
	}


	void SuperOutputText::OutputFunctionData(SuperFuncDataList & funcData, double totalRunTime)
	{
		//First, sort the function list
		SortByTime(funcData, true);

		size_t totalNumFuncCalls = GetTotalNumFunctionCalls(funcData);

		outputFile << "Function List | Total Run Time=" << totalRunTime << " | Total Profiled Function Calls=" << totalNumFuncCalls;
		outputFile << std::endl << "---------------------------------------------------------------------------------" << std::endl;

		SuperFuncDataList::iterator iter;
		for (iter = funcData.begin(); iter != funcData.end(); iter++)
		{
			//The copy is needed in case the actual string has been deleted
			outputFile << (*iter)->GetNameCopy() << " | ";
			outputFile << "Total Time=" << (*iter)->GetTotalTime() << " | ";
			outputFile << ((*iter)->GetTotalTime() / totalRunTime) * 100 << "% of time | ";
			outputFile << "Total Calls=" << (*iter)->GetTotalNumTimesCalled() << " | ";
			outputFile << (static_cast<double>((*iter)->GetTotalNumTimesCalled()) / static_cast<double>(totalNumFuncCalls)) * 100 << "% of calls" << std::endl;
		}

		outputFile << std::endl << std::endl;
	}


	void SuperOutputText::OutputCallTree(SuperStackNode * stack)
	{
		outputFile << "Call Tree";
		outputFile << std::endl << "---------------------------------------------------------------------------------" << std::endl;

		OutputNode(stack, 0, outputFile);
	}


	void SuperOutputText::OutputNode(SuperStackNode * outputNode, size_t currDepth, std::ofstream & outputFile)
	{
		//Special case for the root node
		if (currDepth == 0)
		{
			outputFile << "ROOT" << std::endl;
		}
		else
		{
			for (size_t d = 0; d < currDepth; d++)
			{
				//4 spaces per depth
				outputFile << "    ";
			}
			//The copy is needed in case the actual string has been deleted
			outputFile << outputNode->GetFuncData()->GetNameCopy() << " | ";
			outputFile << "Total Time=" << outputNode->GetTotalTime() << " | ";
			outputFile << "Avg Time=" << outputNode->GetAverageTime() << " | ";
			outputFile << "Times called=" << outputNode->GetNumTimesCalled() << std::endl;
		}

		size_t numChildren = outputNode->GetNumChildren();
		for (size_t i = 0; i < numChildren; i++)
		{
			SuperStackNode * currChild = outputNode->GetChild(i);
			OutputNode(currChild, currDepth + 1, outputFile);
		}
	}
}