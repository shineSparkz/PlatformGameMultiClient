#include "MessageParser.h"

#include "NetworkManager.h"
#include "EventManager.h"
#include "LogFile.h"
#include "Application.h"
#include "SceneGraph.h"
#include "GameObject.h"
#include "NetState.h"
#include "Screen.h"
#include "LogFile.h"
#include "utils.h"

MessageParser::~MessageParser()
{
	m_MsgMap.clear();
}

bool MessageParser::Init()
{
	m_MsgMap[Packet::ID::IN_TCP_Connect] = MessageParser::tcp_connect;
	m_MsgMap[Packet::ID::IN_TCP_ServerMsg] = MessageParser::tcp_server_msg;
	m_MsgMap[Packet::ID::IN_TCP_StartGame] = MessageParser::tcp_start_game;
	m_MsgMap[Packet::ID::IN_TCP_FinishLevel] = MessageParser::tcp_finish_level;
	m_MsgMap[Packet::ID::IN_TCP_ExpQueery] = MessageParser::tcp_exp_queery;
	m_MsgMap[Packet::ID::IN_TCP_LeaderboardRequest] = MessageParser::tcp_leaderboard_request;

	m_MsgMap[Packet::ID::IN_UDP_UpdatedObject] = MessageParser::udp_update_object;
	m_MsgMap[Packet::ID::IN_UDP_ViewUpdate] = MessageParser::udp_view_update;
	m_MsgMap[Packet::ID::IN_UPD_PlayerHealth] = MessageParser::udp_player_health;


	return true;
}

void MessageParser::Parse(const rapidjson::Document& jd)
{
	if (jd.HasMember("name"))
	{
		if (jd["name"].IsInt())
		{
			Packet::ID packetName = static_cast<Packet::ID>(jd["name"].GetInt());

			if (m_MsgMap.find(packetName) == m_MsgMap.end())
			{
				WRITE_LOG("The packet name: " + util::to_str((int)packetName) + " is unknown", "error");
				return;
			}

			m_MsgMap[packetName](jd);
		}
	}
}

// ---- TCP Function Pointers ----
void MessageParser::tcp_connect(const rapidjson::Document& jd)
{
	// Reg
	NetworkManager::Instance()->m_ClientId = jd["clientId"].GetInt();
	NetworkManager::Instance()->m_ServerUdpPort = jd["udpPort"].GetInt();

	// Send First udp msg
	NetworkManager::Instance()->sendUdp(std::to_string(NetworkManager::Instance()->clientId()) + ":UDP:0");
}

void MessageParser::tcp_server_msg(const rapidjson::Document& jd)
{
	// This is actualy only a call back for a failed start game, for sucess we just load the map and inform the lobby
	std::string msg = "Error";
	bool success = false;

	if (jd.IsObject())
	{
		if (jd.HasMember("msg"))
		{
			if (jd["msg"].IsString())
			{
				msg = jd["msg"].GetString();
			}
		}

		if (jd.HasMember("success"))
		{
			if (jd["success"].IsBool())
			{
				success = jd["success"].GetBool();
			}
			else if (jd["success"].IsInt())
			{
				success = (jd["success"].GetInt() == 1);
			}
		}
	}

	// Inform lobby on net manager
	SendEvent(EventID::Net_ServerMsgCallback, &msg);
}

void MessageParser::tcp_start_game(const rapidjson::Document& jd)
{
	bool loadLevel = false;

	if (jd.HasMember("loadLevel"))
	{
		if (jd["loadLevel"].IsBool())
		{
			loadLevel = jd["loadLevel"].GetBool();
		}
		else if (jd["loadLevel"].IsInt())
		{
			loadLevel = jd["loadLevel"].GetInt() == 1;
		}
	}

	// Get Array of player objects (could only be one)
	if (jd.HasMember("objects"))
	{
		if (jd["objects"].IsArray())
		{
			auto client_array = jd["objects"].GetArray();

			// Only load level if we have started game (we could be online and in lobby and still get this)
			if (GetActiveState() == ID::States::Lobby && loadLevel)
			{
				// Tell Lobby to move to game state
				SendEvent(EventID::Net_StartGameCallback, nullptr);
			}

			// Iterate player array
			for (auto arr_it = client_array.Begin(); arr_it != client_array.End(); ++arr_it)
			{
				if (arr_it->IsObject())
				{
					auto obj = arr_it->GetObject();

					int objectId = -1;
					int unqId = -1;
					float x = -1;
					float y = -1;
					int isThisMe = -1;

					// Parse Data
					if (obj.HasMember("oid"))
					{
						if (obj["oid"].IsInt())
						{
							objectId = obj["oid"].GetInt();
						}
					}

					if (obj.HasMember("uid"))
					{
						if (obj["uid"].IsInt())
						{
							unqId = obj["uid"].GetInt();
						}
					}

					if (obj.HasMember("px"))
					{
						if (obj["px"].IsFloat())
						{
							x = obj["px"].GetFloat();
						}
						else if (obj["px"].IsInt())
						{
							x = (float)obj["px"].GetInt();
						}
					}

					if (obj.HasMember("py"))
					{
						if (obj["py"].IsFloat())
						{
							y = obj["py"].GetFloat();
						}
						else if (obj["py"].IsInt())
						{
							y = (float)obj["py"].GetInt();
						}
					}

					if (obj.HasMember("clt"))
					{
						if (obj["clt"].IsInt())
						{
							isThisMe = obj["clt"].GetInt();
						}
					}

					// Create Object from parsed data
					GameObject go;
					go.m_TypeId = objectId;
					go.m_UniqueId = unqId;
					go.m_FrameSizeX = 64;
					go.m_FrameSizeY = 64;

					sf::Sprite spr;
					spr.setPosition(Vec2(x, y));
					spr.setTexture(Application::Instance()->GetTexHolder().Get(ID::Texture::Player));
					spr.setTextureRect(sf::IntRect(0, 0, 64, 64));
					//spr.setScale(Vec2(2.0f, 2.0f));
					spr.setScale(Vec2(1.5f, 1.5f));
					go.m_Sprite = spr;

					if (isThisMe == 1)
					{
						NetworkManager::Instance()->m_PlayerHandle = unqId;
					}

					SendEvent(EventID::Net_NewGameObject, &go);
				}
			}
		}
	}
}

