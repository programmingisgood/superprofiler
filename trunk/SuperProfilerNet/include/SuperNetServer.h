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

#ifndef SUPERNETSERVER_H
#define SUPERNETSERVER_H

#include "SuperNetHost.h"
#include "SuperRootListener.h"
#include "SuperFuncDataList.h"
#include "SuperTimer.h"

namespace SuperProfiler
{
	class SuperIterator;
	class SuperBitStream;

	class SuperNetServer : public SuperNetHost, public SuperRootListener
	{
	public:
		SuperNetServer(size_t setPort);
		~SuperNetServer();

		/**
		* Returns the number of clients currently connected to this server
		**/
		size_t GetNumClients(void) const;

	private:
		//No default construction!
		SuperNetServer();

		bool ConnectedToPeer(SuperNetPeer & connectedToPeer);
		void DisconnectedFromPeer(SuperNetPeer & disconnectedFromPeer);
		bool PeerConnected(SuperNetPeer & connectedPeer);
		void PeerDisconnected(SuperNetPeer & disconnectedPeer);

		void ReceivePacket(SuperBitStream & bitStream, SuperNetPeer & fromPeer);

		/**
		* Send this peer an initial sync of the profiler data
		**/
		void SendInitialSync(SuperNetPeer & sendToPeer);
		void PackageFunctions(const SuperFuncDataList & funcList, SuperBitStream & bitStream, bool withNames);
		void PackageNode(SuperIterator & iter, SuperBitStream & bitStream);

		/**
		* Every updateTime seconds, the clients will be updated with the function list data
		**/
		void SendFuncListUpdate(void);

		/**
		* Every updateTime seconds, the clients will be updated with the call tree data
		**/
		void SendCallTreeUpdate(void);

		void ProcessImp(void);

		/**
		* SuperRootListener functions
		**/
		void FunctionAdded(size_t funcID, const std::string & funcName);

		size_t numClients;

		SuperTimer updateTimer;
		double updateTime;
	};
}

#endif