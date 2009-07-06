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

#include "SuperNetServer.h"
#include "SuperRoot.h"
#include "SuperBitStream.h"
#include "SuperFunctionData.h"
#include "SuperNetPacketIDs.h"

namespace SuperProfiler
{
	SuperNetServer::SuperNetServer(size_t setPort) : SuperNetHost(setPort, true, 10), numClients(0),
									   updateTime(0.5)
	{
		SuperRoot::AddListener(this);
	}


	SuperNetServer::~SuperNetServer()
	{
		SuperRoot::RemoveListener(this);
	}


	size_t SuperNetServer::GetNumClients(void) const
	{
		return numClients;
	}


	bool SuperNetServer::ConnectedToPeer(SuperNetPeer & connectedToPeer)
	{
		//This shouldn't happen as the server does not connect to other peers
		return false;
	}


	void SuperNetServer::DisconnectedFromPeer(SuperNetPeer & disconnectedFromPeer)
	{
		//This should not happen either
	}


	bool SuperNetServer::PeerConnected(SuperNetPeer & connectedPeer)
	{
		//Another friendly client has connected, OH JOY!!!
		numClients++;
		//Send them the initial sync data
		SendInitialSync(connectedPeer);
		return true;
	}


	void SuperNetServer::PeerDisconnected(SuperNetPeer & disconnectedPeer)
	{
		//Shame, this peer left us, what ever did we do to deserve this fate?
		numClients--;
	}


	void SuperNetServer::ReceivePacket(SuperBitStream & bitStream, SuperNetPeer & fromPeer)
	{
	}


	void SuperNetServer::SendInitialSync(SuperNetPeer & sendToPeer)
	{
		const SuperFuncDataList & funcList = SuperRoot::GetFuncList();
		SuperIterator iter = SuperRoot::GetIterator();
		SuperBitStream bitStream;
		//First write the packet ID
		char packetID = PID_INITIAL_SYNC;
		bitStream.WriteChar(packetID);
		//Write the functions first
		PackageFunctions(funcList, bitStream, true);
		//Now write the call tree
		PackageNode(iter, bitStream);

		SendPacket(bitStream, sendToPeer, true);
	}


	void SuperNetServer::PackageFunctions(const SuperFuncDataList & funcList, SuperBitStream & bitStream, bool withNames)
	{
		bitStream.WriteInt(funcList.size());
		SuperFuncDataList::const_iterator iter;
		for (iter = funcList.begin(); iter != funcList.end(); iter++)
		{
			bitStream.WriteInt((*iter)->GetID());
			if (withNames)
			{
				bitStream.WriteString((*iter)->GetName());
			}
			bitStream.WriteFloat(static_cast<float>((*iter)->GetTotalTime()));
			bitStream.WriteInt((*iter)->GetTotalNumTimesCalled());
		}
	}


	void SuperNetServer::PackageNode(SuperIterator & iter, SuperBitStream & bitStream)
	{
		bitStream.WriteInt(iter.GetCurrentParentID());
		bitStream.WriteFloat(static_cast<float>(iter.GetCurrentParentTotalTime()));
		bitStream.WriteInt(iter.GetCurrentParentTotalCalls());

		size_t numChildren = iter.GetNumChildren();
		bitStream.WriteInt(numChildren);
		for (size_t i = 0; i < numChildren; i++)
		{
			iter.EnterChild(i);
			PackageNode(iter, bitStream);
			iter.EnterParent();
		}
	}


	void SuperNetServer::SendFuncListUpdate(void)
	{
		SuperBitStream bitStream;
		char packetID = PID_FUNC_LIST_UPDATE;
		bitStream.WriteChar(packetID);

		PackageFunctions(SuperRoot::GetFuncList(), bitStream, false);

		//Sent as unreliable because another update will follow shortly if this one doesn't make it
		SendPacket(bitStream, SuperNetPeer(), false);
	}


	void SuperNetServer::SendCallTreeUpdate(void)
	{
		SuperBitStream bitStream;
		char packetID = PID_CALL_TREE_UPDATE;
		bitStream.WriteChar(packetID);

		//Write the call tree
		PackageNode(SuperRoot::GetIterator(), bitStream);

		//Sent as unreliable because another update will follow shortly if this one doesn't make it
		SendPacket(bitStream, SuperNetPeer(), false);
	}


	void SuperNetServer::ProcessImp(void)
	{
		if (numClients > 0)
		{
			//Time for an update?
			if (updateTimer.GetTimeSeconds() > updateTime)
			{
				updateTimer.Reset();
				SendFuncListUpdate();
				SendCallTreeUpdate();
			}
		}
	}


	void SuperNetServer::FunctionAdded(size_t funcID, const std::string & funcName)
	{
		SuperBitStream bitStream;
		char packetID = PID_ADD_FUNCTION;
		bitStream.WriteChar(packetID);
		bitStream.WriteInt(funcID);
		bitStream.WriteString(funcName);

		SendPacket(bitStream, SuperNetPeer(), true);
	}
}