#ifndef __GAME_SCENE_H__
#define __GAME_SCENE_H__

#include "IScene.h"
#include "EventHandler.h"

#include <vector>

class GameObject;

class GameScene : public IScene, public EventHandler
{
public:
	GameScene();
	virtual ~GameScene();

	void HandleEvent(Event* event_) override;

	bool OnCreate(Context* const con) override;
	void OnEntry() override;
	bool OnUpdate(const sf::Time& dt) override;
	void OnRender() override;
	void OnExit(ID::States nextState) override;
	void Close() override;

	void LateRender() override;
	void HandleInput(int k, int a) override;

private:
	void LoadLevel();
	void ClearGameObjects();

private:
	std::vector<GameObject*> m_GameObjects;
	sf::Sprite* m_BackgroundSprite;
	size_t m_PlayerHandle;
	bool m_SceneReady = false;
};

#endif

