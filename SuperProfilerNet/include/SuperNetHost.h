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

#ifndef SUPERNETHOST_H
#define SUPERNETHOST_H

#include "SuperNetPeer.h"
#include <list>

struct _ENetHost;
typedef _ENetHost ENetHost;
struct _ENetPacket;
typedef _ENetPacket ENetPacket;
struct _ENetEvent;
typedef _ENetEvent ENetEvent;

namespace SuperProfiler
{
	class SuperBitStream;

	class SuperNetHost
	{
	public:
		SuperNetHost(size_t setPort, bool setAllowInboundConnections, size_t setMaxNumConnections);
		virtual ~SuperNetHost() = 0;

		size_t GetPort(void) const;

		void Process(void);

	protected:
		/**
		* Returns false on failure, true if the connection has started
		**/
		bool RequestConnect(const std::string & serverAddress);
		/**
		* Disconnect from any peers that this host is currently connected to
		**/
		void RequestDisconnect(void);
		/**
		* Disconnect this host from the passed in peer
		**/
		void RequestDisconnect(SuperNetPeer & disconnectPeer);
		/**
		* Forcefully disconnect this host from all peers it is connected to
		**/
		void ForceDisconnect(void);

		/**
		* Force a remote peer to disconnect from this host
		**/
		void DisconnectPeer(SuperNetPeer & disconnectPeer);

		/**
		* This host connected to a remote peer
		* Return false to disconnect from the peer
		**/
		virtual bool ConnectedToPeer(SuperNetPeer & connectedToPeer) = 0;
		/**
		* This host was disconnected from a remote peer
		**/
		virtual void DisconnectedFromPeer(SuperNetPeer & disconnectedFromPeer) = 0;

		/**
		* A remote peer has connected to this host
		* Return false to disconnect them
		**/
		virtual bool PeerConnected(SuperNetPeer & connectedPeer) = 0;
		/**
		* A remote peer has
		**/
		virtual void PeerDisconnected(SuperNetPeer & disconnectedPeer) = 0;

		/**
		* Call SendPacket to send a packet of data out to the passed peer
		* Pass in true to reliable if the data MUST get to the peer
		**/
		void SendPacket(SuperBitStream & bitStream, SuperNetPeer & sendToPeer, bool reliable);
		/**
		* A notification that a packet was received from the passed in peer
		**/
		virtual void ReceivePacket(SuperBitStream & bitStream, SuperNetPeer & fromPeer) = 0;

		virtual void ProcessImp(void) = 0;

	private:
		//No default construction!
		SuperNetHost();
		//No copies!
		SuperNetHost(const SuperNetHost &);
		const SuperNetHost & operator=(const SuperNetHost &);

		typedef std::list<SuperNetPeer> PeerList;

		bool GetPeerInList(ENetPeer * peer, PeerList & inList, SuperNetPeer & retPeer) const;
		void RemoveFromList(PeerList & inList, SuperNetPeer & removePeer);

		static void FreePacket(ENetPacket * freePacket);

		void HandleReceivePacket(ENetEvent & receiveInfo);

		/**
		* Disconnect from all the peers in the passed in list immediately
		* All peers will also be removed from the list
		**/
		void DisconnectAllPeersNow(PeerList & inList);

		size_t port;
		bool allowInboundConnections;
		size_t maxNumConnections;
		ENetHost * host;
		/**
		* These are the peers that this host is connected to
		**/
		PeerList hostConnectedToPeers;
		/**
		* These are the peers that have connected to this host
		**/
		PeerList peersConnectedToHost;
	};
}

#endif