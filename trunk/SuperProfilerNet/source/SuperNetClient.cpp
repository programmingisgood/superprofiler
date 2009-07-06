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

#include "SuperNetClient.h"
#include "SuperNetClientListener.h"
#include "SuperBitStream.h"
#include "SuperNetPacketIDs.h"
#include "SuperFunctionData.h"
#include "SuperException.h"

namespace SuperProfiler
{
	SuperNetClient::SuperNetClient() : SuperNetHost(0, false, 1), listener(NULL), serverStackRoot(NULL, NULL),
													 currState(DISCONNECTED), connectionTimeoutTime(0.5)
	{
	}


	SuperNetClient::~SuperNetClient()
	{
		Reset();
	}


	void SuperNetClient::SetListener(SuperNetClientListener * setListener)
	{
		listener = setListener;
	}


	SuperNetClientListener * SuperNetClient::GetListener(void) const
	{
		return listener;
	}


	bool SuperNetClient::Connect(const std::string & serverAddress)
	{
		if (currState == DISCONNECTED)
		{
			if (RequestConnect(serverAddress))
			{
				SetState(CONNECTING);
				return true;
			}
		}
		return false;
	}


	void SuperNetClient::Disconnect(void)
	{
		if (currState != DISCONNECTED)
		{
			RequestDisconnect();
			SetState(DISCONNECTING);
		}
	}


	void SuperNetClient::DisconnectNow(void)
	{
		ForceDisconnect();
		SetState(DISCONNECTED);
	}


	SuperNetClient::STATE SuperNetClient::GetState(void) const
	{
		return currState;
	}


	std::string SuperNetClient::GetStateString(void) const
	{
		return StateToString(GetState());
	}


	std::string SuperNetClient::StateToString(STATE convertState)
	{
		switch (convertState)
		{
			case DISCONNECTED:
			{
				return "DISCONNECTED";
			}
			break;

			case CONNECTING:
			{
				return "CONNECTING";
			}
			break;

			case INITIAL_SYNC:
			{
				return "INITIAL_SYNC";
			}
			break;

			case CONNECTED:
			{
				return "CONNECTED";
			}
			break;

			case DISCONNECTING:
			{
				return "DISCONNECTING";
			}
			break;

			default:
			{
				return "INVALID_STATE";
			}
		}
	}


	const SuperFuncDataList & SuperNetClient::GetFuncList(void)
	{
		return superFuncDataListWrapper.superFuncDataList;
	}


	SuperNetIterator SuperNetClient::GetIterator(void)
	{
		return SuperNetIterator(&serverStackRoot);
	}


	void SuperNetClient::Reset(void)
	{
		serverStackRoot.Reset();
		superFuncDataListWrapper.Reset();
		funcNames.clear();
	}


	void SuperNetClient::SetState(STATE newState)
	{
		if (listener)
		{
			listener->StateChanged(StateToString(currState), StateToString(newState));
		}
		stateTimer.Reset();
		currState = newState;
	}


	bool SuperNetClient::ConnectedToPeer(SuperNetPeer & connectedToPeer)
	{
		//Successfully connected to the server!
		if (currState == CONNECTING)
		{
			//Now we are waiting for the initial sync data from the server
			SetState(INITIAL_SYNC);
			return true;
		}
		else
		{
			//The client was connected to the server while not in the connecting state, this should happen
			//if it does, just ignore it
			return false;
		}
	}


	void SuperNetClient::DisconnectedFromPeer(SuperNetPeer & disconnectedFromPeer)
	{
		//Disconnected from the server :(
		SetState(DISCONNECTED);
		Reset();
	}


	bool SuperNetClient::PeerConnected(SuperNetPeer & connectedPeer)
	{
		//The client does not accept incoming connections, this shouldn't happen
		return false;
	}


	void SuperNetClient::PeerDisconnected(SuperNetPeer & disconnectedPeer)
	{
		//Because the client does not accept incoming connections, this shouldn't happen either
	}


	void SuperNetClient::ReceivePacket(SuperBitStream & bitStream, SuperNetPeer & fromPeer)
	{
		char packetID = 0;
		bitStream.ReadChar(packetID);
		switch (packetID)
		{
			case PID_INITIAL_SYNC:
			{
				if (GetState() != INITIAL_SYNC)
				{
					throw SUPER_EXCEPTION("Received a PID_INITIAL_SYNC packet while not in the INITIAL_SYNC state");
				}
				ReceiveInitialSync(bitStream);
				SetState(CONNECTED);
			}
			break;

			case PID_ADD_FUNCTION:
			{
				if (GetState() != CONNECTED)
				{
					throw SUPER_EXCEPTION("Received a PID_ADD_FUNCTION packet while not in the CONNECTED state");
				}
				ReceiveAddFunction(bitStream);
			}
			break;

			case PID_FUNC_LIST_UPDATE:
			{
				if (GetState() != CONNECTED)
				{
					throw SUPER_EXCEPTION("Received a PID_FUNC_LIST_UPDATE packet while not in the CONNECTED state");
				}
				ReceiveFuncListUpdate(bitStream);
			}
			break;

			case PID_CALL_TREE_UPDATE:
			{
				if (GetState() != CONNECTED)
				{
					throw SUPER_EXCEPTION("Received a PID_CALL_TREE_UPDATE packet while not in the CONNECTED state");
				}
				ReceiveCallTreeUpdate(bitStream);
			}
			break;
		}
	}


