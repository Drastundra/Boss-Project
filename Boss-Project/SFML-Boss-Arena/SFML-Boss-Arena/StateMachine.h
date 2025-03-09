#pragma once
#include "State.h"
#include <memory>
#include <unordered_map>
#include <string>

class StateMachine {
public:
    StateMachine() : currentState(nullptr) {}

    void addState(const std::string& name, std::shared_ptr<State> state) {
        states[name] = state;
    }

    void changeState(const std::string& name, Boss* boss) {
        if (states.find(name) == states.end()) return;

        if (currentState) {
            currentState->exit(boss);
        }

        currentState = states[name];
        currentState->enter(boss);
    }

    void update(Boss* boss, float deltaTime) {
        if (currentState) {
            currentState->update(boss, deltaTime);
        }
    }

    std::string getCurrentStateName() const {
        return currentState ? currentState->getName() : "NONE";
    }

    std::shared_ptr<State> getState(const std::string& name) {
        auto it = states.find(name);
        if (it != states.end()) {
            return it->second;
        }
        return nullptr;
    }

private:
    std::unordered_map<std::string, std::shared_ptr<State>> states;
    std::shared_ptr<State> currentState;
};