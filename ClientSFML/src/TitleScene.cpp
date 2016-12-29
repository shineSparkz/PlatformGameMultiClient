#include "TitleScene.h"

#include "ResourceHolder.h"
#include "Screen.h"
#include "KeyEvent.h"
#include "Application.h"

const float k_bg_scale = 0.8f;

TitleScene::TitleScene() :
	IScene(),
	m_BgSprite(nullptr),
	m_TextObject(nullptr),
	m_OptionStrings(),
	m_UserInfoStr(""),
	m_Options(Options::Continue),
	m_MenuState(MenuState::Normal),
	CONFIRM_BUTTON(0),
	CANCEL_BUTTON(0)
{

}

TitleScene::~TitleScene()
{
}

bool TitleScene::OnCreate(Context* const con)
{
	m_context = con;

	if (!m_BgSprite)
		m_BgSprite = new sf::Sprite();
	m_BgSprite->setTexture(m_context->textures->Get(ID::Texture::TitleScreenArt));
	m_BgSprite->setScale(Vec2(0.8f, 0.8f));

	m_OptionStrings[0] = "Continue";
	m_OptionStrings[1] = "New Game";
	m_OptionStrings[2] = "Multiplayer";
	m_OptionStrings[3] = "Settings";
	m_OptionStrings[4] = "Quit Game";

	if (!m_TextObject)
		m_TextObject = new sf::Text();
	m_TextObject->setFont(m_context->fonts->Get(ID::Font::ARIEL));
	m_TextObject->setCharacterSize(18);

	return true;
}

void TitleScene::OnEntry()
{
	const Vec2& vs = Screen::Instance()->GetMainView().getSize();
	Screen::Instance()->SetViewCentre(Vec2(vs.x * 0.5f, vs.y * 0.5f));

	CONFIRM_BUTTON = KeyBindings::KeyBindingList[KeyBindings::KeyBinds::UseItemBind].key;
	CANCEL_BUTTON = KeyBindings::KeyBindingList[KeyBindings::KeyBinds::InteractItemBind].key;
	m_MenuState = Normal;
	m_UserInfoStr = "";
}

bool TitleScene::OnUpdate(const sf::Time& dt)
{
	return true;
}

void TitleScene::OnRender()
{
	const auto& time_now = Application::Instance()->TotalGameTime();
	const Vec2& vs = m_context->view->getSize();
	const sf::Vector2u& sprSz = m_BgSprite->getTexture()->getSize();

	const float scaleX = ((float)sprSz.x * k_bg_scale);
	const float scaleY = ((float)sprSz.y * k_bg_scale);
	const float sinx = sinf(time_now) * 1.5f;
	const float cosy = cosf(time_now) * 1.5f;
	float offset = -vs.y * 0.05f;

	// Render the background art
	m_BgSprite->setPosition(Vec2((vs.x * 0.5f) - (scaleX * 0.5f), (vs.y * 0.5f) - (scaleY * 0.5f)));
	m_context->window->draw(*m_BgSprite);

	// Title
	m_TextObject->setCharacterSize(32);
	Screen::RenderText(m_context->window, m_TextObject, "Demon Game", Vec2(vs.x * 0.5f, vs.y * 0.2f), Screen::AlignCentre);

	// Render Selectable text options
	m_TextObject->setCharacterSize(18);
	for (int i = 0; i < Options::OptionSize; ++i)
	{
		// Move the selected one and change it's colour
		bool cs = (i == (int)m_Options);

		Screen::RenderText(m_context->window, m_TextObject,
			m_OptionStrings[i],
			Vec2(cs ? (vs.x * 0.5f) + sinx : vs.x * 0.5f, cs ? (vs.y * 0.5f) + offset + cosy : (vs.y * 0.5f) + offset),
			Screen::AlignCentre,
			sf::Text::Style::Regular,
			cs ? sf::Color::White : sf::Color(128, 128, 128, 255)
			);

		offset += (vs.y * 0.05f);
	}

	// Other info
	Screen::RenderText(m_context->window, m_TextObject, m_UserInfoStr, Vec2(vs.x * 0.5f, vs.y * 0.4f), Screen::AlignCentre);
	Screen::RenderText(m_context->window, m_TextObject, "[" + KeyBindings::GetStringFromKey(CONFIRM_BUTTON) + "] to make selection", Vec2(vs.x * 0.5f, vs.y * 0.8f), Screen::AlignCentre);
	Screen::RenderText(m_context->window, m_TextObject, "Alex Spellman", Vec2(vs.x * 0.5f, vs.y * 0.9f), Screen::AlignCentre);
}

