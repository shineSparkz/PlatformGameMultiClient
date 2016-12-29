#include "NetworkManager.h"

#include "types.h"
#include "utils.h"

#include "GameObject.h"
#include "NetState.h"
#include "EventManager.h"
#include "Application.h"

#include <rapidjson\document.h>
#include <rapidjson\writer.h>
#include <rapidjson\stringbuffer.h>
#include <rapidjson\pointer.h>

const float THREAD_SLEEP = 1 / 60.0f;

// ---- Temp ----
const int PKT_REG_TCP = 0;
const int PKT_GAME_PLAYER_OBJECTS_TCP = 1;
const int PKT_INPUT_UPDATE_UDP = 2;

void rcv_udp_thread(sf::UdpSocket* sock, BoundedBuffer* boundbuffer, bool& quit)
{
	while (!quit)
	{
		char buffer[BUFF_SIZE];
		memset(&buffer, 0, BUFF_SIZE);
		std::size_t received;

		sf::IpAddress rcv_ip;
		unsigned short rcv_port;

		sf::Socket::Status status = sock->receive(&buffer, BUFF_SIZE, received, rcv_ip, rcv_port);
		if (status == sf::Socket::Done)
		{
			boundbuffer->Deposit(std::string(buffer));
		}

		//std::this_thread::sleep_for(std::chrono::duration<float>(THREAD_SLEEP));// FRAME_TIME.asSeconds()));
	}

	sock->unbind();
}

void send_udp_msg(const std::string& msg, sf::UdpSocket* sock, const std::string& ipAddr, int port)
{
	unsigned char buffer[BUFF_SIZE];
	memset(buffer, 0, BUFF_SIZE);
	memcpy(buffer, msg.c_str(), msg.length());

	if (sock->send(&buffer, BUFF_SIZE, ipAddr, port) != sf::Socket::Done)
	{
	}
}

void snd_udp_thread(sf::UdpSocket* sock, BoundedBuffer* buff, bool& quit, const std::string ipAddr, int& serverUdpPort)
{
	while (!quit)
	{
		if (buff->count_ > 0)
		{
			std::string msg = buff->Fetch();

			if (msg.size() > 0)
			{
				send_udp_msg(msg, sock, ipAddr, serverUdpPort);
			}
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(16));
		//std::this_thread::sleep_for(std::chrono::duration<float>(THREAD_SLEEP));// FRAME_TIME.asSeconds()));
	}
}

void rcv_tcp_thread(sf::TcpSocket* sock, BoundedBuffer* boundbuffer, bool& quit)
{
	while (!quit)
	{
		char buffer[BUFF_SIZE];
		memset(buffer, 0, BUFF_SIZE);
		std::size_t received;

		if (sock->receive(buffer, BUFF_SIZE, received) != sf::Socket::Done)
		{
			// error...
		}
		else
		{
			boundbuffer->Deposit(std::string(buffer));
		}
	}
}

void send_tcp_msg(const std::string& msg, sf::TcpSocket* sock)
{
	unsigned char buffer[BUFF_SIZE];
	memset(buffer, 0, BUFF_SIZE);
	memcpy(buffer, msg.c_str(), msg.length());

	if (sock->send(buffer, BUFF_SIZE) != sf::Socket::Done)
	{
		//error
	}
}

void snd_tcp_thread(sf::TcpSocket* sock, BoundedBuffer* buff, bool& quit)
{
	while (!quit)
	{
		if (buff->count_ > 0)
		{
			std::string msg = buff->Fetch();

			if (msg.size() > 0)
			{
				send_tcp_msg(msg, sock);
			}
		}

		// That many millis in one frame
		std::this_thread::sleep_for(std::chrono::milliseconds(16));
		//std::this_thread::sleep_for(std::chrono::duration<float>(THREAD_SLEEP));// FRAME_TIME.asSeconds()));
	}
}


