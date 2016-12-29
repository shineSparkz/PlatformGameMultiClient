
#include "Application.h"

#include "utils.h"
#include "GameScene.h"
#include "EventManager.h"
#include "Screen.h"
#include "KeyEvent.h"
#include "NetworkManager.h"
#include "TitleScene.h"
#include "LobbyScene.h"
//#include "PauseScene.h"
//#include "WorldMapScene.h"
//#include "InventoryScene.h"
//#include "SoundPlayer.h"
//#include "ModFile.h"
//#include "Config.h"


const sf::Time FRAME_TIME = sf::seconds(1 / 60.0f);
const std::string FONT_FILE = "resources/fonts/";

Application::Application() :
	m_TextureHolder(),
	m_FontHolder(),
	m_LogFile(),
	m_ClockTotal(),
	m_Event(),
	m_AppName(""),
	m_Screen(nullptr),
	m_NetManager(nullptr),
	m_Window(nullptr),
	m_SceneGraph(nullptr),
	m_TextObj(nullptr),
	m_ElapsedFadeTime(0.0f),
	m_AltKey(false),
	m_F4Key(false)
{
}

Application::~Application()
{
	this->Close();
}

bool Application::Init(const std::string& app_name)
{
	m_AppName = app_name;
	m_ClockTotal.restart();

	m_LogFile.CreateLogFile("resources/logs/", "debug_log_file.html");
	
	// Load all settings files
	KeyBindings::LoadDefaults();
	
	//ModFileIni::ParseModIni();
	//ConfigFile::LoadConfig();
	
	//this->m_ShowFPS = ConfigFile::currentConfigInMemory.ShowFPS;
	
	new Screen();
	m_Screen = Screen::Instance();
	//m_Window = m_Screen->Init(ConfigFile::currentConfigInMemory.FullScreen,
	//	ConfigFile::currentConfigInMemory.VideoMode,
	//	ConfigFile::currentConfigInMemory.Vsync);
	m_Window = m_Screen->Init(false, 6, true);	// TODO SORT THIS

	// Singletons
	//SoundPlayer* sp = new SoundPlayer();
	//if (!sp->LoadAllSounds()) return false;
	//sp->SetMusicVolume(ConfigFile::currentConfigInMemory.MusicVolume);
	//sp->SetSFXVolume(ConfigFile::currentConfigInMemory.SFXVolume);
	
	// Create Events
	new EventManager();
	for ( int32 i = 0; i < EventID::NumGameEvents; ++i )
	{
		RegisterEvent(static_cast<EventID>(i) );
	}

	// Load Textures
	if (m_TextureHolder.Load(ID::Texture::LevelExit, "resources/textures/exit.png") != GE_OK) return false;
	if(m_TextureHolder.Load(ID::Texture::Bkgrnd_BlackTiles, "resources/textures/bg_black_tiles.png") != GE_OK) return false;
	if (m_TextureHolder.Load(ID::Texture::Bkgrnd_GreenGraveyard, "resources/textures/bg_green_graveyard.png") != GE_OK) return false;
	if (m_TextureHolder.Load(ID::Texture::Bkgrnd_PurpleCave, "resources/textures/bg_purple_cave.png") != GE_OK) return false;
	if (m_TextureHolder.Load(ID::Texture::Bkgrnd_RedMtn, "resources/textures/bg_red_mtn.png") != GE_OK) return false;
	if (m_TextureHolder.Load(ID::Texture::Bkgrnd_Pyramid, "resources/textures/bg_pyramid.png") != GE_OK) return false;
	if (m_TextureHolder.Load(ID::Texture::Slime, "resources/textures/slime.png") != GE_OK) return false;
	if (m_TextureHolder.Load(ID::Texture::LevelSelectScreenArt, "resources/textures/dc_art.png") != GE_OK) return false;
	if (m_TextureHolder.Load(ID::Texture::TitleScreenArt, "resources/textures/pause.png") != GE_OK) return false;
	if (m_TextureHolder.Load(ID::Texture::PauseScreenArt, "resources/textures/title.png") != GE_OK) return false;
	if (m_TextureHolder.Load(ID::Texture::DestructableWall, "resources/textures/box.png") != GE_OK) return false;
	if (m_TextureHolder.Load(ID::Texture::Boulder, "resources/textures/boulder.png") != GE_OK) return false;
	if (m_TextureHolder.Load(ID::Texture::DissappearPlatform, "resources/textures/dis_plat.png") != GE_OK) return false;
	if (m_TextureHolder.Load(ID::Texture::GoldSkull, "resources/textures/goldskull.png") != GE_OK) return false;
	if (m_TextureHolder.Load(ID::Texture::SkullBg, "resources/textures/skull_bg.png") != GE_OK) return false;
	if (m_TextureHolder.Load(ID::Texture::BloodMtn_TileSet, "resources/textures/red_tileset.png") != GE_OK) return false;
	if (m_TextureHolder.Load(ID::Texture::White_TileSet, "resources/textures/white_tileset.png") != GE_OK) return false;
	if (m_TextureHolder.Load(ID::Texture::Player, "resources/textures/player.png") != GE_OK) return false;
	if (m_TextureHolder.Load(ID::Texture::Spikes, "resources/textures/spikes.png") != GE_OK) return false;
	if (m_TextureHolder.Load(ID::Texture::SkullCurrency, "resources/textures/skull_currency.png") != GE_OK) return false;
	if (m_TextureHolder.Load(ID::Texture::Fireball, "resources/textures/fireball.png") != GE_OK) return false;
	if (m_TextureHolder.Load(ID::Texture::BlueMinionEnemy, "resources/textures/minion_45x66.png") != GE_OK) return false;
	if (m_TextureHolder.Load(ID::Texture::MageEnemy, "resources/textures/mage_enemy122x110.png") != GE_OK) return false;
	if (m_TextureHolder.Load(ID::Texture::DiscipleEnemy, "resources/textures/disciple_enemy45x51.png") != GE_OK) return false;
	if (m_TextureHolder.Load(ID::Texture::TaurusEnemy, "resources/textures/gnu-120x100.png") != GE_OK) return false;
	if (m_TextureHolder.Load(ID::Texture::ShadowEnemy, "resources/textures/shadow-80x70.png") != GE_OK) return false;
	if (m_TextureHolder.Load(ID::Texture::GreemHeadEnemy, "resources/textures/andromalius-57x88.png") != GE_OK) return false;
	if (m_TextureHolder.Load(ID::Texture::ProjectileEnemy, "resources/textures/mage_bullet_13x13.png") != GE_OK) return false;
	if (m_TextureHolder.Load(ID::Texture::CheckpointTorch, "resources/textures/checkpoint.png") != GE_OK) return false;
	if (m_TextureHolder.Load(ID::Texture::Potions, "resources/textures/potions.png") != GE_OK) return false;
	if (m_TextureHolder.Load(ID::Texture::WorldMapArt, "resources/textures/25.png") != GE_OK) return false;

	// Load Fonts
	if(m_FontHolder.Load(ID::Font::ARIEL, FONT_FILE + "arial.ttf") != GE_OK) return false;
	if(m_FontHolder.Load(ID::Font::GOTHIC, FONT_FILE + "olde_english.ttf") != GE_OK) return false;

	if (!m_TextObj)
	{
		m_TextObj = new sf::Text();
		m_TextObj->setFont(m_FontHolder.Get(ID::Font::ARIEL));
		m_TextObj->setCharacterSize(24);
	}

	// Create States ( this shouldn't be here)
	m_SceneGraph = new SceneGraph(new IScene::Context(*m_Window, m_Screen->m_MainView, m_TextureHolder, m_FontHolder));
	GameScene gam;
	TitleScene tit;
	LobbyScene lob;
	//PauseState pas;
	//WorldMapState wm;
	//InventoryState inv;

	m_SceneGraph->RegisterState(ID::States::Game, gam);
	m_SceneGraph->RegisterState(ID::States::Title, tit);
	m_SceneGraph->RegisterState(ID::States::Lobby, lob);
	//m_SceneGraph->RegisterState(ID::States::WorldMap, wm);
	//m_SceneGraph->RegisterState(ID::States::Inventory, inv);

	m_SceneGraph->Init();
	ChangeState(ID::States::Title);

	if (!m_NetManager)
	{
		m_NetManager = new NetworkManager();

		//m_NetManager->connectToServer(TCP_PORT, 35);
		// TODO : print if error 
	}

	return true;
}

