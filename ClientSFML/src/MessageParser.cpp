#include "MessageParser.h"

#include "NetworkManager.h"
#include "EventManager.h"
#include "LogFile.h"
#include "Application.h"
#include "SceneGraph.h"
#include "GameObject.h"
#include "NetState.h"

bool MessageParser::Init()
{
	m_MsgMap[Packet::ID::IN_TCP_Connect] = MessageParser::tcp_connect;
	m_MsgMap[Packet::ID::IN_TCP_ServerMsg] = MessageParser::tcp_server_msg;
	m_MsgMap[Packet::ID::IN_TCP_StartGame] = MessageParser::tcp_start_game;

	m_MsgMap[Packet::ID::IN_UDP_UpdatedObject] = MessageParser::udp_update_object;
	//... etc

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
				// Log error
				return;
			}

			m_MsgMap[packetName](jd);
		}
	}
}


void MessageParser::tcp_connect(const rapidjson::Document& jd)
{
	// Reg
	NetworkManager::Instance()->m_ClientId = jd["clientId"].GetInt();
	NetworkManager::Instance()->m_ServerUdpPort = jd["udpPort"].GetInt();

	// Send First udp msg
	NetworkManager::Instance()->m_UdpSndBuffer->Deposit(std::to_string(NetworkManager::Instance()->m_ClientId) + ":UDP:0");
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

	// Get Array of objects (could be one)
	if (jd.HasMember("objects"))
	{
		if (jd["objects"].IsArray())
		{
			auto client_array = jd["objects"].GetArray();

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
					spr.setScale(Vec2(2.0f, 2.0f));
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

void MessageParser::udp_update_object(const rapidjson::Document& jd)
{
	if (jd["name"].IsInt())
	{
		Packet::ID packetName = static_cast<Packet::ID>(jd["name"].GetInt());

		switch (packetName)
		{
		case Packet::ID::IN_UDP_UpdatedObject:
			int obj_id = -1;
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
				SendEvent(EventID::Net_UpdateGameObject, &ns);
			}

			break;
		}
	}
}

