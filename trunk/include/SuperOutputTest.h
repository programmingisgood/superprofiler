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

#ifndef SUPEROUTPUTTEST_H
#define SUPEROUTPUTTEST_H

#include "SuperOutput.h"
#include <string>
#include <map>

namespace SuperProfiler
{
	class SuperOutputTest : public SuperOutput
	{
	public:
		SuperOutputTest();
		~SuperOutputTest();

		void OutputFunctionData(SuperFuncDataList & funcData, double totalRunTime);
		void OutputCallTree(SuperStackNode * stack);

		size_t GetMaxDepth(void);
		double GetTotalRunTime(void) const;

		/**
		* Returns the total run time for the function name passed in.
		* Returns -1 if the passed in function name could not be found.
		**/
		double GetFunctionRunTime(const std::string & functionName) const;
		size_t GetFunctionCallAmount(const std::string & functionName) const;

		class SuperOutputTestCallNode
		{
		public:
			SuperOutputTestCallNode(const std::string & setName);
			~SuperOutputTestCallNode();

			const std::string GetName(void) const;

			void SetCallAmount(size_t setAmount);
			size_t GetCallAmount(void) const;
			void SetRunTime(double setTime);
			double GetRunTime(void) const;
			void SetAvgRunTime(double setTime);
			double GetAvgRunTime(void) const;

			void AddChild(SuperOutputTestCallNode * addChild);

			SuperOutputTestCallNode * GetFunction(const std::string & funcName);

		private:
			//No default construction!
			SuperOutputTestCallNode();
			//No copies!
			SuperOutputTestCallNode(const SuperOutputTestCallNode &);
			const SuperOutputTestCallNode & operator=(const SuperOutputTestCallNode &);

			std::string name;
			size_t callAmount;
			double runTime;
			double avgRunTime;
			typedef std::list<SuperOutputTestCallNode *> CallNodeList;
			CallNodeList functions;
		};

		SuperOutputTestCallNode & GetCallTree(void);

	private:
		//No copies!
		SuperOutputTest(const SuperOutputTest &);
		const SuperOutputTest & operator=(const SuperOutputTest &);

		/**
		* Records the call tree into root.
		**/
		void Record(SuperStackNode * stack, SuperOutputTestCallNode & currNode);

		void FindMaxDepth(SuperStackNode * currNode, size_t currDepth);

		size_t maxDepth;
		double totalRunTime;
		typedef std::map<std::string, double> SuperFuncTimeList;
		SuperFuncTimeList funcTimeList;
		typedef std::map<std::string, size_t> SuperFuncCallAmountList;
		SuperFuncCallAmountList funcCallAmountList;
		SuperOutputTestCallNode root;
	};
}

#endif