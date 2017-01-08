#include "LobbyScene.h"

#include "Screen.h"
#include "KeyEvent.h"
#include "Application.h"
#include "utils.h"
#include "EventManager.h"
#include "utils.h"
#include "NetworkManager.h"
#include "PlayerCredentialsBin.h"

LobbyScene::LobbyScene() :
	IScene(),
	m_TextObject(nullptr)
{
}

LobbyScene::~LobbyScene()
{
}

void LobbyScene::HandleEvent(Event* ev)
{
	switch (ev->GetID())
	{
	case EventID::Net_StartGameCallback:
	{
		ChangeState(ID::States::Game);
	}
	break;
	case EventID::Net_ServerMsgCallback:
		m_UserInfoStr = *(std::string*)ev->GetData();
		break;
	default:
		break;
	}
}

void LobbyScene::HandleInput(int k, int a)
{
	if (a == RELEASE)
	{
		if (m_InputtingName)
		{
			// Quit
			if ((k == sf::Keyboard::Return) && a == RELEASE)
			{
				m_UserInfoStr = "Select Option";
				m_InputtingName = false;
			}
			// Delete Letter
			else if (k == sf::Keyboard::BackSpace && a == RELEASE)
			{
				if(!m_UserName.empty())
					m_UserName.pop_back();
			}
			else if(k == sf::Keyboard::Delete && a == RELEASE)
			{
				m_UserName.clear();
			}
			// Enter name
			else
			{
				if (a == RELEASE)
				{
					if (k >= sf::Keyboard::A && k <= sf::Keyboard::Num9)
					{
						if (m_UserName.size() < MAX_NAME_CHARS)
						{
							m_UserName += KeyBindings::GetStringFromKey(k);
						}
						else
						{
							m_UserInfoStr = "Too many characters!";
						}
					}
					else
					{
						m_UserInfoStr = "Invalid Key!";
					}
				}
			}
		}
		else if (m_InputtingPassword)
		{
			// Quit
			if ((k == sf::Keyboard::Return) && a == RELEASE)
			{
				m_UserInfoStr = "Select Option";
				m_InputtingPassword = false;
			}
			// Delete Letter
			else if (k == sf::Keyboard::BackSpace && a == RELEASE)
			{
				if(!m_PassWordName.empty())
					m_PassWordName.pop_back();
			}
			else if (k == sf::Keyboard::Delete && a == RELEASE)
			{
				m_PassWordName.clear();
			}
			// Enter name
			else
			{
				if (a == RELEASE)
				{
					if (k >= sf::Keyboard::A && k <= sf::Keyboard::Num9)
					{
						if (m_PassWordName.size() < MAX_NAME_CHARS)
						{
							m_PassWordName += KeyBindings::GetStringFromKey(k);
						}
						else
						{
							m_UserInfoStr = "Too many characters!";
						}
					}
					else
					{
						m_UserInfoStr = "Invalid Key!";
					}
				}
			}
		}
		else
		{
			// Move up through menu
			if ((k == sf::Keyboard::Up || k == sf::Keyboard::W))
			{
				if (m_MenuState != Pending)
				{
					//SoundPlayer::Instance()->PlayASound(ID::Audio::UI_Select);
					int selopt = (int)m_Options;
					--selopt;
					m_Options = (LobbyOptions)selopt;

					if (m_Options < LobbyOptions::Connect)
						m_Options = LobbyOptions::Return;
				}
			}

			if ((k == sf::Keyboard::S || k == sf::Keyboard::Down))
			{
				if (m_MenuState != Pending)
				{
					//SoundPlayer::Instance()->PlayASound(ID::Audio::UI_Select);
					int selopt = (int)m_Options;
					++selopt;
					m_Options = (LobbyOptions)selopt;
					if (m_Options > LobbyOptions::Return)
						m_Options = LobbyOptions::Connect;
				}
			}

			// Cancel
			if (k == CANCEL_BUTTON)
			{
				if (m_MenuState == Pending)
				{
					//SoundPlayer::Instance()->PlayASound(ID::Audio::SelectSound);
					m_MenuState = Normal;
					m_UserInfoStr = "";
				}
			}

			// Confirm selection
			if (k == CONFIRM_BUTTON)
			{
				switch (m_Options)
				{
				case LobbyOptions::Connect:
					this->ConnectToServer();
					break;
				case LobbyOptions::EnterUserName:
					m_UserInfoStr = "Enter Name : [Return] finish : [BkSpc] delete : [Del] clear";
					m_InputtingName = true;
					break;
				case LobbyOptions::EnterPassword:
					m_UserInfoStr = "Enter Password : [Return] finish : [BkSpc] delete : [Del] clear";
					m_InputtingPassword = true;
					break;
				case LobbyOptions::CreateAccount:
					this->CreateUserAccount();
					break;
				case LobbyOptions::Login:
					this->LoginToServer();
					break;
				case LobbyOptions::StartGame:
					this->AttemptToStartGame();
					break;
				case LobbyOptions::Return:
					if (NetworkManager::Instance()->connected())
					{
						//NetworkManager::Instance()->disconnectFromServer();
					}
					ChangeState(ID::States::Title);
					break;
				default:
					break;
				}
			}
		}
	}
}

