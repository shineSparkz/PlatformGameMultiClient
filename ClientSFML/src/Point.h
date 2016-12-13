#ifndef __POINT_H__
#define __POINT_H__

#include "types.h"

struct point
{
	int32 x, y;

	point() : x(0), y(0){}
	point(int32 xx, int32 yy) : x(xx), y(yy){}
};


#endif