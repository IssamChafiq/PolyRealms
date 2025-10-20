#pragma once
#include "Card.hpp"

class Champion : public Card {
public:
    Champion(std::string id,
             std::string name,
             int cost,
             Faction faction,
             int defense,
             bool isGuard,
             bool expendable = true)
        : Card(std::move(id), std::move(name), cost, faction, CardType::Champion, expendable, false),
          defense_(defense),
          isGuard_(isGuard) {}

    bool isChampion() const override { return true; }

    bool isGuard() const { return isGuard_; }
    bool isStunned() const { return stunned_; }

    void applyDamage(int amount);

private:
    int defense_;
    bool isGuard_;
    bool stunned_{false};
};
