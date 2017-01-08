#ifndef __EVENTID_H__
#define __EVENTID_H__

#include "types.h"

enum EventID
{
	LoadLevelData,

	// Network accounts
	Net_StartGameCallback,
	Net_ServerMsgCallback,
	Net_DatabaseRequest,

	// Network
	Net_NewGameObject,
	Net_UpdateGameObject,
	Net_PlayerHealth,

	NumGameEvents
};

#endif