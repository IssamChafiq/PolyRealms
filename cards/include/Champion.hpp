#pragma once
#include "Card.hpp"

class Champion : public Card {
public:
    Champion(std::string id,
             std::string name,
             int cost,
             Faction faction,
             int shield,
             bool isGuard,
             bool expendable = true)
        : Card(std::move(id), std::move(name), cost, faction, CardType::Champion, expendable, false),
          shield_(shield),
          maxShield_(shield),   // save initial shield cap
          isGuard_(isGuard),
          stunned_(false) {}

    bool isChampion() const { return true; }

    bool isGuard() const { return isGuard_; }
    bool isStunned() const { return stunned_; }
    int  getShield() const { return shield_; }
    int  getMaxShield() const { return maxShield_; }

    // Combat
    void takeDamage(int attackValue);

    // Healing (cannot exceed initial shield)
    void heal();

    // Targeting rule helper
    bool mustBeAttackedFirst() const { return isGuard_; }

    void printCardInfo() const;

private:
    int  shield_;      // current shield (can go down, can be healed up to maxShield_)
    int  maxShield_;   // initial shield cap
    bool isGuard_;     // must be attacked first if true
    bool stunned_;     // defeated (removed) state
};
