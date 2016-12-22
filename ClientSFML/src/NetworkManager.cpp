#include "NetworkManager.h"

#include "types.h"
#include "utils.h"

#include "GameObject.h"
#include "NetState.h"
#include "EventManager.h"
#include "Application.h"

const float THREAD_SLEEP = 1 / 60.0f;

void rcv_udp_thread(sf::UdpSocket* sock, BoundedBuffer* boundbuffer, bool& quit)
{
	while (!quit)
	{
		char buffer[BUFF_SIZE];
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

bool NetworkManager::connectToServer(const std::string& ipAddr, int onPort, int buffSize)
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
		m_UdpSock->bind(160187);
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

			// See if we got more than one packet here
			auto all_packets = util::split_str(packet, '!');

			for (size_t i = 0; i < all_packets.size() - 1; ++i)
			{
				auto current_packet = util::split_str(all_packets[i], ':');

				if (!current_packet.empty())
				{
					std::string packet_name = current_packet[0];

					if (packet_name == "reg")
					{
						// Log
						//if (g_ShouldPrintUI)
						//	update_ui_info(info_q, "Fetched: " + packet + ", count: " + std::to_string(rcv_tcp_boundedBuffer.count_));

						if (current_packet.size() >= 3)
						{
							m_ClientId = std::stoi(current_packet[1]);
							m_ServerUdpPort = std::stoi(current_packet[2]);

							// Send First udp msg
							m_UdpSndBuffer->Deposit(std::to_string(m_ClientId) + ":UDP:0");
							bool b = false;
						}
					}
					else if (packet_name == "mapdata")
					{
						// Log
						//if (g_ShouldPrintUI)
						//	update_ui_info(info_q, "Fetched: " + packet + ", count: " + std::to_string(rcv_tcp_boundedBuffer.count_));

						if (current_packet.size() > 1)
						{
							// Loop each object
							for (size_t j = 1; j < current_packet.size() - 1; ++j)
							{
								// Split this by comas
								auto obj_split = util::split_str(current_packet[j], ',');

								if (obj_split.size() >= 5)
								{
									int obj_id = std::stoi(obj_split[0]);
									int unq_id = std::stoi(obj_split[1]);
									int x = std::stoi(obj_split[2]);
									int y = std::stoi(obj_split[3]);
									int isMyPlayer = std::stoi(obj_split[4]);

									// Create Object from parsed data
									GameObject go;
									go.m_TypeId = obj_id;
									go.m_UniqueId = unq_id;

									sf::Sprite spr;
									spr.setPosition(Vec2((float)x, (float)y));

									if (obj_id == (int)ID::Type::Wall)
									{
										spr.setTexture(
											Application::Instance()->GetTexHolder().Get(ID::Texture::DestructableWall));
									}
									else if (obj_id == (int)ID::Type::Player)
									{
										spr.setTexture(
											Application::Instance()->GetTexHolder().Get(ID::Texture::Player));
										spr.setTextureRect(sf::IntRect(0, 0, 64, 64));
										spr.setScale(Vec2(2.0f, 2.0f));

										if (isMyPlayer == 1)
										{
											m_PlayerHandle = unq_id;
										}
									}
									else
									{
										// error
										return;
									}

									go.m_Sprite = spr;

									//GameObjects.push_back(go);
									// Send event here or something like that
									SendEvent(EventID::Net_NewGameObject, &go);
								}
							}
						}
					}
				}
			}
		}
	}

	// -- Drain UDP Buffer ---
	if (m_UdpRcvBuffer->count_ > 0)
	{
		// Display UDP
		while (m_UdpRcvBuffer->count_ > 0)
		{
			std::string packet = m_UdpRcvBuffer->Fetch();

			// Try parse for updates
			auto packet_split = util::split_str(packet, ':');

			// Load Level Data
			if (packet_split.size() >= 4)
			{
				if (packet_split[0] == "objupd")
				{
					int obj_id = std::stoi(packet_split[1]);
					int x = std::stoi(packet_split[2]);
					int y = std::stoi(packet_split[3]);
					Vec2 statePos((float)x, (float)y);

					// TODO : tidy all this
					NetState ns;
					ns.object_handle = obj_id;
					ns.position = statePos;

					SendEvent(EventID::Net_UpdateGameObject, &ns);

					/*
					Vec2 diff = statePos - GameObjects[obj_id].m_Sprite.getPosition();
					float dist = Maths::Length(diff);

					if (dist > 2.0f)
					{
						GameObjects[obj_id].m_Sprite.setPosition(statePos);
					}
					else if (dist > 0.1f)
					{
						GameObjects[obj_id].m_Sprite.setPosition(diff * 0.1f);
					}
					*/
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