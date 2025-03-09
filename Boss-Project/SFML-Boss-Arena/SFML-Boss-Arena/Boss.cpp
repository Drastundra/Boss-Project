#include "Boss.h"
#include "BossStates.h"
#include <cmath>

// Constantes pour le Boss
const float BOSS_SPEED = 100.0f;
const float PI = 3.14159265f;

// Implémentation de Bullet (simplifiée)
Bullet::Bullet(sf::Vector2f pos, float angle) {
    boundingBox.setSize(sf::Vector2f(10, 10));
    boundingBox.setFillColor(sf::Color::Transparent);
    boundingBox.setOutlineColor(sf::Color::Yellow);
    boundingBox.setOutlineThickness(2);
    boundingBox.setOrigin(5, 5);
    boundingBox.setPosition(pos);
    boundingBox.setRotation(angle);

    float rad_angle = angle * PI / 180.0f;
    velocity = sf::Vector2f(cos(rad_angle) * 400.0f, sin(rad_angle) * 400.0f);
}

void Bullet::update(float deltaTime) {
    boundingBox.move(velocity * deltaTime);
}

void Bullet::draw(sf::RenderWindow& window) {
    window.draw(boundingBox);
}

bool Bullet::isOutsideArena(const sf::FloatRect& arenaBounds) {
    return !arenaBounds.intersects(boundingBox.getGlobalBounds());
}

// Implémentation de Boss
Boss::Boss(float x, float y) {
    boundingBox.setSize(sf::Vector2f(50, 50));
    boundingBox.setFillColor(sf::Color::Transparent);
    boundingBox.setOutlineColor(sf::Color::Red);
    boundingBox.setOutlineThickness(2);
    boundingBox.setOrigin(25, 25);
    boundingBox.setPosition(x, y);

    health = 300;
    currentPhase = Phase::PHASE_1;

    chargeTimer = 0.0f;
    maxChargeTime = 0.0f;
    charging = false;
    attackCooldown = 0.0f;

    stateMachine = std::make_unique<StateMachine>();
    initializeAI();
}

void Boss::initializeAI() {
    std::vector<Bullet> tempBullets; 


    stateMachine->addState("idle", std::make_shared<IdleState>());
    stateMachine->addState("circle_attack", std::make_shared<CircleAttackState>(tempBullets));

    stateMachine->addState("charge", std::make_shared<ChargeState>());
    stateMachine->addState("rush", std::make_shared<RushState>());  
    stateMachine->addState("cooldown", std::make_shared<CooldownState>());  

    stateMachine->addState("enrage", std::make_shared<EnrageState>(tempBullets));
    stateMachine->addState("teleport", std::make_shared<TeleportState>());

    stateMachine->changeState("idle", this);
}

void Boss::update(float deltaTime, const sf::Vector2f& playerPos, std::vector<Bullet>& bullets) {
    dynamic_cast<CircleAttackState*>(stateMachine->getState("circle_attack").get())->updateBullets(bullets);
    dynamic_cast<RushState*>(stateMachine->getState("rush").get())->updatePlayerPos(playerPos);
    dynamic_cast<CooldownState*>(stateMachine->getState("cooldown").get())->updatePlayerPos(playerPos);
    dynamic_cast<EnrageState*>(stateMachine->getState("enrage").get())->updateBullets(bullets);

    if (charging) {
        chargeTimer += deltaTime;
        if (chargeTimer >= maxChargeTime) {
            charging = false;
        }
    }

    if (attackCooldown > 0) {
        attackCooldown -= deltaTime;
    }

    updatePhase();

    stateMachine->update(this, deltaTime);
}

void Boss::updatePhase() {
    Phase oldPhase = currentPhase;

    if (health <= 100) {
        currentPhase = Phase::PHASE_3;
    }
    else if (health <= 200) {
        currentPhase = Phase::PHASE_2;
    }
    else {
        currentPhase = Phase::PHASE_1;
    }

    if (oldPhase != currentPhase) {
        switch (currentPhase) {
        case Phase::PHASE_1:
            stateMachine->changeState("idle", this);
            break;
        case Phase::PHASE_2:
            stateMachine->changeState("charge", this);
            break;
        case Phase::PHASE_3:
            stateMachine->changeState("enrage", this);
            break;
        }
    }
}

void Boss::draw(sf::RenderWindow& window) {
    window.draw(boundingBox);

    if (charging) {
        sf::RectangleShape chargeBar(sf::Vector2f(50.0f * (chargeTimer / maxChargeTime), 5));
        chargeBar.setFillColor(sf::Color::Yellow);
        chargeBar.setPosition(boundingBox.getPosition().x - 25, boundingBox.getPosition().y - 35);
        window.draw(chargeBar);
    }
}

void Boss::takeDamage(int damage) {
    health -= damage;
    if (health < 0) health = 0;
}

bool Boss::isCollidingWith(const sf::RectangleShape& other) const {
    return boundingBox.getGlobalBounds().intersects(other.getGlobalBounds());
}

float Boss::getChargePercentage() const {
    return (maxChargeTime > 0) ? (chargeTimer / maxChargeTime) : 0.0f;
}

void Boss::moveTowards(const sf::Vector2f& target, float speed, float deltaTime) {
    sf::Vector2f direction = target - boundingBox.getPosition();
    float magnitude = std::sqrt(direction.x * direction.x + direction.y * direction.y);

    if (magnitude > 0) {
        direction /= magnitude;
        boundingBox.move(direction * speed * deltaTime);

        float angle = std::atan2(direction.y, direction.x) * 180 / PI;
        boundingBox.setRotation(angle);
    }
}

void Boss::moveInCircle(float centerX, float centerY, float radius, float speed, float deltaTime) {
    float angle = boundingBox.getRotation() + speed * deltaTime;
    float x = centerX + std::cos(angle * PI / 180) * radius;
    float y = centerY + std::sin(angle * PI / 180) * radius;

    boundingBox.setPosition(x, y);
    boundingBox.setRotation(angle);
}

void Boss::shootAt(const sf::Vector2f& target, std::vector<Bullet>& bullets) {
    if (attackCooldown <= 0) {
        sf::Vector2f direction = target - boundingBox.getPosition();
        float angle = std::atan2(direction.y, direction.x) * 180 / PI;

        bullets.emplace_back(boundingBox.getPosition(), angle);
        attackCooldown = 0.5f; // 2 tirs par seconde
    }
}

void Boss::shootPattern(const std::vector<float>& angles, std::vector<Bullet>& bullets) {
    if (attackCooldown <= 0) {
        for (float angle : angles) {
            bullets.emplace_back(boundingBox.getPosition(), angle);
        }
        attackCooldown = 0.5f;
    }
}

void Boss::teleportTo(float x, float y) {
    boundingBox.setPosition(x, y);
}

void Boss::chargeAttack(float chargeTime) {
    charging = true;
    chargeTimer = 0.0f;
    maxChargeTime = chargeTime;
}
