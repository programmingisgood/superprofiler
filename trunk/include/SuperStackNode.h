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

#ifndef SUPERSTACKNODE_H
#define SUPERSTACKNODE_H

#include <list>
#include <vector>

namespace SuperProfiler
{
	class SuperFunctionData;

	class SuperStackNode
	{
	public:
		SuperStackNode(SuperStackNode * setParent, SuperFunctionData * setFuncData);
		virtual ~SuperStackNode();

		/**
		* Reset will reset this node and delete any children it has
		**/
		virtual void Reset(void);

		void SetFuncData(SuperFunctionData * setFuncData);
		SuperFunctionData * GetFuncData(void) const;

		SuperStackNode * GetParent(void);

		size_t GetNumChildren(void) const;

		SuperStackNode * GetChild(size_t atIndex);
		SuperStackNode * GetChild(SuperFunctionData * childFunc);

		void AddChild(SuperStackNode * addChild);
		/**
		* RemoveChild does not delete removeChild
		**/
		void RemoveChild(SuperStackNode * removeChild);

		void SetStartTime(double setStartTime);
		double GetStartTime(void) const;

		void SetEndTime(double setEndTime);
		double GetEndTime(void) const;

		void SetTotalTime(double setTime);
		double GetTotalTime(void) const;
		double GetAverageTime(void) const;

		void ResetMeasure(void);
		/**
		* Returns the amount of time spent in this node
		* the last time it was measured
		**/
		double GetLastMeasureTime(void) const;

		/**
		* Returns the highest amount of time spent in this node
		* during the last measure time period
		**/
		double GetLastHighestTimeWhileMeasuring(void) const;

		void SetNumTimesCalled(size_t setNumTimes);
		size_t GetNumTimesCalled(void) const;

	private:
		//No default construction!
		SuperStackNode();
		//No copies!
		SuperStackNode(const SuperStackNode &);
		const SuperStackNode & operator=(const SuperStackNode &);

		void AddToTotalTime(double sample);
		void AddAvgSample(double sample);

		SuperStackNode * parent;
		SuperFunctionData * funcData;
		typedef std::list<SuperStackNode *> NodeList;
		NodeList children;
		double startTime;
		double endTime;
		double totalTime;
		double currentMeasureTime;
		double lastMeasureTime;
		double currentHighestTimeWhileMeasuring;
		double lastHighestTimeWhileMeasuring;
		static const size_t AvgBufferSize = 100;
		size_t currentAvgBufferPos;
		typedef std::vector<double> AvgBuffer;
		AvgBuffer avgBuffer;
		size_t numTimesCalled;
	};
}

#endif