void LobbyScene::CreateUserAccount()
{
	if (m_UserName == "" || m_UserName.empty())
	{
		m_UserInfoStr = "You need to create or load user name first";
	}
	else
	{
		NetworkManager* nm = NetworkManager::Instance();

		if (nm->connected())
		{
			// Send data to server
			m_UserInfoStr = "Attempting to create account with server......";

			// Send TCP to create account with the string they have entered
			rapidjson::StringBuffer g_sBuffer;
			rapidjson::Writer<rapidjson::StringBuffer> g_Writer(g_sBuffer);

			g_Writer.StartObject();
			g_Writer.Key("name");
			g_Writer.Int((int)Packet::ID::OUT_TCP_CreateAccount);
			g_Writer.Key("userName");
			g_Writer.String(m_UserName.c_str());
			g_Writer.Key("password");
			g_Writer.String(m_PassWordName.c_str());
			g_Writer.Key("id");
			g_Writer.Uint(NetworkManager::Instance()->clientId());
			g_Writer.EndObject();

			NetworkManager::Instance()->sendTcp(g_sBuffer.GetString());
		}
		else
		{
			m_UserInfoStr = "Need server connection to create account";
		}
	}
}

void LobbyScene::LoginToServer()
{
	if (m_UserName == "" || m_UserName.empty())
	{
		m_UserInfoStr = "You need to create username first";
	}
	else if (m_PassWordName == "" || m_PassWordName.empty())
	{
		m_UserInfoStr = "You need to create password first";
	}
	else
	{
		NetworkManager* nm = NetworkManager::Instance();

		// TODO : Need to check if we already logged in
		if (nm->connected())
		{
			// Send data to server
			m_UserInfoStr = "Attempting to connect to server......";

			// Send TCP to create account with the string they have entered
			rapidjson::StringBuffer g_sBuffer;
			rapidjson::Writer<rapidjson::StringBuffer> g_Writer(g_sBuffer);

			g_Writer.StartObject();
			g_Writer.Key("name");
			g_Writer.Int((int)Packet::ID::OUT_TCP_Login);
			g_Writer.Key("userName");
			g_Writer.String(m_UserName.c_str());
			g_Writer.Key("password");
			g_Writer.String(m_PassWordName.c_str());
			g_Writer.Key("id");
			g_Writer.Uint(NetworkManager::Instance()->clientId());
			g_Writer.EndObject();

			NetworkManager::Instance()->sendTcp(g_sBuffer.GetString());

			// Save these credentials here everytime they try and log in
			PlayerCredentialsFile::SaveFile((int)m_UserName.length(), m_UserName.c_str(), (int)m_PassWordName.length(), m_PassWordName.c_str());
		}
		else
		{
			m_UserInfoStr = "Need a server connection to login";
		}
	}
}

void LobbyScene::ConnectToServer()
{
	auto nm = NetworkManager::Instance();

	if (!nm->connected())
	{
		if (!nm->connectToServer(50))
		{
			m_UserInfoStr = "Failed to connect to server";
			// TODO LOG
			return;
		}

		m_UserInfoStr = "Press Space When Ready to Start Game";
	}
}

void LobbyScene::AttemptToStartGame()
{
	NetworkManager* nm = NetworkManager::Instance();
	if (nm->connected())
	{
		// Send TCP to start game
		rapidjson::StringBuffer g_sBuffer;
		rapidjson::Writer<rapidjson::StringBuffer> g_Writer(g_sBuffer);

		g_Writer.StartObject();
		g_Writer.Key("name");
		g_Writer.Int((int)Packet::ID::OUT_TCP_StartGame);
		g_Writer.Key("id");
		g_Writer.Uint(NetworkManager::Instance()->clientId());
		g_Writer.EndObject();

		NetworkManager::Instance()->sendTcp(g_sBuffer.GetString());
	}
	else
	{
		m_UserInfoStr = "Failed, no server connection";
	}	
}

bool LobbyScene::OnCreate(Context* const context)
{
	m_context = context;

	AttachEvent(EventID::Net_StartGameCallback, *this);
	AttachEvent(EventID::Net_ServerMsgCallback, *this);

	//m_ConfigOptions.resize(4);
	//m_UserInfoStr = "Press Enter to type login details";

	m_OptionStrings[0] = "Connect to Server";
	m_OptionStrings[1] = "Enter User Name";
	m_OptionStrings[2] = "Enter Password";
	m_OptionStrings[3] = "Create Account";
	m_OptionStrings[4] = "Login to Server";
	m_OptionStrings[5] = "Start Game";
	m_OptionStrings[6] = "Return";

	if (!m_TextObject)
		m_TextObject = new sf::Text();
	m_TextObject->setFont(m_context->fonts->Get(ID::Font::ARIEL));
	m_TextObject->setCharacterSize(18);

	return true;
}

