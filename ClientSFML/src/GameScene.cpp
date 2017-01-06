#include "GameScene.h"
#include "SceneGraph.h"
#include "EventManager.h"

#include "GameObject.h"
#include "NetState.h"
#include "NetworkManager.h"
#include "Application.h"
#include "utils.h"
#include "Screen.h"

#include "NetworkManager.h"

// Sort this out
float t = 0.0f;
rapidjson::StringBuffer g_sBuffer;


GameScene::GameScene() : 
	IScene(),
	m_BackgroundSprite(nullptr)
{
}

GameScene::~GameScene()
{
}

void GameScene::HandleEvent(Event* event_)
{
	switch (event_->GetID())
	{
	case LoadLevelData:
		this->LoadLevel();
		break;
	case Net_NewGameObject:
		{
			GameObject* go = (GameObject*)event_->GetData();

			if (go)
			{
				GameObject* newGO = new GameObject();
				newGO->m_Sprite = go->m_Sprite;
				newGO->m_TypeId = go->m_TypeId;
				newGO->m_UniqueId = go->m_UniqueId;
				newGO->m_FrameSizeX = go->m_FrameSizeX;
				newGO->m_FrameSizeY = go->m_FrameSizeY;

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

				if (obj_id >= m_GameObjects.size())
					return;

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
						m_GameObjects[obj_id]->m_Sprite.setPosition(m_GameObjects[obj_id]->m_Sprite.getPosition() +( diff * 0.1f));
					}

					m_GameObjects[obj_id]->m_Sprite.setTextureRect(Rect(
						static_cast<int>(ns->frameX * m_GameObjects[obj_id]->m_FrameSizeX),
						static_cast<int>(ns->frameY * m_GameObjects[obj_id]->m_FrameSizeY),
						static_cast<int>(m_GameObjects[obj_id]->m_FrameSizeX),
						static_cast<int>(m_GameObjects[obj_id]->m_FrameSizeY)));

					m_GameObjects[obj_id]->m_Active = ns->active;
				}
			}
		}
		break;
	}
}

bool GameScene::OnCreate(Context* const con)
{
	m_context = con;
	
	AttachEvent(EventID::LoadLevelData, *this);
	AttachEvent(EventID::Net_NewGameObject, *this);
	AttachEvent(EventID::Net_UpdateGameObject, *this);

	// Background
	m_BackgroundSprite = new sf::Sprite();
	m_BackgroundSprite->setTexture(Application::Instance()->GetTexHolder().Get(ID::Texture::Bkgrnd_RedMtn));
	m_BackgroundSprite->setScale(2.0f, 2.0f);

	return true;
}

void GameScene::LoadLevel()
{
	// This needs to be pre-loaded by the lobby
	if (!m_GameObjects.empty())
		return;

	for (int i = 0; i < 12; ++i)
	{
		// Create Object from parsed data
		GameObject* go = new GameObject();
		go->m_TypeId = (int)ID::Type::Wall;
		go->m_UniqueId = m_GameObjects.size();
		go->m_FrameSizeX = 64;
		go->m_FrameSizeY = 64;

		sf::Sprite spr;
		spr.setPosition(Vec2((float)i * 64, (float)450));
		spr.setTexture(Application::Instance()->GetTexHolder().Get(ID::Texture::DestructableWall));
		go->m_Sprite = spr;

		m_GameObjects.push_back(go);
	}

	// One more wall
	GameObject* go = new GameObject();
	go->m_TypeId = (int)ID::Type::Wall;
	go->m_UniqueId = m_GameObjects.size();
	go->m_FrameSizeX = 64;
	go->m_FrameSizeY = 64;
	sf::Sprite spr;
	spr.setPosition(Vec2(7 * 64, 350));
	spr.setTexture(Application::Instance()->GetTexHolder().Get(ID::Texture::DestructableWall));
	go->m_Sprite = spr;
	m_GameObjects.push_back(go);

	// Enemy
	GameObject* enemy = new GameObject();
	enemy->m_TypeId = (int)ID::Type::EnemyBlueMinion;
	enemy->m_UniqueId = m_GameObjects.size();
	enemy->m_FrameSizeX = 45;
	enemy->m_FrameSizeY = 66;
	sf::Sprite espr;
	espr.setPosition(Vec2(200, 200));
	espr.setTexture(Application::Instance()->GetTexHolder().Get(ID::Texture::BlueMinionEnemy));
	espr.setTextureRect(sf::IntRect(0, 0, (int)enemy->m_FrameSizeX, (int)enemy->m_FrameSizeY));
	enemy->m_Sprite = espr;
	m_GameObjects.push_back(enemy);

	// Exit
	GameObject* exit = new GameObject();
	exit->m_TypeId = (int)ID::Type::Exit;
	exit->m_UniqueId = m_GameObjects.size();
	exit->m_FrameSizeX = 768 / 6;
	exit->m_FrameSizeY = 630 / 5;
	sf::Sprite exspr;
	exspr.setPosition(14 * 64, 400);
	exspr.setTexture(Application::Instance()->GetTexHolder().Get(ID::Texture::LevelExit));
	exspr.setTextureRect(sf::IntRect(0, 2 * (int)exit->m_FrameSizeY, (int)exit->m_FrameSizeX, (int)exit->m_FrameSizeY));
	exit->m_Sprite = exspr;
	m_GameObjects.push_back(exit);

	// Collect Skull
	GameObject* skull = new GameObject();
	skull->m_TypeId = (int)ID::Type::Exit;
	skull->m_UniqueId = m_GameObjects.size();
	skull->m_FrameSizeX = 32;
	skull->m_FrameSizeY = 32;
	sf::Sprite skullspr;
	skullspr.setPosition(10 * 64, 200);
	skullspr.setTexture(Application::Instance()->GetTexHolder().Get(ID::Texture::GoldSkull));
	skullspr.setTextureRect(sf::IntRect(0, 0, (int)skull->m_FrameSizeX, (int)skull->m_FrameSizeY));
	skull->m_Sprite = skullspr;
	m_GameObjects.push_back(skull);

	// Bullets last
	for (int i = 0; i < 10; ++i)
	{
		GameObject* b = new GameObject();
		b->m_TypeId = (int)ID::Type::PlayerProjectile;
		b->m_UniqueId = m_GameObjects.size();
		b->m_FrameSizeX = 32;
		b->m_FrameSizeY = 32;
		b->m_Active = false;
		sf::Sprite s;
		s.setPosition(0, 0);
		s.setTexture(Application::Instance()->GetTexHolder().Get(ID::Texture::Fireball));
		s.setTextureRect(sf::IntRect(2*32, 2*32, (int)b->m_FrameSizeX, (int)b->m_FrameSizeY));
		b->m_Sprite = s;
		m_GameObjects.push_back(b);
	}
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
	DetachEvent(EventID::LoadLevelData, *this);

	this->ClearGameObjects();
	SAFE_DELETE(m_BackgroundSprite);
}