void Application::Run()
{
	sf::Clock clock;
	sf::Time timeSinceLastUpdate = sf::Time::Zero;

	// Fixed delta time game loop
	while ( m_Window->isOpen() )
	{
		ProcessEvents();
		timeSinceLastUpdate += clock.restart();

		while ( timeSinceLastUpdate > FRAME_TIME )
		{
			// Drain Messages
			if (m_NetManager->connected())
				m_NetManager->fetchAllMessages();

			if (m_Screen->IsFading())
				m_ElapsedFadeTime += FRAME_TIME.asSeconds();
			else
			{
				m_ElapsedFadeTime = 0.0f;
			}
			
			timeSinceLastUpdate -= FRAME_TIME;
			m_SceneGraph->UpdateCurrentState(FRAME_TIME);
		}

		Render();

		if(m_AltKey && m_F4Key )
			m_Window->close();
	}
}

float Application::TotalGameTime() const
{
	return m_ClockTotal.getElapsedTime().asSeconds();
}

void Application::PauseUnpauseGame()
{
	// Check that we aren't actually in pause state
	if (GetActiveState() != ID::States::Pause)
		ChangeState(ID::States::Pause);
	else
		ChangeState(GetPrevState());
}

void Application::ForceShutDown()
{
	this->m_AltKey = this->m_F4Key = true;
}

