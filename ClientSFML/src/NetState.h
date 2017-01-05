#ifndef __NET_STATE_H__
#define __NET_STATE_H__

#include "types.h"

struct NetState
{
	Vec2 position;
	int object_handle;
	float frameX = 0;
	float frameY = 0;
	bool active = true;
};

#endif
