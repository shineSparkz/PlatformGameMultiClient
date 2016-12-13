#ifndef __EVENTMANAGER_H__
#define __EVENTMANAGER_H__

#include <unordered_map>

#include "Singleton.h"
#include "EventId.h"
#include "Event.h"
#include "types.h"

class EventManager : public Singleton<EventManager>
{
	friend void SendEvent(EventID evID, void* data);
	friend void SendEventToHandler(EventID eventId, EventHandler& eventHandler, void* pData);
	friend bool RegisterEvent(EventID eventId);
	friend void AttachEvent(EventID eventId, EventHandler& eventHandler);
	friend void DetachEvent(EventID eventId, EventHandler& eventHandler);

private:
	typedef std::unordered_map<EventID, Event*> EventMap;
	typedef EventMap::iterator EventMapIter;

	EventMap mEventMap;

	void SendEvent(EventID eventId, void* pData);
	void SendEventToHandler(EventID eventId, EventHandler& eventHandler, void* pData);
	bool RegisterEvent(EventID eventId);
	void AttachEvent(EventID eventId, EventHandler& eventHandler);
	void DetachEvent(EventID eventId, EventHandler& eventHandler);

public:
	EventManager();
	~EventManager();
};

INLINE void SendEvent(EventID eventId, void* pData = NULL)
{
	EventManager* pEventManager = EventManager::Instance();
	assert(pEventManager);
	if (pEventManager)
	{
		pEventManager->SendEvent(eventId, pData);
	}
}

INLINE void SendEventToHandler(EventID eventId, EventHandler& eventHandler, void* pData = NULL)
{
	EventManager* pEventManager = EventManager::Instance();
	assert(pEventManager);
	if (pEventManager)
	{
		pEventManager->SendEventToHandler(eventId, eventHandler, pData);
	}
}

INLINE bool RegisterEvent(EventID eventId)
{
	EventManager* pEventManager = EventManager::Instance();
	assert(pEventManager);
	if (pEventManager)
	{
		return pEventManager->RegisterEvent(eventId);
	}

	return false;
}

INLINE void AttachEvent(EventID eventId, EventHandler& eventHandler)
{
	EventManager* pEventManager = EventManager::Instance();
	assert(pEventManager);
	if (pEventManager)
	{
		pEventManager->AttachEvent(eventId, eventHandler);
	}
}

INLINE void DetachEvent(EventID eventId, EventHandler& eventHandler)
{
	EventManager* pEventManager = EventManager::Instance();
	assert(pEventManager);
	if (pEventManager)
	{
		pEventManager->DetachEvent(eventId, eventHandler);
	}
}

#endif