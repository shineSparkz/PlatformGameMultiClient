#ifndef __APPLICATION_H__
#define __APPLICATION_H__

#include <string>
#include <SFML/Window/Keyboard.hpp>

#include "Singleton.h"
#include "types.h"
#include "ResourceHolder.h"
#include "identifiers.h"
#include "SceneGraph.h"

class DebugTextRender;
class ConfigData;
class Screen;
class NetworkManager;

class Application : public Singleton<Application>
{
public:
	Application();
	~Application();

	bool Init(const std::string& app_name);
	void Run();
	void Close();
	
	float TotalGameTime() const;
	void PauseUnpauseGame();
	void ForceShutDown();

	bool IsShowingFPS() const;
	void ShowFPS(bool);

	const std::string& GetAppName() const;
	const ResourceHolder<sf::Texture, ID::Texture>& GetTexHolder() const;

private:
	void ProcessEvents();
	void HandleInput(int key, int action);
	void Render();
	void DisplayFPS(float time);

private:
	TextureHolder m_TextureHolder;
	FontHolder m_FontHolder;
	DebugLogFile m_LogFile;
	sf::Clock m_ClockTotal;
	sf::Event m_Event;
	std::string m_AppName;

	Screen* m_Screen;
	NetworkManager* m_NetManager;
	sf::RenderWindow*  m_Window;
	SceneGraph* m_SceneGraph;
	sf::Text* m_TextObj;

	float m_ElapsedFadeTime;

	bool m_AltKey;
	bool m_F4Key;
	bool m_ShowFPS{ true };
	char pad[1];
};

INLINE const ResourceHolder<sf::Texture, ID::Texture>& Application::GetTexHolder() const
{
	return m_TextureHolder;
}

INLINE bool Application::IsShowingFPS() const
{
	return m_ShowFPS;
}

INLINE void Application::ShowFPS(bool show)
{
	m_ShowFPS = show;
}

#endif