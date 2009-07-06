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

#include "SuperBitStream.h"
#include "SuperException.h"

namespace SuperProfiler
{
	SuperBitStream::SuperBitStream() : ownsData(true), data(NULL), dataSize(0), currentPos(0), numberOfBytesAllocated(0)
	{
	}


	SuperBitStream::~SuperBitStream()
	{
		Clear();
	}


	SuperBitStream::SuperBitStream(SuperBitStream & rhs)
	{
		rhs.AssignOwnership(*this);
	}


	const SuperBitStream & SuperBitStream::operator=(SuperBitStream & rhs)
	{
		rhs.AssignOwnership(*this);
		return *this;
	}


	void SuperBitStream::AssignOwnership(SuperBitStream & rhs)
	{
		//First clear the stream that will receive ownership
		rhs.Clear();

		rhs.ownsData = true;
		ownsData = false;
		rhs.data = data;
		data = NULL;
		rhs.dataSize = dataSize;
		dataSize = 0;
		rhs.currentPos = currentPos;
		currentPos = 0;
		rhs.numberOfBytesAllocated = numberOfBytesAllocated;
		numberOfBytesAllocated = 0;

		//Now reset this stream
		this->Clear();
	}


	void SuperBitStream::SetData(char * setData, size_t setDataSize, bool setOwnsData)
	{
		Clear();

		data = setData;
		dataSize = setDataSize;
		ownsData = setOwnsData;
		numberOfBytesAllocated = dataSize;
	}


	char * SuperBitStream::GetData(void)
	{
		return data;
	}


	size_t SuperBitStream::GetDataSize(void) const
	{
		return dataSize;
	}


	void SuperBitStream::SetOwnsData(bool setOwns)
	{
		ownsData = setOwns;
	}


	bool SuperBitStream::GetOwnsData(void) const
	{
		return ownsData;
	}


	void SuperBitStream::Rewind(void)
	{
		currentPos = 0;
	}


	void SuperBitStream::Forward(size_t numBytes)
	{
		currentPos += numBytes;
	}


	void SuperBitStream::Back(size_t numBytes)
	{
		currentPos -= numBytes;
	}


	bool SuperBitStream::AtEnd(void) const
	{
		return (currentPos >= dataSize) ? true : false;
	}


	void SuperBitStream::Clear(void)
	{
		if (ownsData && data)
		{
			free(data);
		}
		data = NULL;
		dataSize = 0;
		ownsData = true;
		currentPos = 0;
		numberOfBytesAllocated = 0;
	}


	void SuperBitStream::Serialize(bool write, std::string & data)
	{
		if (write)
		{
			//Only write new data if we own it
			if (ownsData)
			{
				WriteString(data);
			}
		}
		else
		{
			ReadString(data);
		}
	}


	void SuperBitStream::Serialize(bool write, int & data)
	{
		if (write)
		{
			//Only write new data if we own it
			if (ownsData)
			{
				WriteInt(data);
			}
		}
		else
		{
			ReadInt(data);
		}
	}


	void SuperBitStream::Serialize(bool write, unsigned int & data)
	{
		if (write)
		{
			//Only write new data if we own it
			if (ownsData)
			{
				WriteInt(data);
			}
		}
		else
		{
			ReadInt(data);
		}
	}


	void SuperBitStream::Serialize(bool write, float & data)
	{
		if (write)
		{
			//Only write new data if we own it
			if (ownsData)
			{
				WriteFloat(data);
			}
		}
		else
		{
			ReadFloat(data);
		}
	}


	void SuperBitStream::Serialize(bool write, short & data)
	{
		if (write)
		{
			//Only write new data if we own it
			if (ownsData)
			{
				WriteShort(data);
			}
		}
		else
		{
			ReadShort(data);
		}
	}


	void SuperBitStream::Serialize(bool write, char & data)
	{
		if (write)
		{
			//Only write new data if we own it
			if (ownsData)
			{
				WriteChar(data);
			}
		}
		else
		{
			ReadChar(data);
		}
	}


