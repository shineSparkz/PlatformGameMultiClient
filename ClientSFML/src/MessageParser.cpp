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
	m_MsgMap[Packet::ID::IN_TCP_Register] = MessageParser::tcp_register;
	m_MsgMap[Packet::ID::IN_TCP_NewPlayerObject] = MessageParser::tcp_new_player;
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


void MessageParser::tcp_register(const rapidjson::Document& jd)
{
	// Reg
	NetworkManager::Instance()->m_ClientId = jd["clientId"].GetInt();
	NetworkManager::Instance()->m_ServerUdpPort = jd["udpPort"].GetInt();

	// Send First udp msg
	NetworkManager::Instance()->m_UdpSndBuffer->Deposit(std::to_string(NetworkManager::Instance()->m_ClientId) + ":UDP:0");
}

void MessageParser::tcp_new_player(const rapidjson::Document& jd)
{
	// Get Array of objects (could be one)
	if (jd.HasMember("objects"))
	{
		if (jd["objects"].IsArray())
		{
			auto client_array = jd["objects"].GetArray();

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


			Vec2 statePos((float)x, (float)y);

			// TODO : tidy all this
			NetState ns;
			ns.object_handle = obj_id;
			ns.position = statePos;

			SendEvent(EventID::Net_UpdateGameObject, &ns);
			break;
		}
	}
}

