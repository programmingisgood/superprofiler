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

#ifndef SUPEREXCEPTION_H
#define SUPEREXCEPTION_H

#include <stdexcept>
#include <string>

#define SUPER_EXCEPTION(WHAT) SuperException((WHAT), __FUNCTION__, __FILE__, __LINE__)

namespace SuperProfiler
{
	class SuperException : public std::exception
	{
	public:
		SuperException(const std::string & whatArg, const char * whichFunc = NULL, const char * whichFile = NULL, unsigned long whatLine = 0) throw() :
					   whatStr(whatArg), func(whichFunc), file(whichFile), line(whatLine)
		{
		}

		~SuperException() throw()
		{
		}

		const char * what() const throw()
		{
			return whatStr.c_str();
		}

		void SetWhat(const std::string & setWhat)
		{
			whatStr = setWhat;
		}

		const char * GetFunction(void) const throw()
		{
			return func;
		}

		const char * GetFile() const throw()
		{
			return file;
		}

		unsigned long GetLine() const throw()
		{
			return line;
		}

	private:
		std::string whatStr;
		const char * func;
		const char * file;
		unsigned long line;
	};
}

#endif