void TitleScene::OnExit(ID::States nextState)
{

}

void TitleScene::Close()
{
	SAFE_DELETE(m_BgSprite);
	SAFE_DELETE(m_TextObject);
}

void TitleScene::LateRender()
{

}

void TitleScene::HandleInput(int k, int a)
{
	if (a == RELEASE)
	{
		// Move up through menu
		if ((k == sf::Keyboard::Up || k == sf::Keyboard::W))
		{
			if (m_MenuState != Pending)
			{
				//SoundPlayer::Instance()->PlayASound(ID::Audio::UI_Select);
				int selopt = (int)m_Options;
				--selopt;
				m_Options = (Options)selopt;

				if (m_Options < Options::Continue)
					m_Options = Options::Quit;
			}
		}

		// Move down through menu
		if ((k == sf::Keyboard::S || k == sf::Keyboard::Down))
		{
			if (m_MenuState != Pending)
			{
				//SoundPlayer::Instance()->PlayASound(ID::Audio::UI_Select);
				int selopt = (int)m_Options;
				++selopt;
				m_Options = (Options)selopt;
				if (m_Options > Options::Quit)
					m_Options = Options::Continue;
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
			bool quit = false;
			bool load_game = false;
			bool multi = false;

			switch (m_Options)
			{
			case Options::Continue:
				load_game = true;
				break;
			case Options::NewGame:
				if (m_MenuState == Normal)
				{
					m_MenuState = Pending;
					std::stringstream ss;
					ss << "Start new game? (Yes[";
					ss << KeyBindings::GetStringFromKey(CONFIRM_BUTTON);
					ss << "],No[";
					ss << KeyBindings::GetStringFromKey(CANCEL_BUTTON);
					ss << "])";
					m_UserInfoStr = ss.str();
				}
				else
				{
					m_MenuState = Normal;
					m_UserInfoStr = "";
				}
				break;
			case Options::Multiplayer:
				if (m_MenuState == Normal)
				{
					m_MenuState = Pending;
					std::stringstream ss;
					ss << "Start multiplayer game? (Yes[";
					ss << KeyBindings::GetStringFromKey(CONFIRM_BUTTON);
					ss << "],No[";
					ss << KeyBindings::GetStringFromKey(CANCEL_BUTTON);
					ss << "])";
					m_UserInfoStr = ss.str();
				}
				else
				{
					m_MenuState = Normal;
					m_UserInfoStr = "";
					multi = true;
				}
				break;
			case Options::Settings:
				//SoundPlayer::Instance()->PlayASound(ID::Audio::PauseSound);
				ChangeState(ID::States::Pause, false);
				break;
			case Options::Quit:
				quit = true;
				break;
			default:
				break;
			}

			if (!quit)
			{
				//SoundPlayer::Instance()->PlayASound(ID::Audio::SelectSound);

				if (m_MenuState != Pending)
				{
					if (!multi)
					{
						// **TODO - Single player
						//EventSys::SendEvent(EventSys::LoadGameStartup, &load_game);
						//ChangeState(ID::States::WorldMap, false);
					}
					else
					{
						ChangeState(ID::States::Lobby, false);
					}
				}
			}
			else
			{
				Application::Instance()->ForceShutDown();
			}
		}
	}
}