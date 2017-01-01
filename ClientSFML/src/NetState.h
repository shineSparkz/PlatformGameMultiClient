#ifndef __NET_STATE_H__
#define __NET_STATE_H__

#include "types.h"

struct NetState
{
	int object_handle;
	Vec2 position;
	float frameX = 0;
	float frameY = 0;
};

#endif
