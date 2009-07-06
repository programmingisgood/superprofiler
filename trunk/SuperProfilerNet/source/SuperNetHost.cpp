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

//Always include enet first
#define WIN32_LEAN_AND_MEAN
#include "enet/enet.h"
#include "SuperNetHost.h"
#include "SuperException.h"
#include "SuperBitStream.h"

namespace SuperProfiler
{
	SuperNetHost::SuperNetHost(size_t setPort, bool setAllowInboundConnections, size_t setMaxNumConnections) :
							   port(setPort),
							   allowInboundConnections(setAllowInboundConnections),
							   maxNumConnections(setMaxNumConnections)
	{
		//Should this host allow other hosts to connect to it?
		if (allowInboundConnections)
		{
			ENetAddress address;
			address.host = ENET_HOST_ANY;
			address.port = static_cast<enet_uint16>(port);
			host = enet_host_create(&address, static_cast<enet_uint32>(maxNumConnections), 0, 0);
		}
		else
		{
			host = enet_host_create(NULL, static_cast<enet_uint32>(maxNumConnections), 0, 0);
		}
	}


	SuperNetHost::~SuperNetHost()
	{
		ForceDisconnect();

		if (host)
		{
			enet_host_destroy(host);
			host = NULL;
		}
	}


	size_t SuperNetHost::GetPort(void) const
	{
		return port;
	}


	void SuperNetHost::Process(void)
	{
		ENetEvent enetEvent;
		int serviceRet = enet_host_service(host, &enetEvent, 0);
		if (serviceRet > 0)
		{
			switch (enetEvent.type)
			{
				case ENET_EVENT_TYPE_NONE:
				{
				}
				break;

				case ENET_EVENT_TYPE_CONNECT:
				{
					//If the peer is in the connected to list, then this host
					//has successfully connected to the remote peer
					SuperNetPeer connectPeer;
					if (GetPeerInList(enetEvent.peer, hostConnectedToPeers, connectPeer))
					{
						if (!ConnectedToPeer(connectPeer))
						{
							RequestDisconnect(connectPeer);
						}
					}
					else
					{
						if (allowInboundConnections)
						{
							//If the peer is not in the peersConnectedToHost list, then
							//the remote peer successfully connected to this host
							if (!GetPeerInList(enetEvent.peer, peersConnectedToHost, connectPeer))
							{
								connectPeer = SuperNetPeer(enetEvent.peer, std::string());
								peersConnectedToHost.push_back(connectPeer);
								if (!PeerConnected(connectPeer))
								{
									DisconnectPeer(connectPeer);
								}
							}
							//If it is in the peersConnectedToHost then something weird happened
							else
							{
								throw SUPER_EXCEPTION("An already connected peer somehow connected again");
							}
						}
						else
						{
							DisconnectPeer(connectPeer);
						}
					}
				}
				break;

				case ENET_EVENT_TYPE_DISCONNECT:
				{
					//If the peer is in the hostConnectedToPeers list, then this host
					//was disconnected from the remote peer
					SuperNetPeer connectPeer;
					if (GetPeerInList(enetEvent.peer, hostConnectedToPeers, connectPeer))
					{
						DisconnectedFromPeer(connectPeer);
						RemoveFromList(hostConnectedToPeers, connectPeer);
					}
					//If the peer is in the peersConnectedToHost list, then
					//the remote peer has been disconnected from this host
					if (GetPeerInList(enetEvent.peer, peersConnectedToHost, connectPeer))
					{
						PeerDisconnected(connectPeer);
						RemoveFromList(peersConnectedToHost, connectPeer);
					}
				}
				break;

				case ENET_EVENT_TYPE_RECEIVE:
				{
					HandleReceivePacket(enetEvent);
				}
				break;
			}
		}
		else if (serviceRet < 0)
		{
			throw SUPER_EXCEPTION("Something went wrong in SuperNetHost::Process()");
		}

		ProcessImp();
	}


	void SuperNetHost::HandleReceivePacket(ENetEvent & receiveInfo)
	{
		if (receiveInfo.packet && receiveInfo.packet->data && receiveInfo.packet->dataLength > 0)
		{
			SuperBitStream stream;
			//This stream is just borrowing the packet data
			stream.SetData(reinterpret_cast<char *>(receiveInfo.packet->data), receiveInfo.packet->dataLength, false);
			SuperNetPeer sentFromPeer;
			if (!GetPeerInList(receiveInfo.peer, hostConnectedToPeers, sentFromPeer))
			{
				GetPeerInList(receiveInfo.peer, peersConnectedToHost, sentFromPeer);
			}
			ReceivePacket(stream, sentFromPeer);
			//We no longer need the enet packet
			enet_packet_destroy(receiveInfo.packet);
		}
	}


