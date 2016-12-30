#include "src\Application.h"

int main()

{
#ifdef _WIN32
	// Memory leak checker
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif
	//---------------------
	Application app;

	if (!app.Init("Game Client"))
	{
		app.Close();
		return -1;
	}

	app.Run();
	return 0;
}


/*
unsigned int k_InfoTotal = 0;
const sf::Time FRAME_TIME = sf::seconds(1 / 60.0f);
bool g_ShouldPrintUI = !false;

void update_ui_info(std::list<std::string>& info, const std::string& msg)
{
	++k_InfoTotal;
	info.push_back(std::to_string(k_InfoTotal) + ": " + msg);
}


int main()
{
	sf::RenderWindow window(sf::VideoMode(1280, 720), "Simple Sockets!");

	sf::Font font;
	if (!font.loadFromFile("resources/fonts/arial.ttf"))
		return -1;

	sf::Clock m_ClockTotal;
	float m_ElapsedFadeTime;

	sf::Text text;
	text.setFont(font);

	std::list<std::string> info_q;


	std::string user_info = "0)Quit 1)Null 2)SendMsgTCP 3)SendMsgUDP 4)Clear";

	// --- SF stuff for Game loop ----
	sf::Event event;
	sf::Clock clock;
	sf::Time timeSinceLastUpdate = sf::Time::Zero;

	// ---- Run Game ----
	while (window.isOpen())
	{
		// ---- Process Events ----
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::KeyPressed)
			{			
				// Quit
				if (event.key.code == sf::Keyboard::Num0)
				{
					window.close();
					if (g_ShouldPrintUI)
						update_ui_info(info_q, "Client shutdown press Space to exit");
				}

				// Send input key to server
				else
				{
					if (netman.connected())
					{
						netman.sendUdp(
							"input:" + std::to_string(event.key.code) + ":" + std::to_string(CLIENT_ID));

						if (event.key.code == sf::Keyboard::D)
							GameObjects[netman.playerId()].m_Sprite.move(3.0f, 0.0f);
						else if (event.key.code == sf::Keyboard::A)
							GameObjects[netman.playerId()].m_Sprite.move(-3.0f, 0.0f);
						else if (event.key.code == sf::Keyboard::W)
							GameObjects[netman.playerId()].m_Sprite.move(0.0f, -3.0f);
						else if (event.key.code == sf::Keyboard::D)
							GameObjects[netman.playerId()].m_Sprite.move(0.0f, 3.0f);
					}
				}
			}

			if (event.type == sf::Event::Closed)
			{
				window.close();
			}
		}

		// ---- Update Loop ----
		timeSinceLastUpdate += clock.restart();
		while (timeSinceLastUpdate > FRAME_TIME)
		{	


			timeSinceLastUpdate -= FRAME_TIME;
		}

		// ----- Render ----
		window.clear();


		// Render UI server ingo stuff 
		if (g_ShouldPrintUI)
		{
			// Set Back
			text.setCharacterSize(10);
			text.setStyle(sf::Text::Style::Regular);

			int num = 3;
			for (auto i = info_q.begin(); i != info_q.end(); ++i)
			{
				text.setPosition(32.0f, (float)num * 16.0f);
				text.setString((*i));
				window.draw(text);
				++num;
			}
		}

		window.display();

		// Render them first before removing
		if (info_q.size() > 30)
			info_q.pop_front();
	}

	return 0;
}
*/