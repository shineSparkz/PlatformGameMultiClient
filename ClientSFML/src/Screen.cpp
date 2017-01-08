#include "Screen.h"

#include "Application.h"
#include "utils.h"
//#include "BinSaveData.h"
//#include "ModFile.h"


Screen::Screen() :
	m_Window(nullptr)
{
}

Screen::~Screen()
{
}

sf::RenderWindow* Screen::Init(bool fullscreen, size_t vidModeHandle, bool vsync)
{
	// Convert to params
	Vec2 main_view_centre(400.0f, 300.0f);
	Vec2 main_view_size(800.0f, 600.0f);
	float border_width = 800;
	float border_height = 600;
	float border_thickness = 2.0f;
	sf::FloatRect border_viewport = sf::FloatRect(0.1f, 0.1f, 0.8f, 0.8f);

	// Set Locals
	m_MainViewport = border_viewport;
	m_WholeViewport = sf::FloatRect(0.0f, 0.0f, 1.0f, 1.0f);
	m_MainView = sf::View(main_view_centre, main_view_size);
	m_BorderWidth = sf::RectangleShape(Vec2(border_width, border_thickness));
	m_BorderHeight = sf::RectangleShape(Vec2(border_thickness, border_height));
	m_LeftBorderPos =  Vec2(0.0f, 0.0f);
	m_RightBorderPos = Vec2(0.0f, 598.0f);
	m_TopBorderPos =   Vec2(0.0f, 0.0f);
	m_BotBorderPos =   Vec2(798.0f, 0.0f);

	m_BorderView = sf::View(sf::FloatRect(0.0f, 0.0f, 800.f, 600.0f));
	m_BorderView.setViewport(m_MainViewport);

	// Resolution stuff
	std::vector<sf::VideoMode> modes = sf::VideoMode::getFullscreenModes();
	for ( std::size_t i = 0; i < modes.size(); ++i )
	{
		sf::VideoMode mode = modes[i];
	}

	for ( std::vector<sf::VideoMode>::const_iterator i = modes.begin(); i != modes.end(); ++i )
	{
		if ((*i).bitsPerPixel == 32 && (*i).width >= 640 && (*i).height >= 480)
		{
			m_AvailableVideoModes.push_back((*i));

			std::string av_mode = "Resolution :" + std::to_string((*i).width) +
				"x" + std::to_string((*i).height);

			//if (ModFileIni::AllowReleaseDebug)
			//{
			//	WRITE_LOG(av_mode, "none");
			//}

			m_VidModeNames.push_back(av_mode);
		}
	}

	// Default fall back for failed config
	m_CurrentVidModeHandle = Maths::Min(vidModeHandle, m_AvailableVideoModes.size() - 1);
	m_PreviousVidModeHandle = m_CurrentVidModeHandle;
	m_FullScreen = fullscreen;
	m_VsyncEnabled = vsync;

	m_Window = new sf::RenderWindow(
		*this->GetVideoMode(),
		Application::Instance()->GetAppName(),
		this->IsFullScreen() ? sf::Style::Fullscreen : sf::Style::Close);

	this->SetVSync(this->IsVsync());

	const Vec2& view_centre = m_MainView.getCenter();
	const Vec2& view_size = m_MainView.getSize();
	m_ViewPos = Vec2(view_centre.x - (view_size.x * 0.5f), view_centre.y - (view_size.y * 0.5f));

	return m_Window;
}

void Screen::StartFading()
{
	m_IsFading = true;
	m_ScreenTexture.create(m_Window->getSize().x, m_Window->getSize().y);
	m_ScreenTexture.update(*m_Window);
}

void Screen::DrawBorders(sf::RenderWindow* window)
{
	m_BorderView.setViewport(m_MainViewport);	// probably don't need this
	window->setView(m_BorderView);

	m_BorderWidth.setPosition(m_LeftBorderPos);
	window->draw(m_BorderWidth);
	m_BorderWidth.setPosition(m_RightBorderPos);
	window->draw(m_BorderWidth);

	m_BorderHeight.setPosition(m_TopBorderPos);
	window->draw(m_BorderHeight);
	m_BorderHeight.setPosition(m_BotBorderPos);
	window->draw(m_BorderHeight);
}

void Screen::FadeScreen(float elapsed_fade_time)
{
	const float fade_length = 0.75f;

	sf::Sprite sprite(m_ScreenTexture);
	sprite.setColor(sf::Color(255, 255, 255, (byte)Maths::Fade(255, 0.0f, elapsed_fade_time, fade_length)));

	sf::View full_view = sf::View(sf::Vector2f((float)m_Window->getSize().x / 2, (float)m_Window->getSize().y / 2),
		sf::Vector2f((float)m_Window->getSize().x, (float)m_Window->getSize().y));
	
	m_Window->setView(full_view);
	m_Window->draw(sprite);

	if ( elapsed_fade_time >= fade_length || sprite.getColor().a <= 0 )
	{
		elapsed_fade_time = 0;
		m_IsFading = false;
	}
}

