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

#include "SuperOutputXML.h"
#include "SuperFunctionData.h"
#include "SuperStackNode.h"
#include "SuperUtils.h"
#include <iomanip>

namespace SuperProfiler
{
	SuperOutputXML::SuperOutputXML(const std::string & fileName)
	{
		outputFile.open(fileName.c_str(), std::ios::trunc);

		//Define the format for the stream up front
		outputFile << std::fixed << std::setprecision(3);

		//First, output the start root tag
		outputFile << "<SuperProfiler>" << std::endl;
	}


	SuperOutputXML::~SuperOutputXML()
	{
		//Lastly, output the end root tag
		outputFile << "</SuperProfiler>" << std::endl;

		outputFile.close();
	}


	void SuperOutputXML::OutputFunctionData(SuperFuncDataList & funcData, double totalRunTime)
	{
		//First, sort the function list
		SortByTime(funcData, true);

		size_t totalNumFuncCalls = GetTotalNumFunctionCalls(funcData);

		//One tab since we are one level down from the SuperProfiler tag
		OutputTabs(1, outputFile);
		//Now, the function list tag
		outputFile << "<FunctionList total_time=\"" << totalRunTime << "\" total_profiled_function_calls=\"" << totalNumFuncCalls << "\">";

		SuperFuncDataList::iterator iter;
		for (iter = funcData.begin(); iter != funcData.end(); iter++)
		{
			//First, insert the end line needed before the previous line
			//This makes it so there won't be any unneeded end lines at the end of the file
			outputFile << std::endl;

			//Two tabs since we are one level down from the FunctionList tag
			OutputTabs(2, outputFile);

			std::string funcName = (*iter)->GetName();
			funcName = ReplaceXMLSpecialCharacters(funcName);

			outputFile << "<Function ";
			outputFile << "name=\"" << funcName << "\" ";
			outputFile << "total_time=\"" << (*iter)->GetTotalTime() << "\" ";
			outputFile << "time_percent=\"" << ((*iter)->GetTotalTime() / totalRunTime) * 100 << "\" ";
			outputFile << "num_calls=\"" << (*iter)->GetTotalNumTimesCalled() << "\" ";
			outputFile << "call_percent=\"" << (static_cast<double>((*iter)->GetTotalNumTimesCalled()) / static_cast<double>(totalNumFuncCalls)) * 100 << "\"/>";
		}

		outputFile << std::endl;
		//One tab since we are one level down from the SuperProfiler tag
		OutputTabs(1, outputFile);
		//Close function list tag
		outputFile << "</FunctionList>" << std::endl;
	}


	void SuperOutputXML::OutputCallTree(SuperStackNode * stack)
	{
		//One tab since we are one level down from the SuperProfiler tag
		OutputTabs(1, outputFile);
		//Now, the call tree tag
		outputFile << "<CallTree>" << std::endl;

		OutputNode(stack, 0, outputFile);

		//One tab since we are one level down from the SuperProfiler tag
		OutputTabs(1, outputFile);
		//Close call tree tag
		outputFile << "</CallTree>" << std::endl;
	}


	void SuperOutputXML::OutputNode(SuperStackNode * outputNode, size_t currDepth, std::ofstream & outputFile)
	{
		//start tag
		OutputTabs(currDepth + 2, outputFile);
		//Special case for the root node
		if (currDepth == 0)
		{
			outputFile << "<ROOT>" << std::endl;
		}
		else
		{
			std::string funcName = outputNode->GetFuncData()->GetName();
			funcName = ReplaceXMLSpecialCharacters(funcName);

			outputFile << "<Function ";
			outputFile << "name=\"" << funcName << "\" ";
			outputFile << "total_time=\"" << outputNode->GetTotalTime() << "\" ";
			outputFile << "avg_time=\"" << outputNode->GetAverageTime() << "\" ";
			outputFile << "times_called=\"" << outputNode->GetNumTimesCalled() << "\">";
			outputFile << std::endl;
		}

		size_t numChildren = outputNode->GetNumChildren();
		for (size_t i = 0; i < numChildren; i++)
		{
			SuperStackNode * currChild = outputNode->GetChild(i);
			OutputNode(currChild, currDepth + 1, outputFile);
		}

		//The end tag
		OutputTabs(currDepth + 2, outputFile);
		if (currDepth == 0)
		{
			outputFile << "</ROOT>" << std::endl;
		}
		else
		{
			outputFile << "</Function>" << std::endl;
		}
	}


	void SuperOutputXML::OutputTabs(size_t howMany, std::ofstream & outputFile)
	{
		for (size_t i = 0; i < howMany; i++)
		{
			outputFile << "    ";
		}
	}


	std::string SuperOutputXML::ReplaceXMLSpecialCharacters(const std::string & search)
	{
		//& first since the replacement codes use &
		std::string retStr = SuperUtils::FindAndReplace(search, "&", "&amp;");
		retStr = SuperUtils::FindAndReplace(retStr, "<", "&lt;");
		retStr = SuperUtils::FindAndReplace(retStr, ">", "&gt;");
		retStr = SuperUtils::FindAndReplace(retStr, "\"", "&quot;");
		retStr = SuperUtils::FindAndReplace(retStr, "'", "&apos;");

		return retStr;
	}
}