void MessageParser::tcp_finish_level(const rapidjson::Document& jd)
{
	int exp = -1;

	if (jd.HasMember("exp"))
	{
		if (jd["exp"].IsInt())
		{
			exp = jd["exp"].GetInt();
		}
	}

	NetworkManager::Instance()->m_PlayerExp = exp;

	// Move back to lobby
	if (GetActiveState() == ID::States::Game)
	{
		// All of the game data will unload here, but will be pre-loaded again once we enter the lobby
		ChangeState(ID::States::Lobby);
	}
}

void MessageParser::tcp_exp_queery(const rapidjson::Document& jd)
{
	int exp = -1;

	if (jd.HasMember("exp"))
	{
		if (jd["exp"].IsInt())
		{
			exp = jd["exp"].GetInt();
		}
	}

	NetworkManager::Instance()->m_PlayerExp = exp;
}

void MessageParser::tcp_leaderboard_request(const rapidjson::Document& jd)
{
	std::string msg = "Error";
	if (jd.IsObject())
	{
		if (jd.HasMember("data"))
		{
			if (jd["data"].IsString())
			{
				msg = jd["data"].GetString();
			}
		}
	}

	// Inform Leaderboard scene about this data
	SendEvent(EventID::Net_DatabaseRequest, &msg);
}

// ---- UDP Function pointers ----
void MessageParser::udp_update_object(const rapidjson::Document& jd)
{
	if (jd["name"].IsInt())
	{
		Packet::ID packetName = static_cast<Packet::ID>(jd["name"].GetInt());

		switch (packetName)
		{
		case Packet::ID::IN_UDP_UpdatedObject:
			int obj_id = -1;
			bool active = true;
			float x = -1;
			float y = -1;
			float fx = -1;
			float fy = -1;

			bool fail_parse = false;

			// Parse
			if (jd.HasMember("handle"))
			{
				if (jd["handle"].IsInt())
				{
					obj_id = jd["handle"].GetInt();
				}
			}

			if (jd.HasMember("active"))
			{
				if (jd["active"].IsInt())
				{
					active = (jd["active"].GetInt() == 1);
				}
				else if (jd["active"].IsBool())
				{
					active = jd["active"].GetBool();
				}
			}

			if (jd.HasMember("px"))
			{
				if (jd["px"].IsFloat())
				{
					x = jd["px"].GetFloat();
				}
				else if (jd["px"].IsInt())
				{
					x = (float)jd["px"].GetInt();
				}
				else
				{
					fail_parse = true;
				}
			}
			else
			{
				fail_parse = true;
			}


			if (jd.HasMember("py"))
			{
				if (jd["py"].IsFloat())
				{
					y = jd["py"].GetFloat();
				}
				else if (jd["py"].IsInt())
				{
					y = (float)jd["py"].GetInt();
				}
				else
				{
					fail_parse = true;
				}
			}
			else
			{
				fail_parse = true;
			}

			if (jd.HasMember("fx"))
			{
				if (jd["fx"].IsFloat())
				{
					fx = jd["fx"].GetFloat();
				}
				else if (jd["fx"].IsInt())
				{
					fx = (float)jd["fx"].GetInt();
				}
				else
				{
					fail_parse = true;
				}
			}
			else
			{
				fail_parse = true;
			}


			if (jd.HasMember("fy"))
			{
				if (jd["fy"].IsFloat())
				{
					fy = jd["fy"].GetFloat();
				}
				else if (jd["fy"].IsInt())
				{
					fy = (float)jd["fy"].GetInt();
				}
				else
				{
					fail_parse = true;
				}
			}
			else
			{
				fail_parse = true;
			}

			if (!fail_parse)
			{
				NetState ns;
				ns.object_handle = obj_id;
				ns.position = Vec2(x, y);
				ns.frameX = fx;
				ns.frameY = fy;
				ns.active = active;
				SendEvent(EventID::Net_UpdateGameObject, &ns);
			}

			break;
		}
	}
}

void MessageParser::udp_view_update(const rapidjson::Document& jd)
{
	float x = 0;
	float y = 0;

	if (jd.HasMember("px"))
	{
		if (jd["px"].IsFloat())
		{
			x = jd["px"].GetFloat();
		}
		else if (jd["px"].IsInt())
		{
			x = (float)jd["px"].GetInt();
		}
	}

	if (jd.HasMember("py"))
	{
		if (jd["py"].IsFloat())
		{
			y = jd["py"].GetFloat();
		}
		else if (jd["py"].IsInt())
		{
			y = (float)jd["py"].GetInt();
		}
	}

	Screen::Instance()->SetViewCentre(Vec2(x, y));
}

void MessageParser::udp_player_health(const rapidjson::Document& jd)
{
	int health = 0;

	if (jd.HasMember("health"))
	{
		if (jd["health"].IsInt())
		{
			health = jd["health"].GetInt();
		}
	}
	
	SendEvent(EventID::Net_PlayerHealth, &health);
}


