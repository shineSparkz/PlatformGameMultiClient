#ifndef __GAME_OBJECT_H__
#define __GAME_OBJECT_H__

#include <SFML/Graphics.hpp>

class GameObject
{
public:
	sf::Sprite m_Sprite;
	int m_UniqueId;
	int m_TypeId;
};

#endif
