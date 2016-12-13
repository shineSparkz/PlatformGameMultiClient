#ifndef __EVENTHANDLER_H__
#define __EVENTHANDLER_H__

class Event;

class EventHandler
{
public:
	virtual ~EventHandler(){}

	virtual void HandleEvent(Event* event_) = 0;
};


#endif