	void SuperBitStream::Serialize(bool write, bool & data)
	{
		if (write)
		{
			//Only write new data if we own it
			if (ownsData)
			{
				WriteBool(data);
			}
		}
		else
		{
			ReadBool(data);
		}
	}


	void SuperBitStream::WriteString(const std::string & strData)
	{
		if (!ownsData)
		{
			throw SUPER_EXCEPTION("Data not owned in SuperBitStream::WriteString()");
		}

		WriteBytesInternal(strData.c_str(), static_cast<unsigned int>(strData.length()));
		//Zero termination
		WriteBytesInternal(static_cast<size_t>(0), 1);
	}


	void SuperBitStream::ReadString(std::string & strData)
	{
		//First, find the string length, zero termination
		size_t strLength = 0;
		size_t strSearchPos = currentPos;
		if (strSearchPos >= GetDataSize())
		{
			throw SUPER_EXCEPTION("Data overrun detected in SuperBitStream::ReadString()");
		}
		while (*(data + strSearchPos) != 0)
		{
			strSearchPos++;
			strLength++;
			if (strSearchPos >= GetDataSize())
			{
				throw SUPER_EXCEPTION("Data overrun detected in SuperBitStream::ReadString()");
			}
		}
		char * dataStr = NULL;
		ReadBytesInternal(strLength, &dataStr);
		//Read one extra byte for the zero termination
		size_t readZeroTerm = 0;
		ReadBytesInternal(1, readZeroTerm);
		strData = std::string(dataStr, strLength);
	}


	void SuperBitStream::WriteInt(int data)
	{
		if (!ownsData)
		{
			throw SUPER_EXCEPTION("Data not owned in SuperBitStream::WriteInt(int)");
		}

		WriteBytesInternal(reinterpret_cast<char *>(&data), 4);
	}


	void SuperBitStream::WriteInt(size_t data)
	{
		if (!ownsData)
		{
			throw SUPER_EXCEPTION("Data not owned in SuperBitStream::WriteInt(unsigned int)");
		}

		WriteBytesInternal(reinterpret_cast<char *>(&data), 4);
	}


	void SuperBitStream::ReadInt(int & data)
	{
		char * readData = NULL;
		ReadBytesInternal(4, &readData);
		data = *(reinterpret_cast<int *>(readData));
	}


	void SuperBitStream::ReadInt(size_t & data)
	{
		char * readData = NULL;
		ReadBytesInternal(4, &readData);
		data = *(reinterpret_cast<size_t *>(readData));
	}


	void SuperBitStream::WriteFloat(float data)
	{
		if (!ownsData)
		{
			throw SUPER_EXCEPTION("Data not owned in SuperBitStream::WriteFloat()");
		}

		WriteBytesInternal(reinterpret_cast<char *>(&data), 4);
	}


	void SuperBitStream::ReadFloat(float & data)
	{
		char * readData = NULL;
		ReadBytesInternal(4, &readData);
		data = *(reinterpret_cast<float *>(readData));
	}


	void SuperBitStream::WriteShort(short data)
	{
		if (!ownsData)
		{
			throw SUPER_EXCEPTION("Data not owned in SuperBitStream::WriteShort()");
		}

		WriteBytesInternal(reinterpret_cast<char *>(&data), 2);
	}


	void SuperBitStream::ReadShort(short & data)
	{
		char * readData = NULL;
		ReadBytesInternal(2, &readData);
		data = *(reinterpret_cast<short *>(readData));
	}


	void SuperBitStream::WriteChar(char data)
	{
		if (!ownsData)
		{
			throw SUPER_EXCEPTION("Data not owned in SuperBitStream::WriteChar()");
		}

		WriteBytesInternal(data, 1);
	}


	void SuperBitStream::ReadChar(char & data)
	{
		unsigned int intData = 0;
		ReadBytesInternal(1, intData);
		data = static_cast<char>(intData);
	}