int32 Screen::ScreenWidth() const
{
	return m_ScreenWidth;
}

int32 Screen::ScreenHeight() const
{
	return m_ScreenHeight;
}

size_t Screen::GetCurrentVideoModeIndex() const
{
	return this->m_CurrentVidModeHandle;
}

std::string Screen::GetCurrentVideoModeName() const
{
	return m_VidModeNames[m_CurrentVidModeHandle];
}

bool Screen::IsFullScreen() const
{
	return m_FullScreen;
}

bool Screen::IsVsync() const
{
	return m_VsyncEnabled;
}

bool Screen::CanIncreaseResolution()
{
	return m_CurrentVidModeHandle > 0;
}

bool Screen::CanDecreaseResolution()
{
	return m_CurrentVidModeHandle < (m_AvailableVideoModes.size() - 1);
}

bool Screen::IsFading() const
{
	return this->m_IsFading;
}

const sf::View& Screen::GetMainView() const
{
	return m_MainView;
}

const sf::View& Screen::GetBorderView() const
{
	return m_BorderView;
}

sf::VideoMode* Screen::GetVideoMode()
{
	if(m_CurrentVidModeHandle >=0 && m_CurrentVidModeHandle < m_AvailableVideoModes.size())
		return &m_AvailableVideoModes[m_CurrentVidModeHandle];
	return nullptr;
}

const Vec2& Screen::GetViewPos() const
{
	return m_ViewPos;
}

void Screen::SetGameViewport()
{
	m_MainView.setViewport(m_MainViewport);
}

void Screen::SetFullViewport()
{
	m_MainView.setViewport(m_WholeViewport);
}

void Screen::SetFullScreen(bool full)
{
	m_FullScreen = full;

	if(m_Window)
		m_Window->create(
			*this->GetVideoMode(),
			Application::Instance()->GetAppName(),
			this->IsFullScreen() ? sf::Style::Fullscreen : sf::Style::Close);
}

void Screen::ChangeResolution(bool increase)
{
	if (!m_ChangedResolution)
		m_PreviousVidModeHandle = m_CurrentVidModeHandle;
	
	m_ChangedResolution = true;
	m_AppliedNewResolution = false;
	
	if (increase)
		--m_CurrentVidModeHandle;
	else
		++m_CurrentVidModeHandle;
}

void Screen::ApplyNewResolution()
{
	// Check it is not the same or firs time ever
	if ( m_AppliedNewResolution || (!m_AppliedNewResolution && !m_ChangedResolution)/*first time*/ )
		return;	// Dont set it twice

	m_AppliedNewResolution = true;
	m_ChangedResolution = false;

	this->SetFullScreen(this->m_FullScreen);
}

void Screen::CheckForResolutionIndexReset()
{
	if ( !m_AppliedNewResolution && m_ChangedResolution )
	{
		m_CurrentVidModeHandle = m_PreviousVidModeHandle;
	}

	m_ChangedResolution = false;
}

void Screen::SetVSync(bool v)
{
	m_VsyncEnabled = v;
	if(m_Window)
		m_Window->setVerticalSyncEnabled(v);
}

void Screen::SetViewCentre(const Vec2& cen)
{
	m_MainView.setCenter(cen);

	const Vec2& view_centre = m_MainView.getCenter();
	const Vec2& view_size = m_MainView.getSize();
	m_ViewPos = Vec2(view_centre.x - (view_size.x * 0.5f), view_centre.y - (view_size.y * 0.5f));
}

void Screen::RenderText(sf::RenderWindow* window, sf::Text* TextObj, const std::string& str, const Vec2& pos, int align, uint32 style, const sf::Color& colour)
{
	if (TextObj && window)
	{
		TextObj->setString(str);
		sf::FloatRect r = TextObj->getLocalBounds();

		switch (align)
		{
		case AlignLeft:
			TextObj->setOrigin(Vec2(0.0f, 0.0f));
			break;
		case AlignCentre:
			TextObj->setOrigin(r.left + (r.width * 0.5f), r.top);// +(r.height * 0.5f));
			break;
		case AlignRight:
			TextObj->setOrigin(r.left + r.width, r.top);// +r.height);
			break;
		default:
			TextObj->setOrigin(Vec2(0.0f, 0.0f));
			break;
		}

		TextObj->setPosition(pos);
		TextObj->setFillColor(colour);
		TextObj->setStyle(style);
		window->draw(*TextObj);
	}
}

