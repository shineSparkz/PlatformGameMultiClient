#ifndef __LOBBY_STATE_H__
#define __LOBBY_STATE_H__

#include "IScene.h"
#include <vector>
#include <string>
#include "EventHandler.h"

class LobbyScene : public IScene, public EventHandler
{
	enum LobbyOptions
	{
		Connect,					// Connect to server with creds
		EnterUserName,				
		EnterPassword,				
		CreateAccount,				// For new client when entered creds
		Login,
		StartGame,					// Start a game after connect
		Return,
		NumLobbyOptions,
	};

	enum ServerInfo
	{
		CurrentIP,
		Connected,
		UserName,
		Password,
		ServerInfoSize
	};

public:
	LobbyScene();
	virtual ~LobbyScene();

	void HandleEvent(Event* ev) override;

	bool OnCreate(Context* const context) override;
	void OnEntry() override;
	bool OnUpdate(const sf::Time& dt) override;
	void OnRender() override;
	void OnExit(ID::States nextState) override;
	void Close() override;

	void HandleInput(int k, int a) override;

private:
	void ConnectToServer();
	void CreateUserAccount();
	void LoginToServer();
	void AttemptToStartGame();

private:
	sf::Text* m_TextObject;

	std::string m_OptionStrings[7];
	std::string m_ServerInfoStrings[4];

	std::string m_UserInfoStr;
	std::string m_UserName = "";
	std::string m_PassWordName = "";
	
	LobbyOptions m_Options = LobbyOptions::Connect;
	MenuState m_MenuState = MenuState::Normal;

	int CONFIRM_BUTTON;
	int CANCEL_BUTTON;

	bool m_InputtingName = false;
	bool m_InputtingPassword = false;
};

#endif