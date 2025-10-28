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

void Champion::heal() {
    if (stunned_) {
        std::cout << name_ << " is stunned and cannot be healed.\n";
        return;
    }

    int before = shield_;
    shield_ = maxShield_;

    std::cout << name_ << " heals " << (maxShield_ - before)
              << " shield (now " << shield_ << "/" << maxShield_ << ").\n";
}

void Champion::printCardInfo() const{
    std::cout << "----------------------------------------\n";
    std::cout << "Card Name: " << name_ << "\n";
    std::cout << "Card ID:   " << id_ << "\n";
    std::cout << "Cost:      " << cost_ << "\n";
    std::cout << "Faction:   " << factionToString(faction_) << "\n";
    std::cout << "Type:      " << typeToString(type_) << "\n";

    std::cout << "Shield:    " << shield_ << "\n";
    std::cout << "Guard:     " << (isGuard_ ? "Yes" : "No") << "\n";
    std::cout << "Stunned:   " << (stunned_ ? "Yes" : "No") << "\n";

    std::cout << "Expendable: " << (expendable_ ? "Yes" : "No") << "\n";
    std::cout << "Sacrificeable: " << (sacrificeable_ ? "Yes" : "No") << "\n";

    if (!abilities().empty()) {
        std::cout << "Abilities:\n";
        for (const auto& ab : abilities()) {
            std::cout << "  - Trigger=" << triggerToString(ab.trigger)
                      << ", Name=" << abilityNameToString(ab.ability)
                      << ", Amount=" << ab.amount
                      << ", RequiresAlly=" << (ab.requiresAlly ? "Yes" : "No");
            if (ab.requiresAlly) {
                std::cout << " (" << factionToString(ab.requiredAllyFaction) << " Ally)";
            }
            std::cout << "\n";
        }
    } else {
        std::cout << "Abilities: (none parsed)\n";
    }
    
    std::cout << "----------------------------------------\n\n";
}
