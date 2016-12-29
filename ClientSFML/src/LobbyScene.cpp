#include "LobbyScene.h"

#include "Screen.h"
#include "KeyEvent.h"
#include "Application.h"
#include "utils.h"
#include "EventManager.h"
#include "utils.h"
#include "NetworkManager.h"

LobbyScene::LobbyScene() :
	IScene(),
	m_TextObject(nullptr)
{
}

LobbyScene::~LobbyScene()
{
}

void LobbyScene::HandleInput(int k, int a)
{
	// Move up through menu
	if ((k == sf::Keyboard::W || k == sf::Keyboard::Up) && a == RELEASE)
	{
	}

	// Move down through menu
	if ((k == sf::Keyboard::S || k == sf::Keyboard::Down) && a == RELEASE)
	{
	}

	// Cancel
	if ((k == CANCEL_BUTTON) && a == RELEASE)
	{
	}

	// Confirm selection
	if ((k == CONFIRM_BUTTON) && a == RELEASE)
	{
		// Hardcode for now . But should be stored in message manaher
		std::string name = "alex" + util::to_str(rando::RandomRange(0, 32657));

		auto nm = NetworkManager::Instance();

		if (!nm->connected())
		{
			if (!nm->connectToServer(TCP_PORT, 35))
			{
				// TODO LOG
				return;
			}

			m_UserInfoStr = "Press Space When Ready to Start";
		}
	}

	if ((k == sf::Keyboard::Space) && a == RELEASE)
	{
		NetworkManager* nm = NetworkManager::Instance();

		// TODO : Would check other clients are ready to, possibly in thr client struct map
		//if (mm->Connected() && mm->Registerd() && mm->InMultiplayerMode())
		//{
		//	int levelToLoad = 0;
		//	EventManager::Instance()->SendEvent(events::GameEventID::SetLevelToLoad, &levelToLoad);
			ChangeState(ID::States::Game);
			if (!nm->connectToServer(TCP_PORT, 35))
			{
				// TODO LOG
				return;
			}
		//}
	}
}

bool LobbyScene::OnCreate(Context* context)
{
	m_context = context;
	m_ConfigOptions.resize(4);
	m_UserInfoStr = "";

	if (!m_TextObject)
		m_TextObject = new sf::Text();
	m_TextObject->setFont(m_context->fonts->Get(ID::Font::ARIEL));
	m_TextObject->setCharacterSize(18);

	return true;
}

void LobbyScene::OnEntry()
{
	CONFIRM_BUTTON = KeyBindings::KeyBindingList[KeyBindings::KeyBinds::UseItemBind].key;
	CANCEL_BUTTON = KeyBindings::KeyBindingList[KeyBindings::KeyBinds::InteractItemBind].key;
}

bool LobbyScene::OnUpdate(const sf::Time& dt)
{
	return true;
}

void LobbyScene::OnRender()
{
	const auto& time_now = Application::Instance()->TotalGameTime();

	const float sinx = sinf(time_now) * 0.005f;
	const float cosy = cosf(time_now) * 0.005f;
	const sf::Color white = sf::Color::White;
	const float screenW = (float)Screen::Instance()->ScreenWidth();
	const float screenH = (float)Screen::Instance()->ScreenHeight();
	const Vec2& vs = m_context->view->getSize();

	float offset = -vs.y * 0.05f;

	// Title
	m_TextObject->setCharacterSize(32);
	Screen::RenderText(m_context->window, m_TextObject, "Lobby", Vec2(vs.x * 0.5f, vs.y * 0.2f), Screen::AlignCentre);

	// Server Info
	NetworkManager* netMan = NetworkManager::Instance();
	if (netMan)
	{
		m_ConfigOptions[ServerInfo::CurrentIP] = "IP Address: " + netMan->m_ServerIPAddr;
		m_ConfigOptions[ServerInfo::Connected] = "Connected to server: " + util::bool_to_str(netMan->connected());
		m_ConfigOptions[ServerInfo::Registered] = "Registerd with server: " + util::bool_to_str(netMan->m_Connected);//TODO
		m_ConfigOptions[ServerInfo::NumPlayersOnline] = "Other Players Online: ";
	}

	float cy = 0.8f;
	/*
	// TODO : Store our name and render it
	m_Window->RenderText("Other Players", 0.0, cy, 1.0f, FontAlign::Centre);
	// Render other clients on server
	auto clients = msgMan->GetClients();
	for (auto c = clients.begin(); c != clients.end(); ++c)
	{
		cy -= 0.1f;
		m_Window->RenderText(c->first, 0.0, cy, 1.0f, FontAlign::Centre);
	}
	*/

	// Render Selectable text options
	for (int i = 0; i < ServerInfo::ServerInfoSize; ++i)
	{
		Screen::RenderText(m_context->window,
			m_TextObject,
			m_ConfigOptions[i],
			Vec2(vs.x * 0.5f, vs.y * 0.5f + offset),
			Screen::AlignCentre
		);

		offset += (vs.y * 0.05f);
	}

	// Other info
	Screen::RenderText(m_context->window, m_TextObject, m_UserInfoStr, Vec2(vs.x * 0.5f, vs.y * 0.4f), Screen::AlignCentre);
	Screen::RenderText(m_context->window, m_TextObject, "[" + KeyBindings::GetStringFromKey(CONFIRM_BUTTON) + "] to make selection", Vec2(vs.x * 0.5f, vs.y * 0.8f), Screen::AlignCentre);
}

void LobbyScene::OnExit(ID::States nextState)
{
}

void LobbyScene::Close()
{
	SAFE_DELETE(m_TextObject);
}
