#include "Event.h"

#include <cassert>


Event::Event(EventID evID):
	mID(evID),
	mData(nullptr)
{
}

Event::~Event()
{
	mListeners.clear();
}

void Event::Send(void* data)
{
	mData = data;

	for (EventHandlerListIter i = mListeners.begin(); i != mListeners.end(); ++i)
	{
		EventHandler* eventHandler = *i;
		assert(eventHandler);
		if (eventHandler)
		{
			eventHandler->HandleEvent(this);
		}
	}
}

void Event::SendToHandler(EventHandler& evhandler, void* pData)
{
	mData = pData;
	for (EventHandlerListIter i = mListeners.begin(); i != mListeners.end(); ++i)
	{
		if (&evhandler == *i)
		{
			evhandler.HandleEvent(this);
		}
	}
}

void Event::AttachListener(EventHandler& evHandler)
{
	mListeners.push_back(&evHandler);
}

void Event::DetachListener(EventHandler& evHandler)
{
	for (EventHandlerListIter i = mListeners.begin(); i != mListeners.end(); ++i)
	{
		if (&evHandler == *i)
		{
			mListeners.erase(i);
			break;
		}
	}
}

