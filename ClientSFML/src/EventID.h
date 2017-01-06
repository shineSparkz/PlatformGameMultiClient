#ifndef __EVENTID_H__
#define __EVENTID_H__

#include "types.h"

enum EventID
{
	/*
	// Gameplay
	FinishLevel,
	PlayerDead,
	SetLevelToLoad,
	BossTriggered,
	PlayerCollidingWithExit,
	CheckpointActivate,
	CollectGoldSkull,
	LoadGameStartup,
	GainExperience,
	SaveGameData,
	LoadGameData,
	LoadGameDefaults,
	*/

	LoadLevelData,

	// Network accounts
	Net_StartGameCallback,
	Net_ServerMsgCallback,
	Net_DatabaseRequest,

	// Network
	Net_NewGameObject,
	Net_UpdateGameObject,

	NumGameEvents
};

#endif