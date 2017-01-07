#include "GameScene.h"
#include "SceneGraph.h"
#include "EventManager.h"

#include "GameObject.h"
#include "NetState.h"
#include "NetworkManager.h"
#include "Application.h"
#include "utils.h"
#include "Screen.h"
#include "utils.h"
#include "TextFile.h"

#include "NetworkManager.h"

// Sort this out
float t = 0.0f;

GameScene::GameScene() : 
	IScene(),
	m_BackgroundSprite(nullptr),
	m_TextObject(nullptr)
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

	// Text Rendering
	if (!m_TextObject)
		m_TextObject = new sf::Text();
	m_TextObject->setFont(m_context->fonts->Get(ID::Font::GOTHIC));
	m_TextObject->setCharacterSize(18);

	return true;
}

void GameScene::LoadLevel()
{
	// This needs to be pre-loaded by the lobby
	if (!m_GameObjects.empty())
		return;

	TextFile file;

	if (file.LoadFileAsLinesToBuffer("resources/data/obj_map_0.txt"))
	{
		// First get width and height from first two lines of file
		int CellSize = file.GetDataAsInt(0);
		int MapWidth = file.GetDataAsInt(1);
		int MapHeight = file.GetDataAsInt(2);

		int MapRows = MapHeight / CellSize;
		int MapCols = MapWidth / CellSize;

		// Current line in file
		int32 line_number = 3;

		for (uint32 y = 0; y < MapRows; ++y)
		{
			std::string line = file.GetDataBuffer(line_number);
			auto split = util::split_str(line, ',');

			for (uint32 x = 0; x < split.size(); ++x)
			{
				ID::Type type = static_cast<ID::Type>(atoi(split[x].c_str()));

				Vec2 tile_pos = Vec2(static_cast<float>(x * CellSize),
					static_cast<float> (y * CellSize));

				// We don't care what it returns
				//this->InstantiateGameObject(type, tile_pos, Vec2(1.0f, 1.0f));

				switch (type)
				{
				case ID::Type::DestructablePlatform:
				{
					this->CreateGameObject(ID::Type::DestructablePlatform, 64, 64, tile_pos.x, tile_pos.y, ID::Texture::DestructableWall, 0, 0);
				}
				break;
				case ID::Type::EnemyBlueMinion:
				{
					this->CreateGameObject(ID::Type::EnemyBlueMinion, 45, 66, tile_pos.x, tile_pos.y, ID::Texture::BlueMinionEnemy, 0, 0);
				}
				break;
				case ID::Type::EnemyDisciple:
				{
					this->CreateGameObject(ID::Type::EnemyDisciple, 45, 51, tile_pos.x, tile_pos.y, ID::Texture::DiscipleEnemy, 0, 0, true, 2.0f);
				}
				break;
				case ID::Type::EnemyShadow:
				{
					this->CreateGameObject(ID::Type::EnemyShadow, 80, 70, tile_pos.x, tile_pos.y, ID::Texture::ShadowEnemy, 0, 0);
				}
				break;
				case ID::Type::Exit:
				{
					this->CreateGameObject(ID::Type::Exit, 768 / 6, 630 / 5, tile_pos.x, tile_pos.y, ID::Texture::LevelExit, 0, 2 * (630 / 5));
				}
				break;
				case ID::Type::GoldSkull:
				{
					this->CreateGameObject(ID::Type::GoldSkull, 32, 32, tile_pos.x, tile_pos.y, ID::Texture::GoldSkull, 0, 0);
				}
				break;
				case ID::Type::Player:
				{
				}
				break;
				case ID::Type::Spike:
				{
					this->CreateGameObject(ID::Type::Spike, 32, 32, tile_pos.x, tile_pos.y, ID::Texture::Spikes, 0, 0, true, 2.0f);
				}
				break;
				case ID::Type::Wall:
				{
					this->CreateGameObject(ID::Type::Wall, 64, 64, tile_pos.x, tile_pos.y, ID::Texture::BloodMtn_TileSet, 64, 64);
				}
				break;
				default:
					break;
				}
			}

			++line_number;
		}
	}

	/*
	for (int i = 0; i < 12; ++i)
	{
		// Create Object from parsed data
		this->CreateGameObject(ID::Type::Wall, 64, 64, (float)i * 64, 450, ID::Texture::BloodMtn_TileSet, 64, 64);
	}

	// One more wall
	this->CreateGameObject(ID::Type::Wall, 64, 64, 7 * 64, 350, ID::Texture::BloodMtn_TileSet, 64, 64);

	// Blue Minion Enemy
	this->CreateGameObject(ID::Type::EnemyBlueMinion, 45, 66, 200, 200, ID::Texture::BlueMinionEnemy, 0, 0);

	// Exit
	this->CreateGameObject(ID::Type::Exit, 768/6, 630/5, 14*64, 400, ID::Texture::LevelExit, 0, 2 * (630/5));

	// Collect Skull
	this->CreateGameObject(ID::Type::GoldSkull, 32, 32, 10*64, 200, ID::Texture::GoldSkull, 0, 0);

	// Shadow Enemy
	this->CreateGameObject(ID::Type::EnemyShadow, 80, 70, 300, 450-64, ID::Texture::ShadowEnemy, 0, 0);

	// Disciple
	this->CreateGameObject(ID::Type::EnemyDisciple, 45, 51, 370, 450 - 64, ID::Texture::DiscipleEnemy, 0, 0, true, 2.0f);

	// Destroy Box
	this->CreateGameObject(ID::Type::DestructablePlatform, 64, 64, 590, 450-64, ID::Texture::DestructableWall, 0, 0);

	// Spike
	this->CreateGameObject(ID::Type::Spike, 32, 32, 520, 450-64, ID::Texture::Spikes, 0, 0, true, 2.0f);
	*/
	// Bullets last
	for (int i = 0; i < 10; ++i)
	{
		this->CreateGameObject(ID::Type::PlayerProjectile, 32, 32, 0, 0, ID::Texture::Fireball, 64, 64, false);
	}
}

void GameScene::CreateGameObject(ID::Type typeId, float frameSzX, float frameSzY,
	float xpos, float ypos, ID::Texture texID, int texPosX, int texPosY, bool active, float scale)
{
	GameObject* go = new GameObject();
	go->m_TypeId = (int)typeId;
	go->m_UniqueId = m_GameObjects.size();
	go->m_FrameSizeX = frameSzX;
	go->m_FrameSizeY = frameSzY;
	go->m_Active = active;
	
	sf::Sprite spr;
	spr.setPosition(Vec2(xpos, ypos));
	spr.setTexture(Application::Instance()->GetTexHolder().Get(texID));
	spr.setScale(scale, scale);
	spr.setTextureRect(sf::IntRect(texPosX, texPosY, (int)go->m_FrameSizeX, (int)go->m_FrameSizeY));
	go->m_Sprite = spr;
	m_GameObjects.push_back(go);
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
	SAFE_DELETE(m_TextObject);
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
	Vec2 vp = Screen::Instance()->GetViewPos();
	Screen::RenderText(m_context->window, m_TextObject, "Experience: " + util::to_str(NetworkManager::Instance()->playerExp()), Vec2(vp.x + 16.0f, vp.y + 16.0f), Screen::AlignLeft, 0, sf::Color::Red);
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

	return true;
}

void GameScene::HandleInput(int k, int a)
{
	//57 is space
	// TODO : Put this in a network component
	if (NetworkManager::Instance()->connected())
	{
		rapidjson::StringBuffer g_sBuffer;
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
