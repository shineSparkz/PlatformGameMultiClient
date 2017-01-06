#ifndef __NET_PACKETS_H__
#define __NET_PACKETS_H__

namespace Packet
{
	enum ID
	{
		// TCP in
		IN_TCP_Connect,
		IN_TCP_ServerMsg,
		IN_TCP_StartGame,
		IN_TCP_FinishLevel,
		IN_TCP_ExpQueery,
		IN_TCP_LeaderboardRequest,
		
		// UDP in
		IN_UDP_UpdatedObject,
		IN_UDP_ViewUpdate,

		// TCP out
		OUT_TCP_CreateAccount,
		OUT_TCP_Login,
		OUT_TCP_StartGame,
		OUT_TCP_ExpQueery,
		OUT_TCP_LeaderboardRequest,

		// UDP out
		OUT_UDP_Input
	};
}

#endif