#ifndef __NET_PACKETS_H__
#define __NET_PACKETS_H__

namespace Packet
{
	enum ID
	{
		// TCP in
		IN_TCP_Register,
		IN_TCP_NewPlayerObject,
		
		// UDP in
		IN_UDP_UpdatedObject,

		// TCP out
		OUT_TCP_StartGame,

		// UDP out
		OUT_UDP_Input
	};
}

#endif