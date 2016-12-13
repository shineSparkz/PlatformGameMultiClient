#ifndef __EVENTID_H__
#define __EVENTID_H__

#include "types.h"

enum EventID
{
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

	// Network
	Net_NewGameObject,
	Net_UpdateGameObject,

	NumGameEvents
};

#endif