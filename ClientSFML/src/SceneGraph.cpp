#include "SceneGraph.h"
#include "LogFile.h"

#include "Application.h"
#include "Screen.h"
#include "utils.h"

std::string SceneGraph::ConvertState(ID::States state)
{
	switch ( state )
	{
	case ID::States::Game:
		return "Game";
	case ID::States::WorldMap:
		return "WorldMap";
	case ID::States::None:
		return "None";
	case ID::States::Pause:
		return "Pause";
	case ID::States::Title:
		return "Title";
	default:
		return "Undefined";
	}
}

SceneGraph::SceneGraph(IScene::Context* context) :
	m_context(context),
	m_states(),
	m_current_state_name(""),
	m_current_state(ID::States::None),
	m_prev_state(ID::States::None),
	m_switching_state(false),
	m_state_switch_timer(0.1f)
{
}

SceneGraph::~SceneGraph()
{
}

bool SceneGraph::Init()
{
	if ( m_states.empty() )
	{
		WRITE_LOG("WARNING::Tried to init state stack, without registering any states", "error" );
		return false;
	}

	std::map<ID::States, IScene*>::iterator it;
	for ( it = m_states.begin(); it != m_states.end(); ++it )
	{
		if ( !it->second->OnCreate(m_context) )
		{
			std::string m = "State stack failed to create state: " + this->ConvertState(it->first);
			WRITE_LOG(m, "error");
			return false;
		}
	}

	return true;
}

void SceneGraph::SetState(ID::States stateName, bool forceNewStateFromPaused)
{
	// Check we have this state in the map
	if ( m_states.find(stateName) == m_states.end() )
	{
		std::string er = "StateStack could not set state '" + util::to_str((int)stateName) + "' because it doesn't exist.";
		WRITE_LOG(er, "error");
		return;
	}

	// For first time
	if ( m_current_state == ID::States::None )
	{
		m_current_state = stateName;
		m_prev_state = stateName;
		m_states[stateName]->OnEntry();
	}
	else
	{
		if ( m_switching_state == false && !Screen::Instance()->IsFading() )
		{
			m_states[m_current_state]->OnExit(stateName);

			// We get to pause state from any state, it comes from Application class, and we need to know what to switch too
			if ( m_current_state != ID::States::Pause )
			{
				m_prev_state = m_current_state;
				m_current_state = stateName;
			}
			else
			{
				if ( forceNewStateFromPaused )
					m_current_state = stateName;
				else
					m_current_state = m_prev_state;

				m_prev_state = ID::States::Pause;
			}


			//---------------------------------------------------
			if ( m_current_state == ID::States::Game)
				m_context->window->setMouseCursorVisible(false);
			else
				m_context->window->setMouseCursorVisible(true);

			if(stateName != ID::States::Game)
				Screen::Instance()->StartFading();

			m_current_state_name = ConvertState(m_current_state);
			m_switching_state = true;
			m_state_switch_timer = 0.1f;
		}
	}
}

void SceneGraph::UpdateCurrentState(const sf::Time&  dt)
{
	if ( !m_switching_state )
	{
		m_states[m_current_state]->OnUpdate(dt);
	}
	else
	{
		m_state_switch_timer -= dt.asSeconds();

		if ( m_state_switch_timer <= 0.0f )
		{
			m_states[m_current_state]->OnEntry();
			m_switching_state = false;
		}
	}
}

void SceneGraph::RenderCurrentState(float dt)
{
	if ( !m_switching_state )
	{
		m_states[m_current_state]->OnRender();
	}
}

void SceneGraph::RenderCurrentLateState()
{
	if ( !m_switching_state )
	{
		m_states[m_current_state]->LateRender();
	}
}

void SceneGraph::HandleInput(int k, int a)
{
	if ( !m_switching_state )
	{
		m_states[m_current_state]->HandleInput(k, a);
	}
}

void SceneGraph::Close()
{
	if ( m_context )
		delete m_context;
	m_context = nullptr;

	if ( !m_states.empty() )
	{
		std::map< ID::States, IScene*>::iterator it;

		for ( it = m_states.begin(); it != m_states.end(); ++it )
		{
			SAFE_CLOSE(it->second);
		}

		m_states.clear();
	}

	WRITE_LOG("State Manager shut down correctly", "good");
}