NetworkManager::NetworkManager() :
	m_TcpSock(nullptr),
	m_UdpSock(nullptr),
	m_UdpRcvBuffer(nullptr),
	m_UdpSndBuffer(nullptr),
	m_TcpRcvBuffer(nullptr),
	m_TcpSndBuffer(nullptr),
	m_UdpRcvThread(nullptr),
	m_UdpSndThread(nullptr),
	m_TcpRcvThread(nullptr),
	m_TcpSndThread(nullptr),
	m_LocalUdpPort(-1),
	m_ServerUdpPort(-1),
	m_ClientId(-1),
	m_BoundedBuffSize(0),
	m_PlayerHandle(-1),
	m_Quit(false),
	m_Connected(false)
{
}

NetworkManager::~NetworkManager()
{
	// May need to close sockets
	SAFE_DELETE(m_UdpRcvBuffer);
	SAFE_DELETE(m_UdpSndBuffer);
	SAFE_DELETE(m_TcpSndBuffer);
	SAFE_DELETE(m_TcpRcvBuffer);

	SAFE_DELETE(m_UdpRcvThread);
	SAFE_DELETE(m_UdpSndThread);
	SAFE_DELETE(m_TcpRcvThread);
	SAFE_DELETE(m_TcpSndThread);


	SAFE_DELETE(m_TcpSock);
	SAFE_DELETE(m_UdpSock);
}

bool NetworkManager::connectToServer(int onPort, int buffSize)
{
	if (!m_TcpSock)
	{
		// Look for broadcast
		int broadcast_port = 8081;
		sf::UdpSocket broadcast_socket;
		
		sf::IpAddress remote_ip;
		unsigned short remote_port;
		size_t received_bytes;
		
		char buff[32];
		char rcv_buff[32];
		memset(&buff, 0, 32);
		memset(&rcv_buff, 0, 32);

		//broadcast_socket.send(buff, 32, "255.255.255.255", broadcast_port);
		broadcast_socket.send(buff, 32, "192.168.0.255", broadcast_port);
		// TODO : Bail if that doesn't work
		
		sf::Socket::Status broadcast_status = broadcast_socket.receive(&rcv_buff, 32, received_bytes, remote_ip, remote_port);
		if (broadcast_status != sf::Socket::Status::Done)
		{
			// Error
			WRITE_LOG("Could not resolve server address", "error");
			return false;
		}

		std::string server_address = remote_ip.toString();
		
		// Attempt to connect to server now
		m_TcpSock = new sf::TcpSocket();
		m_TcpSock->setBlocking(true);

		m_BoundedBuffSize = buffSize;

		sf::Socket::Status status = m_TcpSock->connect(server_address, onPort);

		if (status != sf::Socket::Status::Done)
		{
			// TODO : Log
			SAFE_DELETE(m_TcpSock);
			return false;
		}

		// Now we have connection, store servers address for UDP
		m_ServerIPAddr = server_address;

		/*
		switch (status)
		{
		case sf::Socket::Status::Done:
		update_ui_info(info_q, "Sock Done");
		break;
		case sf::Socket::Status::NotReady:
		update_ui_info(info_q, "Sock Not ready");
		break;
		case sf::Socket::Status::Partial:
		update_ui_info(info_q, "Sock partial");
		break;
		case sf::Socket::Status::Disconnected:
		update_ui_info(info_q, "Sock disconnected");
		break;
		case sf::Socket::Status::Error:
		update_ui_info(info_q, "Sock error");
		break;
		}
		*/

		//if (g_ShouldPrintUI)
		//	update_ui_info(info_q, "Local TCP port: " + std::to_string(tcp_sock.getLocalPort()));

		// Log : Connected to server at {ip} : {port}

		// Create Buffers now we have a connection
		m_UdpRcvBuffer = new BoundedBuffer(m_BoundedBuffSize);
		m_UdpSndBuffer = new BoundedBuffer(m_BoundedBuffSize);
		m_TcpRcvBuffer = new BoundedBuffer(m_BoundedBuffSize);
		m_TcpSndBuffer = new BoundedBuffer(m_BoundedBuffSize);

		// Start a Tcp Listen thread first as we expect a message for udp bind
		m_TcpRcvThread = new std::thread(rcv_tcp_thread, std::ref(m_TcpSock), std::ref(m_TcpRcvBuffer), std::ref(m_Quit));
		m_TcpRcvThread->detach();

		// Create Udp Socket
		m_UdpSock = new sf::UdpSocket();
		m_UdpSock->setBlocking(true);

		// Get system to set free port
		//m_UdpSock->bind(sf::Socket::AnyPort);	// For some reason this was not working in certain situations
		//32767
		m_UdpSock->bind(0);
		m_LocalUdpPort = m_UdpSock->getLocalPort();
		// TODO : Log port

		// Finally spin all other threads
		m_TcpSndThread = new std::thread(snd_tcp_thread, std::ref(m_TcpSock), std::ref(m_TcpSndBuffer), std::ref(m_Quit));
		m_TcpSndThread->detach();

		m_UdpRcvThread = new std::thread(rcv_udp_thread, std::ref(m_UdpSock), std::ref(m_UdpRcvBuffer), std::ref(m_Quit));
		m_UdpRcvThread->detach();

		m_UdpSndThread = new std::thread(snd_udp_thread, std::ref(m_UdpSock), std::ref(m_UdpSndBuffer), std::ref(m_Quit), m_ServerIPAddr, std::ref(m_ServerUdpPort));
		m_UdpSndThread->detach();

		m_Connected = true;
		return true;
	}
	else
	{
		// TODO : Log
		return false;
	}
}

