#pragma once
#include "State.h"
#include "Boss.h"
#include <vector>
#include <cmath>
#include <random>

// États pour la Phase 1
class IdleState : public State {
public:
    void enter(Boss* boss) override;
    void update(Boss* boss, float deltaTime) override;
    void exit(Boss* boss) override {}
    std::string getName() const override { return "idle"; }

private:
    float timer = 0.0f;
};

class CircleAttackState : public State {
public:
    CircleAttackState(std::vector<Bullet>& bulletRef) : bullets(bulletRef) {}

    void enter(Boss* boss) override;
    void update(Boss* boss, float deltaTime) override;
    void exit(Boss* boss) override {}
    std::string getName() const override { return "circle_attack"; }
    void updateBullets(std::vector<Bullet>& bulletRef) {
        bullets = bulletRef;
    }

private:
    float attackTimer = 0.0f;
    int attackCount = 0;
    std::vector<Bullet>& bullets;
};

// États pour la Phase 2
class ChargeState : public State {
public:
    void enter(Boss* boss) override;
    void update(Boss* boss, float deltaTime) override;
    void exit(Boss* boss) override {}
    std::string getName() const override { return "charge"; }
};

class RushState : public State {
public:
    RushState() {} 

    void enter(Boss* boss) override;
    void update(Boss* boss, float deltaTime) override;
    void exit(Boss* boss) override {}
    std::string getName() const override { return "rush"; }
    void updatePlayerPos(const sf::Vector2f& pos) {
        playerPos = pos;  
    }

private:
    sf::Vector2f target;
    float rushTime = 0.0f;
    sf::Vector2f playerPos;  
};

class CooldownState : public State {
public:
    CooldownState() {}  

    void enter(Boss* boss) override;
    void update(Boss* boss, float deltaTime) override;
    void exit(Boss* boss) override {}
    std::string getName() const override { return "cooldown"; }
    void updatePlayerPos(const sf::Vector2f& pos) {
        playerPos = pos;  
    }

private:
    float cooldownTimer = 0.0f;
    sf::Vector2f playerPos; 
};


class EnrageState : public State {
public:
    EnrageState(std::vector<Bullet>& bulletRef) : bullets(bulletRef) {}

    void enter(Boss* boss) override;
    void update(Boss* boss, float deltaTime) override;
    void exit(Boss* boss) override {}
    std::string getName() const override { return "enrage"; }
    void updateBullets(std::vector<Bullet>& bulletRef) {
        bullets = bulletRef;
    }

private:
    float enrageTimer = 0.0f;
    std::vector<Bullet>& bullets;
    std::mt19937 rng{ std::random_device{}() };
};

class TeleportState : public State {
public:
    void enter(Boss* boss) override;
    void update(Boss* boss, float deltaTime) override;
    void exit(Boss* boss) override {}
    std::string getName() const override { return "teleport"; }

private:
    float teleportTimer = 0.0f;
    std::mt19937 rng{ std::random_device{}() };
};