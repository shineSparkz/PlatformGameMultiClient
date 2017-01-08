#include "EventManager.h"

EventManager::EventManager()
{
}

EventManager::~EventManager()
{
	for (EventMapIter iter = mEventMap.begin(); iter != mEventMap.end(); ++iter)
	{
		SAFE_DELETE(iter->second);
	}

	mEventMap.clear();
}

void EventManager::SendEvent(EventID eventId, void* pData)
{
	EventMapIter result = mEventMap.find(eventId);
	if (result != mEventMap.end())
	{
		if (result->second)
		{
			result->second->Send(pData);
		}
	}
}

void EventManager::SendEventToHandler(EventID eventId, EventHandler& eventHandler, void* pData)
{
	EventMapIter result = mEventMap.find(eventId);
	if (result != mEventMap.end())
	{
		if (result->second)
		{
			result->second->SendToHandler(eventHandler, pData);
		}
	}
}

bool EventManager::RegisterEvent(EventID eventId)
{
	bool added = false;

	EventMapIter result = mEventMap.find(eventId);
	if (result == mEventMap.end())
	{
		Event* pNewEvent = new Event(eventId);

		if (pNewEvent)
		{
			std::pair<EventID, Event*> newEvent(eventId, pNewEvent);
			std::pair<EventMapIter, bool> addedIter = mEventMap.insert(newEvent);
			added = addedIter.second;
		}
	}

	assert(added);
	return added;
}

void EventManager::AttachEvent(EventID eventId, EventHandler& eventHandler)
{
	EventMapIter result = mEventMap.find(eventId);
	if (result != mEventMap.end())
	{
		result->second->AttachListener(eventHandler);
	}
}

void EventManager::DetachEvent(EventID eventId, EventHandler& eventHandler)
{
	EventMapIter result = mEventMap.find(eventId);
	if (result != mEventMap.end())
	{
		result->second->DetachListener(eventHandler);
	}
}
