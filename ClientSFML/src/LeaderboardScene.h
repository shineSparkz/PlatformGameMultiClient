#ifndef __LEADERBOARDS_H__
#define __LEADERBOARDS_H__

#include "IScene.h"
#include "EventHandler.h"

class LeaderboardScene : public IScene, public EventHandler
{
public:
	LeaderboardScene();
	virtual ~LeaderboardScene();

	void HandleEvent(Event* ev) override;

	bool OnCreate(Context* const con) override;
	void OnEntry() override;
	bool OnUpdate(const sf::Time& dt) override;
	void OnRender() override;
	void OnExit(ID::States nextState) override;
	void Close() override;

	void LateRender() override;
	void HandleInput(int k, int a) override;

private:
	sf::Text* m_TextObject;
	std::string m_LeaderBoardString;
	std::string m_UserInfoStr;
};

#endif