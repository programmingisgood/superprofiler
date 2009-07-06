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

#ifndef SUPERBITSTREAM_H
#define SUPERBITSTREAM_H

#include <string>

namespace SuperProfiler
{
	class SuperBitStream
	{
	public:
		SuperBitStream();
		~SuperBitStream();
		/**
		* Ownership is transfered in assignment
		**/
		SuperBitStream(SuperBitStream & rhs);
		const SuperBitStream & operator=(SuperBitStream & rhs);

		/**
		* SetData will delete any data it manages if it owns it before accepting the new data
		**/
		void SetData(char * setData, size_t setDataSize, bool setOwnsData);
		char * GetData(void);
		size_t GetDataSize(void) const;

		/**
		* If this stream owns it's data then it will delete the data upon destruction
		**/
		void SetOwnsData(bool setOwns);
		bool GetOwnsData(void) const;

		/**
		* Move the read position back to the start of the stream
		**/
		void Rewind(void);
		/**
		* Move the read position forward by the number of bytes passed in
		**/
		void Forward(size_t numBytes);
		/**
		* Move the read position back by the number of bytes passed in
		**/
		void Back(size_t numBytes);
		/**
		* Is the stream read position at or past the end of the stream?
		**/
		bool AtEnd(void) const;

		void Clear(void);

		void Serialize(bool write, std::string & data);
		void Serialize(bool write, int & data);
		void Serialize(bool write, unsigned int & data);
		void Serialize(bool write, float & data);
		void Serialize(bool write, short & data);
		void Serialize(bool write, char & data);
		void Serialize(bool write, bool & data);

		void WriteString(const std::string & strData);
		void ReadString(std::string & strData);
		void WriteInt(int data);
		void WriteInt(size_t data);
		void ReadInt(int & data);
		void ReadInt(size_t & data);
		void WriteFloat(float data);
		void ReadFloat(float & data);
		void WriteShort(short data);
		void ReadShort(short & data);
		void WriteChar(char data);
		void ReadChar(char & data);
		void WriteBool(bool data);
		void ReadBool(bool & data);

	private:
		/**
		* Assings ownership of the stream data to the passed in SuperBitStream
		**/
		void AssignOwnership(SuperBitStream & rhs);
		void WriteBytesInternal(const char * bytes, size_t numBytes);
		void WriteBytesInternal(size_t bytes, size_t numBytes);
		void ReadBytesInternal(size_t numBytes, char ** readData);
		void ReadBytesInternal(size_t numBytes, size_t & readData);
		void AddBytes(size_t numberOfBytesToAdd);

		bool ownsData;
		char * data;
		size_t dataSize;
		size_t currentPos;
		size_t numberOfBytesAllocated;
		/**
		* We allocate memory in this block size so we don't make as many smaller
		* allocations
		**/
		static const unsigned int ALLOCATION_BLOCK_SIZE = 128;
	};
}

#endif