#ifndef __LOBBY_STATE_H__
#define __LOBBY_STATE_H__

#include "IScene.h"
#include <vector>
#include <string>

class LobbyScene : public IScene
{
	enum LobbyOptions
	{
		CreateAccount,				// For new client when entered creds
		LoadOrCreateCredentials,	// Write or load bin
		Connect,					// Connect to server with creds
		StartGame,					// Start a game after connect
		Return,
		NumLobbyOptions,
	};

	enum ServerInfo
	{
		CurrentIP,
		Connected,
		UserName,
		ServerInfoSize
	};

public:
	LobbyScene();
	virtual ~LobbyScene();

	bool OnCreate(Context* context) override;
	void OnEntry() override;
	bool OnUpdate(const sf::Time& dt) override;
	void OnRender() override;
	void OnExit(ID::States nextState) override;
	void Close() override;

	void HandleInput(int k, int a) override;

private:
	void CreateUserAccount();
	void ConnectToServer();
	void AttemptToStartGame();

private:
	sf::Text* m_TextObject;

	std::string m_OptionStrings[5];
	std::string m_ServerInfoStrings[3];

	std::string m_UserInfoStr;
	std::string m_UserName = "";
	
	LobbyOptions m_Options = LobbyOptions::CreateAccount;
	MenuState m_MenuState = MenuState::Normal;

	int CONFIRM_BUTTON;
	int CANCEL_BUTTON;

	bool m_InputtingName = false;
};

#endif