#ifndef __EVENT_H__
#define __EVENT_H__

#include "EventHandler.h"
#include "EventId.h"

class Event
{
public:
	explicit Event(EventID evID);
	~Event();

	void Send(void* data);
	void SendToHandler(EventHandler& evhandler, void* pData);
	void AttachListener(EventHandler& evHandler);
	void DetachListener(EventHandler& evHandler);

	EventID GetID() const { return mID; }
	void* GetData() { return mData; }

private:
	typedef std::vector<EventHandler*> EventHandlerList;
	typedef EventHandlerList::iterator EventHandlerListIter;

	EventHandlerList mListeners;
	EventID mID;
	void* mData;
};


#endif