void NetworkManager::sendUdp(const std::string& msg)
{
	// Assumes we know we have a connection
	m_UdpSndBuffer->Deposit(msg);
}

void NetworkManager::sendTcp(const std::string& msg)
{
	m_TcpSndBuffer->Deposit(msg);
}

void NetworkManager::fetchAllMessages()
{
	// --- Drain TCP Buffer ----
	if (m_TcpRcvBuffer->count_ > 0)
	{
		// Display TCP
		while (m_TcpRcvBuffer->count_ > 0)
		{
			std::string packet = m_TcpRcvBuffer->Fetch();

			if (packet.size() > 0)
			{
				rapidjson::Document jd;
				jd.Parse(packet.c_str());

				if (!jd.IsObject())
				{
					continue;
				}

				if (jd.HasMember("name"))
				{
					if (jd["name"].IsInt())
					{
						int packetName = jd["name"].GetInt();

						switch(packetName)
						{
						case PKT_REG_TCP:
							// Reg
							m_ClientId = jd["clientId"].GetInt();
							m_ServerUdpPort = jd["udpPort"].GetInt();

							// Send First udp msg
							m_UdpSndBuffer->Deposit(std::to_string(m_ClientId) + ":UDP:0");
							break;
						case PKT_GAME_PLAYER_OBJECTS_TCP:
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
											spr.setPosition(Vec2(x,y));
											spr.setTexture(Application::Instance()->GetTexHolder().Get(ID::Texture::Player));
											spr.setTextureRect(sf::IntRect(0, 0, 64, 64));
											spr.setScale(Vec2(2.0f, 2.0f));
											go.m_Sprite = spr;

											if (isThisMe == 1)
											{
												m_PlayerHandle = unqId;
											}

											SendEvent(EventID::Net_NewGameObject, &go);
										}
									}
								}
							}
						}
							break;
						default:
							break;
						}
					}
				}
			}
		}
	}

	// -- Drain UDP Buffer ---
	if (m_UdpRcvBuffer->count_ > 0)
	{
		while (m_UdpRcvBuffer->count_ > 0)
		{
			std::string packet = m_UdpRcvBuffer->Fetch();

			if (packet.size() > 0)
			{
				rapidjson::Document jd;
				jd.Parse(packet.c_str());

				if (!jd.IsObject())
				{
					continue;
				}

				if (jd.HasMember("name"))
				{
					if (jd["name"].IsInt())
					{
						int packetName = jd["name"].GetInt();

						switch (packetName)
						{
						case PKT_INPUT_UPDATE_UDP:
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
			}
		}
	}
}

bool NetworkManager::connected() const
{
	return m_Connected;
}

int NetworkManager::playerId()const
{
	return m_PlayerHandle;
}

int NetworkManager::clientId()const
{
	return m_ClientId;
}