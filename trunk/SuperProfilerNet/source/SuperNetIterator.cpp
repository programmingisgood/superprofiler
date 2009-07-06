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

#include "SuperNetIterator.h"

namespace SuperProfiler
{
	SuperNetIterator::SuperNetIterator()
	{
	}


	SuperNetIterator::SuperNetIterator(SuperStackNode * start) : realIter(start)
	{
	}


	SuperNetIterator::SuperNetIterator(const SuperNetIterator & rhs) : realIter(rhs.realIter)
	{
	}


	const SuperNetIterator & SuperNetIterator::operator=(const SuperNetIterator & rhs)
	{
		realIter = rhs.realIter;
		return *this;
	}


	void SuperNetIterator::SetFirst(void)
	{
		realIter.SetFirst();
	}


	void SuperNetIterator::SetNext(void)
	{
		realIter.SetNext();
	}


	void SuperNetIterator::SetCurrentChild(size_t index)
	{
		realIter.SetCurrentChild(index);
	}


	bool SuperNetIterator::IsEnd(void) const
	{
		return realIter.IsEnd();
	}


	bool SuperNetIterator::IsRoot(void) const
	{
		return realIter.IsRoot();
	}


	size_t SuperNetIterator::GetNumChildren(void) const
	{
		return realIter.GetNumChildren();
	}


	void SuperNetIterator::EnterChild(size_t index)
	{
		realIter.EnterChild(index);
	}


	void SuperNetIterator::EnterParent(void)
	{
		realIter.EnterParent();
	}


	size_t SuperNetIterator::GetCurrentID(void) const
	{
		return realIter.GetCurrentID();
	}


	const char * SuperNetIterator::GetCurrentName(void) const
	{
		return realIter.GetCurrentName();
	}


	size_t SuperNetIterator::GetCurrentTotalCalls(void) const
	{
		return realIter.GetCurrentTotalCalls();
	}


	double SuperNetIterator::GetCurrentTotalTime(void) const
	{
		return realIter.GetCurrentTotalTime();
	}


	size_t SuperNetIterator::GetCurrentParentID(void) const
	{
		return realIter.GetCurrentParentID();
	}


	const char * SuperNetIterator::GetCurrentParentName(void) const
	{
		return realIter.GetCurrentParentName();
	}


	size_t SuperNetIterator::GetCurrentParentTotalCalls(void) const
	{
		return realIter.GetCurrentParentTotalCalls();
	}


	double SuperNetIterator::GetCurrentParentTotalTime(void) const
	{
		return realIter.GetCurrentParentTotalTime();
	}
}