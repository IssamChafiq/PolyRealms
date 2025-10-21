#include "Champion.hpp"
#include <iostream>
#include <algorithm>

void Champion::takeDamage(int attackValue) {
    if (stunned_) {
        std::cout << name_ << " is already stunned.\n";
        return;
    }

    if (isGuard_) {
        // Guards block all damage unless the attack breaks the shield
        if (attackValue < shield_) {
            std::cout << name_ << " blocks the attack with its guard shield ("
                      << shield_ << " defense).\n";
            return;
        } else {
            shield_ = 0;
            stunned_ = true;
            std::cout << name_ << " (Guard) has been defeated and removed from play.\n";
            // faut créer la classe joeur pour avoir la liste de cartes / deck
            // et enlever cette carte des champions ou de la zone en jeu du joueur
            return;
        }
    }

    // Non-guard champion: damage reduces shield gradually
    shield_ -= attackValue;
    if (shield_ > 0) {
        std::cout << name_ << " takes " << attackValue << " damage, "
                  << shield_ << " shield remaining.\n";
    } else {
        shield_ = 0;
        stunned_ = true;
        std::cout << name_ << " has been defeated and removed from play.\n";
        // faut créer la classe joeur pour avoir la liste de cartes / deck
        // et enlever cette carte des champions ou de la zone en jeu du joueur
    }
}

void Champion::heal(int amount) {
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
