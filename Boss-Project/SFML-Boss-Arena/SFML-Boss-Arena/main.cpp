#include <SFML/Graphics.hpp>
#include <vector>
#include <cmath>
#include <memory>
#include "Boss.h"
#include <iostream>

const float PLAYER_SPEED = 200.0f;
const float PLAYER_ROTATION_SPEED = 200.0f;
const float FIRE_RATE = 3.0f; // 3 tirs par seconde

// Structure Player simplifiée
struct Player {
    sf::RectangleShape boundingBox;
    float angle;
    int health;
    float fireCooldown;
    bool isMovingForward;

    Player(float x, float y) {
        boundingBox.setSize(sf::Vector2f(40, 40));
        boundingBox.setFillColor(sf::Color::Transparent);
        boundingBox.setOutlineColor(sf::Color::Blue);
        boundingBox.setOutlineThickness(2);
        boundingBox.setOrigin(20, 20);
        boundingBox.setPosition(x, y);
        angle = -90.0f;
        health = 100;
        fireCooldown = 0.0f;
        isMovingForward = false;
    }

    void update(float deltaTime, int WINDOW_WIDTH, int WINDOW_HEIGHT) {
        if (fireCooldown > 0) {
            fireCooldown -= deltaTime;
        }

        // Rotation
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q)) {
            angle -= PLAYER_ROTATION_SPEED * deltaTime;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
            angle += PLAYER_ROTATION_SPEED * deltaTime;
        }

        if (isMovingForward) {
            float radianAngle = angle * 3.14159f / 180.0f;
            sf::Vector2f newPosition = boundingBox.getPosition() +
                sf::Vector2f(cos(radianAngle) * PLAYER_SPEED * deltaTime,
                    sin(radianAngle) * PLAYER_SPEED * deltaTime);

            if (newPosition.x > 50 && newPosition.x < WINDOW_WIDTH - 50 &&
                newPosition.y > 50 && newPosition.y < WINDOW_HEIGHT - 50) {
                boundingBox.setPosition(newPosition);
            }
        }

        boundingBox.setRotation(angle);
    }

    void handleKeyPressed(sf::Keyboard::Key key) {
        if (key == sf::Keyboard::Z) {
            isMovingForward = true;
        }
    }

    void handleKeyReleased(sf::Keyboard::Key key) {
        if (key == sf::Keyboard::Z) {
            isMovingForward = false;
        }
    }

    bool canShoot() const {
        return fireCooldown <= 0.0f && sf::Keyboard::isKeyPressed(sf::Keyboard::Space);
    }

    void resetFireCooldown() {
        fireCooldown = 1.0f / FIRE_RATE;
    }

    void takeDamage(int amount) {
        health -= amount;
        if (health < 0) health = 0;
    }

    bool isCollidingWith(const sf::RectangleShape& other) const {
        return boundingBox.getGlobalBounds().intersects(other.getGlobalBounds());
    }

    sf::Vector2f getPosition() const {
        return boundingBox.getPosition();
    }

    float getRotation() const {
        return angle;
    }
};

