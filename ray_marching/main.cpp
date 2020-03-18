#include <SFML/Graphics.hpp>
#include <iostream>

using namespace sf;

float VectorLength(const Vector3f& vec) {
  return std::powf(vec.x, 2) + std::powf(vec.y, 2) + std::powf(vec.z, 2);
}

Vector3f Normalized(const Vector3f& vec) {
  float vecLength = VectorLength(vec);
  return Vector3f(vec.x / vecLength, vec.y / vecLength, vec.z / vecLength);
}

void drawScene(sf::RenderWindow& window, VertexArray& view) {
  static Vector3f camera(0.0, 1.0, 0.0);
  static Vector3f rayDir(Normalized(Vector3f()));

  for (uint64_t y = 0; y < window.getSize().y; y++) {
    for (uint64_t x = 0; x < window.getSize().x; x++) {
      const float nx = static_cast<float>(x) / window.getSize().x;
      const float ny = static_cast<float>(y) / window.getSize().y;
      const auto currentArrayIndex = y * window.getSize().x + x;

      view[currentArrayIndex].color = Color(0, ny * 250, 0);
    }
  }

  window.draw(view);
}

void ResizeView(VertexArray& view, uint64_t width, uint64_t height)
{
  const auto pixelCount = width * height;
  view.clear();
  view.resize(pixelCount);

  for (uint64_t y = 0; y < height; y++) {
    for (uint64_t x = 0; x < width; x++) {
      const auto currentArrayIndex = y * width + x;
      view[currentArrayIndex].position = Vector2f(x, height - y);
    }
  }
}

int main() {
  sf::Font font;

  if (!font.loadFromFile("Roboto-Black.ttf")) {
    std::cerr << "ERROR: font not found" << std::endl;
  }

  RenderWindow window(VideoMode(300, 400), "ray_marching");
  window.setFramerateLimit(60);

  sf::Clock clock;
  sf::Time time;

  float fps = 0;

  sf::Text fpsLabel;
  fpsLabel.setFont(font);
  fpsLabel.setCharacterSize(26);
  fpsLabel.setFillColor(sf::Color::White);
  fpsLabel.setStyle(sf::Text::Bold);

  static sf::VertexArray view(sf::Points);
  ResizeView(view, window.getSize().x, window.getSize().y);

  while (window.isOpen()) {
    time = clock.getElapsedTime();
    clock.restart().asSeconds();
    fps = 1.0f / time.asSeconds();

    Event event;
    while (window.pollEvent(event)) {
      switch (event.type) {
        case sf::Event::Closed: {
          window.close();
          break;
        }

        case sf::Event::Resized: {
          window.setView(View(FloatRect(0, 0, event.size.width, event.size.height)));
          ResizeView(view, event.size.width, event.size.height);
          break;
        }

        default:
          break;
      }
    }

    window.clear(Color(0, 0, 0));

    drawScene(window, view);

    fpsLabel.setString(std::to_string(static_cast<int>(fps)));
    window.draw(fpsLabel);

    window.display();
  }

  return 0;
}