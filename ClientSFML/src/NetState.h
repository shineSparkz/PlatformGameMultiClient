#ifndef __NET_STATE_H__
#define __NET_STATE_H__

#include "types.h"

// Wrapper for an object update sent over network
struct NetState
{
	Vec2 position;
	int object_handle;
	float frameX = 0;
	float frameY = 0;
	bool active = true;
};

#endif