int main() {
    sf::VideoMode desktopMode = sf::VideoMode::getDesktopMode();
    int WINDOW_WIDTH = desktopMode.width;
    int WINDOW_HEIGHT = desktopMode.height;

    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Boss Arena", sf::Style::Fullscreen);
    window.setFramerateLimit(60);

    sf::RectangleShape arena(sf::Vector2f(WINDOW_WIDTH - 100, WINDOW_HEIGHT - 100));
    arena.setFillColor(sf::Color::Transparent);
    arena.setOutlineColor(sf::Color::White);
    arena.setOutlineThickness(5);
    arena.setPosition(50, 50);

    Player player(WINDOW_WIDTH / 2, WINDOW_HEIGHT - 150);

    Boss boss(WINDOW_WIDTH / 2, 200);

    std::vector<Bullet> playerBullets;
    std::vector<Bullet> bossBullets;

    sf::Font font;
    if (!font.loadFromFile("D:/Ecole/VisualStudio/boss_project_new_try/SFML-Boss-Arena/Venus Rising Rg.otf")) {
        std::cout << "Impossible de charger la police" << std::endl;
    }

    sf::Text phaseText;
    phaseText.setFont(font);
    phaseText.setCharacterSize(24);
    phaseText.setFillColor(sf::Color::White);
    phaseText.setPosition(20, 20);

    sf::RectangleShape playerHealthBar(sf::Vector2f(200, 20));
    playerHealthBar.setFillColor(sf::Color::Green);
    playerHealthBar.setPosition(WINDOW_WIDTH - 220, 20);

    sf::RectangleShape bossHealthBar(sf::Vector2f(400, 20));
    bossHealthBar.setFillColor(sf::Color::Red);
    bossHealthBar.setPosition(WINDOW_WIDTH / 2 - 200, 20);

    sf::Clock clock;
    while (window.isOpen()) {
        sf::Time dt = clock.restart();
        float deltaTime = dt.asSeconds();

        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Escape)
                    window.close();
                player.handleKeyPressed(event.key.code);
            }
            if (event.type == sf::Event::KeyReleased) {
                player.handleKeyReleased(event.key.code);
            }
        }

        player.update(deltaTime, WINDOW_WIDTH, WINDOW_HEIGHT);

        boss.update(deltaTime, player.getPosition(), bossBullets);

        for (auto it = playerBullets.begin(); it != playerBullets.end();) {
            it->update(deltaTime);

            if (boss.isCollidingWith(it->getBoundingBox())) {
                boss.takeDamage(10);
                it = playerBullets.erase(it);
            }

            else if (!arena.getGlobalBounds().intersects(it->getBoundingBox().getGlobalBounds())) {
                it = playerBullets.erase(it);
            }
            else {
                ++it;
            }
        }

        for (auto it = bossBullets.begin(); it != bossBullets.end();) {
            it->update(deltaTime);

            if (player.isCollidingWith(it->getBoundingBox())) {
                player.takeDamage(5);
                it = bossBullets.erase(it);
            }
            else if (!arena.getGlobalBounds().intersects(it->getBoundingBox().getGlobalBounds())) {
                it = bossBullets.erase(it);
            }
            else {
                ++it;
            }
        }

        if (player.canShoot()) {
            playerBullets.emplace_back(player.getPosition(), player.getRotation());
            player.resetFireCooldown();
        }

        std::string phaseString;
        switch (boss.getCurrentPhase()) {
        case Boss::Phase::PHASE_1:
            phaseString = "Phase 1";
            break;
        case Boss::Phase::PHASE_2:
            phaseString = "Phase 2";
            break;
        case Boss::Phase::PHASE_3:
            phaseString = "Phase 3";
            break;
        }
        phaseText.setString("Boss state: " + phaseString + " - " + boss.getStateMachine()->getCurrentStateName());

        playerHealthBar.setSize(sf::Vector2f(200 * (player.health / 100.0f), 20));
        bossHealthBar.setSize(sf::Vector2f(400 * (boss.getHealth() / 300.0f), 20));

        if (player.health <= 0) {
            phaseText.setString("GAME OVER");
            window.clear();
            window.draw(phaseText);
            window.display();
            sf::sleep(sf::seconds(3));
            break;
        }

        if (boss.getHealth() <= 0) {
            phaseText.setString("YOU WON!!!");
            window.clear();
            window.draw(phaseText);
            window.display();
            sf::sleep(sf::seconds(3));
            break;
        }

        window.clear();
        window.draw(arena);

        for (auto& bullet : playerBullets) {
            bullet.draw(window);
        }
        for (auto& bullet : bossBullets) {
            bullet.draw(window);
        }

        boss.draw(window);
        window.draw(player.boundingBox);

        window.draw(phaseText);
        window.draw(playerHealthBar);
        window.draw(bossHealthBar);

        window.display();
    }

    return 0;
}