	bool SuperNetHost::RequestConnect(const std::string & serverAddress)
	{
		std::string connectToServerAddress = serverAddress;
		bool useDifferentPort = false;
		size_t differentPort = 0;
		std::string::size_type portPos = connectToServerAddress.find(":");
		if (portPos != std::string::npos)
		{
			useDifferentPort = true;
			std::string port = connectToServerAddress.substr(portPos + 1);
			std::istringstream iss(port);
			if ((iss >> std::dec >> differentPort).fail())
			{
				throw SUPER_EXCEPTION("Failed to convert port");
			}
			connectToServerAddress = connectToServerAddress.substr(0, portPos);
		}

		ENetAddress address;
		if (enet_address_set_host(&address, connectToServerAddress.c_str()) < 0)
		{
			//Failure
			throw SUPER_EXCEPTION("Could not convert passed in address");
		}
		if (useDifferentPort)
		{
			address.port = static_cast<enet_uint16>(differentPort);
		}
		else
		{
			address.port = static_cast<enet_uint16>(GetPort());
		}

		ENetPeer * peer = enet_host_connect(host, &address, 2);
		if (!peer)
		{
			return false;
		}
		hostConnectedToPeers.push_back(SuperNetPeer(peer, serverAddress));
		return true;
	}


	void SuperNetHost::RequestDisconnect(SuperNetPeer & disconnectPeer)
	{
		PeerList::iterator iter;
		for (iter = hostConnectedToPeers.begin(); iter != hostConnectedToPeers.end(); iter++)
		{
			if ((*iter) == disconnectPeer)
			{
				DisconnectPeer((*iter));
				break;
			}
		}
	}


	void SuperNetHost::RequestDisconnect(void)
	{
		PeerList::iterator iter;
		for (iter = hostConnectedToPeers.begin(); iter != hostConnectedToPeers.end(); iter++)
		{
			DisconnectPeer((*iter));
		}
	}


	void SuperNetHost::ForceDisconnect(void)
	{
		DisconnectAllPeersNow(hostConnectedToPeers);
		DisconnectAllPeersNow(peersConnectedToHost);
	}


	void SuperNetHost::DisconnectPeer(SuperNetPeer & disconnectPeer)
	{
		enet_peer_disconnect(disconnectPeer.GetPeer(), 0);
	}


	void SuperNetHost::SendPacket(SuperBitStream & bitStream, SuperNetPeer & sendToPeer, bool reliable)
	{
		if (!host)
		{
			throw SUPER_EXCEPTION("No host before calling SuperNetHost::SendPacket()");
		}

		//We take over ownership of the data here
		bitStream.SetOwnsData(false);

		unsigned char channel = (reliable) ? 0 : 1;

		ENetPacket * packet = enet_packet_create(bitStream.GetData(), bitStream.GetDataSize(), ((reliable) ? ENET_PACKET_FLAG_RELIABLE | ENET_PACKET_FLAG_NO_ALLOCATE : ENET_PACKET_FLAG_NO_ALLOCATE));
		packet->freeCallback = SuperNetHost::FreePacket;

		if (sendToPeer.GetPeer())
		{
			if (enet_peer_send(sendToPeer.GetPeer(), channel, packet) < 0)
			{
				//If the packet failed to send, destroy the packet
				enet_packet_destroy(packet);
			}
		}
		//If there was no peer defined, send to all peers
		else
		{
			enet_host_broadcast(host, channel, packet);
		}

		//This function is expected to take control of the data and clear it
		bitStream.Clear();
	}


	void SuperNetHost::FreePacket(ENetPacket * freePacket)
	{
		free(freePacket->data);
	}


	void SuperNetHost::DisconnectAllPeersNow(PeerList & inList)
	{
		PeerList::iterator iter;
		for (iter = inList.begin(); iter != inList.end(); iter++)
		{
			enet_peer_disconnect_now(iter->GetPeer(), 0);
			enet_peer_reset(iter->GetPeer());
		}
		inList.clear();
	}


	bool SuperNetHost::GetPeerInList(ENetPeer * peer, PeerList & inList, SuperNetPeer & retPeer) const
	{
		PeerList::iterator iter;
		for (iter = inList.begin(); iter != inList.end(); iter++)
		{
			if (iter->GetPeer() == peer)
			{
				retPeer = (*iter);
				return true;
			}
		}
		return false;
	}


	void SuperNetHost::RemoveFromList(PeerList & inList, SuperNetPeer & removePeer)
	{
		PeerList::iterator iter;
		for (iter = inList.begin(); iter != inList.end(); iter++)
		{
			if ((*iter) == removePeer)
			{
				enet_peer_reset(iter->GetPeer());
				inList.erase(iter);
				break;
			}
		}
	}
}