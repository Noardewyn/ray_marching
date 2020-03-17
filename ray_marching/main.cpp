#include <iostream>
#include <SFML/Graphics.hpp>

using namespace sf;

void drawScene(sf::RenderWindow &window)
{

}

int main()
{
	sf::Font font;

    if (!font.loadFromFile("Roboto-Black.ttf"))
	{
		std::cerr << "ERROR: font not found" << std::endl;
	}

	RenderWindow window(VideoMode(800, 600), "ray_marching");

	window.setFramerateLimit(60);

	sf::Clock clock;
	sf::Time time;

	float fps = 0;

	sf::Text fpsLabel;
	fpsLabel.setFont(font);
	fpsLabel.setCharacterSize(26);
	fpsLabel.setFillColor(sf::Color::White);
	fpsLabel.setStyle(sf::Text::Bold);

	while (window.isOpen())
	{
		time = clock.getElapsedTime();
		clock.restart().asSeconds();
		fps = 1.0f / time.asSeconds();

		Event event;
		while (window.pollEvent(event))
		{
			if (event.type == Event::Closed)
				window.close();
		}

		window.clear(Color(0, 0, 0, 0));

		drawScene(window);

		fpsLabel.setString(std::to_string(static_cast<int>(fps)));
	    window.draw(fpsLabel);

		window.display();
	}

	return 0;
}