#ifndef __SCREEN_H__
#define __SCREEN_H__

#include "types.h"
#include "Singleton.h"
#include <vector>
#include <string>

class Screen : public Singleton<Screen>
{
public:
	enum
	{
		AlignLeft,
		AlignCentre,
		AlignRight
	};

	Screen();
	~Screen();

	sf::RenderWindow* Init(bool fullscreen, size_t vidModeHandle, bool vsync);
	void StartFading();
	void DrawBorders(sf::RenderWindow* window);
	void FadeScreen(float elapsed_time);
	
	static void RenderText(sf::RenderWindow* window, sf::Text* TextObj, const std::string& str, const Vec2& pos, int align = 0, uint32 style = sf::Text::Regular, const sf::Color& colour = sf::Color::White);

	// Get
	int32 ScreenWidth() const;
	int32 ScreenHeight() const;
	size_t GetCurrentVideoModeIndex() const;
	std::string GetCurrentVideoModeName() const;
	bool IsFullScreen() const;
	bool IsVsync() const;
	bool CanIncreaseResolution();
	bool CanDecreaseResolution();
	bool IsFading() const;
	const sf::View& GetMainView() const;
	const sf::View& GetBorderView() const;
	sf::VideoMode* GetVideoMode();

	const Vec2& GetViewPos() const;

	// Set
	void SetGameViewport();
	void SetFullViewport();
	void SetFullScreen(bool full);
	void ChangeResolution(bool increase);
	void ApplyNewResolution();
	void CheckForResolutionIndexReset();
	void SetVSync(bool vsyncEnabled);
	void SetViewCentre(const Vec2& cen);
	
private:
	friend class Application;
	
	std::vector<sf::VideoMode> m_AvailableVideoModes;
	std::vector<std::string> m_VidModeNames;
	
	sf::View m_MainView;
	sf::View m_BorderView;
	sf::Texture m_ScreenTexture;
	sf::FloatRect m_MainViewport;
	sf::FloatRect m_WholeViewport;
	sf::RectangleShape m_BorderWidth;
	sf::RectangleShape m_BorderHeight;
	sf::Vector2f m_LeftBorderPos;
	sf::Vector2f m_RightBorderPos;
	sf::Vector2f m_TopBorderPos;
	sf::Vector2f m_BotBorderPos;
	sf::Vector2f m_ViewPos;

	sf::RenderWindow* m_Window;
	int32 m_ScreenWidth;
	int32 m_ScreenHeight;
	size_t m_CurrentVidModeHandle;
	size_t m_PreviousVidModeHandle;

	bool m_FullScreen;
	bool m_VsyncEnabled;
	bool m_ChangedResolution = false;
	bool m_AppliedNewResolution = false;
	bool m_IsFading = false;
};

#endif