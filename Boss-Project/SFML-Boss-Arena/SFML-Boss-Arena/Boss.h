#pragma once
#include <SFML/Graphics.hpp>
#include "StateMachine.h"
#include <memory>
#include <vector>
#include <cmath>

class Bullet;

struct Bullet {
    sf::RectangleShape boundingBox;
    sf::Vector2f velocity;

    Bullet(sf::Vector2f pos, float angle);
    void update(float deltaTime);
    void draw(sf::RenderWindow& window);
    const sf::RectangleShape& getBoundingBox() const { return boundingBox; }
    bool isOutsideArena(const sf::FloatRect& arenaBounds);
};

class Boss {
public:
    enum class Phase {
        PHASE_1,  
        PHASE_2,  
        PHASE_3   
    };

    Boss(float x, float y);
    ~Boss() = default;

    void update(float deltaTime, const sf::Vector2f& playerPos, std::vector<Bullet>& bullets);
    void draw(sf::RenderWindow& window);
    void takeDamage(int damage);
    bool isCollidingWith(const sf::RectangleShape& other) const;

    // Getters
    sf::Vector2f getPosition() const { return boundingBox.getPosition(); }
    float getRotation() const { return boundingBox.getRotation(); }
    const sf::RectangleShape& getBoundingBox() const { return boundingBox; }
    int getHealth() const { return health; }
    Phase getCurrentPhase() const { return currentPhase; }
    StateMachine* getStateMachine() { return stateMachine.get(); }

    void moveTowards(const sf::Vector2f& target, float speed, float deltaTime);
    void moveInCircle(float centerX, float centerY, float radius, float speed, float deltaTime);
    void shootAt(const sf::Vector2f& target, std::vector<Bullet>& bullets);
    void shootPattern(const std::vector<float>& angles, std::vector<Bullet>& bullets);
    void teleportTo(float x, float y);
    void chargeAttack(float chargeTime);
    bool isCharging() const { return charging; }
    float getChargePercentage() const;

private:
    sf::RectangleShape boundingBox;
    int health;
    Phase currentPhase;
    std::unique_ptr<StateMachine> stateMachine;

    float chargeTimer;
    float maxChargeTime;
    bool charging;
    float attackCooldown;

    void initializeAI();
    void updatePhase();
};