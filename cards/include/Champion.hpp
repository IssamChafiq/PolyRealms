#pragma once
#include "Card.hpp"
#include <algorithm>
#include <iostream>

class Champion : public Card {
public:
    Champion(std::string id,
             std::string name,
             int cost,
             Faction faction,
             int shield,
             bool isGuard,
             bool expendable = true)
        : Card(std::move(id),
               std::move(name),
               cost,
               faction,
               CardType::Champion,
               expendable,
               /*sacrificeable=*/false),
          shield_(shield),
          maxShield_(shield),
          isGuard_(isGuard),
          stunned_(false) {}

    bool isChampion() const override { return true; }

    bool isGuard() const { return isGuard_; }
    bool isStunned() const { return stunned_; }
    int  getShield() const { return shield_; }
    int  getMaxShield() const { return maxShield_; }

    // combat
    void takeDamage(int attackValue) {
        if (stunned_) {
            std::cout << name_ << " is already stunned.\n";
            return;
        }

        if (isGuard_) {
            // attack doit etre plus fort que le shield
            if (attackValue < shield_) {
                std::cout << name_ << " blocks the attack with its guard shield ("
                          << shield_ << " defense).\n";
                return;
            } else {
                shield_ = 0;
                stunned_ = true;
                std::cout << name_ << " (Guard) has been defeated and removed from play.\n";
                // faut créer la classe joeur pour enlever la carte du plateau
                return;
            }
        }

        // diminuer la valeur du shield si non guard
        shield_ -= attackValue;
        if (shield_ > 0) {
            std::cout << name_ << " takes " << attackValue
                      << " damage, " << shield_ << " shield remaining.\n";
        } else {
            shield_ = 0;
            stunned_ = true;
            std::cout << name_ << " has been defeated and removed from play.\n";
            // faut créer la classe joeur pour enlever la carte du plateau
        }
    }

    void heal(int amount) {
        if (stunned_) {
            std::cout << name_ << " is stunned and cannot be healed.\n";
            return;
        }
        if (amount <= 0) {
            std::cout << name_ << " heal amount must be positive.\n";
            return;
        }

        int before = shield_;
        shield_ = std::min(maxShield_, shield_ + amount);

        std::cout << name_ << " heals " << (shield_ - before)
                  << " shield (now " << shield_ << "/" << maxShield_ << ").\n";
    }

private:
    int  shield_;
    int  maxShield_;
    bool isGuard_;
    bool stunned_;
};
