#pragma once
#include <string>

class Boss;

class State {
public:
    virtual ~State() = default;
    virtual void enter(Boss* boss) = 0;
    virtual void update(Boss* boss, float deltaTime) = 0;
    virtual void exit(Boss* boss) = 0;
    virtual std::string getName() const = 0;
};