#include <SFML/Graphics.hpp>
#include <vector>
#include <cmath>

const float PLAYER_SPEED = 200.0f;
const float PLAYER_ROTATION_SPEED = 200.0f;
const float BULLET_SPEED = 400.0f;
const float BOSS_SPEED = 100.0f;
const float FIRE_RATE = 180.0f / 60.0f; // 120 shots per minute -> 2 shots per second
const float PI = 3.14159265f;

struct Bullet {
    sf::RectangleShape boundingBox;
    sf::Vector2f velocity;

    Bullet(sf::Vector2f pos, float angle) {
        boundingBox.setSize(sf::Vector2f(10, 10));
        boundingBox.setFillColor(sf::Color::Transparent);
        boundingBox.setOutlineColor(sf::Color::Yellow);
        boundingBox.setOutlineThickness(2);
        boundingBox.setOrigin(5, 5);
        boundingBox.setPosition(pos);
        boundingBox.setRotation(angle);
        velocity = sf::Vector2f(cos(angle * PI / 180.0f) * BULLET_SPEED, sin(angle * PI / 180.0f) * BULLET_SPEED);
    }
};

int main() {
    sf::VideoMode desktopMode = sf::VideoMode::getDesktopMode();
    int WINDOW_WIDTH = desktopMode.width;
    int WINDOW_HEIGHT = desktopMode.height;

    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Arena with Boss", sf::Style::Fullscreen);
    window.setFramerateLimit(60);

    // Arena boundaries
    sf::RectangleShape arena(sf::Vector2f(WINDOW_WIDTH - 100, WINDOW_HEIGHT - 100));
    arena.setFillColor(sf::Color::Transparent);
    arena.setOutlineColor(sf::Color::White);
    arena.setOutlineThickness(5);
    arena.setPosition(50, 50);

    // Player Bounding Box
    sf::RectangleShape player(sf::Vector2f(40, 40));
    player.setFillColor(sf::Color::Transparent);
    player.setOutlineColor(sf::Color::Blue);
    player.setOutlineThickness(2);
    player.setOrigin(20, 20);
    player.setPosition(WINDOW_WIDTH / 2, WINDOW_HEIGHT - 150);
    float playerAngle = -90.0f;
    bool isMovingForward = false;
    float fireCooldown = 0.0f;

    // Boss Bounding Box
    sf::RectangleShape boss(sf::Vector2f(50, 50));
    boss.setFillColor(sf::Color::Transparent);
    boss.setOutlineColor(sf::Color::Red);
    boss.setOutlineThickness(2);
    boss.setOrigin(25, 25);
    boss.setPosition(WINDOW_WIDTH / 2, 200);
    float bossAngle = 0.0f;

    std::vector<Bullet> bullets;

    sf::Clock clock;
    while (window.isOpen()) {
        sf::Time dt = clock.restart();
        float deltaTime = dt.asSeconds();
        fireCooldown -= deltaTime;

        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Z) {
                    isMovingForward = true;
                }
            }
            if (event.type == sf::Event::KeyReleased) {
                if (event.key.code == sf::Keyboard::S) {
                    isMovingForward = false;
                }
            }
        }

        // Player controls
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q)) {
            playerAngle -= PLAYER_ROTATION_SPEED * deltaTime;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
            playerAngle += PLAYER_ROTATION_SPEED * deltaTime;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space) && fireCooldown <= 0.0f) {
            bullets.emplace_back(player.getPosition(), playerAngle);
            fireCooldown = 1.0f / FIRE_RATE;
        }

        // Move player forward only if Up is pressed
        if (isMovingForward) {
            float radianAngle = playerAngle * PI / 180.0f;
            sf::Vector2f newPosition = player.getPosition() + sf::Vector2f(cos(radianAngle) * PLAYER_SPEED * deltaTime, sin(radianAngle) * PLAYER_SPEED * deltaTime);
            if (newPosition.x > 50 && newPosition.x < WINDOW_WIDTH - 50 && newPosition.y > 50 && newPosition.y < WINDOW_HEIGHT - 50) {
                player.setPosition(newPosition);
            }
        }
        player.setRotation(playerAngle);

        // Boss movement (circular motion)
        bossAngle += BOSS_SPEED * deltaTime;
        boss.setPosition(WINDOW_WIDTH / 2 + cos(bossAngle * PI / 180.0f) * 200, 300 + sin(bossAngle * PI / 180.0f) * 100);
        boss.setRotation(bossAngle);

        // Move bullets
        for (auto& bullet : bullets) {
            bullet.boundingBox.move(bullet.velocity * deltaTime);
        }

        // Render
        window.clear();
        window.draw(arena);
        window.draw(player);
        window.draw(boss);
        for (auto& bullet : bullets) {
            window.draw(bullet.boundingBox);
        }
        window.display();
    }
    return 0;
}
