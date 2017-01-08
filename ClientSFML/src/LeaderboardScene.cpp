#include "LeaderboardScene.h"
#include "Screen.h"
#include "SceneGraph.h"
#include "ResourceHolder.h"
#include "EventManager.h"
#include "NetworkManager.h"

LeaderboardScene::LeaderboardScene() :
	IScene(),
	m_TextObject(nullptr)
{
}

LeaderboardScene::~LeaderboardScene()
{
}

void LeaderboardScene::HandleEvent(Event* ev)
{
	switch (ev->GetID())
	{
	case EventID::Net_DatabaseRequest:
	{
		std::string* data = (std::string*)ev->GetData();

		if (data)
		{
			m_LeaderBoardString = *data;
		}
	}
		break;
	default:
		break;
	}
}

bool LeaderboardScene::OnCreate(Context* const con)
{
	m_context = con;
	m_UserInfoStr = "Press Any Key to return";

	AttachEvent(EventID::Net_DatabaseRequest, *this);

	if (!m_TextObject)
		m_TextObject = new sf::Text();
	m_TextObject->setFont(m_context->fonts->Get(ID::Font::ARIEL));

	return true;
}

void LeaderboardScene::OnEntry()
{
	// Request Leaderboards
	NetworkManager* nm = NetworkManager::Instance();
	if (nm->connected())
	{
		// Send TCP to start game
		rapidjson::StringBuffer g_sBuffer;
		rapidjson::Writer<rapidjson::StringBuffer> g_Writer(g_sBuffer);

		g_Writer.StartObject();
		g_Writer.Key("name");
		g_Writer.Int((int)Packet::ID::OUT_TCP_LeaderboardRequest);
		g_Writer.Key("id");
		g_Writer.Uint(NetworkManager::Instance()->clientId());
		g_Writer.EndObject();

		NetworkManager::Instance()->sendTcp(g_sBuffer.GetString());
	}
	else
	{
		m_LeaderBoardString = "NOT CONNECTED TO SERVER";
	}
}

bool LeaderboardScene::OnUpdate(const sf::Time& dt)
{
	return true;
}

void LeaderboardScene::OnRender()
{
	const Vec2& vs = m_context->view->getSize();

	// Title
	m_TextObject->setCharacterSize(32);
	Screen::RenderText(m_context->window, m_TextObject, "Leaderboards", Vec2(vs.x * 0.5f, vs.y * 0.1f), Screen::AlignCentre);

	m_TextObject->setCharacterSize(16);
	Screen::RenderText(m_context->window, m_TextObject, m_LeaderBoardString, Vec2(vs.x * 0.5f, vs.y * 0.2f), Screen::AlignCentre);

	m_TextObject->setCharacterSize(12);
	Screen::RenderText(m_context->window, m_TextObject, m_UserInfoStr, Vec2(vs.x * 0.5f, vs.y * 0.85f), Screen::AlignCentre);
}

void LeaderboardScene::OnExit(ID::States nextState)
{

}

void LeaderboardScene::Close()
{
	DetachEvent(EventID::Net_DatabaseRequest, *this);
	SAFE_DELETE(m_TextObject);
}

void LeaderboardScene::LateRender()
{

}

void LeaderboardScene::HandleInput(int k, int a)
{
	if (a == 0)
	{
		ChangeState(ID::States::Lobby);
	}
}