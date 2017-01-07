#ifndef __MESSAGE_DATA_H__
#define __MESSAGE_DATA_H__

#include <string>
#include <map>
#include "NetworkManager.h"

class MessageParser
{
	typedef void(*MsgFunction)(const rapidjson::Document& data);

public:
	~MessageParser();
	bool Init();
	void Parse(const rapidjson::Document& jd);

private:
	// Functors
	static void tcp_connect(const rapidjson::Document& data);
	static void tcp_server_msg(const rapidjson::Document& data);
	static void tcp_start_game(const rapidjson::Document& data);
	static void tcp_finish_level(const rapidjson::Document& data);
	static void tcp_exp_queery(const rapidjson::Document& data);
	static void tcp_leaderboard_request(const rapidjson::Document& data);

	static void udp_update_object(const rapidjson::Document& data);
	static void upd_view_update(const rapidjson::Document& data);

private:
	std::map<Packet::ID, MsgFunction> m_MsgMap;
};

#endif