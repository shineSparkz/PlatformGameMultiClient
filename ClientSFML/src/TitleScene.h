#ifndef __TITLE_SCENE_H__
#define __TITLE_SCENE_H__

#include "IScene.h"

#include <vector>

class TitleScene : public IScene
{
	enum Options
	{
		EnterLobby,
		Quit,
		OptionSize
	};

public:
	TitleScene();
	virtual ~TitleScene();

	bool OnCreate(Context* const con) override;
	void OnEntry() override;
	bool OnUpdate(const sf::Time& dt) override;
	void OnRender() override;
	void OnExit(ID::States nextState) override;
	void Close() override;

	void LateRender() override;
	void HandleInput(int k, int a) override;

private:
	sf::Sprite* m_BgSprite;
	sf::Text* m_TextObject;

	std::string m_OptionStrings[2];
	std::string m_UserInfoStr;

	Options m_Options = Options::EnterLobby;
	MenuState m_MenuState = MenuState::Normal;

	int CONFIRM_BUTTON;
	int CANCEL_BUTTON;
};

#endif