const std::string& Application::GetAppName() const
{
	return m_AppName;
}

void Application::ProcessEvents()
{
	while ( m_Window->pollEvent( m_Event ) )
	{
		switch ( m_Event.type )
		{
		case sf::Event::KeyPressed:
			this->HandleInput(m_Event.key.code, PRESS);
			break;
		case sf::Event::KeyReleased:
			this->HandleInput(m_Event.key.code, RELEASE);
			break;
		case sf::Event::Closed:
			m_Window->close();
			break;
		default:
			break;
		}
	}
}

void Application::Render()
{
	// Clear screen
	m_Window->clear();

	if (!m_Screen->IsFading())
	{
		m_Screen->SetGameViewport();
		m_Window->setView(m_Screen->GetMainView());

		// Render Current State
		m_SceneGraph->RenderCurrentState(0);
		
		// Render border
		m_Screen->DrawBorders(m_Window);

		// Full viewport for UI
		m_Screen->SetFullViewport();
		m_Window->setView(m_Screen->GetMainView());
		m_SceneGraph->RenderCurrentLateState();

		if (m_ShowFPS)
		{
			DisplayFPS(m_ClockTotal.getElapsedTime().asSeconds());
		}
	}
	else
	{
		m_Screen->FadeScreen(m_ElapsedFadeTime);
	}

	// Finally display the frame
	m_Window->display();
}

void Application::Close()
{
	SAFE_DELETE(m_Window);
	SAFE_DELETE(m_TextObj);
	SAFE_CLOSE(m_SceneGraph);

	m_LogFile.WriteLog("Application has Shut Down gracefully", "good");

	//if ( SoundPlayer::Instance() )
	//{
	//	delete SoundPlayer::Instance();
	//}

	if ( m_Screen )
	{
		delete Screen::Instance();
	}

	SAFE_DELETE(m_NetManager);

	delete EventManager::Instance();
}

void Application::DisplayFPS(float time)
{
	//FPS
	static float previous_seconds = time;
	static int frame_count;
	float current_seconds = time;
	float elapsed_seconds = current_seconds - previous_seconds;
	static float average_frames = 0;

	if (elapsed_seconds <= 1.0f)
	{
		++frame_count;
	}
	else
	{
		previous_seconds = current_seconds;
		average_frames = (float)frame_count / elapsed_seconds + 2;
		frame_count = 0;
	}
	const Vec2& vp = Screen::Instance()->GetViewPos();
	const Vec2& vs = Screen::Instance()->GetMainView().getSize();

	Screen::RenderText(m_Window, m_TextObj, "FPS: " + util::to_str(average_frames), Vec2(vp.x + (vs.x - 16.0f), vp.y + (vs.y - 32.0f)), Screen::AlignRight);
}

void Application::HandleInput(int k, int a)
{
	for (std::vector<KeyBindings::KeyState>::iterator i = KeyBindings::KeyBindingList.begin();
		i != KeyBindings::KeyBindingList.end(); ++i)
	{
		if (k == (*i).key)
		{
			(*i).pressed = a;
			break;
		}
	}

	if (k == KeyBindings::ReservedMenuKey && a == RELEASE)// &&m_AppHasFocus)
	{
		if (m_Event.key.code == sf::Keyboard::F4)
			m_F4Key = true;
		if (m_Event.key.code == sf::Keyboard::LAlt)
			m_AltKey = true;

		if (GetActiveState() != ID::States::Pause)
		{
			//SoundPlayer::Instance()->PlayASound(ID::Audio::PauseSound);
			//ChangeState(ID::States::Pause);
		}
		else
		{
			//if (!PauseState::IsBindingKeys())
			//{
			//	SoundPlayer::Instance()->PlayASound(ID::Audio::UnPauseSound);
			//	ChangeState(GetPrevState());
			//}
			//else
			//{
				this->m_SceneGraph->HandleInput(k, a);
			//}
		}
	}
	else if (k == KeyBindings::ReservedPauseKey && a == RELEASE && m_SceneGraph->GetActiveState() == ID::States::Game)
	{
		//int p = !m_AppHasFocus;
		//EventManager::Instance()->SendEvent(events::SystemEventID::WindowFocus, &p);
	}
	else
	{
		//if (m_AppHasFocus)
		this->m_SceneGraph->HandleInput(k, a);
	}
}