void GameScene::OnRender()
{
	if (m_SceneReady)
	{
		const Vec2& vs = Screen::Instance()->GetMainView().getSize();
		const Vec2& vp = Screen::Instance()->GetViewPos();

		// Render Background
		const float bg_width = (float)m_BackgroundSprite->getTexture()->getSize().x;
		const float bg_height = (float)m_BackgroundSprite->getTexture()->getSize().y;
		const bool tileX = true;
		const bool tileY = false;
		int num_x = tileX ? static_cast<int>(vs.x / bg_width + 1) : 1;
		int num_y = tileY ? static_cast<int>(vs.y / bg_height + 1) : 1;

		for (int y = 0; y < num_y; ++y)
		{
			for (int x = 0; x < num_x; ++x)
			{
				m_BackgroundSprite->setPosition(Vec2(vp.x + (x * bg_width), vp.y + (y * bg_height)));
				m_context->window->draw(*m_BackgroundSprite);
			}
		}
		
		for each (GameObject* go in m_GameObjects)
		{
			if(go->m_Active)
				m_context->window->draw(go->m_Sprite);
		}
	}
}

void GameScene::LateRender()
{
}

bool GameScene::OnUpdate(const sf::Time& dt)
{
	// TODO : Remove this hack
	if (!m_SceneReady)
	{
		t += dt.asSeconds();

		if (t > 1.0f)
		{
			m_SceneReady = true;
		}
	}

	size_t playerId = (size_t)NetworkManager::Instance()->playerId();
	
	if (playerId < m_GameObjects.size() && NetworkManager::Instance()->connected())
	{
		//Screen::Instance()->SetViewCentre(m_GameObjects[playerId]->m_Sprite.getPosition());

		// How to do velocity next
		// Quick Gravity hack (awful)
		//NetworkManager::Instance()->sendUdp(
		//	"input:" + std::to_string(sf::Keyboard::S) + ":" + std::to_string(NetworkManager::Instance()->clientId()));
	}

	return true;
}

void GameScene::HandleInput(int k, int a)
{
	//57 is space
	// TODO : Put this in a network component
	if (NetworkManager::Instance()->connected())
	{
		rapidjson::Writer<rapidjson::StringBuffer> g_Writer(g_sBuffer);

		g_Writer.StartObject();
		g_Writer.Key("name");	// Always need this
		g_Writer.Int((int)Packet::ID::OUT_UDP_Input);
		g_Writer.Key("key");
		g_Writer.Int(k);
		g_Writer.Key("act");
		g_Writer.Int(a);
		g_Writer.Key("id");
		g_Writer.Uint(NetworkManager::Instance()->clientId());
		g_Writer.EndObject();

		NetworkManager::Instance()->sendUdp(g_sBuffer.GetString());
		g_sBuffer.Clear();

		// Predict Locally
		/*
		size_t playerId = (size_t)NetworkManager::Instance()->playerId();
		if (k == sf::Keyboard::D)
			m_GameObjects[playerId]->m_Sprite.move(3.0f, 0.0f);
		else if (k == sf::Keyboard::A)
			m_GameObjects[playerId]->m_Sprite.move(-3.0f, 0.0f);
		else if (k == sf::Keyboard::W)
			m_GameObjects[playerId]->m_Sprite.move(0.0f, -3.0f);
		*/
		//else if (k == sf::Keyboard::S)
		//	m_GameObjects[playerId]->m_Sprite.move(0.0f, 3.0f);	// I have left this on purpose for testing
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
