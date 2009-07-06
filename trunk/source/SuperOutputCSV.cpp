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

#include "SuperOutputCSV.h"
#include "SuperFunctionData.h"
#include "SuperUtils.h"
#include <iomanip>

namespace SuperProfiler
{
	SuperOutputCSV::SuperOutputCSV(const std::string & fileName)
	{
		outputFile.open(fileName.c_str(), std::ios::trunc);
	}


	SuperOutputCSV::~SuperOutputCSV()
	{
		outputFile.close();
	}


	void SuperOutputCSV::OutputFunctionData(SuperFuncDataList & funcData, double totalRunTime)
	{
		//First, sort the function list
		SortByTime(funcData, true);

		size_t totalNumFuncCalls = GetTotalNumFunctionCalls(funcData);

		//First, list the headers
		outputFile << "Function Name,Total Run Time,Percent of Time,Total Calls,Percent of Calls";

		SuperFuncDataList::iterator iter;
		for (iter = funcData.begin(); iter != funcData.end(); iter++)
		{
			//First, insert the end line needed before the previous line
			//This makes it so there won't be any unneeded end lines at the end of the file
			outputFile << std::endl;

			//The copy is needed in case the actual string has been deleted
			std::string funcName = (*iter)->GetNameCopy();
			//Replace any commas in the name with semi-colons
			funcName = SuperUtils::FindAndReplace(funcName, ",", ";");

			outputFile << std::fixed << std::setprecision(2);
			outputFile << funcName << ",";
			outputFile << (*iter)->GetTotalTime() << ",";
			outputFile << ((*iter)->GetTotalTime() / totalRunTime) * 100 << "%,";
			outputFile << (*iter)->GetTotalNumTimesCalled() << ",";
			outputFile << (static_cast<double>((*iter)->GetTotalNumTimesCalled()) / static_cast<double>(totalNumFuncCalls)) * 100 << "%";
		}
	}


	void SuperOutputCSV::OutputCallTree(SuperStackNode * stack)
	{
		//Cannot output call tree to CSV file
	}
}