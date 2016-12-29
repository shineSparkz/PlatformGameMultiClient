#include "GameScene.h"
#include "SceneGraph.h"
#include "EventManager.h"

#include "GameObject.h"
#include "NetState.h"
#include "NetworkManager.h"
#include "Application.h"
#include "utils.h"

GameScene::GameScene() : 
	IScene()
{
}

GameScene::~GameScene()
{
}

void GameScene::HandleEvent(Event* event_)
{
	switch (event_->GetID())
	{
		case Net_NewGameObject:
		{
			GameObject* go = (GameObject*)event_->GetData();

			if (go)
			{
				GameObject* newGO = new GameObject();
				newGO->m_Sprite = go->m_Sprite;
				newGO->m_TypeId = go->m_TypeId;
				newGO->m_UniqueId = go->m_UniqueId;

				m_GameObjects.push_back(newGO);
			}
		}
		break;
		case Net_UpdateGameObject:
		{
			NetState* ns = (NetState*)event_->GetData();

			if (ns)
			{
				Vec2 statePos = ns->position;
				size_t obj_id = (size_t)ns->object_handle;

				Vec2 diff = statePos - m_GameObjects[obj_id]->m_Sprite.getPosition();	
				float dist = Maths::Length(diff);

				if (obj_id < m_GameObjects.size())
				{
					if (dist > 2.0f)
					{
						m_GameObjects[obj_id]->m_Sprite.setPosition(statePos);
					}
					else if (dist > 0.1f)
					{
						m_GameObjects[obj_id]->m_Sprite.setPosition(diff * 0.1f);
					}
				}
			}
		}
		break;
	}
}

bool GameScene::OnCreate(Context* const con)
{
	m_context = con;
	
	AttachEvent(EventID::Net_NewGameObject, *this);
	AttachEvent(EventID::Net_UpdateGameObject, *this);

	// TODO : Move this HARDCODED level construction

	for (int i = 1; i < 12; ++i)
	{
		// Create Object from parsed data
		GameObject* go = new GameObject();
		go->m_TypeId = (int)ID::Type::Wall;
		go->m_UniqueId = m_GameObjects.size();

		sf::Sprite spr;
		spr.setPosition(Vec2((float)i*64, (float)450));
		spr.setTexture(Application::Instance()->GetTexHolder().Get(ID::Texture::DestructableWall));
		go->m_Sprite = spr;

		m_GameObjects.push_back(go);
	}


	GameObject* go = new GameObject();
	go->m_TypeId = (int)ID::Type::Wall;
	go->m_UniqueId = m_GameObjects.size();
	sf::Sprite spr;
	spr.setPosition(Vec2(7 * 64, 350));
	spr.setTexture(Application::Instance()->GetTexHolder().Get(ID::Texture::DestructableWall));
	go->m_Sprite = spr;
	m_GameObjects.push_back(go);

	return true;
}

void GameScene::OnEntry()
{
}

void GameScene::OnExit(ID::States nextState)
{
	// This needs to be moved
	this->ClearGameObjects();
}

void GameScene::Close()
{
	DetachEvent(EventID::Net_NewGameObject, *this);
	DetachEvent(EventID::Net_UpdateGameObject, *this);
	this->ClearGameObjects();
}

void GameScene::OnRender()
{
	if (m_SceneReady)
	{
		for each (GameObject* go in m_GameObjects)
		{
			m_context->window->draw(go->m_Sprite);
		}
	}
}

void GameScene::LateRender()
{
}

float t = 0.0f;

bool GameScene::OnUpdate(const sf::Time& dt)
{
	// TODO : Remove this hack
	if (!m_SceneReady)
	{
		t += dt.asSeconds();

		if (t > 2.0f)
		{
			m_SceneReady = true;
		}
	}

	size_t playerId = (size_t)NetworkManager::Instance()->playerId();
	
	if (playerId < m_GameObjects.size() && NetworkManager::Instance()->connected())
	{
		// How to do velocity next
		// Quick Gravity hack (awful)
		//NetworkManager::Instance()->sendUdp(
		//	"input:" + std::to_string(sf::Keyboard::S) + ":" + std::to_string(NetworkManager::Instance()->clientId()));
	}

	return true;
}

void GameScene::HandleInput(int k, int a)
{
	if (NetworkManager::Instance()->connected())
	{
		NetworkManager::Instance()->sendUdp(
			"input:" + std::to_string(k) + ":" + std::to_string(NetworkManager::Instance()->clientId()));
		
		size_t playerId = (size_t)NetworkManager::Instance()->playerId();

		if (k == sf::Keyboard::D)
			m_GameObjects[playerId]->m_Sprite.move(3.0f, 0.0f);
		else if (k == sf::Keyboard::A)
			m_GameObjects[playerId]->m_Sprite.move(-3.0f, 0.0f);
		else if (k == sf::Keyboard::W)
			m_GameObjects[playerId]->m_Sprite.move(0.0f, -3.0f);
		else if (k == sf::Keyboard::D)
			m_GameObjects[playerId]->m_Sprite.move(0.0f, 3.0f);
	}
}

void GameScene::ClearGameObjects()
{
	if (!m_GameObjects.empty())
	{
		for (uint32 i = 0; i < m_GameObjects.size(); ++i)
		{
			if (m_GameObjects[i])
			{
				SAFE_DELETE(m_GameObjects[i]);
			}
		}

		m_GameObjects.clear();
		WRITE_LOG("Game Object heap cleared.", "good");
	}
}
