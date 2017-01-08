#include "NetworkManager.h"

#include "types.h"
#include "utils.h"

#include "GameObject.h"
#include "NetState.h"
#include "EventManager.h"
#include "Application.h"
#include "MessageParser.h"
#include "LogFile.h"

const float THREAD_SLEEP = 1 / 60.0f;

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
		else if(status == sf::Socket::Disconnected || status == sf::Socket::Error)
		{
			break;
		}
	}

	//bool c = false;	// Note* these were breakpoint checks to see if all threads were exiting (they were)
}

void send_udp_msg(const std::string& msg, sf::UdpSocket* sock, const std::string& ipAddr, int port)
{
	unsigned char buffer[BUFF_SIZE];
	memset(buffer, 0, BUFF_SIZE);
	memcpy(buffer, msg.c_str(), msg.length());
	sock->send(&buffer, BUFF_SIZE, ipAddr, port);
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

		// Sleep for 1 frame
		std::this_thread::sleep_for(std::chrono::milliseconds(16));
	}

	//bool c = false;
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
			break;
		}
		else
		{
			boundbuffer->Deposit(std::string(buffer));
		}
	}

	//bool c = false;
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

		std::this_thread::sleep_for(std::chrono::milliseconds(16));
	}

	//bool c = false;
}


NetworkManager::NetworkManager() :
	m_MsgParser(nullptr),
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
	m_MsgParser = new MessageParser();
	m_MsgParser->Init();
}

NetworkManager::~NetworkManager()
{
	SAFE_DELETE(m_MsgParser);

	this->disconnectFromServer();
}

bool NetworkManager::connectToServer(int buffSize)
{
	if (!m_TcpSock)
	{
		m_Quit = false;

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

		sf::Socket::Status broadCastStatus = 
			broadcast_socket.send(buff, 32, "192.168.0.255", broadcast_port);

		if (broadCastStatus != sf::Socket::Status::Done)
		{
			WRITE_LOG("Problem sending data through broadcast socket", "error");
			return false;
		}
		
		broadcast_socket.setBlocking(false);

		// Attempt to connect for a few seconds
		float t = 0.0f;
		bool connectedToBcast = false;
		while (t < 1800.0f)
		{
			sf::Socket::Status broadcast_status = broadcast_socket.receive(&rcv_buff, 32, received_bytes, remote_ip, remote_port);
			if (broadcast_status == sf::Socket::Status::Done)
			{
				connectedToBcast = true;
				break;
			}

			t += 0.001f;
		}

		if (!connectedToBcast)
		{
			WRITE_LOG("Could not get a connection to broadcast socket, did not receive any data", "error");
			return false;
		}

		// Attempt to connect to server now
		std::string server_address = remote_ip.toString();
		
		m_TcpSock = new sf::TcpSocket();
		m_TcpSock->setBlocking(true);

		m_BoundedBuffSize = buffSize;

		sf::Socket::Status status = m_TcpSock->connect(server_address, TCP_SERVER_PORT);

		if (status != sf::Socket::Status::Done)
		{
			WRITE_LOG("Could not connect TCP socket", "error");
			SAFE_DELETE(m_TcpSock);
			return false;
		}

		// Now we have connection, store servers address for UDP
		m_ServerIPAddr = server_address;

		// Log : Connected to server at {ip} : {port}
		WRITE_LOG("Connected to server on ip: " + m_ServerIPAddr, "good");

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
		m_UdpSock->bind(0);
		m_LocalUdpPort = m_UdpSock->getLocalPort();
		WRITE_LOG("Attempting to bind upd socket on local port: " + std::to_string(m_LocalUdpPort), "none");

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
		WRITE_LOG("Tried to connect to server twice", "warning");
		return false;
	}
}

void NetworkManager::disconnectFromServer()
{
	// Close all threads
	m_Quit = true;	

	m_Connected = false;
	m_LocalUdpPort = -1;
	m_ServerUdpPort = -1;
	m_ClientId = -1;
	m_BoundedBuffSize = -1;
	m_PlayerHandle = -1;
	m_ServerIPAddr.clear();

	if (m_TcpSock)
	{
		m_TcpSock->disconnect();
	}

	if (m_UdpSock)
	{
		m_UdpSock->unbind();
	}

	if (m_UdpRcvThread)
	{
		if (m_UdpRcvThread->joinable())
		{
			m_UdpRcvThread->join();
		}
	}

	if (m_TcpRcvThread)
	{
		if (m_TcpRcvThread->joinable())
		{
			m_TcpRcvThread->join();
		}
	}

	SAFE_DELETE(m_TcpSock);
	SAFE_DELETE(m_UdpSock);
	SAFE_DELETE(m_UdpRcvBuffer);
	SAFE_DELETE(m_UdpSndBuffer);
	SAFE_DELETE(m_TcpSndBuffer);
	SAFE_DELETE(m_TcpRcvBuffer);

	SAFE_DELETE(m_UdpRcvThread);
	SAFE_DELETE(m_UdpSndThread);
	SAFE_DELETE(m_TcpRcvThread);
	SAFE_DELETE(m_TcpSndThread);
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

				m_MsgParser->Parse(jd);
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

				m_MsgParser->Parse(jd);
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

int NetworkManager::playerExp()const
{
	return m_PlayerExp;
}

const std::string& NetworkManager::serverIpAddr() const
{
	return m_ServerIPAddr;
}
