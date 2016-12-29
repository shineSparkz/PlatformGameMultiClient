#ifndef __LOBBY_STATE_H__
#define __LOBBY_STATE_H__

#include "IScene.h"
#include <vector>
#include <string>

class LobbyScene : public IScene
{
	enum ServerInfo
	{
		CurrentIP,
		Connected,
		Registered,
		NumPlayersOnline,
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
	std::vector<std::string> m_ConfigOptions;

	sf::Text* m_TextObject;

	std::string m_UserInfoStr;
	int CONFIRM_BUTTON;
	int CANCEL_BUTTON;
};

#endif