	void SuperBitStream::WriteBool(bool data)
	{
		if (!ownsData)
		{
			throw SUPER_EXCEPTION("Data not owned in SuperBitStream::WriteBool()");
		}

		WriteBytesInternal(data, 1);
	}


	void SuperBitStream::ReadBool(bool & data)
	{
		unsigned int boolData = 0;
		ReadBytesInternal(1, boolData);
		data = (boolData) ? true : false;
	}


	void SuperBitStream::WriteBytesInternal(const char * bytes, size_t numBytes)
	{
		if (!ownsData)
		{
			throw SUPER_EXCEPTION("Data not owned in SuperBitStream::WriteBytesInternal()");
		}
		if (bytes == NULL)
		{
			throw SUPER_EXCEPTION("Passed in bytes is NULL to SuperBitStream::WriteBytesInternal()");
		}

		AddBytes(numBytes);
		memcpy(data + currentPos, bytes, numBytes);
		currentPos += numBytes;
	}


	void SuperBitStream::WriteBytesInternal(size_t bytes, size_t numBytes)
	{
		if (!ownsData)
		{
			throw SUPER_EXCEPTION("Data not owned in SuperBitStream::WriteBytesInternal()");
		}
		if (numBytes > 4)
		{
			throw SUPER_EXCEPTION("Attempt to write more than 4 bytes in SuperBitStream::WriteBytesInternal(unsigned int, unsigned int)");
		}

		AddBytes(numBytes);
		memcpy(data + currentPos, &bytes, numBytes);
		currentPos += numBytes;
	}


	void SuperBitStream::ReadBytesInternal(size_t numBytes, char ** readData)
	{
		if (readData == NULL)
		{
			throw SUPER_EXCEPTION("NULL data pointer passed to SuperBitStream::ReadBytesInternal()");
		}
		if (currentPos + numBytes > dataSize)
		{
			throw SUPER_EXCEPTION("Data overread in SuperBitStream::ReadBytesInternal()");
		}

		*readData = data + currentPos;
		currentPos += numBytes;
	}


	void SuperBitStream::ReadBytesInternal(size_t numBytes, size_t & readData)
	{
		if (numBytes > 4)
		{
			throw SUPER_EXCEPTION("Attempt to read more than 4 bytes in SuperBitStream::ReadBytesInternal(unsigned int, unsigned int &)");
		}
		if (currentPos + numBytes > dataSize)
		{
			throw SUPER_EXCEPTION("Data overread in SuperBitStream::ReadBytesInternal()");
		}

		size_t shiftTimes = 0;
		size_t tempPos = currentPos;
		size_t tempNumBytes = numBytes;
		tempPos += tempNumBytes - 1;
		while (tempNumBytes)
		{
			readData = readData << (shiftTimes * 8);
			readData += data[tempPos];
			tempPos--;
			tempNumBytes--;
			shiftTimes++;
		}
		currentPos += numBytes;
	}


	void SuperBitStream::AddBytes(size_t numberOfBytesToAdd)
	{
		//only allocate if we own this data and there are bytes to allocate
		if (!ownsData || numberOfBytesToAdd <= 0)
		{
			return;
		}

		size_t newNumberOfBytesAllocated = dataSize + numberOfBytesToAdd;

		//Only allocate new memory if we need to
		if (newNumberOfBytesAllocated > numberOfBytesAllocated)
		{
			//Add the ALLOCATION_BLOCK_SIZE to save on having to get new memory all the time
			newNumberOfBytesAllocated += ALLOCATION_BLOCK_SIZE;

			if (data == NULL)
			{
				
				data = reinterpret_cast<char *>(malloc(newNumberOfBytesAllocated));
			}
			else
			{
				data = reinterpret_cast<char *>(realloc(data, newNumberOfBytesAllocated));
			}
		}

		dataSize += numberOfBytesToAdd;
		if (newNumberOfBytesAllocated > numberOfBytesAllocated)
		{
			numberOfBytesAllocated = newNumberOfBytesAllocated;
		}
	}
}