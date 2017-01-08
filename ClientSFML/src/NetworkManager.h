#ifndef __NETWORK_MANAGER_H__
#define __NETWORK_MANAGER_H__

#include <string>

#include <SFML\Network.hpp>

#include <rapidjson\document.h>
#include <rapidjson\writer.h>
#include <rapidjson\stringbuffer.h>
#include <rapidjson\pointer.h>

#include "Singleton.h"
#include "BoundedBuffer.h"
#include "NetPackets.h"

#define BUFF_SIZE		512
#define TCP_SERVER_PORT		28000

class MessageParser;

class NetworkManager : public Singleton<NetworkManager>
{
public:
	NetworkManager();
	~NetworkManager();

	bool connectToServer(int bufferSize);
	void disconnectFromServer();
	
	void sendUdp(const std::string& msg);
	void sendTcp(const std::string& msg);
	void fetchAllMessages();

	int playerId()const;
	int clientId()const;
	int playerExp()const;
	bool connected() const;
	const std::string& serverIpAddr() const;

private:
	// We only want the parser to change data in this class
	friend class MessageParser;

	MessageParser* m_MsgParser;

	BoundedBuffer* m_UdpRcvBuffer;
	BoundedBuffer* m_UdpSndBuffer;
	BoundedBuffer* m_TcpRcvBuffer;
	BoundedBuffer* m_TcpSndBuffer;

	sf::TcpSocket* m_TcpSock;
	sf::UdpSocket* m_UdpSock;

	std::thread* m_UdpRcvThread;
	std::thread* m_UdpSndThread;
	std::thread* m_TcpRcvThread;
	std::thread* m_TcpSndThread;

	std::string m_ServerIPAddr;

	int m_LocalUdpPort;
	int m_ServerUdpPort;
	int m_ClientId;
	int m_BoundedBuffSize;
	int m_PlayerHandle;
	int m_PlayerExp = 0;
	bool m_Quit;
	bool m_Connected;
};

#endif