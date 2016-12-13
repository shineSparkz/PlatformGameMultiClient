#include "IScene.h"
#include "SceneGraph.h"


IScene::Context::Context(sf::RenderWindow& window, sf::View& view_, TextureHolder& textures, FontHolder& fonts):
	window(&window),
	view(&view_),
	textures(&textures),
	fonts(&fonts)
{
}

IScene::IScene()
{
}

IScene::~IScene()
{
}

bool IScene::OnCreate(Context* const con)
{
	m_context = con;
	return true;
}