void LobbyScene::OnEntry()
{
	// Re-centre the view
	const Vec2& vs = Screen::Instance()->GetMainView().getSize();
	Screen::Instance()->SetViewCentre(Vec2(vs.x * 0.5f, vs.y * 0.5f));

	// Pre load the game level data
	SendEvent(EventID::LoadLevelData, nullptr);

	m_UserInfoStr = "Select Option";
	CONFIRM_BUTTON = KeyBindings::KeyBindingList[KeyBindings::KeyBinds::UseItemBind].key;
	CANCEL_BUTTON = KeyBindings::KeyBindingList[KeyBindings::KeyBinds::InteractItemBind].key;
	m_MenuState = Normal;

	// Attempt to load credentials file
	PlayerCredentialsFile pcf;
	PlayerCredentials* pc = pcf.LoadFile();
	if (pc)
	{
		if (pc->name)
			m_UserName = pc->name;
		if (pc->password)
			m_PassWordName = pc->password;
	}
}

bool LobbyScene::OnUpdate(const sf::Time& dt)
{
	return true;
}

void LobbyScene::OnRender()
{
	const auto& time_now = Application::Instance()->TotalGameTime();
	const Vec2& vs = m_context->view->getSize();

	const float sinx = sinf(time_now) * 0.005f;
	const float cosy = cosf(time_now) * 0.005f;
	const float screenW = (float)Screen::Instance()->ScreenWidth();
	const float screenH = (float)Screen::Instance()->ScreenHeight();
	float offset = -vs.y * 0.05f;

	// Title
	m_TextObject->setCharacterSize(32);
	Screen::RenderText(m_context->window, m_TextObject, "Lobby", Vec2(vs.x * 0.5f, vs.y * 0.1f), Screen::AlignCentre, 0, sf::Color::Red);

	// Server Info
	NetworkManager* netMan = NetworkManager::Instance();
	
	m_TextObject->setCharacterSize(18);
	for (int i = 0; i < LobbyOptions::NumLobbyOptions; ++i)
	{
		// Move the selected one and change it's colour
		bool cs = (i == (int)m_Options);

		Screen::RenderText(m_context->window, m_TextObject,
			m_OptionStrings[i],
			Vec2(cs ? (vs.x * 0.2f) + sinx : vs.x * 0.2f, cs ? (vs.y * 0.5f) + offset + cosy : (vs.y * 0.5f) + offset),
			Screen::AlignLeft,
			sf::Text::Style::Regular,
			cs ? sf::Color::White : sf::Color(128, 128, 128, 255)
			);

		offset += (vs.y * 0.05f);
	}

	if (netMan)
	{
		m_ServerInfoStrings[ServerInfo::CurrentIP] = "Server IP: " + netMan->serverIpAddr();
		m_ServerInfoStrings[ServerInfo::Connected] = "Connected to server: " + util::bool_to_str(netMan->connected());
		m_ServerInfoStrings[ServerInfo::UserName] = "UserName: " + m_UserName;
		m_ServerInfoStrings[ServerInfo::Password] = "password: " + m_PassWordName;
		m_ServerInfoStrings[ServerInfo::Experience] = "Experience: " + util::to_str(netMan->playerExp());
	}

	offset = -vs.y * 0.05f;

	// Render Selectable text options
	for (int i = 0; i < ServerInfo::ServerInfoSize; ++i)
	{
		Screen::RenderText(m_context->window,
			m_TextObject,
			m_ServerInfoStrings[i],
			Vec2(vs.x * 0.8f, vs.y * 0.5f + offset),
			Screen::AlignRight,
			0,
			sf::Color::Blue
		);

		offset += (vs.y * 0.05f);
	}

	// Other info
	Screen::RenderText(m_context->window, m_TextObject, m_UserInfoStr, Vec2(vs.x * 0.5f, vs.y * 0.3f), Screen::AlignCentre, 0, sf::Color::Green);
	Screen::RenderText(m_context->window, m_TextObject, "[" + KeyBindings::GetStringFromKey(CONFIRM_BUTTON) + "] to make selection", Vec2(vs.x * 0.5f, vs.y * 0.8f), Screen::AlignCentre);
}

void LobbyScene::OnExit(ID::States nextState)
{
}

void LobbyScene::Close()
{
	DetachEvent(EventID::Net_StartGameCallback, *this);
	DetachEvent(EventID::Net_ServerMsgCallback, *this);
	SAFE_DELETE(m_TextObject);
}
