#ifndef __ISCENE_H__
#define __ISCENE_H__

#include <memory>
#include <SFML/System/Time.hpp>
#include <SFML/Window/Event.hpp>

#include "identifiers.h"
#include "LogFile.h"
#include "types.h"

namespace sf
{
	class RenderWindow;
}

class IScene
{
protected:
	enum MenuState
	{
		Normal,
		Pending
	};
public:
	typedef std::unique_ptr<IScene> P_State;

	struct Context
	{
		sf::RenderWindow* window;
		sf::View* view;
		TextureHolder* textures;
		FontHolder* fonts;

		Context(sf::RenderWindow& window, sf::View& view, TextureHolder& textures, FontHolder& fonts);
	};

public:
	IScene();
	virtual ~IScene();

	virtual bool OnCreate(Context* const con);
	virtual void OnEntry() = 0;
	virtual bool OnUpdate(const sf::Time& dt) = 0;
	virtual void OnRender() = 0;
	virtual void OnExit(ID::States nextState) = 0;
	virtual void Close() = 0;

	virtual void HandleInput(int k, int a) = 0;
	virtual void LateRender(){}

protected:
	Context* m_context = nullptr;
};

#endif