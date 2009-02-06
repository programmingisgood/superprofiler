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

#include "TextOutput.h"
#include "SuperFunctionData.h"
#include "SuperStackNode.h"
#include <iomanip>

namespace SuperProfiler
{
	TextOutput::TextOutput(const std::string & fileName)
	{
		outputFile.open(fileName.c_str(), std::ios::trunc);
	}


	TextOutput::~TextOutput()
	{
		outputFile.close();
	}


	void TextOutput::OutputFunctionData(FuncDataList & funcData, double totalRunTime)
	{
		//First, sort the function list
		SortByTime(funcData, true);

		size_t totalNumFuncCalls = GetTotalNumFunctionCalls(funcData);

		outputFile << "Function List | Total Run Time=" << totalRunTime << " | Total Profiled Function Calls=" << totalNumFuncCalls;
		outputFile << std::endl << "---------------------------------------------------------------------------------" << std::endl;

		FuncDataList::iterator iter;
		for (iter = funcData.begin(); iter != funcData.end(); iter++)
		{
			outputFile << std::fixed << std::setprecision(2);
			outputFile << (*iter)->GetName() << " | ";
			outputFile << "Total Time=" << (*iter)->GetTotalTime() << " | ";
			outputFile << ((*iter)->GetTotalTime() / totalRunTime) * 100 << "% of time | ";
			outputFile << "Total Calls=" << (*iter)->GetTotalNumTimesCalled() << " | ";
			outputFile << (static_cast<double>((*iter)->GetTotalNumTimesCalled()) / static_cast<double>(totalNumFuncCalls)) * 100 << "% of calls" << std::endl;
		}

		outputFile << std::endl << std::endl;
	}


	void TextOutput::OutputCallTree(SuperStackNode * stack)
	{
		outputFile << std::fixed << std::setprecision(4) << "Call Tree";
		outputFile << std::endl << "---------------------------------------------------------------------------------" << std::endl;

		OutputNode(stack, 0, outputFile);
	}


	void TextOutput::OutputNode(SuperStackNode * outputNode, size_t currDepth, std::ofstream & outputFile)
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
			outputFile << outputNode->GetFuncData()->GetName() << " | ";
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