#include <SFML/Graphics.hpp>
#include <iostream>

using namespace sf;
const int MAX_STEPS = 50;
const float SURFACE_DIST = 0.05f;
const float MAX_DIST = 50;

namespace sf {
template <typename T>
Vector3<T>& operator-(Vector3<T>& left, float val) {
  left.x -= val;
  left.y -= val;
  left.z -= val;

  return left;
}

template <typename T>
Vector3<T>& operator*(Vector3<T>& left, float val) {
  left.x *= val;
  left.y *= val;
  left.z *= val;

  return left;
}
}  // namespace sf

float Length(const Vector3f& vec) {
  return std::powf(vec.x, 2) + std::powf(vec.y, 2) + std::powf(vec.z, 2);
}

Vector3f Normalized(const Vector3f& vec) {
  float vecLength = Length(vec);
  return Vector3f(vec.x / vecLength, vec.y / vecLength, vec.z / vecLength);
}

Vector3f Absolute(const Vector3f& vec) {
  return Vector3f(std::abs(vec.x), std::abs(vec.y), std::abs(vec.z));
}

float DotProduct(const Vector3f& a, const Vector3f& b) {
  return a.x * b.x + a.y * b.y + a.z * b.z;
}

float SphereDistance(Vector3f point, Vector3f pos, float radius) {
  return Length(point - pos) - radius;
}

float BoxDistance(Vector3f point, Vector3f pos) {
  return Length(Vector3f(std::max<float>(abs(point.x) - pos.x, 0.),
                             std::max<float>(abs(point.y) - pos.y, 0.),
                             std::max<float>(abs(point.z) - pos.z, 0.)));
}

float CapsuleDistance(Vector3f point, Vector3f posA, Vector3f posB, float radius) {
  Vector3f ab = posB - posA;
  Vector3f ap = point - posA;

  float t = DotProduct(ab, ap) / DotProduct(ab, ab);
  t = std::clamp<float>(t, 0., 1.);

  Vector3f c = posA + t * ab;

  return Length(point - c) - radius;
}

float GetDistance(Vector3f point) {
  const float distancePlane = point.y;
  const float boxDist = BoxDistance(point - Vector3f(-3.5, 1, 6), Vector3f(1, .75, 1));
  const float capsuleDistance = 
    CapsuleDistance(point, Vector3f(-1, 1, 6), Vector3f(1, 2, 6), 1);

  float res;
  res = std::min(capsuleDistance, distancePlane);
  res = std::min(res, boxDist);

  return res;
}

Vector3f GetNormal(Vector3f p) {
  float d = GetDistance(p);

  Vector3f n = Vector3f(d - GetDistance(p - Vector3f(0.001f, 0, 0)),
                        d - GetDistance(p - Vector3f(0, 0.001f, 0)),
                        d - GetDistance(p - Vector3f(0, 0, 0.001f)));

  return Normalized(n);
}

float RayMarch(const Vector3f& rayOrigin, const Vector3f& rayDirection) {
  float distanceOrigin = 0;

  for (int i = 0; i < MAX_STEPS; i++) {
    Vector3f point = rayOrigin + rayDirection * distanceOrigin;
    float distanceScene = GetDistance(point);
    distanceOrigin += distanceScene;

    if (distanceScene < SURFACE_DIST || distanceOrigin > MAX_DIST) break;
  }

  return distanceOrigin;
}

float GetLight(Vector3f p) {
  Vector3f lightPos = Vector3f(0, 5, 6);
  lightPos.x += sin(time(0)) * 2.;
  lightPos.z += cos(time(0)) * 2.;

  Vector3f l = Normalized(lightPos - p);
  Vector3f n = GetNormal(p);
  float dif = std::clamp<float>(DotProduct(n, l), 0., 1.);
  float d = RayMarch(p + n * SURFACE_DIST * 2., l);
  if (d < Length(lightPos - p)) dif *= 0.1f;

  return dif;
}

void drawScene(sf::RenderWindow& window, VertexArray& view) {
  static Vector3f camera(0, 2, 0);

  for (uint64_t y = 0; y < window.getSize().y; y++) {
    for (uint64_t x = 0; x < window.getSize().x; x++) {
      const auto currentArrayIndex = y * window.getSize().x + x;

      const Vector2f uv((x - 0.5f * window.getSize().x) / window.getSize().y,
                        (y - 0.5f * window.getSize().y) / window.getSize().y);

      const Vector3f rayDir = Normalized(Vector3f(uv.x, uv.y - 0.2f, 1.0f));
      float finalRayOrigin = RayMarch(camera, rayDir);

      Vector3f point = camera + rayDir * finalRayOrigin;
      float dif = GetLight(point);

      const uint8_t val = 255 - (255 * std::clamp<float>(dif, 0, 1));

      view[currentArrayIndex].color = Color(val, val, val, val);
    }
  }

  window.draw(view);
}

void ResizeView(VertexArray& view, uint64_t width, uint64_t height) {
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

  RenderWindow window(VideoMode(100, 100), "ray_marching");
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
          window.setView(
              View(FloatRect(0, 0, event.size.width, event.size.height)));
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
