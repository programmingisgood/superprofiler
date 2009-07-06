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

#ifndef SUPERITERATOR_H
#define SUPERITERATOR_H

namespace SuperProfiler
{
	class SuperStackNode;
	/**
	* SuperIterator is used to navigate through the call tree.
	**/
	class SuperIterator
	{
	public:
		SuperIterator();
		SuperIterator(SuperStackNode * start);
		~SuperIterator();

		/**
		* Must support copying
		**/
		SuperIterator(const SuperIterator & rhs);
		const SuperIterator & operator=(const SuperIterator & rhs);

		/**
		* Access all the children of the current parent
		**/
		void SetFirst(void);
		void SetNext(void);
		void SetCurrentChild(size_t index);
		bool IsEnd(void) const;
		bool IsRoot(void) const;

		size_t GetNumChildren(void) const;
		void EnterChild(size_t index);
		void EnterParent(void);

		/**
		* Access the current child
		**/
		size_t GetCurrentID(void) const;
		const char * GetCurrentName(void) const;
		size_t GetCurrentTotalCalls(void) const;
		double GetCurrentTotalTime(void) const;
		double GetCurrentMeasureTime(void) const;
		double GetCurrentLastHighestTimeWhileMeasuring(void) const;

		/**
		* Access the current parent
		**/
		size_t GetCurrentParentID(void) const;
		const char * GetCurrentParentName(void) const;
		size_t GetCurrentParentTotalCalls(void) const;
		double GetCurrentParentTotalTime(void) const;
		double GetCurrentParentMeasureTime(void) const;
		double GetCurrentParentLastHighestTimeWhileMeasuring(void) const;

	protected:
		SuperStackNode * currentParent;
		SuperStackNode * currentChild;
	};
}

#endif