	void SuperNetClient::ReceiveInitialSync(SuperBitStream & bitStream)
	{
		//Start off fresh
		serverStackRoot.Reset();
		UnPackageFunctions(bitStream);
		UnPackageNode(serverStackRoot, bitStream);
	}


	void SuperNetClient::UnPackageFunctions(SuperBitStream & bitStream)
	{
		size_t numFunctions = 0;
		bitStream.ReadInt(numFunctions);
		for (size_t i = 0; i < numFunctions; i++)
		{
			size_t funcID = 0;
			bitStream.ReadInt(funcID);
			std::string funcName;
			bitStream.ReadString(funcName);
			float funcTotalTime = 0;
			bitStream.ReadFloat(funcTotalTime);
			size_t funcTotalNumTimesCalled = 0;
			bitStream.ReadInt(funcTotalNumTimesCalled);

			funcNames.push_back(funcName);
			SuperFunctionData * newFunc = new SuperFunctionData(funcID, funcNames.back().c_str());
			newFunc->SetTotalTime(funcTotalTime);
			newFunc->SetTotalNumTimesCalled(funcTotalNumTimesCalled);
			AddNewFuncData(newFunc);
		}
	}


	void SuperNetClient::UnPackageNode(SuperStackNode & node, SuperBitStream & bitStream)
	{
		size_t currentParentID;
		bitStream.ReadInt(currentParentID);
		float currentParentTotalTime = 0;
		bitStream.ReadFloat(currentParentTotalTime);
		size_t currentParentTotalCalls = 0;
		bitStream.ReadInt(currentParentTotalCalls);

		SuperFunctionData * foundFunc = FindFuncData(currentParentID);
		if (currentParentID != 0 && !foundFunc)
		{
			throw SUPER_EXCEPTION("Function not found");
		}
		node.SetFuncData(foundFunc);
		node.SetTotalTime(currentParentTotalTime);
		node.SetNumTimesCalled(currentParentTotalCalls);

		size_t numChildren = 0;
		bitStream.ReadInt(numChildren);
		for (size_t i = 0; i < numChildren; i++)
		{
			//Check if a node exists at this index
			SuperStackNode * childNode = node.GetChild(i);
			if (!childNode)
			{
				childNode = new SuperStackNode(&node, NULL);
				node.AddChild(childNode);
			}

			UnPackageNode(*childNode, bitStream);
		}
	}


	void SuperNetClient::ReceiveAddFunction(SuperBitStream & bitStream)
	{
		size_t funcID = 0;
		bitStream.ReadInt(funcID);
		std::string funcName;
		bitStream.ReadString(funcName);

		if (!FindFuncData(funcID))
		{
			funcNames.push_back(funcName);
			SuperFunctionData * newFunc = new SuperFunctionData(funcID, funcNames.back().c_str());
			newFunc->SetTotalTime(0);
			newFunc->SetTotalNumTimesCalled(0);
			AddNewFuncData(newFunc);
		}
		else
		{
			throw SUPER_EXCEPTION("Function that matches sent ID already exists");
		}
	}


	void SuperNetClient::ReceiveFuncListUpdate(SuperBitStream & bitStream)
	{
		size_t numFunctions = 0;
		bitStream.ReadInt(numFunctions);
		for (size_t i = 0; i < numFunctions; i++)
		{
			size_t funcID = 0;
			bitStream.ReadInt(funcID);
			float funcTotalTime = 0;
			bitStream.ReadFloat(funcTotalTime);
			size_t funcTotalNumTimesCalled = 0;
			bitStream.ReadInt(funcTotalNumTimesCalled);

			SuperFunctionData * funcData = FindFuncData(funcID);
			if (!funcData)
			{
				throw SUPER_EXCEPTION("Invalid function ID received");
			}
			funcData->SetTotalTime(funcTotalTime);
			funcData->SetTotalNumTimesCalled(funcTotalNumTimesCalled);
		}
		if (listener)
		{
			listener->FunctionListUpdated();
		}
	}


	void SuperNetClient::ReceiveCallTreeUpdate(SuperBitStream & bitStream)
	{
		UnPackageNode(serverStackRoot, bitStream);
		if (listener)
		{
			listener->CallTreeUpdated();
		}
	}


	SuperFunctionData * SuperNetClient::FindFuncData(size_t findID)
	{
		SuperFuncDataList::iterator iter;
		for (iter = superFuncDataListWrapper.superFuncDataList.begin(); iter != superFuncDataListWrapper.superFuncDataList.end(); iter++)
		{
			if ((*iter)->GetID() == findID)
			{
				return (*iter);
			}
		}
		return NULL;
	}


	void SuperNetClient::AddNewFuncData(SuperFunctionData * newFuncData)
	{
		if (!newFuncData)
		{
			throw SUPER_EXCEPTION("NULL SuperFunctionData passed in");
		}
		if (FindFuncData(newFuncData->GetID()))
		{
			//Already exists
			throw SUPER_EXCEPTION("Function with name " + std::string(newFuncData->GetName()) + " already exists in this SuperNetClient");
		}

		superFuncDataListWrapper.superFuncDataList.push_back(newFuncData);

		if (listener)
		{
			listener->FunctionAdded(newFuncData->GetID(), newFuncData->GetName());
		}
	}


	void SuperNetClient::ProcessImp(void)
	{
		switch (currState)
		{
			case CONNECTING:
			case INITIAL_SYNC:
			case DISCONNECTING:
			{
				if (stateTimer.GetTimeSeconds() >= connectionTimeoutTime)
				{
					DisconnectNow();
				}
			}
			break;
		}
	}
}