#ifndef __SCENEGRAPH_H__
#define __SCENEGRAPH_H__

#include "Singleton.h"
#include "IScene.h"

#include <SFML/System/NonCopyable.hpp>
#include <SFML/System/Time.hpp>

#include "types.h"
#include "identifiers.h"

namespace sf
{
	class Event;
	class RenderWindow;
}

class SceneGraph : public Singleton<SceneGraph>
{
	// Global public interface
	friend void ChangeState(ID::States id, bool forceNewStateFromPaused = false);
	friend ID::States GetActiveState();
	friend ID::States GetPrevState();
	friend bool StateShouldReload(ID::States id);

public:
	SceneGraph(IScene::Context* context);
	~SceneGraph();

	const static int StateReloadBitmask = (int)ID::States::Pause | (int)ID::States::Inventory;

	template <typename T>
	void RegisterState(ID::States stateName, T state);

	ID::States GetActiveState() const;
	ID::States GetPrevState() const;

	bool Init();
	void UpdateCurrentState(const sf::Time&  dt);
	void RenderCurrentState(float dt);
	void RenderCurrentLateState();
	void HandleInput(int k, int a);
	void Close();

private:
	void SetState(ID::States stateName, bool forceNewStateFromPaused = false);
	std::string ConvertState(ID::States state);

private:
	IScene::Context* m_context = nullptr;

	std::map<ID::States, IScene*> m_states;
	ID::States m_current_state;
	ID::States m_prev_state;
	std::string m_current_state_name;

	float m_state_switch_timer;
	bool m_switching_state;
};

template <typename T>
void SceneGraph::RegisterState(ID::States stateName, T state)
{
	IScene* pState = new T;
	m_states[stateName] = pState;
}

INLINE void ChangeState(ID::States id, bool forceNewStateFromPaused)
{
	SceneGraph* s = SceneGraph::Instance();
	if ( s )
	{
		s->SetState(id, forceNewStateFromPaused);
	}
}

INLINE bool StateShouldReload(ID::States id)
{
	return ((SceneGraph::StateReloadBitmask & (int)id) > 0);
}

INLINE ID::States GetActiveState()
{
	return SceneGraph::Instance()->GetActiveState();
}

INLINE ID::States GetPrevState()
{
	return SceneGraph::Instance()->GetPrevState();
}

INLINE ID::States SceneGraph::GetActiveState() const 
{
	return m_current_state; 
}

INLINE ID::States SceneGraph::GetPrevState() const 
{ 
	return m_prev_state; 
}

#endif

