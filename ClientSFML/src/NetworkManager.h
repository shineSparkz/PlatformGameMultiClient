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
#define TCP_PORT		28000
#define UDP_PORT		23000

class MessageParser;

class NetworkManager : public Singleton<NetworkManager>
{
public:
	NetworkManager();
	~NetworkManager();

	bool connectToServer(int onPort, int bufferSize);
	void sendUdp(const std::string& msg);
	void sendTcp(const std::string& msg);

	void fetchAllMessages();

	int playerId()const;
	int clientId()const;
	bool connected() const;

private:
	friend class LobbyScene;//TODO
	friend class MessageParser;//TODO

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
	bool m_Quit;
	bool m_Connected;
};

#endif