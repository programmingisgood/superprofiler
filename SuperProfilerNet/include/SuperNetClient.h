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

#ifndef SUPERNETCLIENT_H
#define SUPERNETCLIENT_H

#include "SuperNetHost.h"
#include "SuperStackNode.h"
#include "SuperNetIterator.h"
#include "SuperFuncDataList.h"
#include "SuperTimer.h"
#include <string>

namespace SuperProfiler
{
	class SuperNetClientListener;

	class SuperNetClient : public SuperNetHost
	{
	public:
		SuperNetClient();
		~SuperNetClient();

		enum STATE
		{
			DISCONNECTED,
			CONNECTING,
			INITIAL_SYNC,
			CONNECTED,
			DISCONNECTING
		};

		/**
		* Pass in NULL to remove the listener
		**/
		void SetListener(SuperNetClientListener * setListener);
		SuperNetClientListener * GetListener(void) const;

		/**
		* Returns false on failure, true if the connection has started
		**/
		bool Connect(const std::string & serverAddress);
		void Disconnect(void);
		/**
		* The server may not be notified of the disconnection
		**/
		void DisconnectNow(void);

		STATE GetState(void) const;
		std::string GetStateString(void) const;
		static std::string StateToString(STATE convertState);

		SuperFunctionData * FindFuncData(size_t findID);
		const SuperFuncDataList & GetFuncList(void);
		SuperNetIterator GetIterator(void);

	private:
		void Reset(void);
		void SetState(STATE newState);

		bool ConnectedToPeer(SuperNetPeer & connectedToPeer);
		void DisconnectedFromPeer(SuperNetPeer & disconnectedFromPeer);
		bool PeerConnected(SuperNetPeer & connectedPeer);
		void PeerDisconnected(SuperNetPeer & disconnectedPeer);

		void ReceivePacket(SuperBitStream & bitStream, SuperNetPeer & fromPeer);

		/**
		* PID_INITIAL_SYNC receiver
		**/
		void ReceiveInitialSync(SuperBitStream & bitStream);
		void UnPackageFunctions(SuperBitStream & bitStream);
		void UnPackageNode(SuperStackNode & node, SuperBitStream & bitStream);
		/**
		* PID_ADD_FUNCTION receiver
		**/
		void ReceiveAddFunction(SuperBitStream & bitStream);
		/**
		* PID_FUNC_LIST_UPDATE receiver
		**/
		void ReceiveFuncListUpdate(SuperBitStream & bitStream);
		/**
		* PID_CALL_TREE_UPDATE receiver
		**/
		void ReceiveCallTreeUpdate(SuperBitStream & bitStream);

		void AddNewFuncData(SuperFunctionData * newFuncData);

		void ProcessImp(void);

		SuperNetClientListener * listener;
		STATE currState;
		SuperTimer stateTimer;
		double connectionTimeoutTime;
		SuperFuncDataListWrapper superFuncDataListWrapper;
		SuperStackNode serverStackRoot;
		typedef std::list<std::string> FuncNameList;
		FuncNameList funcNames;
	};
}

#endif