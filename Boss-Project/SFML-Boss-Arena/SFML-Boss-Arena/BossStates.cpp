#include "BossStates.h"
#include <random>


void IdleState::enter(Boss* boss) {
    timer = 0.0f;
}

void IdleState::update(Boss* boss, float deltaTime) {
    timer += deltaTime;

    boss->moveInCircle(400, 300, 200, 100.0f, deltaTime);

    if (timer > 3.0f) {
        boss->getStateMachine()->changeState("circle_attack", boss);
    }
}

void CircleAttackState::enter(Boss* boss) {
    attackTimer = 0.0f;
    attackCount = 0;
}

void CircleAttackState::update(Boss* boss, float deltaTime) {
    attackTimer += deltaTime;

    if (attackTimer > 1.0f && attackCount < 3) {
        std::vector<float> pattern;
        for (int i = 0; i < 12; i++) {
            pattern.push_back(i * 30.0f);
        }
        boss->shootPattern(pattern, bullets);
        attackTimer = 0.0f;
        attackCount++;
    }

    if (attackCount >= 3) {
        boss->getStateMachine()->changeState("idle", boss);
    }
}

void ChargeState::enter(Boss* boss) {
    boss->chargeAttack(2.0f);
}

void ChargeState::update(Boss* boss, float deltaTime) {
    if (!boss->isCharging()) {
        boss->getStateMachine()->changeState("rush", boss);
    }
}

void RushState::enter(Boss* boss) {
    target = playerPos;
    rushTime = 0.0f;
}

void RushState::update(Boss* boss, float deltaTime) {
    rushTime += deltaTime;

    boss->moveTowards(target, 400.0f, deltaTime);

    if (rushTime > 1.0f) {
        boss->getStateMachine()->changeState("cooldown", boss);
    }
}

void CooldownState::enter(Boss* boss) {
    cooldownTimer = 0.0f;
}

void CooldownState::update(Boss* boss, float deltaTime) {
    cooldownTimer += deltaTime;

    sf::Vector2f awayDirection = boss->getPosition() - playerPos;
    float magnitude = std::sqrt(awayDirection.x * awayDirection.x + awayDirection.y * awayDirection.y);
    if (magnitude > 0) {
        awayDirection /= magnitude;
    }
    sf::Vector2f targetPos = boss->getPosition() + awayDirection * 200.0f;
    boss->moveTowards(targetPos, 150.0f, deltaTime);

    if (cooldownTimer > 2.0f) {
        boss->getStateMachine()->changeState("charge", boss);
    }
}

void EnrageState::enter(Boss* boss) {
    enrageTimer = 0.0f;
}

void EnrageState::update(Boss* boss, float deltaTime) {
    enrageTimer += deltaTime;

    float noiseX = std::sin(enrageTimer * 10.0f) * 100.0f;
    float noiseY = std::cos(enrageTimer * 8.0f) * 100.0f;
    sf::Vector2f targetPos = sf::Vector2f(400 + noiseX, 300 + noiseY);
    boss->moveTowards(targetPos, 200.0f, deltaTime);

    if (fmod(enrageTimer, 0.25f) < deltaTime) {
        std::vector<float> randomAngles;
        std::uniform_real_distribution<float> dist(0, 360);
        for (int i = 0; i < 4; i++) {
            randomAngles.push_back(dist(rng));
        }
        boss->shootPattern(randomAngles, bullets);
    }

    if (enrageTimer > 5.0f) {
        boss->getStateMachine()->changeState("teleport", boss);
    }
}

void TeleportState::enter(Boss* boss) {
    std::uniform_real_distribution<float> distX(100, 700);
    std::uniform_real_distribution<float> distY(100, 500);
    boss->teleportTo(distX(rng), distY(rng));
    teleportTimer = 0.0f;
}

void TeleportState::update(Boss* boss, float deltaTime) {
    teleportTimer += deltaTime;

    if (teleportTimer > 1.0f) {
        boss->getStateMachine()->changeState("enrage", boss);
    }
}